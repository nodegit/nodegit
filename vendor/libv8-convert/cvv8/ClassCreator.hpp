#if !defined(CODE_GOOGLE_COM_P_V8_CONVERT_CLASS_CREATOR_HPP_INCLUDED)
#define CODE_GOOGLE_COM_P_V8_CONVERT_CLASS_CREATOR_HPP_INCLUDED 1
/** LICENSE

    This software's source code, including accompanying documentation and
    demonstration applications, are licensed under the following
    conditions...

    The author (Stephan G. Beal [http://wanderinghorse.net/home/stephan/])
    explicitly disclaims copyright in all jurisdictions which recognize
    such a disclaimer. In such jurisdictions, this software is released
    into the Public Domain.

    In jurisdictions which do not recognize Public Domain property
    (e.g. Germany as of 2011), this software is Copyright (c) 2011
    by Stephan G. Beal, and is released under the terms of the MIT License
    (see below).

    In jurisdictions which recognize Public Domain property, the user of
    this software may choose to accept it either as 1) Public Domain, 2)
    under the conditions of the MIT License (see below), or 3) under the
    terms of dual Public Domain/MIT License conditions described here, as
    they choose.

    The MIT License is about as close to Public Domain as a license can
    get, and is described in clear, concise terms at:

    http://en.wikipedia.org/wiki/MIT_License

    The full text of the MIT License follows:

    --
    Copyright (c) 2011 Stephan G. Beal (http://wanderinghorse.net/home/stephan/)

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

    --END OF MIT LICENSE--

    For purposes of the above license, the term "Software" includes
    documentation and demonstration source code which accompanies
    this software. ("Accompanies" = is contained in the Software's
    primary public source code repository.)

*/

#include <cassert>
#include <stdexcept>
#include "convert.hpp"
//#include <iostream> // only for debuggering
#include "NativeToJSMap.hpp"
namespace cvv8 {

    /**
       Policy template used by ClassCreator<T> for
       instantiating T objects.
    */
    template <typename T>
    class ClassCreator_Factory
    {
    public:
        typedef T * ReturnType;
        /**
           Must instantiate a new T object based on the given
           arguments. On error it should throw an exception (which the
           binding framework will convert to a JS-side exception). It
           may also return NULL on error, but the error message
           probably won't be as clear for the user.

           Ownership of the object is passed to the caller (the
           binding API internals), and eventually given to v8.

           jsSelf will be the newly-created JS-side 'this' object.  It
           is not normally required by this function but it is
           sometimes useful when we need to bind supplementary
           properties in the ctor, especially when binding a "pure
           C++" class which has no native place to store such
           properties.

           At the time this is called, jsSelf is not connected to the
           native (because it hasn't yet been created).
           Implementations must not perform the actual binding of the
           returned native to jsSelf - ClassCreator will do that
           immediately after Create() returns the new object.
           
           The default implementation simply return (new T).
        */
        static ReturnType Create( v8::Persistent<v8::Object> & jsSelf, v8::Arguments const & argv )
        {
            return new T;
        }

        /**
           Must destroy obj using a mechanism complementary to its
           construction via a prior call to Create().

           The default implementation simply calls (delete obj).
        */
        static void Delete( T * obj )
        {
            delete obj;
        }
    };

    /**
       Base class for static ClassCreator options.
     */
    template <typename ValT, ValT Val>
    struct Opt_ConstVal
    {
        typedef ValT Type;
        const static Type Value = Val;
    };

    /**
       Base class for static integer ClassCreator options.
    */
    template <int Val>
    struct Opt_Int : Opt_ConstVal<int,Val>
    {};

    /**
       Base class for static boolean ClassCreator options.
    */
    template <bool Val>
    struct Opt_Bool : Opt_ConstVal<bool,Val>
    {};

    /**
       A ClassCreator policy/option class responsible specifying whether
       or not a ClassCreator-bound class should allow "Foo()" and "new
       Foo()" to behave the same or not. If the Value member is false
       (the default) then "Foo()" is not allowed to behave as a
       constructor call (it will generate an error), otherwise it will
       be treated exactly as if "new Foo()" had been called.
    */
    template <typename T>
    struct ClassCreator_AllowCtorWithoutNew : Opt_Bool<false>
    {};

    /**
       ClassCreator policy which determines whether lookups for native
       types in JS objects should walk through the prototype
       chain. This can decrease the speed of JS-to-this operations and
       is necessary only if bound types will be subclassed (either from
       other bound native types or from JS classes).

       The default value is true for the sake of usability. If JS-side
       subclassing will never be used, you can potentially optimize out a
       few lookup operations by creating the specialization by subclassing
       Opt_Bool<false>.
    */
    template <typename T>
    struct ClassCreator_SearchPrototypeForThis : Opt_Bool<true>
    {};

    /**
        ClassCreator policy type which defines a "type ID" value
        for a type wrapped using ClassCreator. This is used
        together with JSToNative_ObjectWithInternalFieldsTypeSafe
        (type THAT 10 times fast) to provide a lightweight
        (but highly effective) type check when extracting
        natives from v8 (as void pointers). The default
        implementation is fine for all cases i can think of, but i can
        concieve of one or two uses for specializations (e.g. storing the
        JS-side name of the class as the type ID).
        
        The type id must be unique per type except that subtypes may
        (depending on various other binding options) may need use the same
        value as the parent type. If multiple types share the same type ID,
        the type-safety check can be bypassed, _potentially_ leading to an
        illegal static_cast() and subsequent mis-use of the pointer. i
        stress the word "potentially" because to get that condition one
        would have to (A) abuse the object via the C++ API (which doesn't
        happen via the binding process, and you're probably also not going
        to do it) or (B) write some script code to confuse two bound native
        types about who is really who when a particular member is called.

        In the case of subclassed bound types, the
        ClassCreator_TypeID<SubClass> impl should subclass
        ClassCreator_TypeID<ParentClass>. Whether or not this is _required_
        for proper functionality depends at least in part on whether
        (ClassCreator_InternalFields<ParentType>::TypeIDIndex>=0). If it is
        negative, subclasses do not need to explicitly define this policy
        because the type ID won't be used for purposes of validating a JS-held
        pointer's native type.

        TODO: see if we can consolidate this type with TypeName<>. The problem
        at the moment is that JSToNative_ObjectWithInternalFieldsTypeSafe
        takes a (void const * &) and TypeName::Value is a (char const *), which
        won't convert to (void const *) in the context of template parameters.
    */
    template <typename T>
    struct ClassCreator_TypeID
    {
        const static void * Value;
    };
    template <typename T>
    const void * ClassCreator_TypeID<T>::Value = TypeName<T>::Value;

    /**
       Convenience base type for ClassCreator_InternalFields
       implementations.

       See the member documentation for the meaning of
       HowMany and Index.

       If any of the following conditions are met then
       a compile-time assertion is triggered:

       - (ObjectIndex<0)

       - (ObjectIndex>=HowMany)

       - (TypeIndex>=HowMany).

       - (TypeIndex == ObjectIndex)

        TypeIndex may be negative, which indicates to ClassCreator that the
        binding should not store type ID information. However, if it is
        negative then T must not be used together with
        JSToNative_ObjectWithInternalFieldsTypeSafe - doing so will trigger
        a compile-time assertion.
    */
    template <typename T, int HowMany = 2, int TypeIndex = 0, int ObjectIndex = 1>
    struct ClassCreator_InternalFields_Base
    {
        /**
           Total number of internal fields assigned to JS-side T
           objects.
        */
        static const int Count = HowMany;

        /**
           The internal field index at which ClassCreator policies should
           expect the native object to be found in any given JS object.
           It must be 0 or greater, and must be less than Value.
        */
        static const int NativeIndex = ObjectIndex;
        
        /**
            The internal field index at which ClassCreator policies
            should expect a type identifier tag to be stored.
            This can be used in conjunction with 
            JSToNative_ObjectWithInternalFieldsTypeSafe (or similar)
            to provide an extra level of type safety at JS runtime.

        */
        static const int TypeIDIndex = TypeIndex;
    private:
        typedef char AssertFields[
            (HowMany > TypeIndex)
            && (HowMany > ObjectIndex)
            && (TypeIndex != ObjectIndex)
            && (ObjectIndex >= 0)
            ? 1 : -1];
    };

    /**
       The ClassCreator policy which sets the number of internal
       fields reserved for JS objects and the internal field index
       (0-based) at which the native object is stored . The Count
       value must be greater than 0 and greater than the NativeIndex
       member. Failing to meet these prerequisites will cause a
       compile-time assertion to be triggered.

       ACHTUNG SUBCLASSERS:

       When using a heirarchy of native types, more than one of which
       is compatible with CastFromJS(), conversions from subtype to 
       base type will fail unless all subtypes use the same internal 
       field placement as the parent type. If this code can detect a 
       mismatch then it will fail gracefully (e.g. a JS-side 
       exception), and if not then it might mis-cast an object and 
       cause Undefined Behaviour.

       If a given parent type uses a custom ClassCreator_InternalFields
       specialization then to ensure that subclasses always have the
       same placement, they "should" define their own policy like
       this:

       @code
       template <>
       struct ClassCreator_InternalFields< SubType >
           : ClassCreator_InternalFields< ParentType >
          {};
       @endcode

       That prohibits special internal field handling in the subtypes,
       but experience hasn't shown that subclasses need their own
       internal fields. Normaly a single internal field is all we need
       when binding native data. And when i say "normally", i mean
       "almost always."

       This must-match requirement is partially a side-effect of the library
       internally using the field count as a santiy check before trying to
       extract data from internal fields. It also exists so that the
       optional (but recommended) type-safety-check support (added in late
       June 2011: see JSToNative_ObjectWithInternalFieldsTypeSafe) will
       treat the subclasses as instances of the base class.
    */
    template <typename T>
    struct ClassCreator_InternalFields : ClassCreator_InternalFields_Base<T>
    {
    };


    /**
        This policy is used by ClassCreator::SetupBindings() as the generic
        interface for plugging in a bound class. Clients are not required to
        specialise this, but see this class' Initialize() for what might
        happen if they don't.
    */
    template <typename T>
    struct ClassCreator_SetupBindings
    {
        /**
            Specializations should perform any class/function-related binding
            here, adding their functionality to the given object (which is
            normally the logical global object but need not be). (Note that the
            handle refererence is const but that object itself can be modified.

            The default implementation throws an exception deriving from
            std::exception, so it must be specialized to be useful. A default
            specialization exists because there are probably a few cases
            out there which don't really need this. But most (if not all)
            need a setup function, and this is the official one for
            ClassCreator-wrapped types. Implementations may of course simply
            forward the call to another, client-provided function.

            On error the binding should throw a NATIVE exception (ideally
            deriving from std::exception because (A) it's portable practice
            and (B) parts of the cvv8 API handles those explicitly).

            Several years of experience have shown that this function (or
            similar implementations) should take some care to make sure
            not to set up their bindings twice. We can do that by using the
            following pattern:

            @code
            typedef ClassCreator<T> CC;
            CC & cc( CC::Instance() );
            if( cc.IsSealed() ) {
                cc.AddClassTo( "T", dest );
                return;
            }
            
            // ... do your bindings here...

            // As the final step:            
            cc.AddClassTo( "T", dest );
            return;
            @endcode

            If you do not actually want to add the class to the dest object,
            you should call Seal() instead of AddClassTo() (or pass a different
            destination object to AddClassTo().
        */
        static void Initialize( v8::Handle<v8::Object> const & target )
        {
            throw std::runtime_error("ClassCreator_SetupBindings<T> MUST be specialized "
                                     "in order to be useful!");
        }
    };

    /**
        A concrete ClassCreator_SetupBindings implementation which forwards
        the call to a user-defined function.
    */
    template <typename T, void (*Func)( v8::Handle<v8::Object> const &) >
    struct ClassCreator_SetupBindings_ClientFunc
    {
        /**
            Calls Func(target).
        */
        static void Initialize( v8::Handle<v8::Object> const & target )
        {
            Func(target);
        }
    };

    /**
       The ClassCreator policy class responsible for doing optional
       class-specific binding-related work as part of the JS/Native
       object construction process.

       The default specialization does nothing (which is okay for the
       general case) but defines the interface which specializations
       must implement.

       Reminder to self: we could arguably benefit by splitting this policy
       into 3 classes, but experience has shown that the metadata used by
       the 3 functions are typically shared amongst the 3 implementations
       (or 2 of them in most cases).
    */
    template <typename T>
    struct ClassCreator_WeakWrap
    {
        typedef typename TypeInfo<T>::NativeHandle NativeHandle;

        /**
           Similar to Wrap(), but this is called before the native constructor is called.
           It is rarely needed, but is necessary if one needs to manipulate the JS
           "this" object before the native object is constructed, so that the native ctor
           can access information stored in the JS-side internal fields.

           If this throws a native exception, construction of the
           object will fail and Unwrap() is called, passed
           (jsSelf,NULL), to clean up any data which this function might have
           stored in jsSelf.

           The argv object is the arguments passed to the constructor.

           The default implementation does nothing.
        */
        static void PreWrap( v8::Persistent<v8::Object> const &, v8::Arguments const & )
        {
            return;
        }


        /**
           This operation is called one time from ClassCreator for each
           new object, directly after the native has been connected to
           a Persistent handle.
   
           Note that the ClassCreator code which calls this has already
           taken care of connecting nativeSelf to jsSelf. Client
           specializations of this policy may opt to add their own
           binding mechanisms, e.g. to allow CastToJS<T>() to work.

           Clients should do any bindings-related cleanup in
           Factory::Destruct() or Unwrap(), as appropriate for their
           case.

           Ownership of the objects is unchanged by calling this.

           On error, this function may throw a native exception. If
           that happens, ClassCreator will call
           Unwrap(jsSelf,nativeHandle) and
           Factory<T>::Destruct(nativeSelf) to clean up, and will then
           propagate the exception.

           The default implementation does nothing.
        */
        static void Wrap( v8::Persistent<v8::Object> const &, NativeHandle )
        {
            return;
        }
        
        /**
           This is called from the ClassCreator-generated destructor,
           just before the native destructor is called. If nativeSelf
           is NULL then it means that native construction failed,
           but implementations must (if necessary) clean up any data
           stored in jsSelf by the PreWrap() function.
        
           Specializations may use this to clean up data stored in
           other internal fields of the object (_not_ the field used
           to hold the native itself - that is removed by the
           framework). Optionally, such cleanup may be done in the
           corresponding Factory::Destruct() routine, and must be done
           there if the dtor will need access to such data.

           Note that when this is called, jsSelf and nativeSelf are
           about to be destroyed, so do not do anything crazy with the
           contents of jsSelf and DO NOT destroy nativeSelf (that is
           the job of the ClassCreator_Factory policy).

           Ownership of the objects is unchanged by calling this.

           Unwrap() is called during destruction or when construction
           fails (via a native exception), so any cleanup required for
           the jsSelf object can be delegated to this function, as
           opposed to being performed (and possibly duplicated) in
           PreWrap() and/or Wrap().
           
           The default implementation does nothing.
        */
        static void Unwrap( v8::Handle<v8::Object> const &, NativeHandle )
        {
            return;
        }
    };


#if 0
    namespace Detail
    {
        template <typename Context>
        struct SharedType : public Context
        {
        private:
            SharedType(){}
        public:
            static SharedType & Instance()
            {
                static SharedType bob;
                return bob;
            }
        };
    }
#endif
    /**
       A basic Native-to-JS class binding mechanism. This class does
       not aim to be a monster framework, just something simple,
       mainly for purposes of showing (and testing) what the core
       cvv8 can do.

       The framework must know how to convert JS objects to T objects,
       and for this to work client code must define a JSToNative<T>
       specialization in this manner:

       @code
           template <>
           struct JSToNative<T>
           : JSToNative_ClassCreator<T>
           {};
       @endcode

       If the internal field configuration must be customized then the
       client must define the number of fields by
       specializing/customizing the ClassCreator_InternalFields<T>
       policy class. Additionally, if the client does NOT use the
       above JSToNative<T> implementation then he should create his
       implementation by subclassing
       JSToNative_ObjectWithInternalFields<T,N,M>, where (N,M) are the
       number of internals fields and the index of the field where the
       native object is to be stored. See JSToNative_ClassCreator
       for an example.
       
       TODOs:

       - Certain operations may not work properly when inheriting
       bound classes from JS space, and possibly not even when
       inheriting bound natives from one another. That depends on
       several factors too complex to summarize here.
       
       - See how much of the v8::juice::cw::ClassWrap 
       inheritance-related code we can salvage for re-use here.
       
       - There are known problems when trying to bind inherited methods
       when the parent class has no bound them to JS. i'm not sure how
       i can fix the templates to get this working.
    */
    template <typename T>
    class ClassCreator
    {
    private:
        typedef ClassCreator_InternalFields<T> InternalFields;
        typedef ClassCreator_WeakWrap<T> WeakWrap;
        typedef ClassCreator_TypeID<T> TypeID;
        v8::Persistent<v8::FunctionTemplate> ctorTmpl;
        v8::Handle<v8::ObjectTemplate> protoTmpl;
        bool isSealed;
        typedef ClassCreator_Factory<T> Factory;
        
        
        /**
           A utility function primarily intended to support various
           ClassCreator policy implementations.

           This function tries to extract a native handle from jo by
           looking in the internal field defined by
           ClassCreator_InternalFields<T>::NativeIndex.  If a native is
           found in that field and it is the same as nh, then jo is
           returned. If none is found, jo's prototype object is searched,
           recursively, until either nh is found in the prototype chain or
           the end of the chain is reached. If a match is found, the JS
           object in which the native was found is returned. This does no
           casting - it only compares by address.

           If nh is not found anywhere in the chain, an empty handle is
           returned.

           Note that T must be non-cv qualified, so it is generally
           undesirable to allow the compiler to deduce its type from the
           parameter. Thus the T template parameter should not be omitted
           from calls to this function.
        */
        static v8::Handle<v8::Object> FindHolder( v8::Handle<v8::Object> const & jo,
                                                  T const * nh )
        {
            if( !nh || jo.IsEmpty() ) return v8::Handle<v8::Object>();
            v8::Handle<v8::Value> proto(jo);
            void const * ext = NULL;
            typedef ClassCreator_SearchPrototypeForThis<T> SPFT;
            while( !ext && !proto.IsEmpty() && proto->IsObject() )
            {
                v8::Local<v8::Object> const & obj( v8::Object::Cast( *proto ) );
                ext = (obj->InternalFieldCount() != InternalFields::Count)
                    ? NULL
                    : obj->GetPointerFromInternalField( InternalFields::NativeIndex );
                // FIXME: if InternalFields::TypeIDIndex>=0 then also do a check on that one.
                /*
                    If !ext, there is no bound pointer. If (ext &&
                    (ext!=nh)) then there is one, but it's not the droid
                    we're looking for. In either case, (possibly) check the
                    prototype...
                */
                if( ext == nh ) return obj;
                else if( !SPFT::Value ) break;
                else proto = obj->GetPrototype();
            }
            return v8::Handle<v8::Object>();
        }
        
        static void weak_dtor( v8::Persistent< v8::Value > pv, void *nobj )
        {
            using namespace v8;
            //std::cerr << "Entering weak_dtor<>(native="<<(void const *)nobj<<")\n";
            Local<Object> jobj( Object::Cast(*pv) );
            typedef typename JSToNative<T>::ResultType NT;
            NT native = CastFromJS<T>( pv );
            if( !native )
            {
                /* see: http://code.google.com/p/v8-juice/issues/detail?id=27

                When i call pv.Dispose(), this function is getting called twice,
                and the second time won't work. i'm going to igore (return w/o
                side-effects) this for now for the sake of avoiding a crash
                which i'm seeing only on 64-bit platforms.

                However, even if i return here, v8 is crashing with a
                !NEAR_DEATH assertion right after the second call is made.

                The extra pair of Dispose()/Clear() calls seems to eliminate that
                crash, but the fact that this code block is hit AT ALL is a
                sign of a problem - the dtor shouldn't be called twice!
                */
                pv.Dispose();
                pv.Clear();
#if 1 /* i believe this problem was fixed. If you are reading this b/c
         you followed an assert() message, please report this as a bug.
        */
                assert( 0 && "weak_dtor() got no native object!");
#endif
                return;
            }
            else
            {
                /**
                   Reminder: the FindHolder() bits are here to
                   assist when the bound native exists somewhere in the
                   prototype chain other than jobj itself. In that case,
                   jobj is valid but we cannot clear out the native handle
                   internal field on it because it has no internal fields
                   (or none that belong to us).

                   To fix this properly we have to be able to know
                   _exactly_ which JS object in the prototype chain nh is
                   bound to.
                */
                v8::Handle<v8::Object> nholder = FindHolder( jobj, native );
#if 1 /* reminder: i've never actually seen this error happen, i'm just pedantic about checking... */
                assert( ! nholder.IsEmpty() );
                WeakWrap::Unwrap( nholder /*jobj? subtle difference!*/, native );
                if( nholder.IsEmpty() || (nholder->InternalFieldCount() != InternalFields::Count) )
                {
                    StringBuffer msg;
                    msg << "SERIOUS INTERNAL ERROR:\n"
                        << "ClassCreator<T>::weak_dtor() "
                        << "validated that the JS/Native belong together, but "
                        << "FindHolder() returned an "
                        << (nholder.IsEmpty() ? "empty" : "invalid")
                        << " handle!\n"
                        << "From JS=@"<<(void const *)nobj
                        << ", Converted to Native=@"<<(void const *)native
                        << ", nholder field count="<<nholder->InternalFieldCount()
                        << ", jobj field count="<<jobj->InternalFieldCount()
                        << "\nTHIS MAY LEAD TO A CRASH IF THIS JS HANDLE IS USED AGAIN!!!\n"
                        ;
                    Factory::Delete(native);
                    pv.Dispose(); pv.Clear(); /* see comments below!*/
                    v8::ThrowException(msg.toError());
                    return;
                }
                else
                {
                    nholder->SetInternalField( InternalFields::NativeIndex, Null() );
                    if( 0 <= InternalFields::TypeIDIndex )
                    {
                        nholder->SetInternalField( InternalFields::TypeIDIndex, Null() );
                    }
                    Factory::Delete(native);
                }
#else
                WeakWrap::Unwrap( nholder, native );
                nholder->SetInternalField( InternalFields::NativeIndex, Null() );
                if( 0 <= InternalFields::TypeIDIndex )
                {
                    nholder->SetInternalField( InternalFields::TypeIDIndex, Null() );
                }
                Factory::Delete(native);
#endif
            }
            /*
              According to the v8 gurus i need to call pv.Dispose()
              instead of pv.Clear(), but if i do then this dtor is
              being called twice. If i don't call it, v8 is crashing
              sometime after this function with a !NEAR_DEATH
              assertion.
            */
            pv.Dispose();
            pv.Clear();
        }

        /**
           Gets installed as the NewInstance() handler for T.
         */
        static v8::Handle<v8::Value> ctor_proxy( v8::Arguments const & argv )
        {
            using namespace v8;
            if(ClassCreator_AllowCtorWithoutNew<T>::Value)
            {
                /**
                   Allow construction without 'new' by forcing this
                   function to be called in a ctor context...
                */
                if (!argv.IsConstructCall()) 
                {
                    const int argc = argv.Length();
                    Handle<Function> ctor( Function::Cast(*argv.Callee()));
                    std::vector< Handle<Value> > av(static_cast<size_t>(argc),Undefined());
                    for( int i = 0; i < argc; ++i ) av[i] = argv[i];
                    return ctor->NewInstance( argc, &av[0] );
                }
            }
            else
            {
                /**
                   Why have this limitation? If we don't, v8 pukes
                   when the ctor is called, with
                   "v8::Object::SetInternalField() Writing internal
                   field out of bounds".
                */
                if (!argv.IsConstructCall()) 
                {
                    return Toss("This constructor cannot be called as function!");
                }
            }
            Local<Object> const & jobj( argv.This()
                                        /*CastToJS<T>(*nobj)
                                          
                                        We are not yet far enough
                                        along in the binding that
                                        CastToJS() can work. And it
                                        can't work for the generic
                                        case, anyway.
                                        */);
            if( jobj.IsEmpty() ) return jobj /* assume exception*/;
            Persistent<Object> self( Persistent<Object>::New(jobj) );
            T * nobj = NULL;
            try
            {
                WeakWrap::PreWrap( self, argv  );
                nobj = Factory::Create( self, argv );
                if( ! nobj )
                {
                    return CastToJS<std::exception>(std::runtime_error("Native constructor failed."));
                }
                WeakWrap::Wrap( self, nobj );
                self.MakeWeak( nobj, weak_dtor );
                if( 0 <= InternalFields::TypeIDIndex )
                {
                    self->SetPointerInInternalField( InternalFields::TypeIDIndex, (void *)TypeID::Value );
                }
                self->SetPointerInInternalField( InternalFields::NativeIndex, nobj )
                    /* We do this after the call to Wrap() just in case the Wrap() impl
                       accidentally writes to this field. In that case we end up
                       losing the data they stored there. So this is just as evil as
                       adding the internal field before Wrap(), but only when the
                       client mis-uses the internal fields.
                    */
                    ;
            }
            catch(std::exception const &ex)
            {
                WeakWrap::Unwrap( self, nobj );
                if( nobj ) Factory::Delete( nobj );
                self.Clear();
                return Toss(CastToJS(ex));
            }
            catch(...)
            {
                WeakWrap::Unwrap( self, nobj );
                if( nobj ) Factory::Delete( nobj );
                self.Clear();
                return Toss("Native constructor threw an unknown exception!");
            }
            return self;
        }

        ClassCreator()
            : ctorTmpl(v8::Persistent<v8::FunctionTemplate>::New( v8::FunctionTemplate::New(ctor_proxy) )),
              protoTmpl(v8::Persistent<v8::ObjectTemplate>::New( ctorTmpl->PrototypeTemplate() )),
              isSealed(false)
        {
            ctorTmpl->InstanceTemplate()->SetInternalFieldCount(InternalFields::Count);
        }
    public:
        /**
           The native type being bound to JS.
        */
        typedef typename tmp::PlainType<T>::Type Type;

        /**
           Returns the shared instance of this class.
        */
        static ClassCreator & Instance()
        {
            static ClassCreator bob;
            return bob;
        }

        /**
           Returns this class' prototype object.
        */
        inline v8::Handle<v8::ObjectTemplate> Prototype()
        {
            return this->protoTmpl;
        }

        /**
           Returns this class' constructor template object.
        */
        inline v8::Handle<v8::FunctionTemplate> CtorTemplate()
        {
            return this->ctorTmpl;
        }

        /**
           Returns this class' constructor template.

           ACHTUNG: after this is called, changes made to the Prototype()
           object might not have any effect. Thus this should only be
           called after the prototype object has been fully set up.
           (i have no idea why v8 behaves this way.)

           After calling this, IsSealed() will return true.
        */
        inline v8::Handle<v8::Function> CtorFunction()
        {
            // In my experience, if GetFunction() is called BEFORE setting up
            // the Prototype object, v8 gets very unhappy (class member lookups don't work?).
            this->isSealed = true;
            return this->ctorTmpl->GetFunction();
        }

        /**
           Returns true if CtorFunction() has been called. See that
           function for why.
        */
        inline bool IsSealed() const
        {
            return this->isSealed;
        }

        /**
           Creates a new instanced of the object via the JS API. It calls
           ClassCreator_Factory<T>::Create(), passing it argv, to
           instantiate the object. On success a JS handle to the object is
           returned (it is owned by v8), and the caller can get the native
           pointer with:

           @code
           T * t = CastFromJS<T>(theHandle);
           @endcode
        */
        inline v8::Handle<v8::Object> NewInstance( int argc, v8::Handle<v8::Value> argv[] )
        {
            return this->CtorFunction()->NewInstance(argc, argv);
        }

        /**
            A convenience form of NewInstance() which returns the JS version
            of the object and assigns tgt to the native pointer (which will
            be NULL on error).

            If tgt is NULL when this function returns, or
            returnedObj.IsEmpty(), then we assume that a v8 exception is
            propagating, and the caller should return to v8 as soon as
            possible so the exception can be triggered JS-side (it is not
            actually triggered until we return to v8).

            The returned object is owned by v8.
        */
        v8::Handle<v8::Object> NewInstance( int argc, v8::Handle<v8::Value> argv[], T * & tgt )
        {
            v8::Handle<v8::Object> const & obj( this->CtorFunction()->NewInstance(argc, argv) );
            if( obj.IsEmpty() ) return obj /* assume exception is propagating. */;
            else
            {
                tgt = CastFromJS<T>(obj);
                if( !tgt ) {
                    Toss(StringBuffer()<<"Internal error: NewInstance() returned a non-empty "
                        << "Handle but CastFromJS<"<<TypeName<T>::Value<<">() failed. "
                        << "This is either a serious cvv8 bug or the JSToNative specialization "
                        << "is not working properly.");
                    return v8::Handle<v8::Object>();
                }
                else return obj;
            }
        }

        /**
           Convenience method to add the given property to the
           prototype. Returns this object, for call chaining.

           CastToJS<ValueT>(val) must be valid or a compile-time
           error will be triggered.
        */
        template <typename ValueT>
        inline ClassCreator & Set( char const * name, ValueT val )
        {
            this->protoTmpl->Set(v8::String::New(name), CastToJS(val));
            return *this;
        }
        //! Not quite sure why i need this overload, but i do.
        inline ClassCreator & Set( char const * name, v8::InvocationCallback val )
        {
            this->protoTmpl->Set(v8::String::New(name), CastToJS(val));
            return *this;
        }
        /**
           Equivalent to Set().
        */
        template <typename ValueT>
        inline ClassCreator & operator()( char const * name, ValueT val )
        {
            return this->Set(name, val);
        }
        /**
           Overload to avoid an ambiguity.
        */
        inline ClassCreator & operator()( char const * name, v8::InvocationCallback val )
        {
            return this->Set(name, val);
        }

        /**
           Adds CtorFunction() to dest using the given property name.
           This implicitly "seals" the class (see CtorFunction() for
           details).
        */
        inline void AddClassTo( char const * thisClassName, v8::Handle<v8::Object> const & dest )
        {
            dest->Set(v8::String::New(thisClassName),
                      this->CtorFunction());
        }

        /**
           Destroys the given object by disconnecting its associated
           native object and calling the native destructor function
           for it.

           If jo cannot be converted to a T then false is
           returned. Otherwise the true is returned and the native
           object referenced by jo is no longer valid (it should not
           be used by JS code).

           Native functions bound to that object should take care to
           bail out with an exception once the native pointer is gone,
           as opposed to blindly stepping on its null/dangling pointer
           (which _might_ have been re-allocated to a different
           object, even of a different type, in the mean time).
        */
        static bool DestroyObject( v8::Handle<v8::Object> const & jo )
        {
            T * t = CastFromJS<T>(jo);
            if( ! t ) return false;
            else
            {
                v8::Persistent<v8::Object> p( v8::Persistent<v8::Object>::New( jo ) );
                p.ClearWeak(); // avoid a second call to weak_dtor() via gc!
                weak_dtor( p, t );
                return true;
            }
        }
        /**
           If jv is empty or !jv->IsObject() then false is returned,
           otherwise it returns the result of
           DestroyObject(Handle<Object>).
        */
        static bool DestroyObject( v8::Handle<v8::Value> const & jv )
        {
            return (jv.IsEmpty() || !jv->IsObject())
                ? false
                : DestroyObject( v8::Handle<v8::Object>( v8::Object::Cast(*jv) ) );
        }

        /**
           A v8::InvocationCallback implementation which calls
           DestroyObject( argv.This() ).

           It is intended to be used as a "manual destructor" for
           classes which need it. The canonical examples are
           Stream.close() and Database.close().

           This function is not called DestroyObject to avoid name
           collisions during binding using Set(...,DestroyObjectCallback).
        */
        static v8::Handle<v8::Value> DestroyObjectCallback( v8::Arguments const & argv )
        {
                return DestroyObject(argv.This()) ? v8::True() : v8::False();
        }

        /**
            Tells v8 that this bound type inherits ParentType. 
            ParentType _must_ be a class wrapped by ClassCreator. 
            This function throws if 
            ClassCreator<ParentType>::Instance().IsSealed() returns 
            false). We require that the parent class be sealed to 
            avoid accidental mis-use caused by registering a 
            subclass of a class which has not yet been bound (and may
            may never be bound).
        */
        template <typename ParentType>
        void Inherit()
        {
            typedef ClassCreator<ParentType> PT;
            PT & p(PT::Instance());
            if( ! p.IsSealed() )
            {
                throw std::runtime_error("ClassCreator<ParentType> has not been sealed yet!");
            }
            this->CtorTemplate()->Inherit( p.CtorTemplate() );
        }

        /**
            Simply runs ClassCreator_SetupBindings<T>::Initialize( target ).
            It is provided here to simplify the client-side interface.
        */
        static void SetupBindings( v8::Handle<v8::Object> const & target )
        {
            ClassCreator_SetupBindings<T>::Initialize( target );
        }
        
    };

    /**
        Intended to be the base class for JSToNative<T> specializations
        when T is JS-bound using ClassCreator.

        This particular implementation must be defined _after_
        any of the following policies are customized for T:

        - ClassCreator_InternalFields
        - ClassCreator_SearchPrototypeForThis
        - ClassCreator_TypeID (only if TypeSafe is true!)

        If the client will not specialize those types type then the order is
        irrelevant, but when specializing any of them, they must come before
        this JSToNative implementation is instantiated.

        If TypeSafe is true then this type is a proxy for
        JSToNative_ObjectWithInternalFieldsTypeSafe, else it is a proxy for
        JSToNative_ObjectWithInternalFields. Note that ClassCreator is
        hard-wired to implant/deplant type id information if
        ClassCreator_InternalFields<T>::TypeIDIndex is not negative, with the
        _hope_ that JSToNative<T> will use it, but it does not enforce that
        the type ID is used. For types where the internal fields' TypeIDIndex
        is negative, ClassCreator will not set up bits for the type check,
        which means a slightly smaller runtime memory footprint.
    */
    template <typename T, bool TypeSafe = ClassCreator_InternalFields<T>::TypeIDIndex >= 0 >
    struct JSToNative_ClassCreator :
        tmp::IfElse< TypeSafe,
            JSToNative_ObjectWithInternalFieldsTypeSafe<T,
                                            ClassCreator_TypeID<T>::Value,
                                            ClassCreator_InternalFields<T>::Count,
                                            ClassCreator_InternalFields<T>::TypeIDIndex,
                                            ClassCreator_InternalFields<T>::NativeIndex,
                                            ClassCreator_SearchPrototypeForThis<T>::Value
                                            >,
            JSToNative_ObjectWithInternalFields<T,
                                            ClassCreator_InternalFields<T>::Count,
                                            ClassCreator_InternalFields<T>::NativeIndex,
                                            ClassCreator_SearchPrototypeForThis<T>::Value
                                            >            
        >::Type
    {
    };

#if 0
    //! Experimental.
    template <typename ParentT, typename SubT >
    struct JSToNative_ClassCreator_Subclass
    {
        typedef typename TypeInfo<SubT>::NativeHandle ResultType;
        ResultType operator()( v8::Handle<v8::Value> const & h ) const
        {
            typedef typename TypeInfo<ParentT>::NativeHandle PTP;
            PTP typeCheck; typeCheck = (ResultType)NULL
                /* If compiler errors led you here then SubT probably does not
                    publicly subclass ParentT. */
                ;
            PTP p = CastFromJS<ParentT>(h);
            //std::cerr << "dyncast="<<dynamic_cast<ResultType>(p)<<"\n";
            return p ? dynamic_cast<ResultType>(p) : NULL; 
        }
    };
#endif

#if !defined(DOXYGEN)
    namespace Detail
    {
        /**
           A base class for ClassCreator_Factory_CtorArityDispatcher.
           We don't really need this level of indirection, i think.
        */
        template <typename T>
        struct Factory_CtorForwarder_Base
        {
            typedef typename TypeInfo<T>::Type Type;
            typedef typename TypeInfo<T>::NativeHandle NativeHandle;
            static void Delete( NativeHandle nself )
            {
                delete nself;
            }
        protected:
            /**
               If argv.Length() >= Arity then this function ignores errmsg and
               returns true, otherwise it writes a descriptive error message
               to errmsg and return false.
            */
            static bool argv_check( v8::Arguments const & argv, int Arity )
            {
                if( argv.Length() >= Arity ) return true;
                else
                {
                    StringBuffer msg;
                    msg << "constructor requires " << Arity << " arguments!";
                    throw std::range_error(msg.Content().c_str());
                    return false;
                }
            }
        };
    }
#endif // !DOXYGEN

    /**
        Can be used as a concrete ClassCreator_Factor<T> 
        specialization to forward JS ctor calls directly to native 
        ctors.
    
        T must be the ClassCreator'd type to construct. CtorProxy must
        be a type having this interface:
        
        @code
        TypeInfo<T>::NativeHandle Call( v8::Arguments const & );
        @endcode

        Normally CtorProxy would be CtorForwarder or CtorArityDispatcher,
        but any interface-compatible type will do.

        It must return a new object instance on success. On error it
        may return NULL and "should" throw a native exception explaining
        the problem. The exception will be caught by ClassCreator and
        transformed into a JS-side exception.

        If CtorProxy::Call() succeeds (returns non-NULL and does not throw)
        then NativeToJSMap<T> is used to create a native-to-JS mapping.
        To make use of this, the client should do the following:
        
        @code
        // in the cvv8 namespace:
        template <>
            struct NativeToJS<T> : NativeToJSMap<T>::NativeToJSImpl {};
        @endcode
        
        After that, CastToJS<T>( theNativeObject ) can work.
        
        The mapping is cleaned up when (if!) the object is sent through
        the JS garbage collector or the client somehow triggers its
        JS-aware destruction (e.g. via ClassCreator::DestroyObject(),
        assuming the type was wrapped using ClassCreator).
    */
    template <typename T, typename CtorProxy>
    struct ClassCreator_Factory_NativeToJSMap : Detail::Factory_CtorForwarder_Base<T>
    {
    public:
        typedef NativeToJSMap<T> N2JMap;
        typedef typename TypeInfo<T>::Type Type;
        typedef typename TypeInfo<T>::NativeHandle NativeHandle;
        
        /**
            If CtorProxy::Call(argv) succeeds, N2JMap::Insert(jself, theNative)
            is called. The result of CtorProxy::Call() is returned.
        */
        static NativeHandle Create( v8::Persistent<v8::Object> jself, v8::Arguments const &  argv )
        {
            NativeHandle n = CtorProxy::Call( argv );
            if( n ) N2JMap::Insert( jself, n );
            return n;
        }
        /**
            Calls N2JMap::Remove( nself ) then (delete nself).
        */
        static void Delete( NativeHandle nself )
        {
            N2JMap::Remove( nself );
            delete nself;
        }
    };

    /** @deprecated Use ClassCreator_Factory_Dispatcher instead (same interface).
    */
    template <typename T,typename CtorForwarderList>
    struct ClassCreator_Factory_CtorArityDispatcher : Detail::Factory_CtorForwarder_Base<T>
    {
    public:
        typedef typename TypeInfo<T>::Type Type;
        typedef typename TypeInfo<T>::NativeHandle NativeHandle;
        static NativeHandle Create( v8::Persistent<v8::Object> , v8::Arguments const &  argv )
        {
            typedef CtorArityDispatcher<CtorForwarderList> Proxy;
            return Proxy::Call( argv );
        }
    };

    /**
        A ClassCreator_Factory implementation which forwards its Create()
        member to CtorT::Call() (the interface used by CtorForwarder and friends).

        T must (or is assumed to) be a ClassCreator<T>-wrapped class.
        CtorForwarderList must be a Signature typelist of CtorForwarder
        types and its "return type" must be T (optionally pointer-qualified).
        
        Example:
        
        @code
        typedef CtorForwarder<MyType *()> C0;
        typedef CtorForwarder<MyType *(int)> C1;
        typedef CtorForwarder<MyType *(int, double)> C2;
        typedef Signature< CFT (C0, C1, C2) > CtorList;
        
        // Then create Factory specialization based on those:
        template <>
        struct ClassCreator_Factory<MyType> : 
            ClassCreator_Factory_Dispatcher<MyType, CtorArityDispatcher<CtorList> > {};
        @endcode

        Or:
        
        @code
        template <>
        struct ClassCreator_Factory<MyType> :
            ClassCreator_Factory_Dispatcher< MyType, CtorForwarder<MyType *()> >
        {};
        @endcode
    */
    template <typename T,typename CtorT>
    struct ClassCreator_Factory_Dispatcher : Detail::Factory_CtorForwarder_Base<T>
    {
    public:
        typedef typename TypeInfo<T>::Type Type;
        typedef typename TypeInfo<T>::NativeHandle NativeHandle;
        static NativeHandle Create( v8::Persistent<v8::Object> jself, v8::Arguments const &  argv )
        {
            return CtorT::Call( argv );
        }
    };


}// namespaces

#endif /* CODE_GOOGLE_COM_P_V8_CONVERT_CLASS_CREATOR_HPP_INCLUDED */
