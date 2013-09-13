/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_BlockingQueue_inl_
#define _Stroika_Foundation_Execution_BlockingQueue_inl_    1


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
             *********************************** BlockingQueue<T> ***************************
             ********************************************************************************
             */
            template    <typename T>
            BlockingQueue<T>::BlockingQueue ()
                : fMutex_ ()
                , fDataAvailable_ ()
                , fQueue_ ()
            {
            }
            template    <typename T>
            void    BlockingQueue<T>::Add (T e, Time::DurationSecondsType timeout)
            {
                {
                    lock_guard<mutex> critSec (fMutex_);
                    fQueue_.Enqueue (e);
                }
                fDataAvailable_.Set ();
            }
            template    <typename T>
            T       BlockingQueue<T>::Remove (Time::DurationSecondsType timeout)
            {
                Time::DurationSecondsType   waitTil = Time::GetTickCount () + timeout;
                while (true) {
                    {
                        lock_guard<mutex> critSec (fMutex_);
                        if (not fQueue_.empty ()) {
                            return fQueue_.RemoveHead ();
                        }
                    }
                    fDataAvailable_.Wait (waitTil - Time::GetTickCount ());
                }
            }
            template    <typename T>
            Memory::Optional<T>     BlockingQueue<T>::RemoveIfPossible ()
            {
                lock_guard<mutex> critSec (fMutex_);
                if (fQueue_.empty ()) {
                    return Memory::Optional<T> ();
                }
                return fQueue_.RemoveHead ();
            }
            template    <typename T>
            Memory::Optional<T> BlockingQueue<T>::PeekFront () const
            {
                lock_guard<mutex> critSec (fMutex_);
                if (fQueue_.empty ()) {
                    return Memory::Optional<T> ();
                }
                return fQueue_.Head ();
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_BlockingQueue_inl_*/
