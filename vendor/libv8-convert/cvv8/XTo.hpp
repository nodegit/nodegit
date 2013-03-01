#if !defined (CVV8_TO_X_HPP_INCLUDED)
#define CVV8_TO_X_HPP_INCLUDED
#include "invocable.hpp"
#include "properties.hpp"
/** @file XTo.hpp

    This file provides an alternate approach to the function
    conversion API. It covers:

    - Converting functions and methods to to v8::InvocationCallback,
    v8::AccessorGetter, and v8::AccessorSetter.

    - Converting variables to v8::AccessorGetter and v8::AccessorSetter.

    All conversions of a given category, e.g. FunctionToXYZ or MethodToXYZ
    have a common template, e.g. FunctionTo or MethodTo. The first type
    passed to that template is a "tag" type which tells us what conversion
    to perform. e.g. a function can be used as an v8::InvocationCallback,
    v8::AccessorGetter, or v8::AccessorSetter.

    An example probably explains it best:

    @code
        int aBoundInt = 3;
    void test_to_bindings()
    {
        v8::InvocationCallback cb;
        v8::AccessorGetter g;
        v8::AccessorSetter s;

        using namespace cvv8;

        typedef FunctionTo< InCa, int(char const *), ::puts> FPuts;
        typedef FunctionTo< Getter, int(void), ::getchar> GetChar;
        typedef FunctionTo< Setter, int(int), ::putchar> SetChar;
        cb = FPuts::Call;
        g = GetChar::Get;
        s = SetChar::Set;

        typedef VarTo< Getter, int, &aBoundInt > VarGet;
        typedef VarTo< Setter, int, &aBoundInt > VarSet;
        g = VarGet::Get;
        s = VarSet::Set;
        typedef VarTo< Accessors, int, &aBoundInt > VarGetSet;
        g = VarGetSet::Get;
        s = VarGetSet::Set;

        typedef BoundNative T;
        typedef MethodTo< InCa, const T, int (), &T::getInt > MemInCa;
        typedef MethodTo< Getter, const T, int (), &T::getInt > MemGet;
        typedef MethodTo< Setter, T, void (int), &T::setInt > MemSet;
        cb = MemInCa::Call;
        g = MemGet::Get;
        s = MemSet::Set;
    }
    @endcode

    This unconventional, but nonetheless interesting and arguably
    very readable/writable approach was first proposed by Coen
    Campman.
*/


namespace cvv8 {

    /**
        Base (unimplemented) FunctionTo interface.

        Specializations act as proxies for FunctionToInCa,
        FunctionToGetter and FunctionToSetter. Tag must be one of
        (InCa, InCaVoid, Getter, Setter). The other args are as
        documented for the aforementioned proxied types.

        See FunctionToInCa for more information about the parameters.
    */
    template <typename Tag, typename Sig, typename FunctionSignature<Sig>::FunctionType Func,
              bool UnlockV8 = SignatureIsUnlockable< FunctionSignature<Sig> >::Value >
    struct FunctionTo DOXYGEN_FWD_DECL_KLUDGE;

    //! Behaves like FunctionToInCa.
    template <typename Sig, typename FunctionSignature<Sig>::FunctionType Func, bool UnlockV8>
    struct FunctionTo< InCa, Sig, Func, UnlockV8 > : FunctionToInCa<Sig, Func, UnlockV8>
    {};

    //! Behaves like FunctionToInCaVoid.
    template <typename Sig, typename FunctionSignature<Sig>::FunctionType Func, bool UnlockV8>
    struct FunctionTo< InCaVoid, Sig, Func, UnlockV8 > : FunctionToInCaVoid<Sig, Func, UnlockV8>
    {};

    //! Behaves like FunctionToGetter.
    template <typename Sig, typename FunctionSignature<Sig>::FunctionType Func, bool UnlockV8>
    struct FunctionTo< Getter, Sig, Func, UnlockV8 > : FunctionToGetter<Sig,Func>
    {};

    //! Behaves like FunctionToSetter.
    template <typename Sig, typename FunctionSignature<Sig>::FunctionType Func, bool UnlockV8>
    struct FunctionTo< Setter, Sig, Func, UnlockV8 > : FunctionToSetter<Sig,Func>
    {};

    /** @class VarTo

        Base (unimplemented) VarTo interface.

        Acts as a proxy for VarToGetter and VarToSetter. Tag must be
        one of (Getter, Setter, Accessors). The other args are as
        documented for VarToGetter and VarToSetter.
    */
    template <typename Tag, typename PropertyType, PropertyType * const SharedVar>
    struct VarTo DOXYGEN_FWD_DECL_KLUDGE;

    //! Behaves like VarToGetter.
    template <typename PropertyType, PropertyType * const SharedVar>
    struct VarTo< Getter, PropertyType,SharedVar> : VarToGetter<PropertyType,SharedVar>
    {};

    //! Behaves like VarToSetter.
    template <typename PropertyType, PropertyType * const SharedVar>
    struct VarTo< Setter, PropertyType,SharedVar> : VarToSetter<PropertyType,SharedVar>
    {};

    //! Behaves like VarToAccessors.
    template <typename PropertyType, PropertyType * const SharedVar>
    struct VarTo< Accessors, PropertyType,SharedVar> : VarToAccessors<PropertyType,SharedVar>
    {};

    /**
        Base (unimplemented) type for MemberTo-xxx conversions.

        Acts as a proxy for MemberToGetter, MemberToSetter and
        MemberToAccessors. Tag must be one of (Getter, Setter, Accessors).
        The other args are as documented for MemberToGetter and
        MemberToSetter.
    */
    template <typename Tag, typename T, typename PropertyType, PropertyType T::*MemVar>
    struct MemberTo DOXYGEN_FWD_DECL_KLUDGE;

    //! Behaves like MemberToGetter.
    template <typename T, typename PropertyType, PropertyType T::*MemVar>
    struct MemberTo<Getter, T, PropertyType, MemVar > : MemberToGetter< T, PropertyType, MemVar > {};

    //! Behaves like MemberToSetter.
    template <typename T, typename PropertyType, PropertyType T::*MemVar>
    struct MemberTo<Setter, T, PropertyType, MemVar > : MemberToSetter< T, PropertyType, MemVar > {};

    //! Behaves like MemberToAccessors.
    template <typename T, typename PropertyType, PropertyType T::*MemVar>
    struct MemberTo<Accessors, T, PropertyType, MemVar > : MemberToAccessors< T, PropertyType, MemVar > {};

    /**
        Base (unimplemented) MethodTo interface.

        Acts as a proxy for MethodToInCa, MethodToGetter and
        MethodToSetter (or their const cousins if T is
        const-qualified). Tag must be one of (InCa, InCaVoid,
        Getter, Setter). The other args are as documented for the
        aforementioned proxied types.

        See MethodToInCa for more information about the parameters.
    */
    template <typename Tag, typename T, typename Sig, typename MethodSignature<T,Sig>::FunctionType Func,
              bool UnlockV8 = SignatureIsUnlockable< MethodSignature<T,Sig> >::Value>
    struct MethodTo DOXYGEN_FWD_DECL_KLUDGE;

    //! Behaves like MethodToInCa. For const methods, const-qualify T.
    template <typename T, typename Sig, typename MethodSignature<T,Sig>::FunctionType Func, bool UnlockV8>
    struct MethodTo< InCa, T, Sig, Func, UnlockV8 > : MethodToInCa<T, Sig, Func, UnlockV8>
    {};

    //! Behaves like MethodToInCaVoid. For const methods, const-qualify T.
    template <typename T, typename Sig, typename MethodSignature<T,Sig>::FunctionType Func, bool UnlockV8>
    struct MethodTo< InCaVoid, T, Sig, Func, UnlockV8 > : MethodToInCaVoid<T, Sig, Func, UnlockV8>
    {};

    //! Behaves like MethodToGetter. For const methods, const-qualify T.
    template <typename T, typename Sig, typename MethodSignature<T,Sig>::FunctionType Func, bool UnlockV8>
    struct MethodTo< Getter, T, Sig, Func, UnlockV8 > : MethodToGetter<T,Sig,Func>
    {};

    //! Behaves like MethodToSetter. For const methods, const-qualify T.
    template <typename T, typename Sig, typename MethodSignature<T,Sig>::FunctionType Func, bool UnlockV8>
    struct MethodTo< Setter, T, Sig, Func, UnlockV8 > : MethodToSetter<T,Sig,Func>
    {};

    /**
        Base (unimplemented) FunctorTo interface.

        Behaves like one of the following, depending on the Tag type:

        FunctorToInCa (Tag=InCa), FunctorToInCaVoid (Tag=InCaVoid),
        FunctorToGetter (Tag=Getter), FunctorToSetter (Tag=Setter)

        See FunctorToInCa for more information about the parameters.
    */
    template <typename Tag, typename FtorT, typename Sig,
              bool UnlockV8 = SignatureIsUnlockable< MethodSignature<FtorT,Sig> >::Value
              >
    struct FunctorTo DOXYGEN_FWD_DECL_KLUDGE;

    //! Behaves like FunctorToInCa.
    template <typename FtorT, typename Sig, bool UnlockV8>
    struct FunctorTo< InCa, FtorT, Sig, UnlockV8 > : FunctorToInCa<FtorT,Sig>
    {};

    //! Behaves like FunctorToInCaVoid.
    template <typename FtorT, typename Sig, bool UnlockV8>
    struct FunctorTo< InCaVoid, FtorT, Sig, UnlockV8 > : FunctorToInCaVoid<FtorT,Sig>
    {};

    //! Behaves like FunctorToGetter.
    template <typename FtorT, typename Sig, bool UnlockV8>
    struct FunctorTo< Getter, FtorT, Sig, UnlockV8 > : FunctorToGetter<FtorT,Sig>
    {};

    //! Behaves like FunctorToSetter.
    template <typename FtorT, typename Sig, bool UnlockV8>
    struct FunctorTo< Setter, FtorT, Sig, UnlockV8 > : FunctorToSetter<FtorT,Sig>
    {};
}
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

#endif /* CVV8_TO_X_HPP_INCLUDED */
