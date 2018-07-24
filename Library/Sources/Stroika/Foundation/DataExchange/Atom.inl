/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
#include "../Execution/ModuleInit.h"

namespace Stroika::Foundation::DataExchange {

    /*
        ********************************************************************************
        ****************************** Atom<ATOM_MANAGER> ******************************
        ********************************************************************************
        */
    template <typename ATOM_MANAGER>
    inline constexpr Atom<ATOM_MANAGER>::Atom ()
        : fValue_ (ATOM_MANAGER::kEmpty)
    {
    }
    template <typename ATOM_MANAGER>
    inline Atom<ATOM_MANAGER>::Atom (const wstring& src)
        : fValue_ (ATOM_MANAGER::Intern (src))
    {
    }
    template <typename ATOM_MANAGER>
    inline Atom<ATOM_MANAGER>::Atom (const wchar_t* src)
        : fValue_ (ATOM_MANAGER::Intern (src))
    {
    }
    template <typename ATOM_MANAGER>
    inline Atom<ATOM_MANAGER>::Atom (const String& src)
        : fValue_ (ATOM_MANAGER::Intern (src))
    {
    }
    template <typename ATOM_MANAGER>
    inline constexpr Atom<ATOM_MANAGER>::Atom (const _AtomInternalType& src)
        : fValue_ (src)
    {
    }
    template <typename ATOM_MANAGER>
    inline String Atom<ATOM_MANAGER>::GetPrintName () const
    {
        return fValue_ == ATOM_MANAGER::kEmpty ? String () : ATOM_MANAGER::Extract (fValue_);
    }
    template <typename ATOM_MANAGER>
    inline int Atom<ATOM_MANAGER>::Compare (Atom rhs) const
    {
        return Common::ThreeWayCompareNormalizer (fValue_, rhs.fValue_);
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
    inline typename Atom<ATOM_MANAGER>::_AtomInternalType Atom<ATOM_MANAGER>::_GetInternalRep () const
    {
        return fValue_;
    }

    /*
        ********************************************************************************
        ****************************** Atom operators **********************************
        ********************************************************************************
        */
    template <typename ATOM_MANAGER>
    inline bool operator< (Atom<ATOM_MANAGER> lhs, Atom<ATOM_MANAGER> rhs)
    {
        return lhs.Compare (rhs) < 0;
    }
    template <typename ATOM_MANAGER>
    inline bool operator<= (Atom<ATOM_MANAGER> lhs, Atom<ATOM_MANAGER> rhs)
    {
        return lhs.Compare (rhs) <= 0;
    }
    template <typename ATOM_MANAGER>
    inline bool operator== (Atom<ATOM_MANAGER> lhs, Atom<ATOM_MANAGER> rhs)
    {
        return lhs.Compare (rhs) == 0;
    }
    template <typename ATOM_MANAGER>
    inline bool operator!= (Atom<ATOM_MANAGER> lhs, Atom<ATOM_MANAGER> rhs)
    {
        return lhs.Compare (rhs) != 0;
    }
    template <typename ATOM_MANAGER>
    inline bool operator>= (Atom<ATOM_MANAGER> lhs, Atom<ATOM_MANAGER> rhs)
    {
        return lhs.Compare (rhs) >= 0;
    }
    template <typename ATOM_MANAGER>
    inline bool operator> (Atom<ATOM_MANAGER> lhs, Atom<ATOM_MANAGER> rhs)
    {
        return lhs.Compare (rhs) > 0;
    }

    namespace Private_ {
        struct AtomModuleData {
            AtomModuleData ();
            ~AtomModuleData ();
        };
    }
}

namespace {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::DataExchange::Private_::AtomModuleData> _Stroika_Foundation_DataExchange_Atom_ModuleData_; // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif /*_Stroika_Foundation_DataExchange_Atom_inl_*/
