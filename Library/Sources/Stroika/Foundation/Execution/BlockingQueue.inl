/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
             ******************************** BlockingQueue<T> ******************************
             ********************************************************************************
             */
            template    <typename T>
            BlockingQueue<T>::BlockingQueue ()
                : fDataAvailable_ (WaitableEvent::eAutoReset)
                , fQueue_ ()
            {
            }
            template    <typename T>
            BlockingQueue<T>::BlockingQueue (const Containers::Queue<T>& useQueue)
                : fDataAvailable_ (WaitableEvent::eAutoReset)
                , fQueue_ (useQueue)
            {
                Require (useQueue.empty ());
            }
            template    <typename T>
            void    BlockingQueue<T>::AddTail (const T& e, Time::DurationSecondsType timeout)
            {
                fQueue_->AddTail (e);
                fDataAvailable_.Set ();
            }
            template    <typename T>
            T       BlockingQueue<T>::RemoveHead (Time::DurationSecondsType timeout)
            {
                Time::DurationSecondsType   waitTil = Time::GetTickCount () + timeout;
                while (true) {
                    Memory::Optional<T> tmp = fQueue_->RemoveHeadIf ();
                    if (tmp.IsPresent ()) {
                        return *tmp;
                    }
                    fDataAvailable_.Wait (waitTil - Time::GetTickCount ());
                }
            }
            template    <typename T>
            inline  Memory::Optional<T>     BlockingQueue<T>::RemoveHeadIfPossible (Time::DurationSecondsType timeout)
            {
                return fQueue_->RemoveHeadIf ();
            }
            template    <typename T>
            inline  Memory::Optional<T> BlockingQueue<T>::PeekHead () const
            {
                return fQueue_->HeadIf ();
            }
            template    <typename T>
            inline  bool    BlockingQueue<T>::empty () const
            {
                return fQueue_->empty ();
            }
            template    <typename T>
            inline  size_t      BlockingQueue<T>::GetLength () const
            {
                return fQueue_->GetLength ();
            }
            template    <typename T>
            inline  size_t      BlockingQueue<T>::length () const
            {
                return GetLength ();
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_BlockingQueue_inl_*/
