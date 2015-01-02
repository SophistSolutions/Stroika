/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Atom_inl_
#define _Stroika_Foundation_DataExchange_Atom_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <type_traits>

#include    "../Execution/ModuleInit.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


            /*
             ********************************************************************************
             ****************************** Atom<ATOM_MANAGER> ******************************
             ********************************************************************************
             */
            template    <typename   ATOM_MANAGER>
            inline  Atom<ATOM_MANAGER>::Atom  ()
                : fValue_ (ATOM_MANAGER::kEmpty)
            {
            }
            template    <typename   ATOM_MANAGER>
            inline  Atom<ATOM_MANAGER>::Atom  (const wstring& src)
                : fValue_ (ATOM_MANAGER::Intern (src))
            {
            }
            template    <typename   ATOM_MANAGER>
            inline  Atom<ATOM_MANAGER>::Atom  (const wchar_t* src)
                : fValue_ (ATOM_MANAGER::Intern (src))
            {
            }
            template    <typename   ATOM_MANAGER>
            inline  Atom<ATOM_MANAGER>::Atom  (const String& src)
                : fValue_ (ATOM_MANAGER::Intern (src))
            {
            }
            template    <typename   ATOM_MANAGER>
            inline  Atom<ATOM_MANAGER>::Atom  (const Atom& src)
                : fValue_ (src.fValue_)
            {
            }
            template    <typename   ATOM_MANAGER>
            inline  Atom<ATOM_MANAGER>::Atom  (const _AtomInternalType& src)
                : fValue_ (src)
            {
            }
            template    <typename   ATOM_MANAGER>
            inline  Atom<ATOM_MANAGER>& Atom<ATOM_MANAGER>::operator= (const Atom& rhs)
            {
                fValue_ = rhs.fValue_;
                return *this;
            }
            template    <typename   ATOM_MANAGER>
            inline  String   Atom<ATOM_MANAGER>::GetPrintName () const
            {
                return fValue_ == ATOM_MANAGER::kEmpty ?
                       String () :
                       ATOM_MANAGER::Extract (fValue_)
                       ;
            }
            template    <typename   ATOM_MANAGER>
            inline  int Atom<ATOM_MANAGER>::Compare (Atom rhs) const
            {
                using ST = typename make_signed<_AtomInternalType>::type;
                ST i = static_cast<ST> (fValue_) - static_cast<ST> (rhs.fValue_);
                if (i == 0) {
                    return 0;
                }
                else if (i < 0) {
                    return -1;
                }
                else {
                    return 1;
                }
            }
            template    <typename   ATOM_MANAGER>
            inline  bool Atom<ATOM_MANAGER>::operator< (Atom rhs) const
            {
                return Compare (rhs) < 0;
            }
            template    <typename   ATOM_MANAGER>
            inline  bool Atom<ATOM_MANAGER>::operator<= (Atom rhs) const
            {
                return Compare (rhs) <= 0;
            }
            template    <typename   ATOM_MANAGER>
            inline  bool Atom<ATOM_MANAGER>::operator> (Atom rhs) const
            {
                return Compare (rhs) > 0;
            }
            template    <typename   ATOM_MANAGER>
            inline  bool Atom<ATOM_MANAGER>::operator>= (Atom rhs) const
            {
                return Compare (rhs) >= 0;
            }
            template    <typename   ATOM_MANAGER>
            inline  bool Atom<ATOM_MANAGER>::operator== (Atom rhs) const
            {
                return Compare (rhs) == 0;
            }
            template    <typename   ATOM_MANAGER>
            inline  bool Atom<ATOM_MANAGER>::operator!= (Atom rhs) const
            {
                return Compare (rhs) != 0;
            }
            template    <typename   ATOM_MANAGER>
            inline  bool    Atom<ATOM_MANAGER>::empty () const
            {
                return fValue_ == ATOM_MANAGER::kEmpty;
            }
            template    <typename   ATOM_MANAGER>
            inline  void    Atom<ATOM_MANAGER>::clear ()
            {
                fValue_ = ATOM_MANAGER::kEmpty;
            }
            template    <typename   ATOM_MANAGER>
            template    <typename   T>
            inline  T   Atom<ATOM_MANAGER>::As () const
            {
                return  As_ (type_<T>());
            }
            template    <typename   ATOM_MANAGER>
            inline  String   Atom<ATOM_MANAGER>::As_ (type_<String>) const
            {
                return GetPrintName ();
            }
            template    <typename   ATOM_MANAGER>
            inline  wstring   Atom<ATOM_MANAGER>::As_ (type_<wstring>) const
            {
                return GetPrintName ().template As<wstring> ();
            }
            template    <typename   ATOM_MANAGER>
            inline  typename Atom<ATOM_MANAGER>::_AtomInternalType    Atom<ATOM_MANAGER>::_GetInternalRep () const
            {
                return fValue_;
            }


            namespace Private_ {
                struct  AtomModuleData {
                    AtomModuleData ();
                    ~AtomModuleData ();
                };
            }


        }
    }
}



namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::DataExchange::Private_::AtomModuleData> _Stroika_Foundation_DataExchange_Atom_ModuleData_; // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif  /*_Stroika_Foundation_DataExchange_Atom_inl_*/
