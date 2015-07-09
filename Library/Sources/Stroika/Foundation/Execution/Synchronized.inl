/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Synchronized_inl_
#define _Stroika_Foundation_Execution_Synchronized_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
             ********************************************************************************
             **************************** Synchronized<T, TRAITS> ***************************
             ********************************************************************************
             */
            template    <typename   T, typename TRAITS>
            template    <typename ...ARGUMENT_TYPES>
            inline  Synchronized<T, TRAITS>::Synchronized (ARGUMENT_TYPES&& ...args)
                : fDelegate_ (std::forward<ARGUMENT_TYPES> (args)...)
            {
            }
            template    <typename   T, typename TRAITS>
            inline  Synchronized<T, TRAITS>::Synchronized (const Synchronized& src)
                : fDelegate_ (src.load ())
            {
            }
            template    <typename   T, typename TRAITS>
            inline  auto Synchronized<T, TRAITS>::operator= (const Synchronized& rhs) -> Synchronized& {
                if (&rhs != this)
                {
                    MACRO_LOCK_GUARD_CONTEXT (fLock_);
                    fDelegate_ = rhs.load ();
                }
                return *this;
            }
            template    <typename   T, typename TRAITS>
            inline  auto Synchronized<T, TRAITS>::operator= (const T& rhs) -> Synchronized& {
                MACRO_LOCK_GUARD_CONTEXT (fLock_);
                fDelegate_ = rhs;
                return *this;
            }
            template    <typename   T, typename TRAITS>
            inline  Synchronized<T, TRAITS>::operator T () const
            {
                MACRO_LOCK_GUARD_CONTEXT (fLock_);
                return fDelegate_;
            }
            template    <typename   T, typename TRAITS>
            inline  T Synchronized<T, TRAITS>::load () const
            {
                MACRO_LOCK_GUARD_CONTEXT (fLock_);
                return fDelegate_;
            }
            template    <typename   T, typename TRAITS>
            inline  void    Synchronized<T, TRAITS>::store (const T& v)
            {
                MACRO_LOCK_GUARD_CONTEXT (fLock_);
                fDelegate_ = v;
            }
            template    <typename   T, typename TRAITS>
            inline  auto    Synchronized<T, TRAITS>::GetReference () -> WritableReference {
                return WritableReference (&fDelegate_, &fLock_);
            }
            template    <typename   T, typename TRAITS>
            inline  auto Synchronized<T, TRAITS>::GetReference () const -> const WritableReference
            {
                auto nonConstThis = const_cast<Synchronized<T, TRAITS>*> (this);
                return WritableReference (&nonConstThis->fDelegate_, &nonConstThis->fLock_);
            }
            template    <typename   T, typename TRAITS>
            inline  auto Synchronized<T, TRAITS>::operator-> () const -> const WritableReference
            {
                return GetReference ();
            }
            template    <typename   T, typename TRAITS>
            inline  auto Synchronized<T, TRAITS>::operator-> () -> WritableReference {
                return GetReference ();
            }
            template    <typename   T, typename TRAITS>
            inline  void    Synchronized<T, TRAITS>::lock ()
            {
                fLock_.lock ();
            }
            template    <typename   T, typename TRAITS>
            inline  void    Synchronized<T, TRAITS>::unlock ()
            {
                fLock_.unlock ();
            }


            /*
             ********************************************************************************
             *********************************** operator< **********************************
             ********************************************************************************
             */
            template    <typename   T, typename TRAITS>
            inline  bool    operator< (const Synchronized<T, TRAITS>& lhs, T rhs)
            {
                return lhs.load () < rhs;
            }
            template    <typename   T, typename TRAITS>
            inline  bool    operator< (T lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs < rhs.load ();
            }
            template    <typename   T, typename TRAITS>
            inline  bool    operator< (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs.load () < rhs.load ();
            }


            /*
             ********************************************************************************
             ********************************** operator<= **********************************
             ********************************************************************************
             */
            template    <typename   T, typename TRAITS>
            inline  bool    operator<= (const Synchronized<T, TRAITS>& lhs, T rhs)
            {
                return lhs.load () <= rhs;
            }
            template    <typename   T, typename TRAITS>
            inline  bool    operator<= (T lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs <= rhs.load ();
            }
            template    <typename   T, typename TRAITS>
            inline  bool    operator<= (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs.load () <= rhs.load ();
            }


            /*
             ********************************************************************************
             ********************************** operator== **********************************
             ********************************************************************************
             */
            template    <typename   T, typename TRAITS>
            inline  bool    operator== (const Synchronized<T, TRAITS>& lhs, T rhs)
            {
                return lhs.load () == rhs;
            }
            template    <typename   T, typename TRAITS>
            inline  bool    operator== (T lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs == rhs.load ();
            }
            template    <typename   T, typename TRAITS>
            inline  bool    operator== (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs.load () == rhs.load ();
            }


            /*
             ********************************************************************************
             ********************************** operator!= **********************************
             ********************************************************************************
             */
            template    <typename   T, typename TRAITS>
            inline  bool    operator!= (const Synchronized<T, TRAITS>& lhs, T rhs)
            {
                return lhs.load () != rhs;
            }
            template    <typename   T, typename TRAITS>
            inline  bool    operator!= (T lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs != rhs.load ();
            }
            template    <typename   T, typename TRAITS>
            inline  bool    operator!= (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs.load () != rhs.load ();
            }


            /*
             ********************************************************************************
             ********************************** operator>= **********************************
             ********************************************************************************
             */
            template    <typename   T, typename TRAITS>
            inline  bool    operator>= (const Synchronized<T, TRAITS>& lhs, T rhs)
            {
                return lhs.load () >= rhs;
            }


            /*
             ********************************************************************************
             *********************************** operator> **********************************
             ********************************************************************************
             */
            template    <typename   T, typename TRAITS>
            inline  bool    operator> (const Synchronized<T, TRAITS>& lhs, T rhs)
            {
                return lhs.load () > rhs;
            }


            /*
             ********************************************************************************
             ********************************** operator^ ***********************************
             ********************************************************************************
             */
            template    <typename   T, typename TRAITS>
            inline  auto    operator^ (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T {} ^ T {})
            {
                return lhs.load () ^ rhs;
            }
            template    <typename   T, typename TRAITS>
            inline  auto    operator^ (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} ^ T {})
            {
                return lhs ^ rhs.load ();
            }
            template    <typename   T, typename TRAITS>
            inline  auto    operator^ (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} ^ T {})
            {
                return lhs.load () ^ rhs.load ();
            }


            /*
             ********************************************************************************
             ********************************** operator* ***********************************
             ********************************************************************************
             */
            template    <typename   T, typename TRAITS>
            inline  auto    operator* (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T {} * T {})
            {
                return lhs.load () * rhs;
            }
            template    <typename   T, typename TRAITS>
            inline  auto    operator* (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} * T {})
            {
                return lhs * rhs.load ();
            }
            template    <typename   T, typename TRAITS>
            inline  auto    operator* (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} * T {})
            {
                return lhs.load () * rhs.load ();
            }


            /*
             ********************************************************************************
             ********************************** operator+ ***********************************
             ********************************************************************************
             */
            template    <typename   T, typename TRAITS>
            inline  auto    operator+ (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T {} + T {})
            {
                return lhs.load () + rhs;
            }
            template    <typename   T, typename TRAITS>
            inline  auto    operator+ (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} + T {})
            {
                return lhs + rhs.load ();
            }
            template    <typename   T, typename TRAITS>
            inline  auto    operator+ (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} + T {})
            {
                return lhs.load () + rhs.load ();
            }


            /*
             ********************************************************************************
             ********************************** operator-= **********************************
             ********************************************************************************
             */
            template    <typename   T, typename TRAITS, typename RHSTYPE>
            inline  auto    operator-= (Synchronized<T, TRAITS>& lhs, RHSTYPE rhs) -> decltype (lhs->operator-= (rhs))
            {
                return lhs->operator-= (rhs);
            }


            /*
             ********************************************************************************
             ********************************** operator+= **********************************
             ********************************************************************************
             */
            template    <typename   T, typename TRAITS, typename RHSTYPE>
            inline  auto    operator+= (Synchronized<T, TRAITS>& lhs, RHSTYPE rhs) -> decltype (lhs->operator+= (rhs))
            {
                return lhs->operator+= (rhs);
            }


            /*
             ********************************************************************************
             ********************************** operator- ***********************************
             ********************************************************************************
             */
            template    <typename   T, typename TRAITS>
            inline  auto    operator- (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T {} - T {})
            {
                return lhs.load () + rhs;
            }
            template    <typename   T, typename TRAITS>
            inline  auto    operator- (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} - T {})
            {
                return lhs - rhs.load ();
            }
            template    <typename   T, typename TRAITS>
            inline  auto    operator- (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} - T {})
            {
                return lhs.load () - rhs.load ();
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_Synchronized_inl_*/
