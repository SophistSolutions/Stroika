/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Atom_inl_
#define _Stroika_Foundation_DataExchange_Atom_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


#include    <type_traits>


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
                return  As (type_<T>());
            }
            template    <typename   ATOM_MANAGER>
            inline  String   Atom<ATOM_MANAGER>::As (type_<String>) const
            {
                return GetPrintName ();
            }
            template    <typename   ATOM_MANAGER>
            inline  wstring   Atom<ATOM_MANAGER>::As (type_<wstring>) const
            {
                return GetPrintName ().As<wstring> ();
            }
            template    <typename   ATOM_MANAGER>
            inline  typename Atom<ATOM_MANAGER>::_AtomInternalType    Atom<ATOM_MANAGER>::_GetInternalRep () const
            {
                return fValue_;
            }


        }

    }
}
#endif  /*_Stroika_Foundation_DataExchange_Atom_inl_*/
