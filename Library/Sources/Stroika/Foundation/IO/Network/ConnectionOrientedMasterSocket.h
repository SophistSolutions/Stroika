/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_h_
#define _Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_h_ 1

#include "../../StroikaPreComp.h"

#include "ConnectionOrientedStreamSocket.h"

namespace Stroika::Foundation::IO::Network {

    /**
     *  This class is to be used with ConnectionOrientedStreamSocket. You create a ConnectionOrientedMasterSocket, and
     *  Bind () it, and Listen () on it, and the resulting sockets (from Accept()) are of type ConnectionOrientedStreamSocket.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter</a>
     */
    namespace ConnectionOrientedMasterSocket {

        using namespace Socket;

        class _IRep;

        /**
         *  \par Example Usage
         *      \code
         *          ConnectionOrientedMasterSocket::Ptr ms  = ConnectionOrientedMasterSocket::New (SocketAddress::INET, Socket::STREAM);
         *          ms.Bind (addr);
         *          ms.Listen (backlog);
         *          Sequence<ConnectionOrientedMasterSocket::Ptr>   l;  // cannot do Sequence<ConnectionOrientedMasterSocket> cuz not copyable
         *          l.push_back (ms);
         *      \endcode
         *
         *  \note Since ConnectionOrientedMasterSocket::Ptr is a smart pointer, the constness of the methods depends on whether they modify the smart pointer itself, not
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
             *  @todo   Need timeout on this API? Or global (for instance) timeout?
             *
             *   throws on error, and otherwise means should call accept
             */
            nonvirtual void Listen (unsigned int backlog) const;

        public:
            /**
             *  After Listen() on a connected socket returns (not throws) - you can call Accept() on tha same
             *  socket to allocate a NEW socket with the new connection stream.
             *
             *  @todo   Need timeout on this API? Or global (for instance) timeout?
             *
             *  \note ***Cancelation Point***
             */
            nonvirtual ConnectionOrientedStreamSocket::Ptr Accept () const;

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
        };

        /**
        */
        class _IRep : public Socket::_IRep {
        public:
            virtual ~_IRep ()                                                         = default;
            virtual void                                Listen (unsigned int backlog) = 0;
            virtual ConnectionOrientedStreamSocket::Ptr Accept ()                     = 0;
        };

        /**
         *  \par Example Usage
         *      \code
         *          ConnectionOrientedMasterSocket::Ptr ms = ConnectionOrientedMasterSocket::New (SocketAddress::INET, Socket::STREAM);
         *          ms.Bind (addr);
         *          ms.Listen (backlog);
         *      \endcode
         *
         *  \note unless you call @Detach() - socket is CLOSED in DTOR of rep, so when final reference goes away
         *
         *  \note ConnectionOrientedMasterSocket is not copyable, but it can be copied into a ConnectionOrientedMasterSocket::Ptr or
         *        Socket::Ptr.  This is critical to save them in a container, for example.
         */
        Ptr New (SocketAddress::FamilyType family, Type socketKind, const optional<IPPROTO>& protocol = {});

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
    struct WaitForIOReady_Traits<IO::Network::ConnectionOrientedMasterSocket::Ptr> {
        using HighLevelType = IO::Network::ConnectionOrientedMasterSocket::Ptr;
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
#include "ConnectionOrientedMasterSocket.inl"

#endif /*_Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_h_*/
