#if !defined(BOOST_PP_IS_ITERATING)
#  include <boost/preprocessor/repetition.hpp>
#  include <boost/preprocessor/punctuation/comma_if.hpp>
#  include <boost/preprocessor/iteration/iterate.hpp>
#  include <boost/preprocessor/repetition/enum_params.hpp>
#  include <boost/preprocessor/repetition/enum_shifted_params.hpp>
#  if !defined(CVV8_PP_ITER_MAX)
#    error "Define CVV8_PP_ITER_MAX before including this file."
#  endif
#  if !defined(CVV8_SIGNATURE_GENERATED_HPP_INCLUDED)
#       define BOOST_PP_ITERATION_LIMITS (0, CVV8_PP_ITER_MAX)
#       define BOOST_PP_FILENAME_1       "Signature.hpp"
#       include BOOST_PP_ITERATE()
#  endif /* include guard */
#else
#define n BOOST_PP_ITERATION()

template <typename RV BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, class A)>
struct Signature< RV (BOOST_PP_ENUM_PARAMS(n, A)) >
{
    typedef RV ReturnType;
    static const bool IsConst = false;
    typedef void Context;
    typedef RV (*FunctionType)(BOOST_PP_ENUM_PARAMS(n, A));
#if n > 0
    typedef A0 Head;
    typedef Signature< RV (BOOST_PP_ENUM_SHIFTED_PARAMS(n, A)) > Tail;
#else
    typedef tmp::NilType Head;
    typedef Head Tail;
#endif
};

template <typename RV BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, class A)>
struct Signature< RV (*)(BOOST_PP_ENUM_PARAMS(n, A)) >
    : Signature< RV (BOOST_PP_ENUM_PARAMS(n, A)) >
{};

//! Non-const method.
template <typename T, typename RV BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, class A)>
struct Signature< RV (T::*)(BOOST_PP_ENUM_PARAMS(n, A)) >
    : Signature< RV (BOOST_PP_ENUM_PARAMS(n, A))>
{
    typedef T Context;
    typedef RV (Context::*FunctionType)(BOOST_PP_ENUM_PARAMS(n, A));
#if n > 0
    typedef A0 Head;
    typedef Signature< RV (T::*)(BOOST_PP_ENUM_SHIFTED_PARAMS(n, A)) > Tail;
#else
    typedef tmp::NilType Head;
    typedef Head Tail;
#endif
};

//! Const method.
template <typename T, typename RV BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, class A)>
struct Signature< RV (T::*)(BOOST_PP_ENUM_PARAMS(n, A)) const >
    : Signature< RV (BOOST_PP_ENUM_PARAMS(n, A))>
{
    typedef T const Context;
    typedef RV (Context::*FunctionType)(BOOST_PP_ENUM_PARAMS(n, A)) const;
    static const bool IsConst = true;
#if n > 0
    typedef A0 Head;
    typedef Signature< RV (T::*)(BOOST_PP_ENUM_SHIFTED_PARAMS(n, A)) const > Tail;
#else
    typedef tmp::NilType Head;
    typedef Head Tail;
#endif
};

#endif /* BOOST_PP_IS_ITERATING */
