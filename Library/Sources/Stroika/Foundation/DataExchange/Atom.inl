/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Atom_inl_
#define _Stroika_Foundation_DataExchange_Atom_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <type_traits>

#include "../Common/Compare.h"

namespace Stroika::Foundation::DataExchange {

    /*
     ********************************************************************************
     ****************************** Atom<ATOM_MANAGER> ******************************
     ********************************************************************************
     */
    template <typename ATOM_MANAGER>
    inline constexpr Atom<ATOM_MANAGER>::Atom ()
        : fValue_{ATOM_MANAGER::kEmpty}
    {
    }
    template <typename ATOM_MANAGER>
    inline Atom<ATOM_MANAGER>::Atom (const wstring& src)
        : fValue_{ATOM_MANAGER::Intern (src)}
    {
    }
    template <typename ATOM_MANAGER>
    inline Atom<ATOM_MANAGER>::Atom (const wstring_view& src)
        : fValue_{ATOM_MANAGER::Intern (src)}
    {
    }
    template <typename ATOM_MANAGER>
    inline Atom<ATOM_MANAGER>::Atom (const wchar_t* src)
        : fValue_{ATOM_MANAGER::Intern (src)}
    {
    }
    template <typename ATOM_MANAGER>
    inline Atom<ATOM_MANAGER>::Atom (const String& src)
        : fValue_{ATOM_MANAGER::Intern (src)}
    {
    }
    template <typename ATOM_MANAGER>
    inline constexpr Atom<ATOM_MANAGER>::Atom (const AtomInternalType& src)
        : fValue_{src}
    {
        // IF NOT CONSTEXPR (can do in C++20) validate src is already in ATOM_MANAGER
    }
    template <typename ATOM_MANAGER>
    inline String Atom<ATOM_MANAGER>::GetPrintName () const
    {
        return fValue_ == ATOM_MANAGER::kEmpty ? String{} : ATOM_MANAGER::Extract (fValue_);
    }
    template <typename ATOM_MANAGER>
    inline constexpr bool Atom<ATOM_MANAGER>::empty () const
    {
        return fValue_ == ATOM_MANAGER::kEmpty;
    }
    template <typename ATOM_MANAGER>
    inline void Atom<ATOM_MANAGER>::clear ()
    {
        fValue_ = ATOM_MANAGER::kEmpty;
    }
    template <typename ATOM_MANAGER>
    template <typename T>
    inline T Atom<ATOM_MANAGER>::As () const
    {
        return As_ (type_<T> ());
    }
    template <typename ATOM_MANAGER>
    inline String Atom<ATOM_MANAGER>::As_ (type_<String>) const
    {
        return GetPrintName ();
    }
    template <typename ATOM_MANAGER>
    inline wstring Atom<ATOM_MANAGER>::As_ (type_<wstring>) const
    {
        return GetPrintName ().template As<wstring> ();
    }
    template <typename ATOM_MANAGER>
    inline typename Atom<ATOM_MANAGER>::AtomInternalType Atom<ATOM_MANAGER>::_GetInternalRep () const
    {
        return fValue_;
    }
    template <typename ATOM_MANAGER>
    inline String Atom<ATOM_MANAGER>::ToString () const
    {
        return GetPrintName ();
    }
#if __cpp_impl_three_way_comparison >= 201907
    template <typename ATOM_MANAGER>
    inline strong_ordering Atom<ATOM_MANAGER>::operator<=> (const Atom& rhs) const
    {
        return fValue_ <=> rhs.fValue_;
    }
    template <typename ATOM_MANAGER>
    inline bool Atom<ATOM_MANAGER>::operator== (const Atom& rhs) const
    {
        return fValue_ == rhs.fValue_;
    }
#endif

#if __cpp_impl_three_way_comparison < 201907
    /*
     ********************************************************************************
     ****************************** Atom operators **********************************
     ********************************************************************************
     */
    template <typename ATOM_MANAGER>
    inline bool operator< (Atom<ATOM_MANAGER> lhs, Atom<ATOM_MANAGER> rhs)
    {
        return lhs.fValue_ < rhs.fValue_;
    }
    template <typename ATOM_MANAGER>
    inline bool operator<= (Atom<ATOM_MANAGER> lhs, Atom<ATOM_MANAGER> rhs)
    {
        return lhs.fValue_ <= rhs.fValue_;
    }
    template <typename ATOM_MANAGER>
    inline bool operator== (Atom<ATOM_MANAGER> lhs, Atom<ATOM_MANAGER> rhs)
    {
        return lhs.fValue_ == rhs.fValue_;
    }
    template <typename ATOM_MANAGER>
    inline bool operator!= (Atom<ATOM_MANAGER> lhs, Atom<ATOM_MANAGER> rhs)
    {
        return lhs.fValue_ != rhs.fValue_;
    }
    template <typename ATOM_MANAGER>
    inline bool operator>= (Atom<ATOM_MANAGER> lhs, Atom<ATOM_MANAGER> rhs)
    {
        return lhs.fValue_ >= rhs.fValue_;
    }
    template <typename ATOM_MANAGER>
    inline bool operator> (Atom<ATOM_MANAGER> lhs, Atom<ATOM_MANAGER> rhs)
    {
        return lhs.fValue_ > rhs.fValue_;
    }
#endif

}

#endif /*_Stroika_Foundation_DataExchange_Atom_inl_*/
