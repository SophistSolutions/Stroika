/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <type_traits>

#include "Stroika/Foundation/Common/Compare.h"

namespace Stroika::Foundation::DataExchange {

    /*
     ********************************************************************************
     ****************************** Atom<ATOM_MANAGER> ******************************
     ********************************************************************************
     */
    template <typename ATOM_MANAGER>
    inline constexpr Atom<ATOM_MANAGER>::Atom () noexcept
        : fValue_{ATOM_MANAGER::kEmpty}
    {
    }
    template <typename ATOM_MANAGER>
    template <Characters::IConvertibleToString STRING_LIKE>
    inline Atom<ATOM_MANAGER>::Atom (STRING_LIKE&& src)
        : fValue_{ATOM_MANAGER::Intern (forward<STRING_LIKE> (src))}
    {
    }
    template <typename ATOM_MANAGER>
    inline constexpr Atom<ATOM_MANAGER>::Atom (AtomInternalType src) noexcept
        : fValue_{src}
    {
        // IF NOT CONSTEXPR validate src is already in ATOM_MANAGER
#if qDebug
        if (not is_constant_evaluated ()) {
            (void)ATOM_MANAGER::Extract (src); // for side-effect - assertion valid index
        }
#endif
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
    template <Configuration::IAnyOf<String, wstring, typename ATOM_MANAGER::AtomInternalType> T>
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
    inline auto Atom<ATOM_MANAGER>::As_ (type_<AtomInternalType>) const -> AtomInternalType
    {
        return fValue_;
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

}
