#ifndef CODE_GOOGLE_COM_P_V8_CONVERT_TMP_HPP_INCLUDED
#define CODE_GOOGLE_COM_P_V8_CONVERT_TMP_HPP_INCLUDED

namespace cvv8 {
/**
   The tmp namespace contains code related to template 
   metaprogramming, a-la Alexandrescu's Loki library or Boost MPL.

   All of it is independent of the core library.
   
   This is not a complete/full-featured TMP library - it only 
   contains the functions needed by our library-level code.
*/
namespace tmp {

    struct NilType {};
    typedef NilType nil;
    

    /**
       An utmost most-basic compile-time assertion template.
       If Condition is false, an incomplete specialization of
       this type is invoked, causing a compile-time error.
    */
    template <bool Condition>
    struct Assertion
    {
        enum { Value = 1 };
    };
    /** Unimplemented - causes a compile-time error if used. */
    template <>
    struct Assertion<false>;


    /** A convenience base type for metafunctions holding a constant value. */
    template <typename ValType,ValType Val>
    struct ConstVal
    {
        static const ValType Value = Val;
    };
    /** A metafunction holding an integer constant. */
    template <int Val>
    struct IntVal : ConstVal<int,Val> {};
    /** A metafunction holding an unsigned short constant. */
    template <unsigned short Val>
    struct UShortVal : ConstVal<unsigned short,Val> {};
    /** A metafunction holding a bool constant. */
    template <bool Val>
    struct BoolVal : ConstVal<bool,Val> {};

    /** A metatype whos Value member is true if X and Y are the same type. */
    template <typename X,typename Y>
    struct SameType : BoolVal<false> {};
    /** Specialization for X==Y. */
    template <typename X>
    struct SameType<X,X> : BoolVal<true> {};

    template <typename T>
    struct Identity
    {
        typedef T Type;
    };
    
    template <typename T>
    struct PlainType
    {
        typedef T Type;
    };
    template <typename T>
    struct PlainType<T const> : PlainType<T> {};
    template <typename T>
    struct PlainType<T *> : PlainType<T> {};
    template <typename T>
    struct PlainType<T const *> : PlainType<T> {};

#if 0
    /** Metatemplate whose Type evaluates to (T*). */
    template <typename T>
    struct AddPointer
    {
        typedef T * Type;
    };
    /** Specialization whose Type evaluates to (T *). */
    template <typename T>
    struct AddPointer<T *>
    {
        typedef T * Type;
    };
    /** Specialization whose Type evaluates to (T const *). */
    template <typename T>
    struct AddPointer<T const *>
    {
        typedef T const * Type;
    };

    //! Unimplemented. How to handle this?
    template <typename T>
    struct AddPointer<T const &>;
    //! Unimplemented. How to handle this?
    template <typename T>
    struct AddPointer<T &>;
#endif

    template <typename T>
    struct IsConst : BoolVal<false> {};
    template <typename T>
    struct IsConst<T const> : BoolVal<true> {};
    template <typename T>
    struct IsConst<T const &> : IsConst<T const> {};
    template <typename T>
    struct IsConst<T const *> : IsConst<T const> {};


    template <typename T>
    struct IsNil : SameType<T,NilType> {};



}} // namespaces
#endif // CODE_GOOGLE_COM_P_V8_CONVERT_TMP_HPP_INCLUDED
