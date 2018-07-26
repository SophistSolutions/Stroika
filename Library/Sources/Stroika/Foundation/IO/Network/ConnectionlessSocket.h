/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionlessSocket_h_
#define _Stroika_Foundation_IO_Network_ConnectionlessSocket_h_ 1

#include "../../StroikaPreComp.h"

#include "Socket.h"

namespace Stroika::Foundation::IO::Network {

    /**
     *  \brief ConnectionlessSocket is typically a UDP socket you use for packet oriented communications (ie not tcp/streams)
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter</a>
     */
    class ConnectionlessSocket : public Socket {
    private:
        using inherited = Socket;

    protected:
        class _IRep;

    public:
        class Ptr;

    public:
        ConnectionlessSocket ()                              = delete;
        ConnectionlessSocket (ConnectionlessSocket&& s)      = delete;
        ConnectionlessSocket (const ConnectionlessSocket& s) = delete;

    public:
        /**
         *  For copyability, use ConnectionlessSocket::Ptr for assigned-to type.
         */
        nonvirtual ConnectionlessSocket& operator= (ConnectionlessSocket&& s) = delete;
        nonvirtual ConnectionlessSocket& operator= (const ConnectionlessSocket& s) = delete;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          ConnectionlessSocket::Ptr   s  = ConnectionlessSocket::New (Socket::INET, Socket::DGRAM);
         *      \endcode
         *
         *  \req socketKind != SOCK_STREAM
         *
         *  \note unless you call @Detatch() - socket is CLOSED in DTOR of rep, so when final reference goes away
         *
         *  \note ConnectionlessSocket is not copyable, but it can be copied into a ConnectionlessSocket::Ptr or
         *        Socket::Ptr.  This is critical to save them in a container, for example.
         */
        static ConnectionlessSocket::Ptr New (SocketAddress::FamilyType family, Type socketKind, const optional<IPPROTO>& protocol = {});

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
        static Ptr Attach (PlatformNativeHandle sd);
    };

    /**
     *  \par Example Usage
     *      \code
     *          ConnectionlessSocket::Ptr cs  = ConnectionlessSocket (Socket::INET, Socket::DGRAM);
     *          Sequence<ConnectionlessSocket::Ptr> l;  // cannot do Sequence<ConnectionlessSocket> cuz not copyable
     *          l.push_back (cs);
     *      \endcode
     *
     *  \note Since ConnectionlessSocket::Ptr is a smart pointer, the constness of the methods depends on whether they modify the smart pointer itself, not
     *        the underlying thread object.
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter</a>
     */
    class ConnectionlessSocket::Ptr : public Socket::Ptr {
    private:
        using inherited = Socket::Ptr;

    public:
        /**
         *  defaults to null (empty ())
         */
        Ptr () = default;
        Ptr (nullptr_t);
        Ptr (const Ptr& src) = default;
        Ptr (Ptr&& src)      = default;

    protected:
        Ptr (shared_ptr<_IRep>&& rep);
        Ptr (const shared_ptr<_IRep>& rep);

    public:
        /**
        */
        nonvirtual Ptr& operator= (const Ptr& rhs) = default;
        nonvirtual Ptr& operator= (Ptr&& rhs) = default;

    public:
        /**
         */
        nonvirtual void JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface = V4::kAddrAny) const;

    public:
        /**
         */
        nonvirtual void LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface = V4::kAddrAny) const;

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
        nonvirtual void SetMulticastTTL (uint8_t ttl) const;

    public:
        /**
         *  This determines whether the data sent will be looped back to sender host or not.
         */
        nonvirtual bool GetMulticastLoopMode () const;

    public:
        /**
         *  This determines whether the data sent will be looped back to sender host or not.
         */
        nonvirtual void SetMulticastLoopMode (bool loopMode) const;

    public:
        /**
         *  Send the argument data to the argument socket address.
         *
         *  @see https://linux.die.net/man/2/sendto
         */
        nonvirtual void SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr) const;

    public:
        /**
         *  Read the next message (typically a full packet) from the socket.
         *
         *  @see https://linux.die.net/man/2/recvfrom
         *
         *  if fromAddress != nullptr (legal to pass nullptr) - then it it is filled in with the source address the packet came from.
         *
         *  \note ***Cancelation Point***
         */
        nonvirtual size_t ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress, Time::DurationSecondsType timeout = Time::kInfinite) const;

    protected:
        /**
         */
        nonvirtual shared_ptr<_IRep> _GetSharedRep () const;

    protected:
        /**
         * \req fRep_ != nullptr
         */
        nonvirtual _IRep& _ref () const;

    protected:
        /**
         * \req fRep_ != nullptr
         */
        nonvirtual const _IRep& _cref () const;

    private:
        friend class ConnectionlessSocket;
    };

    /**
     */
    class ConnectionlessSocket::_IRep : public Socket::_IRep {
    public:
        virtual ~_IRep () = default;

        virtual void    SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr)                                            = 0;
        virtual size_t  ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress, Time::DurationSecondsType timeout) = 0;
        virtual void    JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface)                                 = 0;
        virtual void    LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface)                                = 0;
        virtual uint8_t GetMulticastTTL () const                                                                                              = 0;
        virtual void    SetMulticastTTL (uint8_t ttl)                                                                                         = 0;
        virtual bool    GetMulticastLoopMode () const                                                                                         = 0;
        virtual void    SetMulticastLoopMode (bool loopMode)                                                                                  = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ConnectionlessSocket.inl"

#endif /*_Stroika_Foundation_IO_Network_ConnectionlessSocket_h_*/
