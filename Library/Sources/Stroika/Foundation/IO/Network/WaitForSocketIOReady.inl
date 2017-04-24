/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_WaitForSocketIOReady_inl_
#define _Stroika_Foundation_IO_Network_WaitForSocketIOReady_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                /*
                 ********************************************************************************
                 ********************** IO::Network::WaitForSocketIOReady ***********************
                 ********************************************************************************
                 */
                template <typename SOCKET_SUBTYPE>
                WaitForSocketIOReady<SOCKET_SUBTYPE>::WaitForSocketIOReady (const Traversal::Iterable<SOCKET_SUBTYPE>& sockets)
                    : inherited (sockets.template Select<Socket::PlatformNativeHandle> ([](const Socket& s) { return s.GetNativeSocket (); }))
                {
                    for (SOCKET_SUBTYPE i : sockets) {
                        fMapping_.Add (i, i.GetNativeSocket ());
                    }
                }
                template <typename SOCKET_SUBTYPE>
                inline Traversal::Iterable<SOCKET_SUBTYPE> WaitForSocketIOReady<SOCKET_SUBTYPE>::Wait (Time::DurationSecondsType waitFor)
                {
                    return fMapping_.InverseMap (inherited::Wait (waitFor));
                }
                template <typename SOCKET_SUBTYPE>
                inline Traversal::Iterable<SOCKET_SUBTYPE> WaitForSocketIOReady<SOCKET_SUBTYPE>::WaitUntil (Time::DurationSecondsType timeoutAt)
                {
                    return fMapping_.InverseMap (inherited::WaitUntil (timeoutAt));
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_Network_WaitForSocketIOReady_inl_*/
