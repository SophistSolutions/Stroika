/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Socket_h_
#define _Stroika_Foundation_IO_Network_Socket_h_    1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"
#include    "../../Execution/ErrNoException.h"

#include    "SocketAddress.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {


                using   Characters::String;
                using   Memory::Byte;


                /**
                 * TODO:
                 *
                 *      @todo   In socket class, set CLOSE_ON_EXEC?
                 *
                 *      @todo   Document (or define new expcetion) thrown when operaiton done on CLOSED socket.
                 *              and acutally handle all the nullptr cases...
                 *
                 *      @todo   Document THREADSAFETY.
                 *
                 *      @todo   See about socket 'connected' state, and the 'connect' operation.
                 *              And see about send/recv() API - and docuemnt about only working when
                 *              connected.
                 */


                /**
                 *  Note that Socket acts a bit like a smart_ptr<> - to an underlying operating system object.
                 *  They can be assigned to one another, and those assigned copies all refer to the same
                 *  underlying object.
                 *
                 *  Closing one, closes them all (though overwriting one just has the effect of detatching
                 *  from the underlying socket.
                 *
                 *  When the last reference to an underlying socket represenation is lost, the native socket
                 *  is automatically closed (unless manually Detached first).
                 *
                 */
                class   Socket {
                public:
                    /**
                     *  Platform Socket descriptor - file descriptor on unix (something like this on windoze)
                     */
#if     qPlatform_Windows
                    using   PlatformNativeHandle    =   SOCKET;
#else
                    using   PlatformNativeHandle    =   int;
#endif
                protected:
                    class   _Rep;

                public:
                    /**
                     * 'second arg' to ::socket() call
                     */
                    enum    class   SocketKind : int {
                        STREAM  =   SOCK_STREAM,
                        DGRAM   =   SOCK_DGRAM,
                        RAW     =   SOCK_RAW,
                    };


                public:
                    /**
                     *  Note - socket is CLOSED (filesystem close for now) in DTOR
                     *
                     *  TODO:
                     *          We will need an abstract Socket object, and maybe have  it refernce
                     *          counted so close can happen when last refernce goes
                     *          away!
                     */
                    Socket ();
                    Socket (SocketKind socketKind);
                    Socket (Socket&&  s);
                    Socket (const Socket& s);

                protected:
                    explicit Socket (shared_ptr<_Rep>&&  rep);
                    explicit Socket (const shared_ptr<_Rep>& rep);

                public:
                    ~Socket ();
                    nonvirtual  Socket& operator= (Socket && s);
                    nonvirtual  Socket& operator= (const Socket& s);

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
                    static  Socket  Attach (PlatformNativeHandle sd);

                public:
                    /**
                     *  Marks this Socket (and and sockets copied from it, before or after). This can be used
                     *  to prevent the underlying native socket from being closed.
                     */
                    nonvirtual  PlatformNativeHandle    Detach ();

                public:
                    struct  BindFlags {
                        bool    fReUseAddr: 1;          // SO_REUSEADDR
                        BindFlags (bool reUseAddr = false);
                    };

                public:
                    /**
                     *  Associate this socket object with the given address.
                     *
                     *  @todo   SB an overload taking just a port, and defaults to INADDRANY with that port.
                     *          Port is only thing really needed, but InternetAddress part provided as arg
                     *          too in case you want to bind to a particular interface.
                     *
                     *  @todo   CLARIFY if a socket can be bound to more than one address (and what about unbind)?
                     *
                     *  @see POSIX bind()
                     */
                    nonvirtual void   Bind (const SocketAddress& sockAddr, BindFlags bindFlags = BindFlags ());


                public:
                    /**
                     */
                    nonvirtual  void    JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface = V4::kAddrAny);

                public:
                    /**
                     */
                    nonvirtual  void    LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface = V4::kAddrAny);

                public:
                    /**
                     *  This specifies the number of networks to traverse in sending the multicast message.
                     *  It defaults to 1.
                     */
                    nonvirtual  uint8_t     GetMulticastTTL ();

                public:
                    /**
                     *  This specifies the number of networks to traverse in sending the multicast message.
                     *  It defaults to 1.
                     */
                    nonvirtual  void        SetMulticastTTL (uint8_t ttl);

                public:
                    /**
                     *  This determines whether the data sent will be looped back to sender host or not.
                     */
                    nonvirtual  bool        GetMulticastLoopMode ();

                public:
                    /**
                     *  This determines whether the data sent will be looped back to sender host or not.
                     */
                    nonvirtual  void        SetMulticastLoopMode (bool loopMode);

                public:
                    /**
                     *  @todo   Need timeout on this API? Or global (for instance) timeout?
                     *
                     *   throws on error, and otherwise means should call accept
                     */
                    nonvirtual  void    Listen (unsigned int backlog);

                public:
                    /**
                     *  After Listen() on a connected socket returns (not throws) - you can call Accept() on tha same
                     *  socket to allocate a NEW socket with the new connection stream.
                     *
                     *  @todo   Need timeout on this API? Or global (for instance) timeout?
                     *
                     */
                    nonvirtual  Socket  Accept ();


                public:
                    /**
                     *  @todo   Need timeout on this API? Or global (for instance) timeout?
                     *
                     */
                    nonvirtual  size_t  Read (Byte* intoStart, Byte* intoEnd);


                public:
                    /**
                     *  @todo   Need timeout on this API? Or global (for instance) timeout?
                     *
                     */
                    nonvirtual  void    Write (const Byte* start, const Byte* end);

                public:
                    /**
                     *  @todo   Clarify distinctions between read/write and send/sendto/recv/recvfrom
                     *
                     */
                    nonvirtual  void    SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr);

                public:
                    /**
                     *  @todo   Clarify distinctions between read/write and send/sendto/recv/recvfrom
                     *
                     */
                    nonvirtual  size_t    ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress);

                public:
                    /**
                     *  Note that Socket is an envelope class, and there could be multiple references to
                     *  the same underlying platform socket. But this closes ALL of them. It also removes the reference
                     *  to the underlying rep (meaning that some Socket envelopes COULD have a rep with an
                     *  underlying closed socket).
                     */
                    nonvirtual  void    Close ();

                public:
                    /**
                     *  A socket can be open or closed. Open is roughly analagous to non-null. A socket once closed
                     *  can never be 'Opened' - but you can assign a new Open socket to the Socket object.
                     *
                     *  @see Close
                     */
                    nonvirtual  bool    IsOpen () const;

                public:
                    /**
                     *  Return the native platform handle object associated with this socket
                     *  (typically an integer file descriptor)
                     */
                    nonvirtual  PlatformNativeHandle    GetNativeSocket () const;

                private:
                    shared_ptr<_Rep> fRep_;
                };


                /**
                 */
                class   Socket::_Rep {
                public:
                    virtual ~_Rep ();
                    virtual void                    Close () = 0;
                    virtual size_t                  Read (Byte* intoStart, Byte* intoEnd) = 0;
                    virtual void                    Write (const Byte* start, const Byte* end) = 0;
                    virtual void                    SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr) = 0;
                    virtual size_t                  ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress) = 0;
                    virtual void                    Listen (unsigned int backlog) = 0;
                    virtual Socket                  Accept () = 0;
                    virtual void                    JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) = 0;
                    virtual void                    LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface) = 0;
                    virtual uint8_t                 GetMulticastTTL () const = 0;
                    virtual void                    SetMulticastTTL (uint8_t ttl) = 0;
                    virtual bool                    GetMulticastLoopMode () const = 0;
                    virtual void                    SetMulticastLoopMode (bool loopMode) = 0;
                    virtual PlatformNativeHandle    GetNativeSocket () const = 0;
                };


#if     qPlatform_Windows
                /**
                 * This must be called before any Sockets are created, otherwise its
                 *  an erorr (requirement failure).
                 *
                 *  This defaults to ON
                 */
                void    AutosetupWinsock(bool setup);
#endif


            }
        }
    }
}


#if     qPlatform_Windows
namespace Stroika {
    namespace Foundation {
        namespace Execution {
            template    <>
            IO::Network::Socket::PlatformNativeHandle    ThrowErrNoIfNegative (IO::Network::Socket::PlatformNativeHandle returnCode);
        }
    }
}
#endif



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Socket.inl"

#endif  /*_Stroika_Foundation_IO_Network_Socket_h_*/
