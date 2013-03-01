#if !defined(CODE_GOOGLE_COM_P_V8_CONVERT_PROPERTIES_HPP_INCLUDED)
#define CODE_GOOGLE_COM_P_V8_CONVERT_PROPERTIES_HPP_INCLUDED 1

#include "invocable.hpp"

namespace cvv8 {

    /**
        Marker class, primarily for documentation purposes.
        
        This class models the v8::AccessorGetter interface, and
        XyzToGetter classes inherit this type as a sign that they
        implement this interface.
        
        This class has no implemention - it only exists for 
        documentation purposes.
    */
    struct AccessorGetterType
    {
        /**
            The v8::AccessorGetter() interface.
        */
        static v8::Handle<v8::Value> Get(v8::Local<v8::String> property, const v8::AccessorInfo &info);
        //{ return Toss(StringBuffer()<<"Property '"<<property<<"' getter is unspecialized!");}
    };

    /**
        Marker class, primarily for documentation purposes.
        
        This class models the v8::AccessorSetter interface, and
        XyzToSetter classes inherit this type as a sign that they
        implement this interface.
        
        This class has no useful implemention - it only exists for
        documentation purposes.
    */
    struct AccessorSetterType
    {
        /** The v8::AccessorSetter() interface. */
        static void Set(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    };

    /** @typedef AccessorGetterType Getter

        Convenience typedef, primarily to simplify usage of
        FunctionTo and friends.
    */
    typedef AccessorGetterType Getter;

    /** @typedef AccessorSetterType Setter

        Convenience typedef, primarily to simplify usage of
        FunctionTo and friends.
    */
    typedef AccessorSetterType Setter;

    /**
        A tag type for use with VarTo, MemberTo, MethodTo, and FunctionTo.
    */
    struct Accessors : AccessorGetterType, AccessorSetterType {};

    /**
       This template create an v8::AccessorGetter from a static/shared
       variable.

       SharedVar must be pointer to a static variable and must not
       be NULL.

       CastToJS(*SharedVar) must be legal.
    */
    template <typename PropertyType, PropertyType * const SharedVar>
    struct VarToGetter : AccessorGetterType
    {
        /** Implements the v8::AccessorGetter() interface. */
        inline static v8::Handle<v8::Value> Get(v8::Local<v8::String> property, const v8::AccessorInfo &info)
        {
            return CastToJS( *SharedVar );
        }
    };

    /**
       The setter counterpart of StaticVarToGetter().

       SharedVar must be pointer to a static variable and must not
       be NULL.

       (*SharedVar = CastFromJS<PropertyType>()) must be legal.
       
       Reminder: this is not included in the StaticVarToGetter 
       template so that we can avoid either the Get or Set 
       conversion for cases where it is not legal (or not desired). 
       If they were both in one class, both Get and Set would _have_ 
       to be legal.
    */
    template <typename PropertyType, PropertyType * const SharedVar>
    struct VarToSetter : AccessorSetterType
    {
        /** Implements the v8::AccessorSetter() interface. */
        inline static void Set(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
        {
            *SharedVar = CastFromJS<PropertyType>( value );
        }
    };

    /**
        A proxy for both VarToGetter and VarToSetter, providing both
        Get() and Set() functions.
    */
    template <typename PropertyType, PropertyType * const SharedVar>
    struct VarToAccessors : VarToGetter<PropertyType,SharedVar>,
                            VarToSetter<PropertyType,SharedVar>
    {};

    /**
       This template creates a v8::AcessorGetter which binds directly to
       a non-const native class member variable.

       Requirements:

       - T must be convertible to (T*) via CastFromJS<T>().
       - MemVar must be an accessible member of T.
       - PropertyType must be convertible via CastToJS<PropertyType>().

       If the underlying native 'this' object cannot be found (that
       is, if CastFromJS<T>() fails) then this routine will
       trigger a JS exception.
    */
    template <typename T, typename PropertyType, PropertyType T::*MemVar>
    struct MemberToGetter : AccessorGetterType
    {
        /** Implements the v8::AccessorGetter() interface. */
        inline static v8::Handle<v8::Value> Get(v8::Local<v8::String> property, const v8::AccessorInfo &info)
        {
            typedef typename TypeInfo<T>::Type Type;
            typedef typename JSToNative<T>::ResultType NativeHandle;
            NativeHandle self = CastFromJS<T>( info.This() );
            return ( ! self )
                ? Toss( StringBuffer() << "Native member property getter '"
                        << property << "' could not access native 'this' object!" )
                : CastToJS( (self->*MemVar) );
        }
    };

    /**
       This is the Setter counterpart of MemberToGetter.

       Requirements:

       - T must be convertible to (T*) via CastFromJS<T>().
       - PropertyType must be convertible via CastToJS<PropertyType>().
       - MemVar must be an accessible member of T.

       If the underlying native This object cannot be found then this
       routine will trigger a JS exception.
    */
    template <typename T, typename PropertyType, PropertyType T::*MemVar>
    struct MemberToSetter : AccessorSetterType
    {
        /** Implements the v8::AccessorSetter() interface. */
        inline static void Set(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
        {
            typedef typename TypeInfo<T>::Type Type;
            typedef typename JSToNative<T>::ResultType NativeHandle;
            NativeHandle self = CastFromJS<T>( info.This() );
            if( self ) self->*MemVar = CastFromJS<PropertyType>( value );
            else Toss( StringBuffer() << "Native member property setter '"
                        << property << "' could not access native 'this'his object!" );
        }
    };

    /**
        A proxy for both MemberToGetter and MemberToSetter, providing both
        Get() and Set() functions.
        
        This should be called MembersToAccessors (plural Members).
    */
    template <typename T, typename PropertyType, PropertyType T::*MemVar>
    struct MemberToAccessors : MemberToGetter<T,PropertyType,MemVar>,
                               MemberToSetter<T,PropertyType,MemVar>
    {};
    
    /**
       An AccessorSetter() implementation which always triggers a JS exception.
       Can be used to enforce "pedantically read-only" variables. Note that
       JS does not allow us to assign an AccessorSetter _without_ assigning
       an AccessorGetter. Also note that there is no AccessorGetterThrow,
       because a write-only var doesn't make all that much sense (use
       methods for those use cases).
    */
    struct ThrowingSetter : AccessorSetterType
    {
        inline static void Set(v8::Local<v8::String> property, v8::Local<v8::Value>, const v8::AccessorInfo &)
        {
             Toss(StringBuffer() <<
                  "Native member property setter '"
                  << property
                  << "' is configured to throw an exception when modifying "
                  << "this read-only member!");
        }
    };

    /**
       Implements the v8::AccessorGetter interface to bind a JS
       member property to a native getter function. This function
       can be used as the getter parameter to
       v8::ObjectTemplate::SetAccessor().

       Sig must be a function-signature-style type and Getter must
       capable of being called with no arguments and returning a
       value which can be CastToJS() to a JS value.

       If Getter() throws a native exception it is converted to a JS
       exception.
    */
    template <typename Sig, typename FunctionSignature<Sig>::FunctionType Getter>
    struct FunctionToGetter : AccessorGetterType
    {
        inline static v8::Handle<v8::Value> Get( v8::Local< v8::String > property, const v8::AccessorInfo & info )
        {
            return CastToJS( (*Getter)() );
        }
    };
       
    /**
       Implements the v8::AccessorSetter interface to bind a JS
       member property to a native getter function. This function
       can be used as the getter parameter to
       v8::ObjectTemplate::SetAccessor().

       SigSet must be function-signature-style pattern
       for the setter function. The native
       function must follow conventional mutator signature:

       ReturnType ( PropertyType )

       PropertyType may differ from the return type. PropertyType
       may not be void but the ReturnType may be. Any return value
       from the setter is ignored by the JS engine.

       Note that the v8 API appears to not allow us to just set
       a setter, but not a getter. We have to set a getter without
       a setter, a getter with a setter, or neither. At least that's
       been my experience.

       If Setter() throws a native exception it is converted to a JS
       exception.
    */
    template <typename Sig, typename FunctionSignature<Sig>::FunctionType Func>
    struct FunctionToSetter : AccessorSetterType
    {
        inline static void Set( v8::Local< v8::String > property, v8::Local< v8::Value > value, const v8::AccessorInfo &info)
        {
            typedef FunctionSignature<Sig> FT;
            (*Func)( CastFromJS<typename sl::At<0,FT>::Type>( value ) );
        }
    };


    /**
       Implements the v8::AccessorGetter interface to bind a JS
       member property to a native getter function. This function
       can be used as the getter parameter to
       v8::ObjectTemplate::SetAccessor().

       Sig must be a function-pointer-style argument with a
       non-void return type convertible to v8 via CastToJS(), and
       Getter must be function capable of being called with 0
       arguments (either because it has none or they have
       defaults).
    */
    template <typename T, typename Sig, typename MethodSignature<T,Sig>::FunctionType Getter>
    struct MethodToGetter : AccessorGetterType
    {
        inline static v8::Handle<v8::Value> Get( v8::Local< v8::String > property, const v8::AccessorInfo & info )
        {
            typedef typename TypeInfo<T>::Type Type;
            typedef typename JSToNative<T>::ResultType NativeHandle;
            NativeHandle self = CastFromJS<T>( info.This() );
            return self
                ? CastToJS( (self->*Getter)() )
                : Toss( StringBuffer() << "Native member property getter '"
                      << property << "' could not access native This object!" );
        }
    };

   /**
       Overload for const native getter functions.
    */
    template <typename T, typename Sig, typename ConstMethodSignature<T,Sig>::FunctionType Getter>
    struct ConstMethodToGetter : AccessorGetterType
    {
        inline static v8::Handle<v8::Value> Get( v8::Local< v8::String > property, const v8::AccessorInfo & info )
        {
            typedef typename TypeInfo<T>::Type Type;
            typedef typename JSToNative<T>::ResultType NativeHandle;
            NativeHandle const self = CastFromJS<T>( info.This() );
            return self
                ? CastToJS( (self->*Getter)() )
                : Toss( (StringBuffer() << "Native member property getter '"
                       << property << "' could not access native This object!").toError() )
               ;
        }
    };

    /**
        Implements v8::AccessorSetter interface to proxy a JS
        member property through a native member setter function.

        This function can be used as the setter parameter to
        v8::ObjectTemplate::SetAccessor().

        Sig must be a function-pointer-style type and Getter must
        be a T member function of that type. The function must be
        capable of being called with only 1 argument (either
        because it only accepts 1 or has defaults for the others),
        and its return value is discarded (not converted to v8)
        because that's how v8's native setters work.

        Exceptions thrown by the underlying function are
        translated to JS exceptions.
        
        FIXME: code is 100% identical to ConstMethodToSetter except 
        for FunctionType typedef. Consolidate them in a base class.
    */
    template <typename T, typename Sig, typename MethodSignature<T,Sig>::FunctionType Setter>
    struct MethodToSetter : AccessorSetterType
    {
        static void Set(v8::Local< v8::String > property, v8::Local< v8::Value > value, const v8::AccessorInfo &info)
        {
            typedef typename TypeInfo<T>::Type Type;
            typedef typename JSToNative<T>::ResultType NativeHandle;
            NativeHandle self = CastFromJS<NativeHandle>( info.This() );
            if( ! self )
            {
                Toss( StringBuffer() << "Native member property setter '"
                     << property << "' could not access native This object!" );
            }
            else
            {

                typedef typename sl::At< 0, Signature<Sig> >::Type ArgT;
                (self->*Setter)( CastFromJS<ArgT>( value ) );
            }
            return;
        }
    };

    /**
        Const-method equivalent of MethodToSetter.
    
        FIXME: code is 100% identical to ConstMethodToSetter except 
        for FunctionType typedef. Consolidate them in a base class.
    */
    template <typename T, typename Sig, typename ConstMethodSignature<T,Sig>::FunctionType Setter>
    struct ConstMethodToSetter : AccessorSetterType
    {
        static void Set(v8::Local< v8::String > property, v8::Local< v8::Value > value, const v8::AccessorInfo &info)
        {
            typedef typename TypeInfo<T>::Type Type;
            typedef typename JSToNative<T>::ResultType NativeHandle;
            NativeHandle self = CastFromJS<NativeHandle>( info.This() );
            if( ! self )
            {
                Toss( StringBuffer() << "Native member property setter '"
                     << property << "' could not access native This object!" );
            }
            else
            {
                typedef typename sl::At< 0, Signature<Sig> >::Type ArgT;
                (self->*Setter)( CastFromJS<ArgT>( value ) );
            }
        }
    };


    /**
        Similar to FunctionToGetter but uses a functor as a getter.
        This is rarely useful, since the functor has no direct access
        to any application state (unless that is static/shared within
        the Ftor class).

        Ftor must be a functor. Sig must be a signature matching a const
        Ftor::operator() implementation. CastToJS(Signature<Sig>::ReturnType)
        must be legal.

        The signature's return type may not be void (this is a getter,
        and getters don't return void).
    */
    template <typename Ftor, typename Sig>
    struct FunctorToGetter
    {
        inline static v8::Handle<v8::Value> Get( v8::Local< v8::String > property, const v8::AccessorInfo & info )
        {
            //const static Ftor f();
            return CastToJS(Ftor()());
        }
    };

    /**
        The setter counterpart of FunctorToGetter.

        Ftor must be a functor which accepts 1 arguments.
        Sig must be a signature matching a Ftor::operator()
        implementation.

        The return value of Ftor::operator() is not evaluated, so it may be
        void or any non-convertible type. It is semantically illegal to bind
        a setter which return resources allocated by the setter, if those
        resources are passed to the caller. In such a case, each access
        _will_ leak memory. But nobody returns allocated memory from a
        setter, right?

        CastFromJS<ArgType1>() must be legal, where ArgType1
        is sl::At<0, Signature<Sig> >::Type.
    */
    template <typename Ftor, typename Sig>
    struct FunctorToSetter
    {
        inline static void Set(v8::Local< v8::String > property, v8::Local< v8::Value > value, const v8::AccessorInfo &info)
        {
            typedef typename sl::At< 0, Signature<Sig> >::Type ArgT;
            Ftor()( CastFromJS<ArgT>( value ) );
        }
    };
    
    /**
        SetterCatcher is the AccessorSetter equivalent of InCaCatcher, and
        is functionality identical except that its 4th template parameter
        must be-a AccessorSetterType instead of an InCa type.
    */
    template < typename ExceptionT,
               typename SigGetMsg,
               typename ConstMethodSignature<ExceptionT,SigGetMsg>::FunctionType Getter,
               typename SetterT,
               bool PropagateOtherExceptions = false
    >
    struct SetterCatcher : AccessorSetterType
    {
        static void Set(v8::Local< v8::String > property, v8::Local< v8::Value > value, const v8::AccessorInfo &info)
        {
            try
            {
                SetterT::Set( property, value, info );
            }
            catch( ExceptionT const & e2 )
            {
                Toss((e2.*Getter)());
            }
            catch( ExceptionT const * e2 )
            {
                Toss((e2->*Getter)());
            }
            catch(...)
            {
                if( PropagateOtherExceptions ) throw;
                else Toss("Unknown native exception thrown!");
            }
        }
    };

    /**
        A convenience form of SetterCatcher which catches std::exception
        and subtyes. See InCaCatcher_std for full details - this type is
        identical except that its first template parameter must be-a
        AccessorSetterType instead of InCa type.
    */
    template <
            typename SetterT,
            typename ConcreteException = std::exception,
            bool PropagateOtherExceptions = !tmp::SameType< std::exception, ConcreteException >::Value
    >
    struct SetterCatcher_std :
        SetterCatcher<ConcreteException,
                      char const * (),
                      &ConcreteException::what,
                      SetterT,
                      PropagateOtherExceptions
                    >
    {};

    /**
        GetterCatcher is the AccessorSetter equivalent of InCaCatcher, and
        is functionality identical except that its 4th template parameter
        must be-a AccessorGetterType instead of an InCa type.
    */
    template < typename ExceptionT,
               typename SigGetMsg,
               typename ConstMethodSignature<ExceptionT,SigGetMsg>::FunctionType Getter,
               typename GetterT,
               bool PropagateOtherExceptions = false
    >
    struct GetterCatcher : AccessorGetterType
    {
        static v8::Handle<v8::Value> Get( v8::Local< v8::String > property, const v8::AccessorInfo & info )
        {
            try
            {
                return GetterT::Get( property, info );
            }
            catch( ExceptionT const & e2 )
            {
                return Toss(CastToJS((e2.*Getter)()));
            }
            catch( ExceptionT const * e2 )
            {
                return Toss(CastToJS((e2->*Getter)()));
            }
            catch(...)
            {
                if( PropagateOtherExceptions ) throw;
                else return Toss("Unknown native exception thrown!");
            }
        }
    };

    /**
        A convenience form of GetterCatcher which catches std::exception
        and subtyes. See InCaCatcher_std for full details - this type is
        identical except that its first template parameter must be-a
        AccessorGetterType instead of InCa type.
    */
    template <
            typename GetterT,
            typename ConcreteException = std::exception,
            bool PropagateOtherExceptions = !tmp::SameType< std::exception, ConcreteException >::Value
    >
    struct GetterCatcher_std :
        GetterCatcher<ConcreteException,
                      char const * (),
                      &ConcreteException::what,
                      GetterT,
                      PropagateOtherExceptions
                    >
    {};


    /**
        AccessAdder is a convenience class for use when applying several
        (or more) accessor bindings to a prototype object.
            
        Example:
        
        @code
        AccessorAdder acc(myPrototype);
        acc("foo", MemberToGetter<MyType,int,&MyType::anInt>(),
                   MemberToSetter<MyType,int,&MyType::anInt>())
           ("bar", ConstMethodToGetter<MyType,int(),&MyType:getInt>(),
                   MethodToSetter<MyType,void(int),&MyType:setInt>())
           ...
           ;
        @endcode
    */
    class AccessorAdder
    {
    private:
        v8::Handle<v8::ObjectTemplate> const & proto;

        /** A "null" setter which does nothing. Used only as default
            argument to operator(). Its Set() is not actually used
            but this type is used as a placeholder for a NULL
            v8::AccessorSetter.
        */
        struct NullSetter
        {
            /** The v8::AccessorSetter() interface. */
            static void Set(v8::Local< v8::String > property, v8::Local< v8::Value > value, const v8::AccessorInfo &info)
            {
            }
        };
    public:
        /**
            Initializes this object so that calls to operator() will
            apply to p. p must out-live this object. More specifically,
            operator() must not be called after p has been destroyed.
        */
        explicit AccessorAdder( v8::Handle<v8::ObjectTemplate> const & p )
            : proto(p)
        {}
        AccessorAdder const & operator()( char const * name,
                                          v8::AccessorGetter g,
                                          v8::AccessorSetter s = NULL,
                                          v8::Handle< v8::Value > data=v8::Handle< v8::Value >(),
                                          v8::AccessControl settings=v8::DEFAULT,
                                          v8::PropertyAttribute attribute=v8::None) const
        {
            proto->SetAccessor(v8::String::New(name), g, s, data, settings, attribute);
            return *this;
        }
        /**
            Adds GetterT::Get and SetterT::Set as accessors for the 
            given property in the prototype object.
            
            GetterT must be-a AccessorGetterType. SetterT must be-a 
            AccessorSetterType. Note that their values are not used, 
            but GetterT::Get and SetterT::Set are used 
            directly. The objects are only passed in to keep the 
            client from having to specify them as template 
            parameters (which is clumsy for operator()), as their 
            types can be deduced. 
            
            The 3rd and higher arguments are as documented (or not!)
            for v8::ObjectTemplate::SetAccessor().
            
            Returns this object, for chaining calls.
        */
        template <typename GetterT, typename SetterT>
        AccessorAdder const & operator()( char const * name,
                                          GetterT const &,
                                          SetterT const & = NullSetter(),
                                          v8::Handle< v8::Value > data=v8::Handle< v8::Value >(),
                                          v8::AccessControl settings=v8::DEFAULT,
                                          v8::PropertyAttribute attribute=v8::None) const
        {
            // jump through a small hoop to ensure identical semantics vis-a-vis
            // the other overload.
            return this->operator()( name, GetterT::Get,
                                    tmp::SameType<NullSetter,SetterT>::Value ? NULL : SetterT::Set,
                                    data, settings, attribute);
        }
    };

#if 0 /* i'm still undecided on the in-class naming conventions here... */
    /**
        ClassAccessor provides a slight simplification for cases 
        where several different members/methods of a given bound 
        class (T) need to be bound to v8. It wraps the following 
        classes so that clients can use them one fewer template 
        parameters:
        
        MemberToGetter, MemberToSetter, MethodToGetter, MethodToSetter, 
        ConstMethodToGetter, ConstMethodToSetter
        
        Example:
        
        @code
        typedef ClassAccessor<MyType> CA;
        v8::AccessorGetter get;
        
        get = MemberToGetter<MyType, int, &MyType::anInt>::Get
        // is equivalent to:
        get = CA::MemGet<int, &MyType::anInt>::Set
        @endcode
        
        Its only real benefit is saving a bit of typing when several T
        member/method bindings are made and T's real type name is longer
        than 'T'. e.g. it gets tedious to repeatedly type
        MyExceedinglyLongClassName in method/member-binding templates.
        (Function-local typedefs help.)
    */
    template <typename T>
    struct ClassAccessor
    {
        template <typename PropertyType, PropertyType T::*MemVar>
        struct MemGet : MemberToGetter<T, PropertyType, MemVar> {};
        template <typename PropertyType, PropertyType T::*MemVar>
        struct MemSet : MemberToSetter<T, PropertyType, MemVar> {};
        template <typename Sig, typename MethodSignature<T,Sig>::FunctionType Getter>
        struct MethGet : MethodToGetter<T,Sig,Getter> {};
        template <typename Sig, typename ConstMethodSignature<T,Sig>::FunctionType Getter>
        struct CMethGet : ConstMethodToGetter<T,Sig,Getter> {};
        template <typename Sig, typename MethodSignature<T,Sig>::FunctionType Setter>
        struct MethSet : MethodToSetter<T,Sig,Setter> {};
        template <typename Sig, typename ConstMethodSignature<T,Sig>::FunctionType Setter>
        struct CMethSet : ConstMethodToSetter<T,Sig,Setter> {};
    };
#endif /* ClassAccessor */

} // namespaces
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
#endif /* CODE_GOOGLE_COM_P_V8_CONVERT_PROPERTIES_HPP_INCLUDED */
