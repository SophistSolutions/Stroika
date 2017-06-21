/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionlessSocket_h_
#define _Stroika_Foundation_IO_Network_ConnectionlessSocket_h_ 1

#include "../../StroikaPreComp.h"

#include "Socket.h"

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                using Characters::String;
                using Memory::Byte;
                using Memory::Optional;

                /**
                 */
                class ConnectionlessSocket : public Socket::Ptr {
                private:
                    using inherited = Socket::Ptr;

                protected:
                    class _IRep;

                public:
                    class Ptr;

                public:
                    /**
                     *  \par Example Usage
                     *      \code
                     *          ConnectionlessSocket      s  { Socket::INET, Socket::DGRAM };
                     *      \endcode
                     *
                     *  \req socketKind != SOCK_STREAM
                     *
                     *  \note unless you call @Detatch() - socket is CLOSED in DTOR of rep, so when final reference goes away
                     */
                    ConnectionlessSocket ()                              = delete;
                    ConnectionlessSocket (ConnectionlessSocket&& s)      = delete;
                    ConnectionlessSocket (const ConnectionlessSocket& s) = delete;
                    ConnectionlessSocket (SocketAddress::FamilyType family, Type socketKind, const Optional<IPPROTO>& protocol = {});

                private:
                    ConnectionlessSocket (const shared_ptr<_IRep>& rep);

                public:
                    nonvirtual ConnectionlessSocket& operator= (ConnectionlessSocket&& s);
                    nonvirtual ConnectionlessSocket& operator= (const ConnectionlessSocket& s) = default;

                public:
                    /**
                     *  This function associates a Platform native socket handle with a Stroika wrapper object.
                     *
                     *  Once a PlatformNativeHandle is attached to Socket object, it will be automatically closed
                     *  when the last reference to the socket disappears (or when someone calls close).
                     *
                     *  To prevent that behavior, you can Detatch the PlatformNativeHandle before destroying
                     *  the associated Socket object.
                     */
                    static ConnectionlessSocket::Ptr Attach (PlatformNativeHandle sd);

                public:
                    /**
                     */
                    nonvirtual void JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface = V4::kAddrAny);

                public:
                    /**
                     */
                    nonvirtual void LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface = V4::kAddrAny);

                public:
                    /**
                     *  This specifies the number of networks to traverse in sending the multicast message.
                     *  It defaults to 1.
                     */
                    nonvirtual uint8_t GetMulticastTTL () const;

                public:
                    /**
                     *  This specifies the number of networks to traverse in sending the multicast message.
                     *  It defaults to 1.
                     */
                    nonvirtual void SetMulticastTTL (uint8_t ttl);

                public:
                    /**
                     *  This determines whether the data sent will be looped back to sender host or not.
                     */
                    nonvirtual bool GetMulticastLoopMode ();

                public:
                    /**
                     *  This determines whether the data sent will be looped back to sender host or not.
                     */
                    nonvirtual void SetMulticastLoopMode (bool loopMode);

                public:
                    /**
                     *  Send the argument data to the argument socket address.
                     *
                     *  @see https://linux.die.net/man/2/sendto
                     */
                    nonvirtual void SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr);

                public:
                    /**
                     *  Read the next message (typically a full packet) from the socket.
                     *
                     *  @see https://linux.die.net/man/2/recvfrom
                     *
                     *  if fromAddress != nullptr (legal to pass nullptr) - then it it is filled in with the source address the packet came from.
                     */
                    nonvirtual size_t ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress, Time::DurationSecondsType timeout = Time::kInfinite);

                protected:
                    /**
                    */
                    nonvirtual shared_ptr<_IRep> _GetSharedRep () const;

                protected:
                    /**
                    * \req fRep_ != nullptr
                    */
                    nonvirtual _IRep& _ref ();

                protected:
                    /**
                     * \req fRep_ != nullptr
                     */
                    nonvirtual const _IRep& _cref () const;
                };

                class ConnectionlessSocket::Ptr : public ConnectionlessSocket {
                private:
                    using inherited = ConnectionlessSocket;

                public:
                    /**
                     *  defaults to null (empty ())
                     */
                    Ptr () = default;
                    Ptr (nullptr_t);
                    Ptr (const Ptr& src);
                    Ptr (Ptr&& src);
                    Ptr (const ConnectionlessSocket& src);

                public:
                    /**
                    */
                    nonvirtual Ptr& operator= (const Ptr&) = default;
                    nonvirtual Ptr& operator= (Ptr&&) = default;
                };

                class ConnectionlessSocket::_IRep : public Socket::Ptr::_IRep {
                public:
                    virtual ~_IRep () = default;

                    virtual void SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr) = 0;
                    virtual size_t ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress, Time::DurationSecondsType timeout) = 0;
                    virtual void JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface)  = 0;
                    virtual void LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) = 0;
                    virtual uint8_t GetMulticastTTL () const          = 0;
                    virtual void SetMulticastTTL (uint8_t ttl)        = 0;
                    virtual bool GetMulticastLoopMode () const        = 0;
                    virtual void SetMulticastLoopMode (bool loopMode) = 0;
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
#include "ConnectionlessSocket.inl"

#endif /*_Stroika_Foundation_IO_Network_ConnectionlessSocket_h_*/
