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


#if qSUPPORT_LEGACY_SYNCHO
            /*
             ********************************************************************************
             ********************************* LEGACY_Synchronized *********************************
             ********************************************************************************
             */
            template    <typename   T>
            inline  LEGACY_Synchronized<T>::LEGACY_Synchronized ()
                : fDelegate_ ()
                , fLock_ ()
            {
            }
            template    <typename   T>
            inline  LEGACY_Synchronized<T>::LEGACY_Synchronized (const T& from)
                : fDelegate_ (from)
                , fLock_ ()
            {
            }
            template    <typename   T>
            inline  LEGACY_Synchronized<T>::LEGACY_Synchronized (const LEGACY_Synchronized& from)
                : fDelegate_ (static_cast<T> (from))
                , fLock_ ()
            {
            }
            template    <typename   T>
            inline  const LEGACY_Synchronized<T>& LEGACY_Synchronized<T>::operator= (const LEGACY_Synchronized& rhs)
            {
                if (this != &rhs) {
                    unique_lock<SpinLock> lock1 (rhs.fLock_, defer_lock);
                    unique_lock<SpinLock> lock2 (fLock_, defer_lock);
                    lock (lock1, lock2);
                    fDelegate_ = rhs.fDelegate_;
                }
                return *this;
            }
            template    <typename   T>
            inline  LEGACY_Synchronized<T>::operator T () const
            {
                MACRO_LOCK_GUARD_CONTEXT (fLock_);
                return fDelegate_;
            }

#endif

        }
    }
}
#endif  /*_Stroika_Foundation_Execution_Synchronized_inl_*/
