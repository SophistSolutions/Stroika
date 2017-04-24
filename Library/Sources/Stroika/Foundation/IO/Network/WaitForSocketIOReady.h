/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_WaitForSocketIOReady_h_
#define _Stroika_Foundation_IO_Network_WaitForSocketIOReady_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Containers/Bijection.h"
#include "../../Execution/WaitForIOReady.h"

#include "Socket.h"

/*
 * TODO:
 */

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                /**
                 */
                template <typename SOCKET_SUBTYPE>
                class WaitForSocketIOReady : public Execution::WaitForIOReady {
                private:
                    using inherited = Execution::WaitForIOReady;

                public:
                    /**
                     */
                    WaitForSocketIOReady (const Traversal::Iterable<SOCKET_SUBTYPE>& sockets);

                public:
                    /**
                     */
                    nonvirtual Traversal::Iterable<SOCKET_SUBTYPE> Wait (Time::DurationSecondsType waitFor = Time::kInfinite);

                public:
                    /**
                     */
                    nonvirtual Traversal::Iterable<SOCKET_SUBTYPE> WaitUntil (Time::DurationSecondsType timeoutAt = Time::kInfinite);

                private:
                    Containers::Bijection<SOCKET_SUBTYPE, Socket::PlatformNativeHandle> fMapping_;
                };
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WaitForSocketIOReady.inl"

#endif /*_Stroika_Foundation_IO_Network_WaitForSocketIOReady_h_*/
