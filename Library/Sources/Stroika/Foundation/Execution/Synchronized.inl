/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
             ********************************* Synchronized *********************************
             ********************************************************************************
             */
            template    <typename   T>
            inline  Synchronized<T>::Synchronized ()
                : fDelegate_ ()
                , fLock_ ()
            {
            }
            template    <typename   T>
            inline  Synchronized<T>::Synchronized (const T& from)
                : fDelegate_ (from)
                , fLock_ ()
            {
            }
            template    <typename   T>
            inline  Synchronized<T>::Synchronized (const Synchronized& from)
                : fDelegate_ (static_cast<T> (from))
                , fLock_ ()
            {
            }
            template    <typename   T>
            inline  const Synchronized<T>& Synchronized<T>::operator= (const Synchronized& rhs)
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
            inline  Synchronized<T>::operator T () const
            {
                MACRO_LOCK_GUARD_CONTEXT (fLock_);
                return fDelegate_;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_Synchronized_inl_*/
