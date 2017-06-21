/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Socket_Private_h_
#define _Stroika_Foundation_IO_Network_Socket_Private_h_ 1

#include "../../StroikaPreComp.h"

// ONLY INCLUDED BY Network-Socket CPP implementation code. Do not include directly

#include <cstdlib>
#include <sys/types.h>

#if qPlatform_POSIX
#include <netdb.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#elif qPlatform_Windows
#include <winsock2.h>

#include <io.h>
#include <ws2tcpip.h>

#include <Mstcpip.h>
#endif
#if qPlatform_Linux
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif

//#include "../../Characters/String_Constant.h"
//#include "../../Characters/ToString.h"
#include "../../Debug/Trace.h"
//#include "../../Execution/ErrNoException.h"
#include "../../Execution/OperationNotSupportedException.h"
//#include "../../Execution/Sleep.h"
//#include "../../Execution/Thread.h"
//#include "../../Execution/TimeOutException.h"
//#include "../../Execution/WaitForIOReady.h"
#if qPlatform_Windows
#include "../../../Foundation/Execution/Platform/Windows/Exception.h"
#include "Platform/Windows/WinSock.h"
#endif
//#include "../../IO/FileAccessException.h"
//#include "../../Math/Common.h"
//#include "../../Memory/BlockAllocated.h"
#include "Socket.h"

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                namespace PRIVATE_ {
                    using namespace Stroika::Foundation;
                    using namespace Stroika::Foundation::Characters;
                    using namespace Stroika::Foundation::Execution;
                    using namespace Stroika::Foundation::Memory;
                    using namespace Stroika::Foundation::IO;
                    using namespace Stroika::Foundation::IO::Network;

                    namespace {
#if qPlatform_POSIX
                        /*
                        *  Internally we use this -1 value to mean invalid socket, but keep that a private implementation
                        *  detail, since I'm not sure it will be good for all socket implementations?
                        */
                        constexpr Socket::PlatformNativeHandle kINVALID_NATIVE_HANDLE_ = -1; // right value??
#elif qPlatform_Windows
                        constexpr Socket::PlatformNativeHandle kINVALID_NATIVE_HANDLE_ = INVALID_SOCKET;
#endif
                    }

                    namespace {
                        template <typename T>
                        void BreakWriteIntoParts_ (const T* start, const T* end, size_t maxSendAtATime, const function<size_t (const T*, const T*)>& writeFunc)
                        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                            Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"{}::BreakWriteIntoParts_", L"end-start=%lld", static_cast<long long> (end - start))};
#endif
                            ptrdiff_t amountToSend          = end - start;
                            ptrdiff_t amountRemainingToSend = amountToSend;
                            const T*  remainingSendFrom     = start;
                            while (amountRemainingToSend > 0) {
                                size_t amountToSend = min<size_t> (maxSendAtATime, amountRemainingToSend);
                                size_t amountSent   = writeFunc (remainingSendFrom, remainingSendFrom + amountToSend);
                                Assert (amountSent <= amountToSend);
                                Assert (static_cast<size_t> (amountRemainingToSend) >= amountSent);
                                amountRemainingToSend -= amountSent;
                                remainingSendFrom += amountSent;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                                if (amountSent < amountToSend) {
                                    DbgTrace (L"write broken into parts - amountSent=%lld out of amountToSend=%lld, amountRemainingToSend=%lld", static_cast<long long> (amountSent), static_cast<long long> (amountToSend), static_cast<long long> (amountRemainingToSend));
                                }
#endif
                            }
                        }
                    }

                    namespace {
                        template <typename BASE>
                        struct BackSocketImpl_ : public BASE {
                            struct Rep_ : public BASE::_IRep {
                                Socket::PlatformNativeHandle fSD_;
                                Rep_ (Socket::PlatformNativeHandle sd)
                                    : fSD_ (sd)
                                {
                                }
                                ~Rep_ ()
                                {
                                    if (fSD_ != kINVALID_NATIVE_HANDLE_) {
                                        Close ();
                                    }
                                }
                                virtual void Shutdown (Socket::ShutdownTarget shutdownTarget) override
                                {
                                    Require (fSD_ != kINVALID_NATIVE_HANDLE_);
                                    // Intentionally ignore shutdown results because in most cases there is nothing todo (maybe in some cases we should log?)
                                    switch (shutdownTarget) {
                                        case Socket::ShutdownTarget::eReads:
#if qPlatform_POSIX
                                            ::shutdown (fSD_, SHUT_RD);
#elif qPlatform_Windows
                                            ::shutdown (fSD_, SD_RECEIVE);
#endif
                                            break;
                                        case Socket::ShutdownTarget::eWrites:
// I believe this triggers TCP FIN
#if qPlatform_POSIX
                                            ::shutdown (fSD_, SHUT_WR);
#elif qPlatform_Windows
                                            ::shutdown (fSD_, SD_SEND);
#endif
                                            break;
                                        case Socket::ShutdownTarget::eBoth:
#if qPlatform_POSIX
                                            ::shutdown (fSD_, SHUT_RDWR);
#elif qPlatform_Windows
                                            ::shutdown (fSD_, SD_BOTH);
#endif
                                            break;
                                        default:
                                            RequireNotReached ();
                                    }
                                }
                                virtual void Close () override
                                {
                                    if (fSD_ != kINVALID_NATIVE_HANDLE_) {
#if qPlatform_POSIX
                                        ::close (fSD_);
#elif qPlatform_Windows
                                        ::closesocket (fSD_);
#else
                                        AssertNotImplemented ();
#endif
                                        fSD_ = kINVALID_NATIVE_HANDLE_;
                                    }
                                }
                                virtual Optional<IO::Network::SocketAddress> GetLocalAddress () const override
                                {
                                    struct sockaddr_storage radr;
                                    socklen_t               len = sizeof (radr);
                                    if (::getsockname (static_cast<int> (fSD_), (struct sockaddr*)&radr, &len) == 0) {
                                        IO::Network::SocketAddress sa{radr};
                                        return sa;
                                    }
                                    return Optional<IO::Network::SocketAddress>{};
                                }
                                virtual SocketAddress::FamilyType GetAddressFamily () const override
                                {
#if defined(SO_DOMAIN)
                                    return getsockopt<SocketAddress::FamilyType> (SOL_SOCKET, SO_DOMAIN);
#elif defined(SO_PROTOCOL)
                                    return getsockopt<SocketAddress::FamilyType> (SOL_SOCKET, SO_PROTOCOL);
#elif qPlatform_Windows
                                    /*
                                    *  According to https://msdn.microsoft.com/en-us/library/windows/desktop/ms741621(v=vs.85).aspx,
                                    *      WSAENOPROTOOPT  The socket option is not supported on the specified protocol. For example,
                                    *                      an attempt to use the SIO_GET_BROADCAST_ADDRESS IOCTL was made on an IPv6 socket
                                    *                      or an attempt to use the TCP SIO_KEEPALIVE_VALS IOCTL was made on a datagram socket.
                                    */
                                    DWORD            dwBytesRet;
                                    sockaddr_storage bcast;
                                    bool             isV6 = (WSAIoctl (this->GetNativeSocket (), SIO_GET_BROADCAST_ADDRESS, NULL, 0, &bcast, sizeof (bcast), &dwBytesRet, NULL, NULL) == SOCKET_ERROR);
                                    if (isV6) {
                                        Assert (::WSAGetLastError () == WSAENOPROTOOPT);
                                    }
                                    return isV6 ? SocketAddress::FamilyType::INET6 : SocketAddress::FamilyType::INET;
#else
                                    Execution::Throw (Execution::OperationNotSupportedException (L"SO_DOMAIN"));
#endif
                                }
                                virtual Socket::PlatformNativeHandle GetNativeSocket () const override
                                {
                                    return fSD_;
                                }
                                virtual void getsockopt (int level, int optname, void* optval, socklen_t* optvallen) const override
                                {
                                    // According to http://linux.die.net/man/2/getsockopt cannot return EINTR, so no need to retry
                                    RequireNotNull (optval);
#if qPlatform_POSIX
                                    ThrowErrNoIfNegative (::getsockopt (fSD_, level, optname, reinterpret_cast<char*> (optval), optvallen));
#elif qPlatform_Windows
                                    ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::getsockopt (fSD_, level, optname, reinterpret_cast<char*> (optval), optvallen));
#else
                                    AssertNotImplemented ();
#endif
                                }
                                template <typename RESULT_TYPE>
                                inline RESULT_TYPE getsockopt (int level, int optname) const
                                {
                                    RESULT_TYPE r{};
                                    socklen_t   roptlen = sizeof (r);
                                    this->getsockopt (level, optname, &r, &roptlen);
                                    return r;
                                }
                                virtual void setsockopt (int level, int optname, const void* optval, socklen_t optvallen) override
                                {
                                    // According to http://linux.die.net/man/2/setsockopt cannot return EINTR, so no need to retry
                                    RequireNotNull (optval);
#if qPlatform_POSIX
                                    ThrowErrNoIfNegative (::setsockopt (fSD_, level, optname, optval, optvallen));
#elif qPlatform_Windows
                                    ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::setsockopt (fSD_, level, optname, reinterpret_cast<const char*> (optval), optvallen));
#else
                                    AssertNotImplemented ();
#endif
                                }
                                template <typename ARG_TYPE>
                                inline void setsockopt (int level, int optname, ARG_TYPE arg)
                                {
                                    socklen_t optvallen = sizeof (arg);
                                    this->setsockopt (level, optname, &arg, optvallen);
                                }
                            };
                        };
                    }
                }
            }
        }
    }
}

#endif /*_Stroika_Foundation_IO_Network_Socket_Private_h_*/
