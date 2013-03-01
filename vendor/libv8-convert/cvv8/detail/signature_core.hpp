#if !defined(CODE_GOOGLE_COM_V8_CONVERT_SIGNATURE_CORE_HPP_INCLUDED)
#define CODE_GOOGLE_COM_V8_CONVERT_SIGNATURE_CORE_HPP_INCLUDED 1
#include "tmp.hpp"
#include "doxygen_hack.hpp"

namespace cvv8 {
/** @file signature_core.hpp

This file houses the core-most templates related to handling
function/method signatures as full-fleged types.
*/



/** @class Signature

    Base (unimplemented) template for holding function-signature-style
    argument lists in a type-rich manner. Most implementations are
    script-generated and accept up to some library-build-time-defined number
    of types in their argument list (the interface guarantees at least 10
    unless the client builds a custom copy with a smaller limit).

    All specializations implement a "type list" interface. The sl namespace
    contains several different compile-time algorithms (sometimes called
    template metafunctions) for querying the arity and types in a signature.
    
    Sig must be a function-signature-style parameter list, e.g.:
    
    @code
    Signature< void (int, double) >
    Signature< void (MyType::*)( int double ) >
    Signature< void (MyType::*)( char const * ) const >
    @endcode

    This interface treates the "function parameter part" of its arguments as
    a "type list", and several algorithms in the sl namespace are available
    for fetching type information from a Signature type. This is an
    "extended" typelist, however, because it also remembers the return type
    and optionally the class containing a member function described by the
    signature (neither of those are counted as part of the list, but are
    accessible separately).

    Required interface for specializations:

    @code
    typedef functionSignature FunctionType; // e.g. void () or void (T::*)() const.
    typedef functionReturnType ReturnType;
    typedef T Context; // void for non-member functions, non-vp-qualified T
                       // for all T members. const-qualified T for const members.
    typedef firstArgType Head; // head type of type-list.
    typedef Signature< RV (...)> Tail; // tail of type-list. (...)==arg types 2..N.
    // When Arity==0, Head and Tail must both be tmp::NilType. For Arity==1
    // Tail is Signature<RV()> but one could argue that it should be tmp::NilType.
    
    @endcode

    It is intended to be used like this:
    
    @code
    typedef Signature< int (char const *, double) > Sig;
    assert( 2 == sl::Arity<Sig>::Value );
    assert( 2 == sl::Length<Sig>::Value )
    assert( (tmp::SameType< char const *, sl::At<0,Sig>::Type >::Value) );
    assert( (tmp::SameType< double, sl::At<1,Sig>::Type >::Value) );
    assert( 1 == sl::Index< double, Sig >::Value) );
    assert( !sl::Contains< int, Sig >::Value) ); // Sig::ReturnType doesn't count here!
    assert( sl::IsConstMethod< Signature< void (MyType::*)() const > >::Value );
    assert( sl::IsMethod< Signature< void (MyType::*)() const > >::Value );
    assert( !sl::IsConstMethod< Signature< void (MyType::*)() > >::Value );
    // Those can all be made into static assertions, by the way.
    @endcode
    
    Note that the length of the typelist does not include the return value
    type nor (for member methods) the containing class (the Context typedef).

    Functions taking one (v8::Arguments const &) argument and 
    returning any type are considered to be 
    "InvocationCallback-like" and are treated specially. They have 
    an Arity value of -1, which is used as a hint by binding code 
    that the function can accept any number of arguments when called 
    from JS code (there is no way to create Arguments objects 
    directly from C++, only indirectly via Call()ing or Apply()ing a 
    v8::Function).
*/
template <typename Sig> struct Signature DOXYGEN_FWD_DECL_KLUDGE;

/** @def CVV8_TYPELIST

    CVV8_TYPELIST is a (slightly) convenience form of
    Signature for creating typelists where we do not care about the
    "return type" or "context" parts of the list.

    It is used like this:

    @code
    typedef CVV8_TYPELIST(( int, double, char )) MyList;
    @endcode

    NOTE the doubled parenthesis!

    Many members of the API which take a type-list require a
    Signature-compatible typelist because they need the ReturnValue and/or
    Context parts. CVV8_TYPELIST is intended for cases where niether the
    ReturnValue nor Context are evaluated (both are void for CVV8_TYPELIST).

    The maximum number of types the typelist can hold is limited
    to some build-time configuration option.
*/
#define CVV8_TYPELIST(X) ::cvv8::Signature< void (*)X >

/** \namespace cvv8::sl

    The sl namespace exclusively holds template metafunctions for working
    with Signature-style typelists.
*/
namespace sl {

    /**
        Metafunction whose Value member evaluates to the length of the
        given Signature.
    */
    template < typename ListT >
    struct Length : tmp::IntVal<
            tmp::IsNil<typename ListT::Head>::Value ? 0 : (1 + Length<typename ListT::Tail>::Value)
            > {};

    //! End-of-list specialization.
    template <>
    struct Length<tmp::nil> : tmp::IntVal<0> {};
    /**
        Metafunction whose Type member evaluates to the type of the
        I'th argument type in the given Signature. Fails to compile
        if I is out of range.
    */
    template < unsigned short I, typename ListT >
    struct At : At<I-1, typename ListT::Tail>
    {
        typedef char AssertIndex[ (I >= Length<ListT>::Value) ? -1 : 1 ];
    };

    //! Beginning-of-list specialization.
    template < typename ListT >
    struct At<0, ListT>
    {
         typedef typename ListT::Head Type;
    };
    /**
        End-of-list specialization. i don't think we need this, actually.
    */
    template <unsigned short I>
    struct At<I, tmp::nil> : tmp::Identity<tmp::nil>
    {};

    /**
        Metafunction whose Type Value member evaluates to the 0-based
        index of the first occurrance of the the type T in the 
        given Signature's argument list. Evaluates to -1 if T is not 
        contained in the argument list. Signature::ReturnType and 
        Signature::Context are not evaluated.

        Clients _must not_ pass a value for the 3rd template parameter.
    */
    template < typename T, typename ListT, unsigned short Internal = 0 >
    struct Index : tmp::IntVal< tmp::SameType<T, typename ListT::Head>::Value
                            ? Internal
                            : Index<T, typename ListT::Tail, Internal+1>::Value>
    {
    };

    //! End-of-list specialization.
    template < typename T, unsigned short Internal >
    struct Index<T,tmp::nil,Internal> : tmp::IntVal<-1> {};
    
    /**
        Convenience form of Index<T,ListT> which evaluates to true 
        if Index returns a non-negative value, else it evaluates
        to false.        
    */
    template < typename T, typename ListT>
    struct Contains : tmp::BoolVal< Index<T,ListT>::Value >= 0  > {};
    
    
    /**
        A metatype which calculates the number of arguments in the given
        typelist, but evaluates to -1 if SigT's only argument is
        (v8::Arguments const &), as such function signatures are considered
        to be n-arity.
    */
    template <typename SigT>
    struct Arity
    {
        enum {
            Value = ((1==Length<SigT>::Value)
                    && (0==Index<v8::Arguments const &,SigT>::Value))
                    ? -1
                    : Length<SigT>::Value
        };
    };
    
    /**
        This metafunction evaluates to true if SigT appears to be
        "InvocationCallback-like" (returns any type and takes one
        (v8::Arguments const &) parameter).
        
        We could implement this a number of different ways. The 
        current impl simply checks if the arity is -1.
    */
    template <typename SigT>
    struct IsInCaLike : tmp::BoolVal< -1 == Arity<SigT>::Value > {};

    /**
        A metafunction which has a true Value if the Signature type SigT
        represents a non-member function.
    */
    template <typename SigT>
    struct IsFunction : tmp::BoolVal< tmp::SameType<void, typename SigT::Context>::Value > {};

    /**
        A metafunction which has a true Value if the Signature type SigT
        represents a member function (const or not).
    */
    template <typename SigT>
    struct IsMethod : tmp::BoolVal< !tmp::SameType<void, typename SigT::Context>::Value > {};

    /**
        A metafunction which has a true Value if the Signature type SigT
        represents a non-const member function.
    */
    template <typename SigT>
    struct IsNonConstMethod : tmp::BoolVal< !tmp::IsConst< typename SigT::Context >::Value && IsMethod<SigT>::Value > {};

    /**
        A metafunction which has a true Value if the Signature type SigT
        represents a const member function.
    */
    template <typename SigT>
    struct IsConstMethod :
        tmp::BoolVal< tmp::IsConst< typename SigT::Context >::Value && IsMethod<SigT>::Value > {};
        //tmp::BoolVal< SigT::IsConst && IsMethod<SigT>::Value > {};

}

namespace tmp {
    /**
       A metatemplate who's Type member resolves to IF if Cond is
       true, or ELSE if Cond is false. Its Value member evaluates
       to 1 or 0, accordingly.
    */
    template <bool Cond, typename IF, typename ELSE>
    struct IfElse : sl::At< Cond ? 0 : 1, Signature<void (IF,ELSE)> >
    {
    };
}

#if 0
//! Highly arguably specialization.
template <typename Sig> struct Signature< Signature<Sig> > : Signature<Sig> {};
#endif

/**
    Specialization to give "InvacationCallback-like" functions
    an Arity value of -1.

    Reminder: we can get rid of this if we factory out the Arity definition
    and use sl::Arity instead.
*/
template <typename RV>
struct Signature<RV (v8::Arguments const &)>
{
    typedef RV ReturnType;
    typedef RV (*FunctionType)(v8::Arguments const &);
    typedef void Context;
    typedef v8::Arguments const & Head;
    typedef Signature<RV ()> Tail;
};

template <typename RV>
struct Signature<RV (*)(v8::Arguments const &)> : Signature<RV (v8::Arguments const &)>
{};

template <typename T, typename RV>
struct Signature<RV (T::*)(v8::Arguments const &)> : Signature<RV (v8::Arguments const &)>
{
    typedef T Context;
    typedef RV (Context::*FunctionType)(v8::Arguments const &);
};


template <typename T, typename RV>
struct Signature<RV (T::*)(v8::Arguments const &) const> : Signature<RV (v8::Arguments const &)>
{
    typedef T const Context;
    typedef RV (Context::*FunctionType)(v8::Arguments const &) const;
};



/** @class FunctionSignature
   Base (unimplemented) signature for FunctionSignature
   specializations. The type passed to it must be a function
   signature.

   All implementations must define the interface described for 
   Signature and its Context typedef must be void for this type.

   Examples:

   @code
   // void func_foo():
   typedef FunctionSignature< void () > NoArgsReturnsVoid;

   // int func_foo(double):
   typedef FunctionSignature< int (double) > OneArgReturnsInt;

   // double func_foo(int,int):
   typedef FunctionSignature< double (int,int) > TwoArgsReturnsDouble;
   @endcode
   
*/
template <typename FunctionSig>
struct FunctionSignature : Signature< FunctionSig > {};

/** @class MethodSignature
   Base (unimplemented) signature for MethodSignature
   specializations. The Sig type passed to it must match a member method
   signature of a function from the class T.
   e.g. (void (T::*)(int)) or its equivalent (void (int)).

   All implementations must have the interface called for by Signature
   and the Context typedef must be non-cvp-qualified T.

   Examples: 

   @code
   // void MyType::func():
   typedef MethodSignature< MyType, void () > NoArgsReturnsVoid;

   // int MyType::func(double):
   typedef MethodSignature< MyType, int (double) > OneArgReturnsInt;

   // double MyType::func(int,int):
   typedef MethodSignature< MyType, double (int,int) > TwoArgsReturnsDouble;
   @endcode

   As of r2019 (20110723), MethodSignature<T const,...> and 
   ConstMethodSignature<T,...> are equivalent.

   Reminders to self:

   i would really like this class to simply subclass Signature<Sig> and we
   would add in a couple typedefs we need. This would cut the specializations
   we generate. However, i don't know how to make this work. The problems
   include:

   - i can't "refactor" Signature<Sig>::FunctionType to the proper type
   at this level.
*/
template <typename T, typename Sig>
struct MethodSignature DOXYGEN_FWD_DECL_KLUDGE;

/** @class ConstMethodSignature
   Base (unimplemented) signature for ConstMethodSignature
   specializations. The Sig type passed to it must be a member
   method signature of a const function from the class T.
   e.g. (void (T::*)(int) const)

   All implementations must have the interface called for by Signature
   and the Context typedef must be non-cvp-qualified T. The IsConst
   member (enum or static/const boolean) must be a true value.

   Examples: 

   @code
   // void MyType::func() const:
   typedef ConstMethodSignature< MyType, void () > NoArgsReturnsVoid;

   // int MyType::func(double) const:
   typedef ConstMethodSignature< MyType, int (double) > OneArgReturnsInt;

   // double MyType::func(int,int) const:
   typedef ConstMethodSignature< MyType, double (int,int) > TwoArgsReturnsDouble;
   @endcode

    As of r2019 (20110723), MethodSignature<T const,...> and 
    ConstMethodSignature<T,...> are equivalent.
*/
template <typename T, typename Sig>
struct ConstMethodSignature DOXYGEN_FWD_DECL_KLUDGE;
//template <typename T, typename Sig>
//struct ConstMethodSignature<T const, Sig> : ConstMethodSignature<T,Sig> {};

template <typename T, typename RV >
struct MethodSignature< T, RV () > : Signature< RV () >
{
    typedef T Context;
    typedef RV (Context::*FunctionType)();
};


template <typename T, typename RV >
struct MethodSignature< T, RV (T::*)() > : MethodSignature<T, RV ()>
{
};

template <typename T, typename RV >
struct MethodSignature< T const, RV () > : ConstMethodSignature< T, RV () >
{};

template <typename T, typename RV >
struct MethodSignature< T const, RV (T::*)() > : MethodSignature<T const, RV ()>
{
};
#if 1 //msvc?
template <typename T, typename RV >
struct MethodSignature< T const, RV (T::*)() const > : MethodSignature<T const, RV ()>
{
};
#endif

template <typename T, typename RV >
struct ConstMethodSignature< T, RV () > : Signature< RV (T::*)() const >
{
    typedef T const Context;
    typedef RV (Context::*FunctionType)() const;
};
template <typename T, typename RV >
struct ConstMethodSignature< T, RV (T::*)() const > : ConstMethodSignature<T const, RV ()>
{
};


/**
   A "type-rich" function pointer.

   Sig must be a function signature type usable in the construct
   FunctionSignature<Sig>. FuncPtr must be a function of that type.
*/
template <typename Sig, typename FunctionSignature<Sig>::FunctionType FuncPtr>
struct FunctionPtr : FunctionSignature<Sig>
{
    /**
       This type's full "signature" type.
    */
    typedef FunctionSignature<Sig> SignatureType;
    /**
       The data type of FuncPtr.
     */
    typedef typename SignatureType::FunctionType FunctionType;
    
    /** The function specifies in the template arguments. */
    static const FunctionType Function;
};
template <typename Sig, typename FunctionSignature<Sig>::FunctionType FuncPtr>
typename FunctionPtr<Sig,FuncPtr>::FunctionType const FunctionPtr<Sig,FuncPtr>::Function = FuncPtr;

/**
   Used like FunctionPtr, but in conjunction with non-const
   member functions ("methods") of the T class. See FunctionPtr
   for the requirements of the Sig type.
*/
template <typename T, typename Sig, typename MethodSignature<T,Sig>::FunctionType FuncPtr>
struct MethodPtr : MethodSignature<T,Sig>
{
    typedef MethodSignature<T,Sig> SignatureType;
    typedef typename SignatureType::FunctionType FunctionType;
    static const FunctionType Function;
};
template <typename T, typename Sig, typename MethodSignature<T,Sig>::FunctionType FuncPtr>
typename MethodPtr<T,Sig,FuncPtr>::FunctionType const MethodPtr<T,Sig,FuncPtr>::Function = FuncPtr;
/**
   Used like MethodPtr, but in conjunction with const methods of the T
   class.
*/
template <typename T, typename Sig, typename ConstMethodSignature<T,Sig>::FunctionType FuncPtr>
struct ConstMethodPtr : ConstMethodSignature<T,Sig>
{
    typedef ConstMethodSignature<T,Sig> SignatureType;
    typedef typename SignatureType::FunctionType FunctionType;
    static const FunctionType Function;
};
template <typename T, typename Sig, typename ConstMethodSignature<T,Sig>::FunctionType FuncPtr>
typename ConstMethodPtr<T,Sig,FuncPtr>::FunctionType const ConstMethodPtr<T,Sig,FuncPtr>::Function = FuncPtr;

#if 0 //!msvc
//! Specialization to treat (const T) as ConstMethodPtr.
template <typename T, typename Sig, typename MethodSignature<T,Sig>::FunctionType FuncPtr>
struct MethodPtr<T const, Sig, FuncPtr> : ConstMethodPtr<T,Sig,FuncPtr>
{
    typedef MethodSignature<T,Sig> SignatureType;
    typedef typename SignatureType::FunctionType FunctionType;
    static const FunctionType Function;
};
#endif

#include "signature_generated.hpp"
} // namespaces

#endif /* CODE_GOOGLE_COM_V8_CONVERT_SIGNATURE_CORE_HPP_INCLUDED */
