/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionlessSocket_h_
#define _Stroika_Foundation_IO_Network_ConnectionlessSocket_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <variant>

#include "Socket.h"

namespace Stroika::Foundation::IO::Network {

    struct Interface;

    /**
     *  \brief ConnectionlessSocket is typically a UDP socket you use for packet oriented communications (ie not tcp/streams)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter</a>
     */
    namespace ConnectionlessSocket {
        using namespace Socket;

        class _IRep;

        /**
         *  \par Example Usage
         *      \code
         *          ConnectionlessSocket::Ptr cs  = ConnectionlessSocket::New (Socket::INET, Socket::DGRAM);
         *      \endcode
         *
         *  \note Since ConnectionlessSocket::Ptr is a smart pointer, the constness of the methods depends on whether they modify the smart pointer itself, not
         *        the underlying thread object.
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter</a>
         */
        class Ptr : public Socket::Ptr {
        private:
            using inherited = Socket::Ptr;

        public:
            /**
             */
            Ptr () = delete;
            Ptr (nullptr_t);
            Ptr (const Ptr& src) = default;
            Ptr (Ptr&& src)      = default;
            Ptr (shared_ptr<_IRep>&& rep);
            Ptr (const shared_ptr<_IRep>& rep);

        public:
            /**
             */
            nonvirtual Ptr& operator= (const Ptr& rhs) = default;
            nonvirtual Ptr& operator= (Ptr&& rhs)      = default;

        public:
            /**
             *  Join the multicast group iaddr (IPv4 or IPv6) - so this socket receives what is sent to it - on one interface: best
             *  given as an Interface (its index is looked up at the call - @see Interface::GetCurrentIndex), else by one of its
             *  addresses, or by its index (Interface::fIndex). V4::kAddrAny, V6::kAddrAny or Interface::kAnyIndex: the OS picks one.
             *
             *  \note RACE: an index is right only for now - give the Interface, or get the index just before the call (and again
             *        for the matching LeaveMulticastGroup) - @see Interface::fIndex
             *
             *  \note For IPv6, give an Interface (or index) rather than an address - a link-local address can be on more than one
             *        interface.
             *
             *  \note macOS (as of 26.5) cannot pick the interface for a link-local-scope IPv6 group (ff02::...): it has no route
             *        to choose one by, so the join fails (EADDRNOTAVAIL) - name the interface.
             */
            nonvirtual void JoinMulticastGroup (const InternetAddress& iaddr, const Interface& onInterface) const;
            nonvirtual void JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface = V4::kAddrAny) const; ///< \brief join group iaddr on the interface with this address (V4::kAddrAny or V6::kAddrAny: the OS picks one)
            nonvirtual void JoinMulticastGroup (const InternetAddress& iaddr, unsigned int onInterfaceIndex) const; ///< \brief join group iaddr on the interface whose Interface::fIndex this is (Interface::kAnyIndex: the OS picks one)

        public:
            /**
             *  Leave a group this socket joined (JoinMulticastGroup), on the interface it joined it on - best given as an Interface.
             *  Fails if there is no such membership - including when the interface went away since the join, as the OS drops a
             *  membership with its interface.
             *
             *  \note RACE: given by index, look the index up again rather than reusing the one joined with - @see Interface::fIndex
             */
            nonvirtual void LeaveMulticastGroup (const InternetAddress& iaddr, const Interface& onInterface) const;
            nonvirtual void LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface = V4::kAddrAny) const; ///< \brief leave group iaddr on the interface with this address
            nonvirtual void LeaveMulticastGroup (const InternetAddress& iaddr, unsigned int onInterfaceIndex) const; ///< \brief leave group iaddr on the interface whose Interface::fIndex this is (Interface::kAnyIndex: the one the OS picked)

        public:
            /**
             *  Choose the interface multicast datagrams sent from this socket go out of - else the OS picks one, on a machine with
             *  several networks often not the one wanted. Best given as an Interface - looked up at the call: for an IPv6 socket
             *  its current index (@see Interface::GetCurrentIndex), for an IPv4 socket one of its IPv4 addresses; else by one of
             *  its addresses, or (for an IPv6 socket) by its index (Interface::fIndex) - there is no portable way to choose an IPv4
             *  socket's by index.
             *
             *  \note RACE: the socket keeps the choice as given - set it again when the interfaces change - @see Interface::fIndex
             */
            nonvirtual void SetMulticastInterface (const Interface& i) const;
            nonvirtual void SetMulticastInterface (const InternetAddress& interfaceAddress) const; ///< \brief the interface with this address
            nonvirtual void SetMulticastInterface (unsigned int interfaceIndex) const; ///< \brief IPv6 socket: the interface whose Interface::fIndex this is (Interface::kAnyIndex: the OS's default again)

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
            nonvirtual void SendTo (span<const byte> data, const SocketAddress& sockAddr) const;

        public:
            /**
             *  Read the next message (typically a full packet) from the socket.
             *
             *  @see https://linux.die.net/man/2/recvfrom
             *
             *  if fromAddress != nullptr (legal to pass nullptr) - then it it is filled in with the source address the packet came from.
             * 
             *  returns a subspan (initial segment) of into, of length number of bytes read in packet.
             * 
             *  @todo DOCUMENT WHAT HAPPENS IF PACKET DOESNT FIT IN BUF!????
             *
             *  \note ***Cancelation Point***
             */
            nonvirtual span<byte> ReceiveFrom (span<byte> into, int flag, SocketAddress* fromAddress, Time::DurationSeconds timeout = Time::kInfinity) const;

        protected:
            /**
             */
            nonvirtual shared_ptr<_IRep> _GetSharedRep () const;

        protected:
            /**
             * \pre fRep_ != nullptr
             */
            nonvirtual _IRep& _ref () const;

        protected:
            /**
             * \pre fRep_ != nullptr
             */
            nonvirtual const _IRep& _cref () const;

        public:
            [[deprecated ("Since Stroika 3.0d15 use span{} overload")]] size_t ReceiveFrom (byte* intoStart, byte* intoEnd, int flag,
                                                                                            SocketAddress* fromAddress,
                                                                                            Time::DurationSeconds timeout = Time::kInfinity) const
            {
                return ReceiveFrom (span{intoStart, intoEnd}, flag, fromAddress, timeout).size ();
            }
            [[deprecated ("Since Stroika v3.0d15 use span overload")]] void SendTo (const byte* start, const byte* end, const SocketAddress& sockAddr) const
            {
                SendTo (span<const byte>{start, end}, sockAddr);
            }
        };

        /**
         */
        class _IRep : public Socket::_IRep {
        public:
            virtual ~_IRep () = default;

            virtual void SendTo (const byte* start, const byte* end, const SocketAddress& sockAddr) = 0;
            virtual size_t ReceiveFrom (byte* intoStart, byte* intoEnd, int flag, SocketAddress* fromAddress, Time::DurationSeconds timeout) = 0;
            // the interface: by address or by index (as the Ptr's overloads take it)
            virtual void JoinMulticastGroup (const InternetAddress& iaddr, const variant<InternetAddress, unsigned int>& onInterface)  = 0;
            virtual void LeaveMulticastGroup (const InternetAddress& iaddr, const variant<InternetAddress, unsigned int>& onInterface) = 0;
            virtual void SetMulticastInterface (const variant<InternetAddress, unsigned int>& onInterface)                             = 0;
            virtual uint8_t GetMulticastTTL () const                                                                                   = 0;
            virtual void    SetMulticastTTL (uint8_t ttl)                                                                              = 0;
            virtual bool    GetMulticastLoopMode () const                                                                              = 0;
            virtual void    SetMulticastLoopMode (bool loopMode)                                                                       = 0;
        };

        /**
         *  \par Example Usage
         *      \code
         *          ConnectionlessSocket::Ptr   s  = ConnectionlessSocket::New (SocketAddress::INET, Socket::DGRAM);
         *      \endcode
         *
         *  \pre socketKind != SOCK_STREAM
         *
         *  \note unless you call @Detach() - socket is CLOSED in DTOR of rep, so when final reference goes away
         *
         *  \note ConnectionlessSocket is not copyable, but it can be copied into a ConnectionlessSocket::Ptr or
         *        Socket::Ptr.  This is critical to save them in a container, for example.
         */
        ConnectionlessSocket::Ptr New (SocketAddress::FamilyType family, Type socketKind, const optional<IPPROTO>& protocol = nullopt);

        /**
         *  This function associates a Platform native socket handle with a Stroika wrapper object.
         *
         *  Once a PlatformNativeHandle is attached to Socket object, it will be automatically closed
         *  when the last reference to the socket disappears (or when someone calls close).
         *
         *  To prevent that behavior, you can Detach the PlatformNativeHandle before destroying
         *  the associated Socket object.
         */
        Ptr Attach (PlatformNativeHandle sd);

    };

}

namespace Stroika::Foundation::Execution::WaitForIOReady_Support {

    // Specialize to override GetSDKPollable
    template <typename T>
    struct WaitForIOReady_Traits;
    template <>
    struct WaitForIOReady_Traits<IO::Network::ConnectionlessSocket::Ptr> {
        using HighLevelType = IO::Network::ConnectionlessSocket::Ptr;
        static inline auto GetSDKPollable (const HighLevelType& t)
        {
            return t.GetNativeSocket ();
        }
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ConnectionlessSocket.inl"

#endif /*_Stroika_Foundation_IO_Network_ConnectionlessSocket_h_*/
