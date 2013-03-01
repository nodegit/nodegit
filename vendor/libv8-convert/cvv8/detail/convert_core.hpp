#if !defined(CODE_GOOGLE_COM_P_V8_CONVERT_CORE_HPP_INCLUDED)
#define CODE_GOOGLE_COM_P_V8_CONVERT_CORE_HPP_INCLUDED 1
/*
  A min-framework to simplify casting between v8 JavaScript and
  C++ native objects.

  v8: http://code.google.com/p/v8/

  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

  License: Dual MIT/Public Domain
*/

//#include <cstdint> // arg! Requires C++0x!
#include <v8.h>
#include <stdint.h> // hope the client's platform is recent!
#include <string>
#include <cstring>
#include <list>
#include <vector>
#include <map>
#include <stdexcept>
#include <sstream>

#if !defined(CVV8_CONFIG_HAS_LONG_LONG)
/* long long in C++ requires C++0x or compiler extensions. */
#  define CVV8_CONFIG_HAS_LONG_LONG 0
#endif

#include "signature_core.hpp" /* only needed for the Signature used by the generated code. */
#include "tmp.hpp"

namespace cvv8 {


    /**
       A convenience base type for TypeInfo<T> specializations.
    */
    template <typename T, typename NHT = T *>
    struct TypeInfoBase
    {
        /**
           The unqualified type T. In some special cases, Type _may_
           differ from T.
        */
        typedef T Type;
        /**
           The "handle" type used to pass around native objects
           between the JS and Native worlds.

           In _theory_ we can also use shared/smart pointers with 
           this typedef, but that requires custom handling in other 
           template code (mainly because we cannot store a 
           full-fledged shared pointer object directly inside a JS 
           object).           
        */
        typedef NHT NativeHandle;

        // MAYBE to do: add a function to get a pointer to the object, e.g.
        // for dereferencing smart pointers. So far it's not been necessary.
        // static NativeHandle Pointer( NativeHandle x ) { return x; }
    };

    /**
        This may optionally be specialized for client-defined types
        to define the type name used by some error reporting
        code.
        
        The default implementation is usable but not all that useful.
    */
    template <typename T>
    struct TypeName
    {
        /** Specializations must have a non-NULL value, and any number
            of specializations may legally have the same value.
        */
        static char const * Value;
    };
#if 0
    /**
        This default implementation is unfortunate, but quite a bit 
        of error-reporting code uses this, with the assumption that 
        if it was worth binding to JS then it's worth having a 
        useful type name in error strings.
        
        FIXME: remove the default specialization if we can, as it will
        cause us problems with some planned/potential uses which cross
        DLL boundaries (there may be multiple definitions with different
        addresses).
    */
    template <typename T>
    char const * TypeName<T>::Value = "T";
#endif

    /** @def CVV8_TypeName_DECL

        A convenience macro for declaring a TypeName specialization. X must
        be a type name with extra wrapping parenthesis, e.g.:

        @code
        CVV8_TypeName_DECL((MyType));
        @endcode

        They are required so that we can also support template types with commas
        in the names, e.g. (std::map<int,double>).

        It must be called from inside the cvv8 namespace.
    */
#define CVV8_TypeName_DECL(X) template <> struct TypeName< cvv8::sl::At<0,CVV8_TYPELIST(X) >::Type > \
    { const static char * Value; }

    /** @def CVV8_TypeName_IMPL

        The counterpart of CVV8_TypeName_DECL, this must be called from the
        cvv8 namespace. The X argument is as documented for CVV8_TypeName_DECL
        and the NAME argument must be a C string.

        Example:

        @code
        CVV8_TypeName_IMPL((MyType),"MyType");
        @endcode
    */
#define CVV8_TypeName_IMPL(X,NAME) char const * TypeName< cvv8::sl::At<0,CVV8_TYPELIST(X) >::Type >::Value = NAME
    /** @def CVV8_TypeName_IMPL2
    
        Almost identical to CVV8_TypeName_IMPL(), but can be used without
        having first explicitly specializing TypeName.
    */
#define CVV8_TypeName_IMPL2(X,NAME) template <> CVV8_TypeName_IMPL(X,NAME)


#if 1
    template <typename T>
    struct TypeName<T *> : TypeName<T> {};
    template <typename T>
    struct TypeName<T const> : TypeName<T> {};
    template <typename T>
    struct TypeName<T const *> : TypeName<T> {};
#endif

    /**
       Describes basic type information regarding a type, for purposes
       of static typing during JS-to/from-Native conversions.

       The default instantiation is suitable for most
       cases. Specializations may be required in certain JS/Native
       binding cases.
    */
    template <typename T>
    struct TypeInfo : TypeInfoBase<T,T *>
    {
    };

    template <typename T>
    struct TypeInfo<T const> : TypeInfo<T> {};
    
    template <typename T>
    struct TypeInfo<T *> : TypeInfo<T> {};

    template <typename T>
    struct TypeInfo<T const *> : TypeInfo<T> {};

    template <typename T>
    struct TypeInfo<T const &> : TypeInfo<T> {};

    template <typename T>
    struct TypeInfo<T &> : TypeInfo<T> {};

#if 1 // this will only theoretically do what i want. Never actually tried to use a non-pointer NativeHandle type...
    template <typename T>
    struct TypeInfo< v8::Handle<T> >
    {
        typedef v8::Handle<T> Type;
        typedef v8::Handle<T> NativeHandle;
    };
#endif

    /**
       Base instantiation for T-to-v8::Handle<v8::Value> conversion functor.
       Must be specialized or it will not compile.
    */
    template <typename NT>
    struct NativeToJS
    {
        /**
            Must be specialized. The argument type may be pointer-qualified.
            Implementations for non-pod types are encouraged to have two
            overloads, one taking (NT const &) and one taking (NT const *),
            as this gives the underlying CastToJS() calls more leeway.
        **/
        template <typename X>
        v8::Handle<v8::Value> operator()( X const & ) const;
    private:
        typedef tmp::Assertion<false> NativeToJSMustBeSpecialized;
    };

    /**
       Specialization to treat (NT*) as (NT).

       NativeToJS<NT> must have an operator() taking
       (NT const *).
    */
    template <typename NT>
    struct NativeToJS<NT *> : NativeToJS<NT>
#if 1
    {};
#else
    {
        v8::Handle<v8::Value> operator()( NT const * v ) const
        {
            typedef NativeToJS<NT> Proxy;
            if( v ) return Proxy()(v);
            else return v8::Null()
            ;
        }
    };
#endif
    /**
       Specialization to treat (NT const *) as (NT).
    */
    template <typename NT>
    struct NativeToJS<NT const *> : NativeToJS<NT *> {};
    //     {
    //         typedef typename TypeInfo<NT>::Type const * ArgType;
    // 	v8::Handle<v8::Value> operator()( ArgType n ) const
    //         {
    //             typedef NativeToJS<NT*> Proxy;
    //             return Proxy()( n );
    //         }
    //     };

    /**
       Specialization to treat (NT const &) as (NT).
    */
    template <typename NT>
    struct NativeToJS<const NT &> : NativeToJS<NT> {};
    template <typename NT>
    struct NativeToJS<const NT> : NativeToJS<NT> {};

#if 0
    /**
       Specialization to treat (NT &) as (NT).
    */
    template <typename NT>
    struct NativeToJS<NT &> : NativeToJS<NT> {};
#else
    /**
       A specialization to convert from (T&) to JS.

       Be very careful with this, and make sure that
       NativeToJS<NT*> has its own specialization,
       as this implementation uses that one as its
       basis.
    */
    template <typename NT>
    struct NativeToJS<NT &>
    {
        typedef typename TypeInfo<NT>::Type & ArgType;
        v8::Handle<v8::Value> operator()( ArgType n ) const
        {
            typedef NativeToJS< typename TypeInfo<NT>::NativeHandle > Cast;
            return Cast()( &n );
        }
    };
#endif
    
#if 0
    template <>
    struct NativeToJS<void>
    {
        /**
           Returns v8::Undefined().
        */
        template <typename Ignored>
        v8::Handle<v8::Value> operator()(Ignored const &) const
        {
            return ::v8::Undefined();
        }
    };
#endif

#if !defined(DOXYGEN)
    namespace Detail {
        /**
           Base implementation for "small" integer conversions (<=32
           bits).
        */
        template <typename IntegerT>
        struct NativeToJS_int_small
        {
            v8::Handle<v8::Value> operator()( IntegerT v ) const
            {
                return v8::Integer::New( static_cast<int32_t>(v) );
            }
        };
        /**
           Base implementation for "small" unsigned integer conversions
           (<=32 bits).
        */
        template <typename IntegerT>
        struct NativeToJS_uint_small
        {
            v8::Handle<v8::Value> operator()( IntegerT v ) const
            {
                return v8::Integer::NewFromUnsigned( static_cast<uint32_t>(v) );
            }
        };
    }
#endif // if !defined(DOXYGEN)

    template <>
    struct NativeToJS<int16_t> : Detail::NativeToJS_int_small<int16_t> {};

    template <>
    struct NativeToJS<uint16_t> : Detail::NativeToJS_uint_small<uint16_t> {};

    template <>
    struct NativeToJS<int32_t> : Detail::NativeToJS_int_small<int32_t> {};

    template <>
    struct NativeToJS<uint32_t> : Detail::NativeToJS_uint_small<uint32_t> {};

#if !defined(DOXYGEN)
    namespace Detail {
        /**
           Base implementation for "big" numeric conversions (>32 bits).
        */
        template <typename IntegerT>
        struct NativeToJS_int_big
        {
            /** Returns v as a double value. */
            v8::Handle<v8::Value> operator()( IntegerT v ) const
            {
                return v8::Number::New( static_cast<double>(v) );
            }
        };
    }
#endif // if !defined(DOXYGEN)

    template <>
    struct NativeToJS<int64_t> : Detail::NativeToJS_int_big<int64_t> {};

    template <>
    struct NativeToJS<uint64_t> : Detail::NativeToJS_int_big<uint64_t> {};

    template <>
    struct NativeToJS<double>
    {
        v8::Handle<v8::Value> operator()( double v ) const
        {
            return v8::Number::New( v );
        }
    };

    template <>
    struct NativeToJS<bool>
    {
        v8::Handle<v8::Value> operator()( bool v ) const
        {
            return v8::Boolean::New( v );
        }
    };

    template <typename T>
    struct NativeToJS< ::v8::Handle<T> >
    {
        typedef ::v8::Handle<T> handle_type;
        v8::Handle<v8::Value> operator()( handle_type const & li ) const
        {
            return li;
        }
    };

    template <typename T>
    struct NativeToJS< ::v8::Local<T> >
    {
	typedef ::v8::Local<T> handle_type;
        v8::Handle<v8::Value> operator()( handle_type const & li ) const
        {
            return li;
	}
    };

    template <typename T>
    struct NativeToJS< ::v8::Persistent<T> >
    {
        typedef ::v8::Persistent<T> handle_type;
        v8::Handle<v8::Value> operator()( handle_type const & li ) const
        {
            return li;
        }
    };

    template <>
    struct NativeToJS< ::v8::InvocationCallback >
    {
        v8::Handle<v8::Value> operator()( ::v8::InvocationCallback const f ) const
        {
            return ::v8::FunctionTemplate::New(f)->GetFunction();
        }
    };


    template <>
    struct NativeToJS<std::string>
    {
	v8::Handle<v8::Value> operator()( std::string const & v ) const
	{
	    /** This use of v.data() instead of v.c_str() is highly arguable. */
            char const * const cstr  = v.data();
	    return cstr ? v8::String::New( cstr, static_cast<int>( v.size() ) ) : v8::String::New("",0);
	}
    };
	
    template <>
    struct NativeToJS<char const *>
    {
        v8::Handle<v8::Value> operator()( char const * v ) const
        {
            if( ! v ) return v8::Null();
            else return v8::String::New( v );
        }
    };

    /**
       "Casts" v to a JS value using NativeToJS<T>.
    */
    template <typename T>
    inline v8::Handle<v8::Value> CastToJS( T const & v )
    {
        typedef NativeToJS<T const> F;
        return F()( v );
    }

    /**
       Overload to avoid ambiguity in certain calls.
    */
    static inline v8::Handle<v8::Value> CastToJS( char const * v )
    {
        typedef NativeToJS<char const *> F;
        return F()( v );
    }

    /**
       Overload to avoid mis-selection of templates.
    */
    static inline v8::Handle<v8::Value> CastToJS( v8::InvocationCallback v )
    {
        typedef NativeToJS<v8::InvocationCallback> F;
        return F()( v );
    }

    /**
       Overload to avoid ambiguity in certain calls.
    */
    static inline v8::Handle<v8::Value> CastToJS( char * v )
    {
        typedef NativeToJS<char const *> F;
        return F()( v );
    }

    /** Convenience instance of NativeToJS. */
    static const NativeToJS<int16_t> Int16ToJS = NativeToJS<int16_t>();
    /** Convenience instance of NativeToJS. */
    static const NativeToJS<uint16_t> UInt16ToJS = NativeToJS<uint16_t>();
    /** Convenience instance of NativeToJS. */
    static const NativeToJS<int32_t> Int32ToJS = NativeToJS<int32_t>();
    /** Convenience instance of NativeToJS. */
    static const NativeToJS<uint32_t> UInt32ToJS = NativeToJS<uint32_t>();
    /** Convenience instance of NativeToJS. */
    static const NativeToJS<int64_t> Int64ToJS = NativeToJS<int64_t>();
    /** Convenience instance of NativeToJS. */
    static const NativeToJS<uint64_t> UInt64ToJS = NativeToJS<uint64_t>();
    /** Convenience instance of NativeToJS. */
    static const NativeToJS<double> DoubleToJS = NativeToJS<double>();
    /** Convenience instance of NativeToJS. */
    static const NativeToJS<bool> BoolToJS = NativeToJS<bool>();
    /** Convenience instance of NativeToJS. */
    static const NativeToJS<std::string> StdStringToJS = NativeToJS<std::string>();

    /**
       Converts a native std::exception to a JS exception and throws
       that exception via v8::ThrowException().
    */
    template <>
    struct NativeToJS<std::exception>
    {
        /** Returns v8::Exception::Error(ex.what()).
            
            ACHTUNG: on 20110717 this function was changed to NOT trigger a
            JS exception. Prior versions triggered a JS exception, but that
            was done due to a misunderstanding on my part (i didn't know v8
            provided a way to create Error objects without throwing). This
            change is semantically incompatible with older code which uses
            this conversion.

            OLD way of doing it (don't do this!):

            @code
            catch( std::exception const & ex ) { return CastToJS(ex); }
            @endcode

            The equivalent is now:

            @code
            catch( std::exception const & ex ) { return Toss(CastToJS(ex)); }
            @endcode
        */
        v8::Handle<v8::Value> operator()( std::exception const & ex ) const
        {
            char const * msg = ex.what();
            return v8::Exception::Error(v8::String::New( msg ? msg : "unspecified std::exception" ));
            /** ^^^ String::New() internally calls strlen(), which hates it when string==0. */
        }
    };
    template <>
    struct NativeToJS<std::runtime_error> : NativeToJS<std::exception> {};
    template <>
    struct NativeToJS<std::range_error> : NativeToJS<std::exception> {};
    template <>
    struct NativeToJS<std::logic_error> : NativeToJS<std::exception> {};
    
    
    /**
       Base interface for converting from native objects to JS
       objects. There is no default implementation, and it must be
       specialized to be useful.

       When creating custom implementations, remember that in
       practice, all custom client-bound types are passed around
       internally by non-const pointer. This is in contrast to
       conversions of POD- and POD-like types (numbers and strings),
       which are passed around by value.

       It is theoretically possible to use smart pointers (with value
       semantics) via this API, but it is untested.
    */
    template <typename JST>
    struct JSToNative
    {
        typedef typename TypeInfo<JST>::NativeHandle ResultType;
        //! Must be specialized to be useful.
        ResultType operator()( v8::Handle<v8::Value> const & h ) const;
    };
    
    /** Specialization to treat (JST*) as JST. */
    template <typename JST>
    struct JSToNative<JST const> : JSToNative<JST> {};

    /** Specialization to treat (JST*) as JST. */
    template <typename JST>
    struct JSToNative<JST *> : JSToNative<JST> {};

    /** Specialization to treat (JST const *) as (JST *). */
    template <typename JST>
    struct JSToNative<JST const *> : JSToNative<JST> {};

    /**
       A specialization to convert from JS to (T&).
    */
    template <typename JST>
    struct JSToNative<JST &>
    {
        /**
           Uses JSTNative<T*>() to try to convert a JS object to a
           pointer, and then dereferences that pointer to form
           (T&). Beware, however, that this operation throws a native
           exeception (deriving from std::exception) if it fails,
           because the only other other option is has is to
           dereference null and crash your app.
        */
        typedef typename TypeInfo<JST>::Type & ResultType;
        ResultType operator()( v8::Handle<v8::Value> const & h ) const
        {
            typedef JSToNative<JST*> Cast;
            typedef typename Cast::ResultType NH;
            NH n = Cast()( h );
            if( ! n )
            {
                throw std::runtime_error("JSToNative<T&> could not get native pointer. Throwing to avoid dereferencing null!");
            }
            else return *n;
        }
    };

    /** Specialization to treat (JST const &) as (JST). */
    template <typename JST>
    struct JSToNative<JST const &>
    {
        typedef typename TypeInfo<JST>::Type const & ResultType;
        ResultType operator()( v8::Handle<v8::Value> const & h ) const
        {
            typedef JSToNative<JST &> Cast;
            typedef typename Cast::ResultType NH;
#if 0
            NH n = Cast()( h );
            if( ! n )
            {
                throw std::runtime_error("JSToNative<T const &> could not get native pointer. Throwing to avoid dereferencing null!");
            }
            else return *n;
#else
            return Cast()(h);
#endif
        }
    };


    
    /** Specialization which passes on v8 Handles as-is. */
    template <typename T>
    struct JSToNative<v8::Handle<T> >
    {
        typedef v8::Handle<T> ResultType;
        ResultType operator()( v8::Handle<T> const & h ) const
        {
            return h;
        }
    };
    template <typename T>
    struct JSToNative<v8::Handle<T> const &> : JSToNative< v8::Handle<T> > {};
    template <typename T>
    struct JSToNative<v8::Handle<T> &> : JSToNative< v8::Handle<T> > {};

    /** Specialization which passes on v8 local Handles as-is. */
    template <typename T>
    struct JSToNative<v8::Local<T> >
    {
        typedef v8::Local<T> ResultType;
        ResultType operator()( v8::Local<T> const & h ) const
        {
            return h;
        }
    };
    template <typename T>
    struct JSToNative<v8::Local<T> const &> : JSToNative< v8::Local<T> > {};
    template <typename T>
    struct JSToNative<v8::Local<T> &> : JSToNative< v8::Local<T> > {};

#if !defined(DOXYGEN)
    namespace Detail
    {
        template <typename V8Type,bool (v8::Value::*IsA)() const>
        struct JSToNative_V8Type
        {
            /**
                If h is not empty and is of the correct type
                then its converted handle is returned, else
                an empty handle is returned.
            */
            typedef v8::Handle<V8Type> ResultType;
            ResultType operator()( v8::Handle<v8::Value> const & h ) const
            {
                return (h.IsEmpty() || !((*h)->*IsA)()/*man, what a construct!*/)
                ? v8::Handle<V8Type>()
                : v8::Handle<V8Type>(V8Type::Cast(*h));
            }
        };  
    }
#endif
    template <>
    struct JSToNative<v8::Handle<v8::Object> > : Detail::JSToNative_V8Type< v8::Object, &v8::Value::IsObject>
    {};
    template <>
    struct JSToNative< v8::Handle<v8::Object> &> : JSToNative< v8::Handle<v8::Object> > {};
    template <>
    struct JSToNative< v8::Handle<v8::Object> const &> : JSToNative< v8::Handle<v8::Object> > {};


    template <>
    struct JSToNative<v8::Handle<v8::Array> > : Detail::JSToNative_V8Type< v8::Array, &v8::Value::IsArray>
    {};
    template <>
    struct JSToNative< v8::Handle<v8::Array> &> : JSToNative< v8::Handle<v8::Array> > {};
    template <>
    struct JSToNative< v8::Handle<v8::Array> const &> : JSToNative< v8::Handle<v8::Array> > {};

    template <>
    struct JSToNative<v8::Handle<v8::Function> > : Detail::JSToNative_V8Type< v8::Function, &v8::Value::IsFunction>
    {};
    template <>
    struct JSToNative< v8::Handle<v8::Function> &> : JSToNative< v8::Handle<v8::Function> > {};
    template <>
    struct JSToNative< v8::Handle<v8::Function> const &> : JSToNative< v8::Handle<v8::Function> > {};

    /**
       An X-to-void specialization which we cannot use in the generic
       case due to the syntactic limitations of void.
    */
    template <>
    struct JSToNative<void>
    {
        typedef void ResultType;
        ResultType operator()( ... ) const
        {
            return;
        }
    };

    /**
       A very arguable specialization which tries to convert a JS
       value to native (void*) via v8::External.
    */
    template <>
    struct JSToNative<void *>
    {
        typedef void * ResultType;
        /**
            If h is-a External then this return its value, else it
            return 0.
            
            We could arguably check if it is an object and has 
            internal fields, and return the first one, but i think 
            that would be going too far considering how arguably the 
            v8-to-(void *) conversion is in the first place.
        */
        ResultType operator()( v8::Handle<v8::Value> const & h ) const
        {
            if( h.IsEmpty() || ! h->IsExternal() ) return 0;
            return v8::External::Cast(*h)->Value();
        }
    };
    /**
       A very arguable specialization which tries to convert a JS
       value to native (void*) via v8::External.
    */
    template <>
    struct JSToNative<void const *>
    {
        typedef void const * ResultType;
        /**
            If h is-a External then this return its value, else it
            return 0.
            
            We could arguably check if it is an object and has 
            internal fields, and return the first one, but i think 
            that would be going to far considering how arguably the 
            v8-to-(void *) conversion is in the first place.
        */
        ResultType operator()( v8::Handle<v8::Value> const & h ) const
        {
            if( h.IsEmpty() || ! h->IsExternal() ) return 0;
            return v8::External::Cast(*h)->Value();
        }
    };

    /**
       A concrete JSToNative implementation intended to be used as the
       base class for JSToNative<T> implementations where T is "bound
       to JS" in JS objects, and those JS objects meet the following
       requirements:

       - They have exactly InternalFieldCount internal fields.
       
       - They have a C/C++-native object of type (T*) (or convertible
       to type (T*)) stored as a v8::External value in the JS-object
       internal field number InternalFieldIndex.       

       Note that the InternalFieldCount check is only a quick
       sanity-checking mechanism, and is by far not foolproof because
       the majority of JS-bound native types only use 1 internal
       field.

       It is illegal for InternalFieldCount to be equal to or smaller
       than InternalFieldIndex, or for either value to be
       negative. Violating these invariants results in a compile-time
       assertion.
       
       If an object to convert matches the field specification but
       is not a T (or publically derived from T) then results are
       undefined (this could lead to a crash at runtime).
       
       There are cases where very-misbehaving JS code can force this 
       particular conversion algorithm to mis-cast the native. While 
       they have never been seen "in the wild", they can 
       theoretically happen. If that bothers you, and you want to be 
       able to sleep soundly at night, use 
       JSToNative_ObjectWithInternalFieldsTypeSafe instead of this 
       class.
       
       @see JSToNative_ObjectWithInternalFieldsTypeSafe
       
    */
    template <typename T,
              int InternalFieldCount = 1,
              int InternalFieldIndex = 0,
              bool SearchPrototypeChain = false>
    struct JSToNative_ObjectWithInternalFields
    {
    private:
        typedef char AssertIndexRanges
        [tmp::Assertion<
         (InternalFieldIndex>=0)
         && (InternalFieldCount>0)
         && (InternalFieldIndex < InternalFieldCount)
         >::Value
         ? 1 : -1];
    public:
        typedef typename TypeInfo<T>::NativeHandle ResultType;
        /**
           If h is-a Object and it meets the requirements described
           in this class' documentation, then this function returns
           the result of static_cast<ResultType>(void*), using the (void*)
           extracted from the Object. If the requirements are not met
           then NULL is returned.
           
           See the class docs for more details on the requirements
           of the passed-in handle.

           If SearchPrototypeChain is true and this object does not
           contain a native then the prototype chain is searched.
           This is generally only required when bound types are
           subclassed.
        */
        ResultType operator()( v8::Handle<v8::Value> const & h ) const
        {
            if( h.IsEmpty() || ! h->IsObject() ) return NULL;
            else
            {
                void * ext = NULL;
                v8::Handle<v8::Value> proto(h);
                while( !ext && !proto.IsEmpty() && proto->IsObject() )
                {
                    v8::Local<v8::Object> const & obj( v8::Object::Cast( *proto ) );
                    ext = (obj->InternalFieldCount() != InternalFieldCount)
                        ? NULL
                        : obj->GetPointerFromInternalField( InternalFieldIndex );
                    if( ! ext )
                    {
                        if( !SearchPrototypeChain ) break;
                        else proto = obj->GetPrototype();
                    }
                }
                return ext ? static_cast<ResultType>(ext) : NULL;
            }
        }
    };

    /**
        A concrete JSToNative implementation (to be used as a base class)
        which does a type-safe conversion from JS to (T*).
        
        T is the bound native type. A pointer of this type is expected
        in the object-internal field specified by ObjectFieldIndex.
        
        Each value to be converted is checked for an internal field 
        (of type v8::External) at the index specified by 
        TypeIdFieldIndex. If (and only if) that field contains a 
        v8::External which holds the value TypeID is the conversion 
        considered legal. If they match but the native value stored 
        in field ObjectFieldIndex is NOT-a-T then results are 
        undefined. Note that the TypeID value is compared by pointer 
        address, not by its byte contents (which may legally be 
        NULL).
        
        If SearchPrototypeChain is true and the passed-in object 
        contains no native T then the prototype chain is checked 
        recursively. This is normally only necessary if the native 
        type is allowed to be subclasses from JS code (at which point
        the JS 'this' is not the same exact object as the one 
        holding the native 'this' pointer, and we need to search the 
        prototype chain).

        For this all to work: the class-binding mechanism being used 
        by the client must store the TypeID value in all new 
        JS-created instances of T by calling SetInternalField( 
        TypeIdFieldIndex, theTypeID ) and store the native object 
        pointer in the field internal field ObjectFieldIndex. And 
        then they should do:

        @code
        // in the cvv8 namespace:
        template <>
        struct JSToNative<MyType> :
          JSToNative_ObjectWithInternalFieldsTypeSafe<MyType,MyTypeID...>
          {};
        @endcode

        At which point CastFromJS<T>() will take advantage of this
        type check.


        The real docs end here. What follows is pseudorandom blather...

        Theoretically (i haven't tried this but i know how the 
        underlying code works), it is possible for script code to 
        create a condition which will lead to an invalid cast if the 
        check performed by this class (or something equivalent) is 
        _not_ performed:
        
        @code
        var x = new Native1();
        var y = new Native2();
        // assume y.someFunc and x.otherFunc are both bound to native functions
        y.someFunc = x.otherFunc;
        y.someFunc( ... ); // here is where it will likely end badly
        @endcode
        
        When y.someFunc() is called, JS will look for a 'this' object of
        type Native1, not Native2, because the type information related to
        the conversion is "stored" in the callback function itself, not in
        the native object. (This is a side-effect of us using templates to
        create InvocationCallback implementations.) It will not find a
        Native1, but it might (depending on how both classes are bound to
        JS) find a Native2 pointer and _think_ it is a Native1. And by
        "think it is" i mean "it will try to cast it to," but the cast is
        illegal in that case. In any case it would be bad news.
        
        The check performed by this class can catch that condition 
        (and similar ones) and fail gracefully (i.e. returning a 
        NULL instead of performing an illegal cast).
    */
    template <typename T,
              void const * & TypeID,
              int InternalFieldCount = 2,
              int TypeIdFieldIndex = 0,
              int ObjectFieldIndex = 1,
              bool SearchPrototypeChain = false>
    struct JSToNative_ObjectWithInternalFieldsTypeSafe
    {
    private:
        typedef char AssertIndexRanges
        [(InternalFieldCount>=2)
         && (TypeIdFieldIndex != ObjectFieldIndex)
         && (TypeIdFieldIndex >= 0)
         && (TypeIdFieldIndex < InternalFieldCount)
         && (ObjectFieldIndex >= 0)
         && (ObjectFieldIndex < InternalFieldCount)
         ? 1 : -1];
    public:
        typedef typename TypeInfo<T>::NativeHandle ResultType;
        /**
           If h is-a Object and it meets the requirements described
           in this class' documentation, then this function returns
           the result of static_cast<ResultType>(void*), using the (void*)
           extracted from the Object. If the requirements are not met
           then NULL is returned.
           
           See the class docs for more details on the requirements
           of the passed-in handle.

           If SearchPrototypeChain is true and the object does not 
           contain a native then the prototype chain is searched 
           recursively. This is generally only required when bound 
           types are subclassed from JS code.           
        */
        ResultType operator()( v8::Handle<v8::Value> const & h ) const
        {
            if( h.IsEmpty() || ! h->IsObject() ) return NULL;
            else
            {
                void const * tid = NULL;
                void * ext = NULL;
                v8::Handle<v8::Value> proto(h);
                while( !ext && !proto.IsEmpty() && proto->IsObject() )
                {
                    v8::Local<v8::Object> const & obj( v8::Object::Cast( *proto ) );
                    tid = (obj->InternalFieldCount() != InternalFieldCount)
                        ? NULL
                        : obj->GetPointerFromInternalField( TypeIdFieldIndex );
                    ext = (tid == TypeID)
                        ? obj->GetPointerFromInternalField( ObjectFieldIndex )
                        : NULL;
                    if( ! ext )
                    {
                        if( ! SearchPrototypeChain ) break;
                        else proto = obj->GetPrototype();
                    }
                }
                return ext ? static_cast<ResultType>(ext) : NULL;
            }
        }
    };

    /** Specialization to convert JS values to int16_t. */
    template <>
    struct JSToNative<int16_t>
    {
        typedef int16_t ResultType;
        ResultType operator()( v8::Handle<v8::Value> const & h ) const
        {
            return h->IsNumber()
                ? static_cast<ResultType>(h->Int32Value())
                : 0;
        }
    };	

    /** Specialization to convert JS values to uint16_t. */
    template <>
    struct JSToNative<uint16_t>
    {
        typedef uint16_t ResultType;
        ResultType operator()( v8::Handle<v8::Value> const & h ) const
        {
            return h->IsNumber()
                ? static_cast<ResultType>(h->Int32Value())
                : 0;
        }
    };

    /** Specialization to convert JS values to int32_t. */
    template <>
    struct JSToNative<int32_t>
    {
        typedef int32_t ResultType;
        ResultType operator()( v8::Handle<v8::Value> const & h ) const
        {
            return h->IsNumber()
                ? h->Int32Value()
                : 0;
        }
    };

    /** Specialization to convert JS values to uint32_t. */
    template <>
    struct JSToNative<uint32_t>
    {
        typedef uint32_t ResultType;
        ResultType operator()( v8::Handle<v8::Value> const & h ) const
        {
            return h->IsNumber()
            ? static_cast<ResultType>(h->Uint32Value())
            : 0;
        }
    };


    /** Specialization to convert JS values to int64_t. */
    template <>
    struct JSToNative<int64_t>
    {
        typedef int64_t ResultType;
        ResultType operator()( v8::Handle<v8::Value> const & h ) const
        {
            return h->IsNumber()
                ? static_cast<ResultType>(h->IntegerValue())
                : 0;
        }
    };

    /** Specialization to convert JS values to uint64_t. */
    template <>
    struct JSToNative<uint64_t>
    {
        typedef uint64_t ResultType;
        ResultType operator()( v8::Handle<v8::Value> const & h ) const
        {
            return h->IsNumber()
                ? static_cast<ResultType>(h->IntegerValue())
                : 0;
        }
    };

    /** Specialization to convert JS values to double. */
    template <>
    struct JSToNative<double>
    {
        typedef double ResultType;
        ResultType operator()( v8::Handle<v8::Value> const & h ) const
        {
            return h->IsNumber()
                ? h->NumberValue()
                : 0;
        }
    };

    /** Specialization to convert JS values to bool. */
    template <>
    struct JSToNative<bool>
    {
        typedef bool ResultType;
        ResultType operator()( v8::Handle<v8::Value> const & h ) const
        {
            return h->BooleanValue();
        }
    };

    /** Specialization to convert JS values to std::string. */
    template <>
    struct JSToNative<std::string>
    {
        typedef std::string ResultType;
        ResultType operator()( v8::Handle<v8::Value> const & h ) const
        {
            static const std::string emptyString;
            v8::String::Utf8Value const utf8String( h );
            const char* s = *utf8String;
            return s
                ? std::string(s, utf8String.length())
                : emptyString;
        }
    };
    
    /**
       Specialization necessary to avoid incorrect default behaviour
       for this special case.
        
       Reminder to self: we'll need a specialization like this any 
       time we want to use a type which is returned by VALUE from 
       JSToNative() and might be passed as a const reference as a 
       function argument type. The vast majority of bound native 
       types are bound internally as pointers (and don't need a 
       specialization like this one), whereas the std::string 
       conversion uses value semantics to simplify usage.
    */
    template <>
    struct JSToNative<std::string const &> : public JSToNative<std::string> {};

#if 0 // leave this unused code here for the sake of the next guy who tries to re-add it
    /**
       Nonono! This will Cause Grief when used together with CastFromJS()
       because the returned pointer's lifetime cannot be guaranteed.
       See ArgCaster for more details on the problem.
    */
    template <>
    struct JSToNative<char const *>
    {
    public:
        typedef char const * ResultType;
        ResultType operator()( v8::Handle<v8::Value> const & h );
    };
#else
#if 0 // it turns out no current (20110627) code uses this.
// We might want to leave it in b/c that will cause compile errors
// in some use cases rather than link errors ("undefined ref...").
    /** Not great, but a happy medium. */
    template <>
    struct JSToNative<char const *> : JSToNative<std::string> {};
#endif
#endif

    namespace Detail
    {
        /** A kludge placeholder type for a ulong-is-not-uint64
            condition on some platforms.

            T is ignored, but is required to avoid class-redefinition
            errors for the templates using this class.
        */
        template <typename T>
        struct UselessConversionType
        {
        };
    }

    /**
       This specialization is a kludge/workaround for use in cases
       where (unsigned long int) is:

       1) The same type as the platform's pointer type.
       2) Somehow NOT the same as one of the standard uintNN_t types.
       3) Used in CastToJS() or CastFromJS() calls.

       If ulong and uint64 are the same type then this specialization
       is a no-op (it generates a converter for a type which will
       never be converted), otherwords it performs a numeric conversion.
    */
    template <>
    struct NativeToJS< tmp::IfElse< tmp::SameType<unsigned long int,uint64_t>::Value,
                                    Detail::UselessConversionType<unsigned long>,
                                    unsigned long >::Type >
    : Detail::NativeToJS_int_big<unsigned long int>
    {
    };

    /**
       This is a kludge/workaround for use in cases where (unsigned
       long int) is:

       1) The same type as the platform's pointer type.
       2) Somehow NOT the same as one of the standard uintNN_t types.
       3) Used in CastToJS() or CastFromJS() calls.

       If ulong and uint64 are the same type then this specialization
       is a no-op (it generates a converter for a type which will
       never be converted), otherwords it performs a numeric
       conversion.
    */
    template <>
    struct JSToNative< tmp::IfElse<
        tmp::SameType<unsigned long int,uint64_t>::Value,
        Detail::UselessConversionType<unsigned long>,
        unsigned long >::Type >
    : JSToNative<uint64_t>
    {
    };

    /**
       See the equivalent NativeToJS kludge for unsigned long.
    */
    template <>
    struct NativeToJS< tmp::IfElse< tmp::SameType<long,int64_t>::Value,
                                    Detail::UselessConversionType<long>,
                                    long >::Type >
    : Detail::NativeToJS_int_big<int64_t>
    {
    };

    /**
       See the equivalent JSToNative kludge for unsigned long.
    */
    template <>
    struct JSToNative< tmp::IfElse<
        tmp::SameType<long,int64_t>::Value,
        Detail::UselessConversionType<long>,
        long >::Type > : JSToNative<int64_t>
    {
    };

#if CVV8_CONFIG_HAS_LONG_LONG
    /**
       See the equivalent JSToNative kludge for unsigned long.

       Added 20100606 to please the sqlite3 plugin, where sqlite3_int64
       collides with (long long) on some platforms and causes an invalid
       conversion compile-time error.
    */
    template <>
    struct JSToNative< tmp::IfElse<
        tmp::SameType<long long int,int64_t>::Value,
        Detail::UselessConversionType<long long>,
        long long int >::Type > : JSToNative<int64_t>
    {
    };
    /**
       See the equivalent JSToNative kludge for unsigned long.

       Added 20101126 to please the sqlite3 plugin, where
       sqlite3_int64 collides with (long long) on some platforms and
       causes a link-time error.
    */
    template <>
    struct NativeToJS< tmp::IfElse< tmp::SameType<long long int,int64_t>::Value,
                                    Detail::UselessConversionType<long long>,
                                    long long int >::Type >
    : Detail::NativeToJS_int_big<int64_t>
    {
    };
#endif

    
    /**
       Converts h to an object of type NT, using JSToNative<NT> to do
       the conversion.
    */
    template <typename NT>
    typename JSToNative<NT>::ResultType CastFromJS( v8::Handle<v8::Value> const & h )
    {
        typedef JSToNative<NT> F;
        return F()( h );
    }

    /** Convenience instance of JSToNative. */
    static const JSToNative<int16_t> JSToInt16 = JSToNative<int16_t>();
    /** Convenience instance of JSToNative. */
    static const JSToNative<uint16_t> JSToUInt16 = JSToNative<uint16_t>();
    /** Convenience instance of JSToNative. */
    static const JSToNative<int32_t> JSToInt32 = JSToNative<int32_t>();
    /** Convenience instance of JSToNative. */
    static const JSToNative<uint32_t> JSToUInt32 = JSToNative<uint32_t>();
    /** Convenience instance of JSToNative. */
    static const JSToNative<int64_t> JSToInt64 = JSToNative<int64_t>();
    /** Convenience instance of JSToNative. */
    static const JSToNative<uint64_t> JSToUInt64 = JSToNative<uint64_t>();
    /** Convenience instance of JSToNative. */
    static const JSToNative<double> JSToDouble = JSToNative<double>();
    /** Convenience instance of JSToNative. */
    static const JSToNative<bool> JSToBool = JSToNative<bool>();
    /** Convenience instance of JSToNative. */
    static const JSToNative<std::string> JSToStdString = JSToNative<std::string>();

    /**
       A utility to add properties to a JS v8::Object or v8::ObjectTemplate.
       ObjectType must be either v8::Object or v8::ObjectTemplate. It
       _might_ work in limited context with v8::Array, but that is untested.
       ObjectType is intended to be v8::Object or v8::ObjectTemplate,
       but Object subclasses, e.g. v8::Array and v8::Function, "should" work
       as well.

       It is intended to be used like this:

       \code
       v8::Handle<v8::Object> obj = ...;
       ObjectPropSetter<v8::Object> set(obj);
       set("propOne", CastToJS(32) )
         ("propTwo", ... )
         (32, ... )
         ("func1", anInvocationCallback )
       ;
       \endcode
    */
    template <typename ObjectType = v8::Object,
              typename KeyType = typename tmp::IfElse<
                            tmp::SameType<ObjectType,v8::ObjectTemplate>::Value,
                            v8::Handle<v8::String>,
                            v8::Handle<v8::Value>
                        >::Type
             >
    class ObjectPropSetter
    {
    private:
        v8::Handle<ObjectType> const target;
    public:
        /**
           Initializes this object to use the given array
           as its append target. Results are undefined if
           target is not a valid Object.
        */
        explicit ObjectPropSetter( v8::Handle< ObjectType > const & obj ) :target(obj)
        {}
        ~ObjectPropSetter(){}


        /**
           Adds an arbtirary property to the target object.
        */
        inline ObjectPropSetter & Set( KeyType const & key, v8::Handle<v8::Value> const & v )
        {
            this->target->Set(key->ToString(), CastToJS(v));
            return *this;
        }
        /**
           Adds an arbtirary property to the target object.
        */
        inline ObjectPropSetter & Set( char const * key, v8::Handle<v8::Value> const & v )
        {
            this->target->Set( v8::String::New(key), CastToJS(v));
            return *this;
        }

        /**
           Adds an arbitrary property to the target object using
           CastToJS(v).
        */
        template <typename T>
        inline ObjectPropSetter & operator()( KeyType const & key, T const & v )
        {
            this->target->Set(key, CastToJS(v));
            return *this;
        }

        /**
           Adds a numeric property to the target object.
        */
        template <typename T>
        inline ObjectPropSetter & operator()( int32_t ndx, T const & v )
        {
            return this->Set( v8::Integer::New(ndx), CastToJS(v) );
        }

        /**
           Adds a string-keyed property to the target object.
           Note that if key is NULL, the v8::String constructor
           will crash your app. (Good luck with that!)
        */
        template <typename T>
        inline ObjectPropSetter & operator()( char const * key, T const & v )
        {
            return this->Set( v8::String::New(key), CastToJS(v) );
        }


        /**
           Adds the given function as a member of the target object.
        */
        inline ObjectPropSetter & operator()( char const * name, v8::InvocationCallback pf )
        {
            return this->Set( name, v8::FunctionTemplate::New(pf)->GetFunction() );
        }
            
        /**
           Returns this object's JS object.
        */
        v8::Handle< v8::Object > Object() const
        {
            return this->target;
        }
    };

    /**
       NativeToJS classes which act on list types compatible with the
       STL can subclass this to get an implementation.
    */
    template <typename ListT>
    struct NativeToJS_list
    {
	v8::Handle<v8::Value> operator()( ListT const & li ) const
	{
	    typedef typename ListT::const_iterator IT;
	    IT it = li.begin();
	    const size_t sz = li.size();
#if 1
	    v8::Handle<v8::Array> rv( v8::Array::New( static_cast<int>(sz) ) );
	    for( int i = 0; li.end() != it; ++it, ++i )
	    {
		rv->Set( v8::Integer::New(i), CastToJS( *it ) );
	    }
	    return rv;
#else
	    ObjectPropSetter app(Array::New( static_cast<int>(sz) ));
	    for( int32_t i = 0; li.end() != it; ++it, ++i )
	    {
		app( i, CastToJS( *it ) );
	    }
	    return app.Object();
#endif
	}
    };

    /** Partial specialization for std::list<>. */
    template <typename T>
    struct NativeToJS< std::list<T> > : NativeToJS_list< std::list<T> > {};
    /** Partial specialization for std::vector<>. */
    template <typename T>
    struct NativeToJS< std::vector<T> > : NativeToJS_list< std::vector<T> > {};

    /**
       NativeToJS classes which act on map types compatible with the
       STL can subclass this to get an implementation.

       Both the key and mapped types of the given Map Type must be
       converitible to v8 types using CastToJS().
    */
    template <typename MapT>
    struct NativeToJS_map
    {
        v8::Handle<v8::Value> operator()( MapT const & li ) const
        {
            typedef typename MapT::const_iterator IT;
            IT it( li.begin() );
            v8::Handle<v8::Object> rv( v8::Object::New() );
            for( int i = 0; li.end() != it; ++it, ++i )
            {
                rv->Set( CastToJS( (*it).first ), CastToJS( (*it).second ) );
            }
            return rv;
        }
    };

    /** Partial specialization for std::map<>. */
    template <typename KeyT,typename ValT>
    struct NativeToJS< std::map<KeyT,ValT> > : NativeToJS_map< std::map<KeyT,ValT> > {};

    /**
       A base class for JSToNative<SomeStdListType>
       specializations. ListT must be compatible with std::list and
       std::vector, namely:

       - Must support push_back( ListT::value_type ).

       - Must define value_type typedef or the second template
       argument must be specified for this template.

       It is technically legal for ValueType to differ from
       ListT::value_type if
       ListT::push_back(JSToNative<ValueType>::ResultType) is
       legal. e.g. if ListT is std::vector<double> and we want to
       truncate the values we could use, e.g. int32_t as the
       ValueType.
    */
    template <typename ListT, typename ValueType = typename ListT::value_type>
    struct JSToNative_list
    {
	typedef ListT ResultType;
        /**
           Converts jv to a ListT object.

           If jv->IsArray() then the returned object is populated from
           jv, otherwise the returned object is empty. Since it is
           legal for an array to be empty, it is not generically
           possible to know if this routine got an empty Array object
           or a non-Array object.
        */
        ResultType operator()( v8::Handle<v8::Value> jv ) const
        {
            //typedef typename ListT::value_type VALT;
            typedef ValueType VALT;
            ListT li;
            if( jv.IsEmpty() || ! jv->IsArray() ) return li;
            v8::Handle<v8::Array> ar( v8::Array::Cast(*jv) );
            uint32_t ndx = 0;
            for( ; ar->Has(ndx); ++ndx )
            {
                li.push_back( CastFromJS<VALT>( ar->Get(v8::Integer::New(ndx)) ) );
            }
            return li;
        }
    };

    /** Partial specialization for std::list<>. */
    template <typename T>
    struct JSToNative< std::list<T> > : JSToNative_list< std::list<T> > {};

    /** Partial specialization for std::vector<>. */
    template <typename T>
    struct JSToNative< std::vector<T> > : JSToNative_list< std::vector<T> > {};

#if 0 // untested code
    /**
       UNTESTED!

       Intended as a base class for JSToNative specializations
       which proxy a std::map-like map.
    */
    template <typename MapT,
              typename KeyType = typename MapT::mapped_type,
              typename ValueType = typename MapT::value_type>
    struct JSToNative_map
    {
        typedef MapT ResultType;
        /**
           Converts jv to a MapT object.

           If jv->IsObject() then the returned object is populated from
           jv, otherwise the returned object is empty. Since it is
           legal for an object to be empty, it is not generically
           possible to know if this routine got an empty Object
           or a non-Object handle.
        */
        ResultType operator()( v8::Handle<v8::Value> jv ) const
        {
            typedef ValueType VALT;
            MapT map;
            if( jv.IsEmpty() || ! jv->IsObject() ) return map;
            Local<Object> obj( Object::Cast(*jv) );
            Local<Array> ar( obj->GetPropertyNames() );
            uint32_t ndx = 0;
            for( ; ar->Has(ndx); ++ndx )
            {
                Local<Value> const & k = ar->Get(Integer::New(ndx));
                if( ! obj->HasRealNamedProperty(k) ) continue;
                map[CastFromJS<KeyType>(k)] = CastFromJS<ValueType>(obj->Get(k));
            }
            return map;
        }
    };
#endif
    /**
       A utility class for building up message strings, most notably
       exception messages, using a mixture of native and JS message
       data.

       It is used like a std::ostream:

       @code
       StringBuffer msg;
       msg << "Could not set property "
           << "'" << propName
           <<"' on object " << myJSObject << '!';
       return v8::ThrowException(msg.toError());
       // or:
       return Toss(msg);
       @endcode
    */
    class StringBuffer
    {
    private:
        std::ostringstream os;
    public:
        /**
           Initializes the message stream.
        */
        StringBuffer() : os()
        {
        }
        StringBuffer(StringBuffer const & other) : os(other.os.str())
        {
        }
        StringBuffer & operator=(StringBuffer const & other)
        {
             this->os.str(other.os.str());
             return *this;
        }

        /**
           Empties out the message buffer. This invalidates any value
           returned from previous calls to the (char const *)
           operator.
        */
        inline void Clear()
        {
            this->os.str("");
        }

        /**
           Returns a copy of the current message content.
        */
        inline std::string Content() const
        {
            return this->os.str();
        }

        /**
           Converts the message state to a JS string.
        */
        inline operator v8::Handle<v8::Value>() const
        {
            return CastToJS<std::string>( this->os.str() );
        }

        /**
           Converts the message state to a v8::String (for use
           with v8::Exception::Error() and friends).
        */
        inline operator v8::Handle<v8::String>() const
        {
            std::string const & str(this->os.str());
            char const * cstr = str.c_str();
            return v8::String::New( cstr ? cstr : "", cstr ? str.size() : 0 );
        }

        /**
           Appends to the message using CastFromJS<std::string>(t) 
        */
        template <typename T>
        inline StringBuffer & operator<<( v8::Handle<T> const & t )
        {
            this->os << CastFromJS<std::string>( t );
            return *this;
        }
        /**
           Appends to the message using CastFromJS<std::string>(t)

           Reminder to self: if this function is changed to take
           a const reference instead of a copy, we get overload
           ambiguity errors in some contexts. See:

           http://code.google.com/p/v8-juice/issues/detail?id=19

           (And thanks to Feng Fillano for reporting and localizing
           the problem.)           
        */
        template <typename T>
        inline StringBuffer & operator<<( v8::Local<T> const t )
        {
            this->os << CastFromJS<std::string>( t );
            return *this;
        }

        /**
           Appends t to the message via std::ostream<<.
        */
        template <typename T>
        inline StringBuffer & operator<<( T const t)
        {
            this->os << t;
            return *this;
        }

        /**
           Returns this buffer's value wrapped in
           a JS Error object.
        */
        v8::Local<v8::Value> toError() const
        {
            return v8::Exception::Error(*this);
        }
    };

    /** Outputs sb.Content() to os and returns os. */
    inline std::ostream & operator<<( std::ostream & os, StringBuffer const & sb )
    {
        return os << sb.Content();
    }


    /**
        Requi
        "Lexically casts" msg to a string and throws a new JS-side
        Error. ValT may be any type which can be sent to StringBuffer's
        ostream operator.
        
        The return value is the result of calling 
        v8::ThrowException() (Undefined except in the face of a 
        serious internal error like OOM, i'm told by the v8 devs).
    */
    template <typename ValT>
    inline v8::Handle<v8::Value> Toss( ValT const & msg )
    {
        return v8::ThrowException((StringBuffer() << msg).toError());
    }
    
    /**
        Overload to avoid an ambiguity (and it's more efficient than
        the default impl).
    */
    inline v8::Handle<v8::Value> Toss( char const * msg )
    {
        return v8::ThrowException(v8::Exception::Error(v8::String::New( msg ? msg : "Unspecified error.")));
    }

    /**
        Eqivalent to v8::ThrowException(err). Note that if err is not an
        Error object, the JS side will not get an Error object. e.g. if err
        is-a String then the JS side will see the error as a string.

        The reason this does not convert err to an Error is because the v8
        API provides no way for us to know if err is already an Error object.
        This function is primarily intended to be passed the results of
        CastToJS(std::exception), which generates Error objects.
    */
    inline v8::Handle<v8::Value> Toss( v8::Handle<v8::Value> const & err )
    {
        return v8::ThrowException(err);
    }

    /**
        Efficiency overload.
    */
    inline v8::Handle<v8::Value> Toss( StringBuffer const & msg )
    {
        return v8::ThrowException(msg.toError());
    }
    /**
        Like Toss(Handle<Value>), but converts err to a string and creates an
        Error object, which is then thrown.
    */
    inline v8::Handle<v8::Value> TossAsError( v8::Handle<v8::Value> const & err )
    {
        return Toss(v8::Exception::Error(err->ToString()));
    }

    /**
       ArgCaster is a thin wrapper around CastFromJS(), and primarily
       exists to give us a way to convert JS values to (char const *)
       for purposes of passing them to native functions. The main
       difference between this type and JSToNative<T> is that this
       interface explicitly allows for the conversion to be stored by
       an instance of this type. That allows us to get more lifetime
       out of converted values in certain cases (namely (char const*)).

       The default implementation is suitable for all cases which
       JSToNative<T> supports, but specializations can handle some of
       the corner cases which JSToNative cannot (e.g. (char const *)).
       
       Added 20091121.
    */
    template <typename T>
    struct ArgCaster
    {
        typedef typename JSToNative<T>::ResultType ResultType;
        /**
           Default impl simply returns CastFromJS<T>(v).
           Specializations are allowed to store the result of the
           conversion, as long as they release it when the destruct.
           See ArgCaster<char const *> for an example of that.
        */
        inline ResultType ToNative( v8::Handle<v8::Value> const & v ) const
        {
            return CastFromJS<T>( v );
        }
        /**
            To eventually be used for some internal optimizations.
        */
        enum { HasConstOp = 1 };
    };
    /**
       Specialization for (char const *). The value returned from
       ToNative() is guaranteed to be valid as long as the ArgCaster
       object is alive or until ToNative() is called again (which will
       almost certainly change the pointer). Holding a pointer to the
       ToNative() return value after the ArgCaster is destroyed will
       lead to undefined behaviour. Likewise, fetching a pointer, then
       calling ToNative() a second time, will invalidate the first
       pointer.

       BEWARE OF THESE LIMITATIONS:

       1) This will only work properly for nul-terminated strings,
       and not binary data!

       2) Do not use this to pass (char const *) as a function 
       parameter if that function will hold a copy of the pointer 
       after it returns (as opposed to copying/consuming the 
       pointed-to-data before it returns) OR if it returns the 
       pointer passed to it. Returning is a specific corner-case
       of "holding a copy" for which we cannot guaranty the lifetime
       at the function-binding level.

       3) Do not use the same ArgCaster object to convert multiple
       arguments, as each call to ToNative() will invalidate the
       pointer returned by previous calls.

       4) The to-string conversion uses whatever encoding 
       JSToNative<std::string> uses.

       Violating any of those leads to undefined behaviour, and
       very possibly memory corruption for cases 2 or 3.
    */
    template <>
    struct ArgCaster<char const *>
    {
    private:
        /**
            Reminder to self: we cannot use v8::String::Utf8Value
            here because at the point the bindings call ToNative(),
            v8 might have been unlocked, at which point dereferencing
            the Utf8Value becomes illegal.
        */
        std::string val;
        typedef char Type;
    public:
        typedef Type const * ResultType;
        /**
           Returns the toString() value of v unless:

           - v.IsEmpty()
           - v->IsNull()
           - v->IsUndefined()

           In which cases it returns 0.

           The returned value is valid until:

           - ToNative() is called again.
           - This object is destructed.
        */
        ResultType ToNative( v8::Handle<v8::Value> const & v )
        {
            typedef JSToNative<std::string> C;
            if( v.IsEmpty() || v->IsNull() || v->IsUndefined() )
            {
                return 0;
            }
            this->val = C()( v );
            return this->val.c_str();
        }
        /**
            To eventually be used for some internal optimizations.
        */
        enum { HasConstOp = 0 };
    };

#if !defined(DOXYGEN)
    namespace Detail {
        /**
            Default (unimplemented) CtorForwarderProxy impl. A helper
            for the CtorForwarder class. All specializations except
            the 0-arity one are generated from script code.
        */
        template <typename Sig, int Arity = sl::Arity< Signature<Sig> >::Value >
        struct CtorForwarderProxy
        {
            typedef typename Signature<Sig>::ReturnType ReturnType;
            static ReturnType Call( v8::Arguments const & );
        };

        //! Specialization for 0-arity ctors.
        template <typename Sig>
        struct CtorForwarderProxy<Sig,0>
        {
            typedef typename Signature<Sig>::ReturnType ReturnType;
            static ReturnType Call( v8::Arguments const & )
            {
                typedef typename TypeInfo<ReturnType>::Type RType;
                return new RType;
            }
        };
        //! Specialization for ctors taking (v8::Arguments const &).
        template <typename Sig>
        struct CtorForwarderProxy<Sig,-1>
        {
            typedef typename Signature<Sig>::ReturnType ReturnType;
            static ReturnType Call( v8::Arguments const & argv )
            {
                typedef typename TypeInfo<ReturnType>::Type T;
                return new T(argv);
            }
        };

    }
#endif
    /**
       A utility type to help forward v8::Arguments to native 
       constructors. This type is intended to assist in the creation 
       of ctor functions for JS-bound C++ classes.
       
       Requirements:
       
       - Sig is "almost" a function-signature-style type, but 
       because ctors don't have a return value in the conventional 
       sense, we have to use a tiny workaround: Sig should be passed 
       in like this from client code:

       @code
       typedef CtorForwarder<T * (argType1, argType2)> CF;
       @endcode

       - (new T(...)) must be legal, taking a number of 
       arguments equal to Sig's Arity.

       - All arguments to the native ctor must be convertible
       using CastFromJS().

        Example:
       
       @code
       typedef CtorForwarder<MyType *()> CF0;
       typedef CtorForwarder<MyType *(int)> CF1;
       typedef CtorForwarder<MyType *(double,int)> CF2;
       typedef CtorForwarder<MyType *(v8::Arguments const &)> CFAny;
       @endcode

       @see CtorArityDispatcher
    */
    template <typename Sig>
    struct CtorForwarder : Signature<Sig>
    {
        typedef Signature<Sig> STL;
        //typedef typename tmp::AddPointer<typename STL::ReturnType>::Type ReturnType;
        typedef typename STL::ReturnType ReturnType;
        /**
            If (argv.Length()>=Arity) or Arity is less than 0,
            then the constructor is called with Arity arguments
            (if it >=0) or with 1 v8::Arguments parameter (for Arity<0).
            
            Returns the result of (new Type(...)), transfering ownership
            to the caller.
            
            May propagate native exceptions.
        */
        static ReturnType Call( v8::Arguments const & argv )
        {
            enum { Arity = sl::Arity< STL >::Value };
            typedef Detail::CtorForwarderProxy<Sig> Proxy;
            return Proxy::Call( argv );
        }
    };

#if !defined(DOXYGEN)
    namespace Detail
    {

        /**
           Internal dispatch routine. CTOR _must_ be a CtorForwarder implementation
           (or interface-compatible).
        */
        template <typename T,typename CTOR>
        struct CtorFwdDispatch
        {
            typedef typename TypeInfo<T>::NativeHandle ReturnType;
            static ReturnType Call( v8::Arguments const &  argv )
            {
                return CTOR::Call( argv );
            }
        };
        /**
           Internal dispatch end-of-list routine.
        */
        template <typename T>
        struct CtorFwdDispatch<T,tmp::NilType>
        {
            typedef typename TypeInfo<T>::NativeHandle ReturnType;
            static ReturnType Call( v8::Arguments const & )
            {
                return 0;
            }
        };
        /**
           Internal type to dispatch a v8::Arguments list to one of
           several a bound native constructors, depending on on the
           argument count.
        
           List MUST be a Signature< ... > containing ONLY
           CtorFowarder types (or compatible).
        */
        template <typename T,typename List>
        struct CtorFwdDispatchList
        {
            typedef typename TypeInfo<T>::NativeHandle ReturnType;
            /**
               Tries to dispatch Arguments to one of the constructors
               in the List type, based on the argument count.
             */
            static ReturnType Call( v8::Arguments const &  argv )
            {
                typedef typename List::Head CTOR;
                typedef typename List::Tail Tail;
                enum { Arity = (0==sl::Index<v8::Arguments const &,CTOR>::Value)
                                ? -1 : sl::Length<CTOR>::Value
                };
                return ( (Arity < 0) || (Arity == argv.Length()) )
                    ? CtorFwdDispatch< T, CTOR >::Call(argv )
                    : CtorFwdDispatchList<T,Tail>::Call(argv);
            }
        };
        /**
           End-of-list specialization.
        */
        template <typename T>
        struct CtorFwdDispatchList<T,tmp::NilType>
        {
            typedef typename TypeInfo<T>::NativeHandle ReturnType;
            /** Writes an error message to errmsg and returns 0. */
            static ReturnType Call( v8::Arguments const &  argv )
            {
                StringBuffer msg;
                msg << "No native constructor was defined for "<<argv.Length()<<" arguments!\n";
                throw std::range_error(msg.Content().c_str());
                return 0;
            }
        };
    }
#endif // !DOXYGEN
    
    /**
        Proxies a list of constructors from v8::Arguments.
        
        CtorList must be-a a Signature type in this form:
        
        @code
        typedef Signature<MyType ( // may optionally be (MyType *) - same effect
            CtorForwarder<MyType *()>,
            CtorForwarder<MyType *(char const *)>,
            CtorForwarder<MyType *( int, double )>,
            CtorForwarder<MyType *( v8::Arguments const &)>
        )> Ctors;
        @endcode
        
        All entries in the typelist must be interface-compatible with
        CtorForwarder. No two entries should have the same number 
        of arguments with one exception: an InvocationCallback-like 
        function taking (v8::Arguments const &) can be used as a 
        catch-all for any number of arguments. If used, it must be 
        the LAST entry because it will always match any argument 
        count (and will therefore trump any which (would) come after 
        it.
        
        The ctors are dispatched based solely on the argument count,
        not their types. The first one with a matching arity
        is called.
        
        IN THEORY (untested), the factories passed in the list may 
        legally return a type publically derived from 
        CtorList::ReturnType.
    */
    template <typename CtorList>
    struct CtorArityDispatcher
    {
        typedef typename CtorList::ReturnType RT;
        typedef typename TypeInfo<RT>::NativeHandle NativeHandle;
        static NativeHandle Call( v8::Arguments const & argv )
        {
            typedef typename tmp::PlainType<RT>::Type Type;
            typedef Detail::CtorFwdDispatchList<Type, CtorList> Proxy;
            return Proxy::Call( argv );
        }
    };

} // namespaces


#endif /* CODE_GOOGLE_COM_P_V8_CONVERT_CORE_HPP_INCLUDED */
