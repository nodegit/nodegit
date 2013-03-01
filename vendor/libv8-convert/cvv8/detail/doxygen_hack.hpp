/** @file doxygen_hack.hpp

    doxygen REFUSES to document class templates i forward-decl, even when
    adding "@class" to the docs. Being the doc-pedant that i am, i felt
    compelled to work around that. With gcc we can add an empty body to
    those classes but MSVC doesn't like it. So we leave it out unless
    doxygen is building, and then doxygen needs to be configured with:

    - ENABLE_PREPROCESSING = YES
    - EXPAND_AS_DEFINED = DOXYGEN_FWD_DECL_KLUDGE

    and possibly:

    - MACRO_EXPANSION = YES

    and/or:

    - EXPAND_ONLY_PREDEF = YES
*/
#if !defined(DOXYGEN)
#  if !defined DOXYGEN_FWD_DECL_KLUDGE
#    define DOXYGEN_FWD_DECL_KLUDGE
#  endif
#else
#  if !defined DOXYGEN_FWD_DECL_KLUDGE
#    define DOXYGEN_FWD_DECL_KLUDGE {}
#  endif
#endif
