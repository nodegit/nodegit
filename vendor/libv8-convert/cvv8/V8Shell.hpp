#if !defined(V8_CONVERT_V8Shell_HPP_INCLUDED)
#define V8_CONVERT_V8Shell_HPP_INCLUDED
/** @file V8Shell.hpp

    This file contains the v8::convert::V8Shell class, a convenience
    wrapper for bootstrapping integration of v8 into arbitrary
    client applications.

    Dependencies: v8 and the STL.

    License: released into the Public Domain by its author,
    Stephan Beal (http://wanderinghorse.net/home/stephan/).
*/
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <fstream>

#include <v8.h>

namespace cvv8 {
    namespace Detail {
        template <bool UseLocker>
        struct V8MaybeLocker
        {
            private:
                v8::Locker lock;
            public:
                V8MaybeLocker() : lock() {}
        };
        template <>
        struct V8MaybeLocker<false>
        {
        };
    }
    /**
        This class implements a very basic shell for v8.
 
         
        These objects are basically thin wrappers around the 
        bootstrap code necessary for getting v8 running in an 
        application. They are intended to be stack-created in main() 
        (or equivalent) and used as a front-end for passing JS code 
        into v8 for execution.

        Because library-level JS code activated via this class 
        _might_ use v8::Unlocker to unlock the VM while C-level 
        routines are running, each instance of this class includes a 
        v8::Locker instance if UseLocker is true. (If it did not, 
        clients would be required to add one or accept crashes when 
        called code uses v8::Unlocker.) Only set UseLocker to false 
        if you _know_ that _no_ JS code run through this API will 
        end up trying to unlock v8. (If you're using this class 
        together with the v8::convert function binding API then you 
        are almost certainly using v8::Unlocker without realizing it.)

        Maintenance reminder: keep this class free of dependencies 
        on other library-level code so that we can re-use it 
        in arbitrary v8 clients.

        FIXME: the way this class uses v8::TryCatch is "all wrong", and any
        functions using it need to be revisited.
    */
    template <bool UseLocker = true>
    class V8Shell
    {
    public:
        /**
           A callback function signature for reporing JS-side
           exception messages to the native world.
           
           TODO: consider passing a v8::Handle<v8::Value> argument 
           instead of a (char const *) and possibly a (void *) State 
           handle for use by the client.
        */
        typedef void (*ErrorMessageReporter)( char const * msg );
    private:
        // The declaration order of the v8-related objects is important!
        Detail::V8MaybeLocker<UseLocker> locker;
        v8::HandleScope hscope;
        //v8::Handle<v8::ObjectTemplate> globt;
        v8::Handle<v8::Context> context;
        v8::Context::Scope cxscope;
        v8::Handle<v8::Object> global;
        /**
            tryCatch is only here until i can track down a post-main()
            v8 assertion which happens when V8Shell-executed JS code
            exits with an exception. It is just a workaround.
        */
        v8::TryCatch tryCatch;
        ErrorMessageReporter reporter;
        static void DefaultErrorMessageReporter( char const * msg )
        {
            if( msg && *msg ) std::cerr
                //<< "[V8Shell default exception reporter says:]\n"
                << msg << std::endl;
        }
        
        /**
            An v8::InvocationCallback implementation which implements a
            JS-conventional print() routine. OS must be a pointer to
            an ostream, e.g. std::cout or std::cerr.

            Each argument is converted to a string (using 
            v8::String::Utf8Value) and is output, separated by a 
            space. For compatibility with other toolkits' print() 
            implementations (some of which only accept one 
            argument), it is recommended that client script code 
            only rely on the first argument being output.

            Always returns v8::Undefined().

            It is a little-known fact that one can replace the output 
            buffer used by std::cout (and other std::ostreams) with a 
            custom one, such that calling print() from JS code will 
            redirect the output to a different destination. This can 
            be used, e.g., to redirect std::cout to a libcurses window.
        */
        template <std::ostream * OS>
        static v8::Handle<v8::Value> PrintToStdOstream( v8::Arguments const & argv )
        {
            v8::HandleScope hscope;
            int const argc = argv.Length();
            const char * cstr = NULL;
            for (int i = 0; i < argc; i++)
            {
                if( 0 != i ) *OS << ' ';
                v8::String::Utf8Value const str(argv[i]);
                cstr = *str;
                if( cstr ) *OS << cstr;
            }
            *OS << '\n';
            OS->flush();
            return v8::Undefined();
        }

        void init( char const * globalObjectName,
                    int argc, char const * const * argv,
                    unsigned short argOffset )
        {
            if( globalObjectName && *globalObjectName )
            {
                this->global->Set( v8::String::New(globalObjectName), this->global );
            }
            if( (0 < argc) && (NULL != argv) )
            {
                this->ProcessMainArgv( argc, argv, argOffset );
            }
        }

        static void SetupTryCatch( v8::TryCatch & tc )
        {
            tc.SetVerbose(true);
            tc.SetCaptureMessage(true);
        }
    public:
        /**
           Initialize a v8 context and global object belonging to this object.

           If globalObjectName is not null and not empty then the global object
           is given a refernce to itself using the given name, such that client
           JS code can then refer to it.
           
           If argc is greater than 0 and argv is not NULL then argv is
           assumed to be an arguments list in the format conventional
           for main() and ProcessMainArgv(argc,argv,argOffset) is called.
        */
        V8Shell( char const * globalObjectName = NULL,
                int argc = 0, char const * const * argv = NULL,
                unsigned short argOffset = 1 ) :
            locker(),
            hscope(),
            //globt( v8::ObjectTemplate::New() ),
            context( v8::Context::New(NULL, v8::ObjectTemplate::New()) ),
            cxscope(context),
            global( context->Global() ),
            reporter( DefaultErrorMessageReporter )
        {
            this->init( globalObjectName, argc, argv, argOffset );
        }

        /**
            Destructs all v8 resources used by this object, e.g. the JS context.
        */
        ~V8Shell()
        {
            if( ! v8::V8::IsDead() ) {
                tryCatch.Reset();
            }
        }
        
        /**
           Sets the error reporter function used by
           ExecuteString(). Passing 0 will disable exception
           reporting. The default reporter sends its output to
           std::cerr.
        */
        void SetExecuteErrorReporter( ErrorMessageReporter r )
        {
            this->reporter = r;
        }
    
        /**
           Outputs an exception message using the current
           error reporter function.

           If try_catch or the current error reporter are
           null then nothing is done.
           
           @see SetExecuteErrorReporter().
        */
 
        void ReportException(v8::TryCatch* try_catch)
        {
            if( !try_catch || ! this->reporter ) return;
            v8::HandleScope hsc;
            v8::String::Utf8Value const excUtf(try_catch->Exception());
#define TOCSTR(X) (*X ? *X : "<exception string conversion failed!>")
            const char* excCstr = TOCSTR(excUtf);
            v8::Handle<v8::Message> const & message( try_catch->Message() );
            std::ostringstream os;
            os << "V8Shell Exception Reporter: ";
            if (message.IsEmpty())
            {
                // V8 didn't provide any extra information about this error; just
                // print the exception.
                os << excCstr << '\n';
            }
            else
            {
                // output (filename):(line number): (message)...
                int linenum = message->GetLineNumber();
                os << *v8::String::Utf8Value(message->GetScriptResourceName()) << ':'
                   << std::dec << linenum << ": "
                   << excCstr << '\n';
                // output source code line...
                os << *v8::String::AsciiValue(message->GetSourceLine()) << '\n';
                // output decoration pointing to error location...
                int start = message->GetStartColumn();
                for (int i = 0; i < start; i++) {
                    os << '-';
                }
                int end = message->GetEndColumn();
                for (int i = start; i < end; i++) {
                    os << '^';
                }
                os << '\n';
            }
            std::string const & str( os.str() );
            this->reporter( str.c_str() );
#undef TOCSTR
        }
        
        /**
            Adds the given function to the global object.
            
            Returns this object.
        */
        V8Shell & operator()( char const * name, v8::Handle<v8::Function> const & f )
        {
            this->global->Set( v8::String::New(name), f );
            return *this;
        }

        /**
            Adds the given function to the global object.
            
            Returns this object.
        */
        V8Shell & operator()( char const * name, v8::Handle<v8::FunctionTemplate> const & f )
        {
            return this->operator()( name, f->GetFunction() );
        }

        /**
            Adds the given function to the global object.
            
            Returns this object.
        */
        V8Shell & operator()( char const * name, v8::InvocationCallback const f )
        {
            return this->operator()( name, v8::FunctionTemplate::New(f) );
        }

        /**
            Returns the global object for this shell.
        */
        v8::Handle<v8::Object> Global()
        {
            return this->global;
        }

        /**
            Returns the context object for this shell.
        */
        v8::Handle<v8::Context> Context()
        {
            return this->context;
        }

#if 0 // changes made to global ObjectTemplate have no effect after cx is set up.
        v8::Handle<v8::ObjectTemplate> GlobalTemplate()
        {
            return this->globt;
        }
#endif

        /**
           Intended to be called from main() and passed the argc/argv
           which are passed to main. offset is the number of arguments
           to skip, and defaults to one to skip the argv[0] argument,
           which is conventionally the application name.

           It skips all arguments up to "--". For each argument after
           "--", it adds the argument to a list. At the end of the
           list, the global object is assigned a property named
           "arguments" which contains that list.
           
           If the argument list has no arguments after a "--" entry
           then the "arguments" global value will be an empty array,
           as opposed to null or undefined.

           This function does no interpretation of the arguments.
        */
        V8Shell & ProcessMainArgv( int argc, char const * const * _argv, unsigned short offset = 1 )
        {
            if( (argc<1) || !_argv ) return *this;
            char const * endofargs = "--";
            v8::Handle<v8::Array> argv( v8::Array::New() );
            int i = (int)offset;
            for( ; i < argc; ++i )
            {
                if( 0 == strcmp(_argv[i],endofargs) )
                {
                    ++i;
                    break;
                }
            }
            int ndx = 0;
            for( ; i < argc; ++i )
            {
                char const * arg = _argv[i];
                if( arg )
                { // String::New() calls strlen(), which hates NULL
                    argv->Set( ndx++, v8::String::New(arg) );
                }
            }
            this->global->Set( v8::String::New("arguments"), argv );
            return *this;
        }
        
        /**
           Executes the given source string in the current
           context.

           If the script throws an exception then a TryCatch object is used
           to build an error string, which is passed to this object's error
           reporter function. The default sends the output to std::cerr.

           If resultGoesTo is not null and the result a valid handle, then
           the result is converted to a string and sent to that stream.

           Returns the result of the last expression evaluated in the script,
           or an empty handle on error.
        */
        v8::Handle<v8::Value> ExecuteString(v8::Handle<v8::String> const & source,
                                           v8::Handle<v8::Value> name,
                                           std::ostream * out = NULL )
        {
            //this->executeThrew = false;
            v8::HandleScope scope;
            v8::TryCatch tc;
            SetupTryCatch(tc);
            v8::Handle<v8::Script> script = v8::Script::Compile(source, name);
            if( script.IsEmpty())//tc.HasCaught())
            {
                // Report errors that happened during compilation.
                //this->executeThrew = true;
                this->ReportException(&tc);
                return scope.Close(tc.ReThrow());
                //return v8::Handle<v8::Value>();
            }
            else
            {
                v8::Handle<v8::Value> const & result( script->Run() );
                if( tc.HasCaught())//(result.IsEmpty())
                {
                    //this->executeThrew = true;
                    this->ReportException(&tc);
                    //return v8::Handle<v8::Value>();
                    return scope.Close(tc.ReThrow());
                }
                else
                {
                    if (out && !result.IsEmpty())
                    {
                        (*out) << *v8::String::Utf8Value(result) << '\n';
                    }
                    return scope.Close(result);
                }
            }
        }
        
#if 0
        bool ExecThrewException() const
        {
            return this->executeThrew;
        }
#endif

        /**
           Convenience form of ExecuteString(source,"some default name", reportExceptions, 0).
        */
        v8::Handle<v8::Value> ExecuteString(std::string const & source,
                                           std::string const & name,
                                           std::ostream * resultGoesTo )
        {
            v8::HandleScope scope;
            v8::Local<v8::String> const & s( v8::String::New( source.c_str(), static_cast<int>(source.size()) ) );
            v8::Local<v8::String> const & n( v8::String::New( name.c_str(), static_cast<int>(name.size()) ) );
            return scope.Close(this->ExecuteString( s, n, resultGoesTo ));
        }

        /**
           Convenience overload taking input from a native string.
        */
        v8::Handle<v8::Value> ExecuteString(std::string const & source )
        {
            return this->ExecuteString(source, "ExecuteString()", 0);
        }

        /**
           Convenience form of ExecuteString(source,"some default name", 0, reportExceptions).
        */
        v8::Handle<v8::Value> ExecuteString(v8::Handle<v8::String> source )
        {
            return this->ExecuteString(source, v8::String::New("ExecuteString()"), 0);
        }
        
        /**
           Convenience form of ExecuteString() reading from an opened input stream.

           Throws a std::exception if reading fails or the input is empty.

           An empty input is not necessarily an error. Todo: re-think this decision.
        */
        v8::Handle<v8::Value> ExecuteStream( std::istream & is, std::string const & name,
                                            std::ostream * resultGoesTo = NULL )
        {
            std::ostringstream os;
            is >> std::noskipws;
            std::copy( std::istream_iterator<char>(is), std::istream_iterator<char>(), std::ostream_iterator<char>(os) );
            std::string const & str( os.str() );
            if( str.empty() )
            {
                std::ostringstream msg;
                msg << "Input stream ["<<name<<"] is empty.";
                //return v8::ThrowException(v8::Exception::Error(v8::String::New(msg.str().c_str())));
                std::string const & str(os.str());
                throw std::runtime_error(str.c_str());
            }
            return this->ExecuteString( str, name, resultGoesTo );
        }
        
        /**
           Convenience form of ExecuteString() reading from a local file.
        */
        v8::Handle<v8::Value> ExecuteFile( char const * filename,
                                           std::ostream * resultGoesTo = NULL )
        {
            if( ! filename || !*filename )
            {
                throw std::runtime_error("filename argument must not be NULL/empty.");
            }
            std::ifstream inf(filename);
            if( ! inf.good() )
            {
                // FIXME: throw a v8 exception and report it via our reporter.
                // Nevermind: the result is useless b/c the exception has no proper vm stack/state info here...
                std::ostringstream msg;
                msg << "Could not open file ["<<filename<<"].";
                std::string const & str( msg.str() );
                throw std::runtime_error( str.c_str() );
            }
            return this->ExecuteStream( inf, filename, resultGoesTo );
        }

        /**
           An v8::InvocationCallback implementation which implements 
           a JS-conventional print() routine, sending its output to 
           std::cout. See PrintToStdOstream() for the exact semantics
           argument/return.
        */
        static v8::Handle<v8::Value> PrintToCout( v8::Arguments const & argv )
        {
            return PrintToStdOstream<&std::cout>( argv );
        }
        
        /**
           Identical to PrintToCout(), but sends its output to
           std::cerr instead.
        */
        static v8::Handle<v8::Value> PrintToCerr( v8::Arguments const & argv )
        {
            return PrintToStdOstream<&std::cerr>( argv );
        }
        
    private:
        static v8::Handle<v8::Value> Include( v8::Arguments const & argv )
        {
            int const argc = argv.Length();
            if( argc < 1 ) return v8::Undefined();
            v8::HandleScope hsc;
            v8::Local<v8::Value> const jvself(argv.Data());
            if( jvself.IsEmpty() || !jvself->IsExternal() )
            {
                return v8::ThrowException(v8::Exception::Error(v8::String::New("Include() callback is missing its native V8Shell object.")));
            }
            V8Shell * self = static_cast<V8Shell *>( v8::External::Cast(*jvself)->Value() );
            v8::String::Utf8Value fn(argv[0]);
            try
            {
                return hsc.Close(self->ExecuteFile( *fn ));
            }
            catch( std::exception const & ex )
            {
                char const * msg = ex.what();
                return v8::ThrowException(v8::Exception::Error(v8::String::New(msg ? msg : "Unspecified native exception.")));
            }
        }
    
    public:
        /**
            Returns a Function object implementing conventional 
            include(filename) functionality (called load() in some JS
            shells). This function must be created dynamically 
            because the generated function internally refers back to 
            this object (so that we can re-use ExecuteString() for the
            implementation).
            
            The return value of the Function is the value of the last
            expression evaluated in the given JS code.
            
            Results are undefined, and almost certainly fatal, if 
            the generated function is ever called after this native 
            object has been destructed. For best results, to avoid 
            potential lifetime issues, never install the returned 
            function in any object other than this object's Global().            
        */
        v8::Handle<v8::Function> CreateIncludeFunction()
        {
            return v8::FunctionTemplate::New(Include, v8::External::New(this))->GetFunction();
        }
        
        /**
           Implements the v8::InvocationCallback interface and has the
           following JS interface:

           @code
           Array getStracktrace([unsigned int limit = some reasonable default])
           @endcode

           Each element in the returned array represents a stack frame and
           is a plain object with the following properties:

           column = 1-based column number (note that this is 
           different from most editors, but this is how v8 returns 
           this value).
               
           line = 1-based line number

           scriptName = name of the script

           functionName = name of the function

           isConstructor = true if this is a constructor call

           isEval = true if this is part of an eval()

           TODO:

           - Add a toString() member to the returned array which creates a
           conventional-looking stacktrace string.
        */
        static v8::Handle<v8::Value> GetStackTrace( v8::Arguments const & argv )
        {
            using namespace v8;
            int32_t limitSigned = (argv.Length() > 0) ? argv[0]->Int32Value() : 0;
            if( limitSigned <= 0 ) limitSigned = 8;
            else if( limitSigned > 100 ) limitSigned = 100;
            uint32_t limit = static_cast<uint32_t>(limitSigned);
            HandleScope hsc;
            Local<StackTrace> const st = StackTrace::CurrentStackTrace( limit, StackTrace::kDetailed );
            int const fcountI = st->GetFrameCount();
            // Who the hell designed the StackTrace API to return an int in GetFrameCount() but take
            // an unsigned int in GetFrame()???
            uint32_t const fcount = static_cast<uint32_t>(fcountI);
            Local<Array> jst = Array::New(fcount);
#define STR(X) v8::String::New(X)
            for( uint32_t i = 0; (i < fcount) && (i<limit); ++i )
            {
                Local<StackFrame> const & sf( st->GetFrame(i) );
                Local<Object> jsf = Object::New();
                jsf->Set(STR("column"), v8::Integer::New(sf->GetColumn()));
                jsf->Set(STR("functionName"), sf->GetFunctionName());
                jsf->Set(STR("line"), v8::Integer::New(sf->GetLineNumber()));
                jsf->Set(STR("scriptName"), sf->GetScriptName());
                jsf->Set(STR("isConstructor"), sf->IsConstructor() ? v8::True() : v8::False() );
                jsf->Set(STR("isEval"), sf->IsEval() ? v8::True() : v8::False() );
                jst->Set(i,jsf);
            }
            return hsc.Close(jst);
#undef STR
        }
        
        /**
            Can optionally be called to include the following functionality
            in this shell's Global() object:
            
            JS Functions:
            
            print(...) (see PrintToCout())
            
            getStacktrace([int limit]) (see GetStackTrace())
            
            load(filename) (see CreateIncludeFunction())
            
            Returns this object, for use in chaining.
        */
        V8Shell & SetupDefaultBindings()
        {
            (*this)( "print", PrintToCout )
                ("getStacktrace", GetStackTrace)
                ("load", this->CreateIncludeFunction())
            ;
            return *this;
        }
    };

    /**
        Convenience typedef for V8Shell<>.
    */
    typedef V8Shell<> Shell;

}
#endif /* V8_CONVERT_V8Shell_HPP_INCLUDED */
