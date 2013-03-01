#if !defined(V8_CONVERT_ARGUMENTS_HPP_INCLUDED)
#define V8_CONVERT_ARGUMENTS_HPP_INCLUDED
#include "convert.hpp"
#include <limits>

namespace cvv8 {

    /**
        A functor which fetches an argument by index.
        
        I = the argument index to fetch.
        
        The class is intended mainly to be invoked via code paths 
        selected by template metaprograms, thus the hard-coding of 
        the argument parameter index at compile-time.
    */
    template <int I>
    struct ArgAt
    {
        typedef char AssertIndex[ (I>=0) ? 1 : -1];
        /**
            Returns argv[I] if argv.Length() is <= I, else v8::Undefined()
            is returned.
        */
        inline v8::Handle<v8::Value> operator()( v8::Arguments const & argv ) const
        {
            return (argv.Length() > I) ? argv[I] : v8::Undefined();
        }
    };

    /**
        Functor to fetch an argument and return its result
        as a native value.
        
        I = the argument index to fetch.
        
        T = the native type to convert to. CastFromJS<T>() must be legal.
    */
    template <int I, typename T>
    struct ArgAtCast
    {
        typedef JSToNative<T> J2N;
        typedef typename J2N::ReturnType ReturnType;
        typedef char AssertIndex[ (I>=0) ? 1 : -1];
        /**
            Returns CastFromJS<T>( ArtAt<I>(argv) ).
        */
        inline ReturnType operator()( v8::Arguments const & argv ) const
        {
            typedef ArgAt<I> Proxy;
            return CastFromJS<T>( Proxy()(argv) );
        }
    };

    /**
        Marker class, for documentation purposes.
    */
    struct ValuePredicate
    {
        /**
            Must evaluate the handle and return true or false.
            The framework cannot guaranty that h.IsEmpty() is false,
            so implementations should be in the habit of checking it.
        */
        bool operator()( v8::Handle<v8::Value> const & h ) const;
    };

    /**
        A functor interface for determining if a JS value
        "is-a" value of a particular native type. The default
        implementation is useable for any type for which the
        following is legal:
        
        @code
        T const * t = CastFromJS<T const *>( aV8Handle );
        @endcode
        
        Specializations are used for most data types, but this
        one is fine for client-bound types conformant with
        CastFromJS().

        Note that the default specializations treat T as a plain type,
        discarding const/pointer/reference qualifiers. Certain types may
        require that ValIs<T const &> (and similar) be specialized in order
        to behave properly.        
    */
    template <typename T>
    struct ValIs : ValuePredicate
    {
        typedef T Type;
        /**
            Returns true if v appears to contain a (T*).
        */
        inline bool operator()( v8::Handle<v8::Value> const & v ) const
        {
            return NULL != CastFromJS<T const *>(v);
        }
    };

    //! Specialization to treat (T const) as T.
    template <typename T> struct ValIs<T const> : ValIs<T> {};
    //! Specialization to treat (T const &) as T.
    template <typename T> struct ValIs<T const &> : ValIs<T> {};
    //! Specialization to treat (T *) as T.
    template <typename T> struct ValIs<T *> : ValIs<T> {};
    //! Specialization to treat (T const *) as T.
    template <typename T> struct ValIs<T const *> : ValIs<T> {};

    /**
        Specialization which treats void as the v8::Undefined() value.
    */
    template <>
    struct ValIs<void>
    {
        typedef void Type;
        /**
            Returns true only if h is not empty and h->IsUndefined(). Note
            that an empty handle evaluates to false in this context because
            Undefined is a legal value whereas an empty handle is not.
            (Though Undefined might not be _semantically_ legal in any given
            use case, it is legal to dereference such a handle.)
        */
        inline bool operator()( v8::Handle<v8::Value> const & h ) const
        {
            return h.IsEmpty() ? false : h->IsUndefined();
        }
    };

#if !defined(DOXYGEN)    
    namespace Detail {
        /**
            ValuePredicate impl which returns retrue if
            Getter returns true for the given value.
        
            Getter must be a pointer to one of the v8::Value::IsXXX()
            functions. This functor returns true if the passed-in handle is
            not empty and its IsXXX() function returns true.
        */
        template <bool (v8::Value::*Getter)() const>
        struct ValIs_X : ValuePredicate
        {
            inline bool operator()( v8::Handle<v8::Value> const & v ) const
            {
                return v.IsEmpty() ? false : ((*v)->*Getter)();
            }  
        };

        /**
            A ValuePredicate impl which returns true only if 
            the given handle is-a number and the number is in the 
            inclusive range (std::numeric_limits<NumT>::min .. 
            max()).
        */
        template <typename NumT>
        struct ValIs_NumberStrictRange : ValuePredicate
        {
            typedef NumT Type;
            inline bool operator()( v8::Handle<v8::Value> const & h ) const
            {
                if( h.IsEmpty() || ! h->IsNumber() ) return false;
                else
                {
                    double const dv( h->NumberValue() );
                    return (dv >= std::numeric_limits<NumT>::min())
                        && (dv <= std::numeric_limits<NumT>::max());
                }
            }
        };
        /** Special-case specialization which returns true if the given
            handle is-a Number (without checking the range, which is 
            not necessary for this specific type in this context).
        */
        template <>
        struct ValIs_NumberStrictRange<double> : ValuePredicate
        {
            typedef double Type;
            inline bool operator()( v8::Handle<v8::Value> const & h ) const
            {
                return !h.IsEmpty() && h->IsNumber();
            }
        };
    }
#endif // DOXYGEN

    /** A Value predicate which returns true if its argument is-a Array. */
    struct ValIs_Array :  Detail::ValIs_X<&v8::Value::IsArray> {};
    /** A Value predicate which returns true if its argument is-a Object. */
    struct ValIs_Object : Detail::ValIs_X<&v8::Value::IsObject> {};
    /** A Value predicate which returns true if its argument is-a Boolean. */
    struct ValIs_Boolean : Detail::ValIs_X<&v8::Value::IsBoolean> {};
    /** A Value predicate which returns true if its argument is-a Date. */
    struct ValIs_Date : Detail::ValIs_X<&v8::Value::IsDate> {};
    /** A Value predicate which returns true if its argument is-a External. */
    struct ValIs_External : Detail::ValIs_X<&v8::Value::IsExternal> {};
    /** A Value predicate which returns true if its argument has a false value. */
    struct ValIs_False : Detail::ValIs_X<&v8::Value::IsFalse> {};
    /** A Value predicate which returns true if its argument is-a Function. */
    struct ValIs_Function : Detail::ValIs_X<&v8::Value::IsFunction> {};
    /** A Value predicate which returns true if its argument is-a In32. */
    struct ValIs_Int32 : Detail::ValIs_X<&v8::Value::IsInt32> {};
    /** A Value predicate which returns true if its argument is-a UInt32. */
    struct ValIs_UInt32 : Detail::ValIs_X<&v8::Value::IsUint32 /* Note the "UInt" vs "Uint" descrepancy. i consider Uint to be wrong.*/ > {};
    /** A Value predicate which returns true if its argument is Null (JS null, not C++ NULL). */
    struct ValIs_Null : Detail::ValIs_X<&v8::Value::IsNull> {};
    /** A Value predicate which returns true if its argument has the special Undefined value. */
    struct ValIs_Undefined : Detail::ValIs_X<&v8::Value::IsUndefined> {};
    /** A Value predicate which returns true if its argument is-a Number. */
    struct ValIs_Number : Detail::ValIs_X<&v8::Value::IsNumber> {};
    /** A Value predicate which returns true if its argument is-a RegExp. */
    struct ValIs_RegExp : Detail::ValIs_X<&v8::Value::IsRegExp> {};
    /** A Value predicate which returns true if its argument is-a String. */
    struct ValIs_String : Detail::ValIs_X<&v8::Value::IsString> {};
    /** A Value predicate which returns true if its argument has a true value. */
    struct ValIs_True : Detail::ValIs_X<&v8::Value::IsTrue> {};

    // FIXME: reverse the parent relationships between e.g. ValIs<v8::Array> and ValIs_Array.
    /** A Value predicate which returns true if its argument is a number capable
        of fitting in an int8_t. */
    template <> struct ValIs<int8_t> : Detail::ValIs_NumberStrictRange<int8_t> {};
    /** A Value predicate which returns true if its argument is a number capable
        of fitting in an uint8_t. */
    template <> struct ValIs<uint8_t> : Detail::ValIs_NumberStrictRange<uint8_t> {};
    /** A Value predicate which returns true if its argument is a number capable
        of fitting in an int16_t. */
    template <> struct ValIs<int16_t> : Detail::ValIs_NumberStrictRange<int16_t> {};
    /** A Value predicate which returns true if its argument is a number capable
        of fitting in an uint16_t. */
    template <> struct ValIs<uint16_t> : Detail::ValIs_NumberStrictRange<uint16_t> {};
    /** A Value predicate which returns true if its argument is a number capable
        of fitting in an int32_t. */
    template <> struct ValIs<int32_t> : Detail::ValIs_NumberStrictRange<int32_t> {};
    /** A Value predicate which returns true if its argument is a number capable
        of fitting in an uint32_t. */
    template <> struct ValIs<uint32_t> : Detail::ValIs_NumberStrictRange<uint32_t> {};
    /** A Value predicate which returns true if its argument is a number capable
        of fitting in an int64_t. */
    template <> struct ValIs<int64_t> : Detail::ValIs_NumberStrictRange<int64_t> {};
    /** A Value predicate which returns true if its argument is a number capable
        of fitting in an uint64_t. */
    template <> struct ValIs<uint64_t> : Detail::ValIs_NumberStrictRange<uint64_t> {};
    /** A Value predicate which returns true if its argument is-a Number value. */
    template <> struct ValIs<double> : ValIs_Number {};
    //! Special-case specialization to treat C strings as JS strings.
    template <> struct ValIs<char const *> : ValIs_String {};
    //! Special-case specialization to treat v8 strings as JS strings.
    template <> struct ValIs<std::string> : ValIs_String {};
    //! A Value predicate which returns true if its argument is-a Array. */
    template <> struct ValIs<v8::Array> : ValIs_Array {};
    //! A Value predicate which returns true if its argument is-a Object. */
    template <> struct ValIs<v8::Object> : ValIs_Object {};
    //! A Value predicate which returns true if its argument is-a Boolean. */
    template <> struct ValIs<v8::Boolean> : ValIs_Boolean {};
    //! A Value predicate which returns true if its argument is-a Date. */
    template <> struct ValIs<v8::Date> : ValIs_Date {};
    //! A Value predicate which returns true if its argument is-a External. */
    template <> struct ValIs<v8::External> : ValIs_External {};
    //! A Value predicate which returns true if its argument is-a Function. */
    template <> struct ValIs<v8::Function> : ValIs_Function {};
    //! A Value predicate which returns true if its argument is-a Int32. */
    template <> struct ValIs<v8::Int32> : ValIs_Int32 {};
    //! A Value predicate which returns true if its argument is-a UInt32. */
    template <> struct ValIs<v8::Uint32> : ValIs_UInt32 {};
    //! A Value predicate which returns true if its argument is-a Number. */
    template <> struct ValIs<v8::Number> : ValIs_Number {};
    //! A Value predicate which returns true if its argument is-a RegExp. */
    template <> struct ValIs<v8::RegExp> : ValIs_RegExp {};
    //! A Value predicate which returns true if its argument is-a String. */
    template <> struct ValIs<v8::String> : ValIs_String {};
    //! Specialization to treat Handle<T> as T. */
    template <typename T> struct ValIs< v8::Handle<T> > : ValIs< T > {};
    //! Specialization to treat Local<T> as T. */
    template <typename T> struct ValIs< v8::Local<T> > : ValIs< T > {};
    //! Specialization to treat Persistent<T> as T. */
    template <typename T> struct ValIs< v8::Persistent<T> > : ValIs< T > {};

    /**
        Marker class, mainly for documentation purposes.
        
        Classes matching this concept "evaluate" a v8::Arguments 
        object for validity without actually performing any 
        "application logic." These are intended to be used as 
        functors, primarily triggered via code paths selected by 
        template metaprograms.
        
        They must be default-construcable and should have no
        private state. Their public API consists of only operator().

        This Concept's operator() is intended only to be used for 
        decision-making purposes ("are there enough arguments?" or 
        "are the arguments of the proper types?"), and not 
        higher-level application logic.
    */
    struct ArgumentsPredicate
    {
        /**
            Must "evaluate" the arguments and return true or false. 
        */
        bool operator()( v8::Arguments const & ) const;
    };

    /**
        Functor to evaluate whether an Arguments list
        has a certain range of argument count.
        
        Min is the minimum number. Max is the maximum. The range is 
        inclusive. Use (Max<Min) to mean (at least MIN). Use 
        (Min==Max) to mean only that many arguments.
    */
    template <int Min_, int Max_ = Min_>
    struct Argv_Length : ArgumentsPredicate
    {
    private:
        typedef char AssertMinIsPositive[ (Min_>=0) ? 1 : -1 ];
        enum { Min = Min_, Max = Max_ };
    public:
        /**
            Returns true if av meets the argument count
            requirements defined by the Min and Max
            values.
        */
        bool operator()( v8::Arguments const & av ) const
        {
            
            int const argc = av.Length();
            return (Max < Min)
                ? argc >= Min
                : (argc>=Min) && (argc<=Max);
        }
    };

    /**
        Arguments predicate functor.
    
        Index = arg index to check.
        
        ValIsType must match the ValuePredicate interface.
    */
    template <unsigned short Index, typename ValIsType >
    struct ArgAt_Is : ArgumentsPredicate
    {
        /**
            Returns true if ValType()( av[Index] ) is true.
        */
        inline bool operator()( v8::Arguments const & av ) const
        {
            return (Index >= av.Length())
                ? false
                : ValIsType()( av[Index] );
        }
    };

    /**
        Arguments predicate functor.
    
        Index = arg index to check.
        
        T is a type for which ValIs<T> is legal. The functor
        returns true if ValIs<T> returns true the argument
        at the given index.
    */
    template <unsigned short Index, typename T>
    struct ArgAt_IsA : ArgAt_Is< Index, ValIs<T> > {};

    namespace Detail {
        /**
            Functor which proxies the v8::Value "is-a" functions.
            
            Index is the argument index to check. Getter is the 
            member to be used to perform the is-a evaluation.
        */
        template <unsigned short Index, bool (v8::Value::*Getter)() const>
        struct ArgAt_IsX : ArgumentsPredicate
        {
            /**
                Returns true only if (Index < av.Length())
                and av->Getter() returns true.
            */
            inline bool operator()( v8::Arguments const & av ) const
            {
                return ( av.Length() <= Index )
                    ? false
                    : ((*av[Index])->*Getter)();
            }  
        };
    }

    //! ArgumentsPredicate which returns true if the argument at Index is-a Array. */
    template <unsigned short Index>
    struct ArgAt_IsArray : Detail::ArgAt_IsX<Index, &v8::Value::IsArray> {};

    //! ArgumentsPredicate which returns true if the argument at Index is-a Object. */
    template <unsigned short Index>
    struct ArgAt_IsObject : Detail::ArgAt_IsX<Index, &v8::Value::IsObject> {};

    //! ArgumentsPredicate which returns true if the argument at Index is-a Boolean. */
    template <unsigned short Index>
    struct ArgAt_IsBoolean : Detail::ArgAt_IsX<Index, &v8::Value::IsBoolean> {};

    //! ArgumentsPredicate which returns true if the argument at Index is-a Date. */
    template <unsigned short Index>
    struct ArgAt_IsDate : Detail::ArgAt_IsX<Index, &v8::Value::IsDate> {};

    //! ArgumentsPredicate which returns true if the argument at Index is-a External. */
    template <unsigned short Index>
    struct ArgAt_IsExternal : Detail::ArgAt_IsX<Index, &v8::Value::IsExternal> {};

    //! ArgumentsPredicate which returns true if the argument at Index has a false value. */
    template <unsigned short Index>
    struct ArgAt_IsFalse : Detail::ArgAt_IsX<Index, &v8::Value::IsFalse> {};

    //! ArgumentsPredicate which returns true if the argument at Index is-a Function. */
    template <unsigned short Index>
    struct ArgAt_IsFunction : Detail::ArgAt_IsX<Index, &v8::Value::IsFunction> {};

    //! ArgumentsPredicate which returns true if the argument at Index is-a Int32Boolean. */
    template <unsigned short Index>
    struct ArgAt_IsInt32 : Detail::ArgAt_IsX<Index, &v8::Value::IsInt32> {};

    //! ArgumentsPredicate which returns true if the argument at Index is-a UInt32. */
    template <unsigned short Index>
    struct ArgAt_IsUInt32 : Detail::ArgAt_IsX<Index, &v8::Value::IsUint32> {};

    //! ArgumentsPredicate which returns true if the argument at Index has the special Null value. */
    template <unsigned short Index>
    struct ArgAt_IsNull : Detail::ArgAt_IsX<Index, &v8::Value::IsNull> {};

    //! ArgumentsPredicate which returns true if the argument at Index has the special Undefined value. */
    template <unsigned short Index>
    struct ArgAt_IsUndefined : Detail::ArgAt_IsX<Index, &v8::Value::IsUndefined> {};

    //! ArgumentsPredicate which returns true if the argument at Index is-a Number. */
    template <unsigned short Index>
    struct ArgAt_IsNumber : Detail::ArgAt_IsX<Index, &v8::Value::IsNumber> {};

    //! ArgumentsPredicate which returns true if the argument at Index is-a RegExp. */
    template <unsigned short Index>
    struct ArgAt_IsRegExp : Detail::ArgAt_IsX<Index, &v8::Value::IsRegExp> {};

    //! ArgumentsPredicate which returns true if the argument at Index is-a String. */
    template <unsigned short Index>
    struct ArgAt_IsString : Detail::ArgAt_IsX<Index, &v8::Value::IsString> {};

    //! ArgumentsPredicate which returns true if the argument at Index has a True value. */
    template <unsigned short Index>
    struct ArgAt_IsTrue : Detail::ArgAt_IsX<Index, &v8::Value::IsTrue> {};


    /**
        ArgumentsPredicate functor which always returns true.

        This currently has only one obscure use: as the predicate given to a
        PredicatedInCa in conjunction with an N-arity callback, used as a
        catch-all fallback as the last item in a list of PredicatedInCas
        passed to PredicatedInCaDispatcher. (Got that?)
    */
    struct Argv_True : ArgumentsPredicate
    {
        inline bool operator()( v8::Arguments const & ) const
        {
            return true;
        }
    };
    /** ArgumentsPredicate which always returns false.

        This predicate has only one known, rather obscure use in combination
        with type lists.
    */
    struct Argv_False : ArgumentsPredicate
    {
        inline bool operator()( v8::Arguments const & ) const
        {
            return false;
        }
    };

    /**
        An ArgumentsPredicate implementation which takes
        two ArgumentsPredicate functors as template parameters
        and combines them using an AND operation.

        See Argv_AndN if you just want to combine more than two functors.
        (Argv_AndN can also be used for two functors but is more verbose
        than this form for that case.)
    */
    template <typename ArgPred1, typename ArgPred2>
    struct Argv_And : ArgumentsPredicate
    {
        /**
            Returns true only if ArgPred1()(args) and
            ArgPred2()(args) both return true.
        */
        inline bool operator()( v8::Arguments const & args ) const
        {
            return ArgPred1()( args ) && ArgPred2()( args );
        }
    };

    /**
        The "or" equivalent of Argv_And.

        Use Argv_OrN for chaining more than two predicates.
    */
    template <typename ArgPred1, typename ArgPred2>
    struct Argv_Or : ArgumentsPredicate
    {
        /**
            Returns true only if one of ArgPred1()(args) or
            ArgPred2()(args) return true.
        */
        inline bool operator()( v8::Arguments const & args ) const
        {
            return ArgPred1()( args ) || ArgPred2()( args );
        }
    };

    /**
        This ArgumentsPredicate implementation combines a list of
        other ArgumentsPredicates using an AND operation on the combined
        results of each functor.

        PredList must be a type-list containing ArgumentsPredicate types.
        This functor is a predicate which performs an AND operation on all
        of the predicates in the type list.

        Note that an empty typelist evaluates to True in this context, for
        deeply arcane reasons.

        See Argv_And if you just want to combine two functors
        (it is more succinct for that case).

        Example:

        @code
        // Predicate matching (function, ANYTHING, function) signature:
        typedef Argv_AndN< CVV8_TYPELIST((
            Argv_Length<3>,
            ArgAt_IsFunction<0>,
            ArgAt_IsFunction<2>
        )) > PredFuncXFunc;
        @endcode
    */
    template <typename PredList>
    struct Argv_AndN : ArgumentsPredicate
    {
        /**
            Returns true only if all predicates in PredList
            return true when passed the args object.
        */
        inline bool operator()( v8::Arguments const & args ) const
        {
            typedef typename PredList::Head Head;
            typedef typename tmp::IfElse< tmp::SameType<tmp::NilType,Head>::Value,
                                            Argv_True,
                                            Head>::Type P1;
            typedef typename PredList::Tail Tail;
            typedef Argv_AndN<Tail> P2;
            return P1()( args ) && P2()(args);
        }
    };

    //! End-of-list specialization.
    template <>
    struct Argv_AndN<tmp::nil> : Argv_True {};

    /**
        The "or" equivalent of Argv_AndN.

        When chaining only two predicates Argv_Or offers
        a more succinct equivalent to this type.

        Note that an empty typelist evaluates to False in this context, for
        deeply arcane reasons.
    */
    template <typename PredList>
    struct Argv_OrN : ArgumentsPredicate
    {
        /**
            Returns true only if one of the predicates in PredList
            returns true when passed the args object.
        */
        inline bool operator()( v8::Arguments const & args ) const
        {
            typedef typename PredList::Head P1;
            typedef typename PredList::Tail Tail;
            typedef Argv_OrN<Tail> P2;
            return P1()( args ) || P2()(args);
        }
    };

    //! End-of-list specialization.
    template <>
    struct Argv_OrN<tmp::NilType> : ArgumentsPredicate
    {
        /**
            Always returns false.
        */
        inline bool operator()( v8::Arguments const & ) const
        {
            return false;
        }
    };

    /**
        Intended as a base class for a couple other
        PredicatedXyz types.
        
        This type combines ArgumentsPredicate ArgPred and
        CallT::Call() into one type, for template-based dispatching
        purposes.
        
        ArgPred must be-a ArgumentsPredicate. CallT must
        be a type having a static Call() function taking one
        (v8::Arguments const &) and returning any type.
        
        This type uses subclassing instead of composition
        to avoid having to specify the CallT::Call() return
        type as an additional template parameter. i don't seem to 
        be able to template-calculate it from here. We can't use
        CallT::ReturnType because for most bindings that value
        is different from its Call() return type. We will possibly
        eventually run into a problem with the lack of a way
        to get Call()'s return type.

        @see PredicatedInCaDispatcher
    */
    template <typename ArgPred, typename CallT>
    struct PredicatedInCaLike : ArgPred, CallT {};

    /**
        PredicatedInCa combines an ArgumentsPredicate functor
        (ArgPred) with an InCa type, such that we can create lists
        of functor/callback pairs for use in dispatching callbacks
        based on the results of ArgumentPredicates.

        InCaT must implement the InCa interface.

        This type is primarily intended to be used together with
        PredicatedInCaDispatcher.
        
        @see PredicatedInCaLike

        Reminder to self: this class is only in arguments.hpp, as opposed to
        invocable_core.hpp, because i want (for pedantic
        documentation-related reasons) this class to inherit
        ArgumentsPredicate, which invocable*.hpp does not know about.
        We might want to move ArgumentsPredicate and
        ValuePredicate into convert_core.hpp and move this class back
        into invocable_core.hpp.

        @see PredicatedInCaDispatcher
    */
    template <typename ArgPred, typename InCaT>
    struct PredicatedInCa : PredicatedInCaLike<ArgPred, InCaT> {};

    /**
        This class creates an InvocationCallback which dispatches to one of an
        arbitrarily large set of other InvocationCallbacks, as determined by
        predicate rules.

        PredList must be a type-list (e.g. Signature) of PredicatedInCa
        implementations. See Call() for more details.

        Basic example:

        @code
        // Overloads for 1-3 arguments:
        typedef PredicatedInCa< Argv_Length<1>, ToInCa<...> > Cb1;
        typedef PredicatedInCa< Argv_Length<2>, ToInCa<...> > Cb2;
        typedef PredicatedInCa< Argv_Length<3>, ToInCa<...> > Cb3;
        // Fallback impl for 0 or 4+ args:
        typedef PredicatedInCa< Argv_True, InCaToInCa<my_invocation_callback> > CbN;
        // Side note: this ^^^^^^^^^^^^^^ is the only known use for the
        // Argv_True predicate.

        // Combine them into one InvocationCallback:
        typedef PredicatedInCaDispatcher< CVV8_TYPELIST((
            Cb1, Cb2, Cb3, CbN
        ))> AllOverloads;
        v8::InvocationCallback cb = AllOverloads::Call;
        @endcode
    */
    template <typename PredList>
    struct PredicatedInCaDispatcher
        : InCa
    {
        /**
            For each PredicatedInCa (P) in PredList, if P()(argv)
            returns true then P::Call(argv) is returned, else the next
            predicate in the list is tried.

            If no predicates match then a JS-side exception will be triggered.
        */
        static inline v8::Handle<v8::Value> Call( v8::Arguments const & argv )
        {
            typedef typename PredList::Head Head;
            typedef typename tmp::IfElse< tmp::SameType<tmp::NilType,Head>::Value,
                                            Argv_False,
                                            Head>::Type Ftor;
            typedef typename PredList::Tail Tail;
            typedef char AssertEndOfListCheck[ tmp::SameType<tmp::NilType,Head>::Value
                                        ? (tmp::SameType<tmp::NilType,Tail>::Value ? 1 : -1)
                                        : 1
                                        /* ensures that the Argv_False kludge does not call
                                           any specialization other than the NilType one.
                                        */
                                        ];
            return ( Ftor()( argv ) )
                ? Detail::OverloadCallHelper<Head>::Call( argv )
                : PredicatedInCaDispatcher<Tail>::Call(argv);
        }
    };

    //! End-of-list specialization.
    template <>
    struct PredicatedInCaDispatcher< tmp::NilType > : InCa
    {
        /**
            Triggers a JS-side exception explaining (in English text) that no
            overloads could be matched to the given arguments.
        */
        static inline v8::Handle<v8::Value> Call( v8::Arguments const & argv )
        {
            return Toss(StringBuffer()<<"No predicates in the "
                        << "argument dispatcher matched the given "
                        << "arguments (arg count="<<argv.Length()
                        << ").");
        }
    };

    /**
        The constructor counterpart of PredicatedInCa.
        ArgPred must be an ArgumentsPredicate type and
        FactoryT must be CtorForwarder or interface-compatible.

        We could probably have this take a signature instead of 
        FactoryT, and wrap that signature in a CtorForwarder. That 
        would which would simplify most client code but:

        a) ctor signatures are a bit kludgy to work with at the 
        templates level.

        b) That would preclude the use of ctor-like factories which 
        return subclasses of some base interface type (the 
        parameterized type).
    */
    template <typename ArgPred, typename FactoryT>
    struct PredicatedCtorForwarder
        : PredicatedInCaLike<ArgPred, FactoryT> {};

    /**
        The constructor counterpart of PredicatedInCaDispatcher. 
        PredList must be a typelist of PredicatedCtorForwarder (or 
        interface-compatible) types. The ReturnType part of the 
        typelist must be the base type the constructors in the list 
        can return (they must all share a common base). Clients must 
        not pass the ContextT parameter - it is required internally 
        for handling the end-of-typelist case.
        
        This template instantiates TypeName<PredList::ReturnType>, so
        if that template is to be specialized by client code, it should
        be specialized before this template used.
    */
    template <typename PredList, typename ContextT = typename PredList::ReturnType>
    struct PredicatedCtorDispatcher
    {
        /**
            Force the ContextT into a native handle type.
        */
        typedef typename TypeInfo<ContextT>::NativeHandle ReturnType;
        /**
            For each PredicatedCtorForwarder (P) in PredList, if P()(argv)
            returns true then P::Call(argv) is returned, else the next
            predicate in the list is tried.

            If no predicates match then a JS-side exception will be triggered.
        */
        static ReturnType Call( v8::Arguments const & argv )
        {
            typedef typename PredList::Head Head;
            typedef typename tmp::IfElse< tmp::SameType<tmp::NilType,Head>::Value,
                                            Argv_False,
                                            Head>::Type Ftor;
            typedef typename PredList::Tail Tail;
            return ( Ftor()( argv ) )
                ? Detail::CtorFwdDispatch<typename PredList::ReturnType,Head>::Call( argv )
                : PredicatedCtorDispatcher<Tail,ContextT>::Call(argv);
        }
    };

    //! End-of-list specialization.
    template <typename ContextT>
    struct PredicatedCtorDispatcher< tmp::NilType, ContextT > : Signature< ContextT * () >
    {
        typedef typename TypeInfo<ContextT>::NativeHandle ReturnType;
        /**
            Triggers a native exception explaining (in English text) that no
            overloads could be matched to the given arguments. It's kinda
            cryptic, but we can't be more specific at this level of
            the API.
        */
        static ReturnType Call( v8::Arguments const & argv )
        {
            StringBuffer os;
            os << "No predicates in the "
               << TypeName<ContextT>::Value
               << " ctor argument dispatcher matched the given "
               << "arguments (arg count="<<argv.Length()
               << ").";
            std::string const & str(os.Content());
            throw std::runtime_error(str.c_str());
            return NULL;
        }

    };
}

#endif
