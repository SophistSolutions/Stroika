/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_BlockingQueue_inl_
#define _Stroika_Foundation_Execution_BlockingQueue_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "TimeOutException.h"

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /*
             ********************************************************************************
             ******************************** BlockingQueue<T> ******************************
             ********************************************************************************
             */
            template <typename T>
            BlockingQueue<T>::BlockingQueue (const Containers::Queue<T>& useQueue)
                : fQueue_ (useQueue)
            {
                Require (useQueue.empty ()); // this constructor is only used to control the 'type' (data structure/backend) used by the Blocking Queue
            }
            template <typename T>
            inline void BlockingQueue<T>::AddTail (const T& e, Time::DurationSecondsType timeout)
            {
                Require (not fEndOfInput_);
                fQueue_.rwget ()->AddTail (e);
                fDataAvailable_.Set ();
            }
            template <typename T>
            inline void BlockingQueue<T>::EndOfInput ()
            {
                fEndOfInput_ = true;
                fDataAvailable_.Set ();
            }
            template <typename T>
            T BlockingQueue<T>::RemoveHead (Time::DurationSecondsType timeout)
            {
                Time::DurationSecondsType waitTil = Time::GetTickCount () + timeout;
                while (true) {
                    Memory::Optional<T> tmp = fQueue_.rwget ()->RemoveHeadIf ();
                    if (tmp.IsPresent ()) {
                        return *tmp;
                    }
                    if (fEndOfInput_) {
                        Execution::Throw (Execution::TimeOutException::kThe);
                    }
                    fDataAvailable_.WaitUntil (waitTil);
                    if (not fEndOfInput_) {
                        fDataAvailable_.Reset ();
                    }
                }
            }
            template <typename T>
            Memory::Optional<T> BlockingQueue<T>::RemoveHeadIfPossible (Time::DurationSecondsType timeout)
            {
                Time::DurationSecondsType waitTil = Time::GetTickCount () + timeout;
                while (true) {
                    if (Memory::Optional<T> tmp = fQueue_.rwget ()->RemoveHeadIf ()) {
                        return tmp;
                    }
                    if (not fDataAvailable_.WaitUntilQuietly (waitTil)) {
                        return Memory::Optional<T> (); // on timeout, return 'missing'
                    }
                    if (not fEndOfInput_) {
                        fDataAvailable_.Reset ();
                    }
                }
            }
            template <typename T>
            inline Memory::Optional<T> BlockingQueue<T>::PeekHead () const
            {
                return fQueue_.cget ()->HeadIf ();
            }
            template <typename T>
            inline bool BlockingQueue<T>::empty () const
            {
                return fQueue_.cget ()->empty ();
            }
            template <typename T>
            inline size_t BlockingQueue<T>::GetLength () const
            {
                return fQueue_.cget ()->GetLength ();
            }
            template <typename T>
            inline size_t BlockingQueue<T>::length () const
            {
                return GetLength ();
            }
            template <typename T>
            inline void BlockingQueue<T>::clear ()
            {
                fQueue_.rwget ()->clear ();
            }
        }
    }
}
#endif /*_Stroika_Foundation_Execution_BlockingQueue_inl_*/
