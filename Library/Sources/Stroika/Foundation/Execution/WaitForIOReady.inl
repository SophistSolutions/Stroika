/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WaitForIOReady_inl_
#define _Stroika_Foundation_Execution_WaitForIOReady_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "Exceptions.h"

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /*
             ********************************************************************************
             ************************** Execution::WaitForIOReady ***************************
             ********************************************************************************
             */
            inline void WaitForIOReady::clear ()
            {
                fPollData_.rwget ()->clear ();
            }
            inline auto WaitForIOReady::GetDescriptors () const -> Containers::Collection<pair<FileDescriptorType, TypeOfMonitorSet>>
            {
                return fPollData_;
            }
            inline void WaitForIOReady::AddAll (const Traversal::Iterable<FileDescriptorType>& fds, const TypeOfMonitorSet& flags)
            {
                for (auto i : fds) {
                    Add (i, flags);
                }
            }
            inline void WaitForIOReady::AddAll (const Traversal::Iterable<pair<FileDescriptorType, TypeOfMonitorSet>>& fds)
            {
                for (auto i : fds) {
                    Add (i.first, i.second);
                }
            }
            inline auto WaitForIOReady::Wait (Time::DurationSecondsType waitFor) -> Containers::Set<FileDescriptorType>
            {
                return WaitUntil (waitFor + Time::GetTickCount ());
            }
            inline auto WaitForIOReady::WaitQuietly (Time::DurationSecondsType waitFor) -> optional<Containers::Set<FileDescriptorType>>
            {
                return WaitQuietlyUntil (waitFor + Time::GetTickCount ());
            }
        }
    }
}
#endif /*_Stroika_Foundation_Execution_WaitForIOReady_inl_*/
