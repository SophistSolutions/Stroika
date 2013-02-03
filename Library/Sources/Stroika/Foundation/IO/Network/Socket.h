/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Socket_h_
#define _Stroika_Foundation_IO_Network_Socket_h_    1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"
#include    "SocketAddress.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {


                using   Characters::String;


                /**
                 * TODO:
                 *
                 *      @todo   Add close method. Since as smart pr just detach envelope not enuf.
                 *              Effectively closures other smart pointers to same logical socket.
                 *              Means apid cannot assert about closed but instead treat as exception
                 *
                 *      @todo   Docuemnt (or define new expcetion) thrown when operaiton done on CLOSED socket.
                 *
                 *      @todo   Document THREADSAFETY.
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
                    typedef SOCKET  PlatformNativeHandle;
#else
                    typedef int     PlatformNativeHandle;
#endif
                protected:
                    class   _Rep;

                public:
                    /**
                     * 'second arg' to ::socket() call
                    */
                enum class SocketKind : int {
                        STREAM  =   SOCK_STREAM,
                        DGRAM   =   SOCK_DGRAM,
                        RAW     =   SOCK_RAW,
                    };


                public:
                    /**
                    // Note - socket is CLOSED (filesystem close for now) in DTOR
                    // TODO:
                    //          We will need an abstract Socket object, and maybe have  it refernce counted so close can happen when last refernce goes
                    //  away!
                    */
                    Socket ();
                    Socket (SocketKind socketKind);
                    Socket (Socket && s);
                    Socket (const Socket& s);

                protected:
                    explicit Socket (shared_ptr<_Rep> && rep);
                    explicit Socket (const shared_ptr<_Rep>& rep);

                public:
                    ~Socket ();
                    nonvirtual  const Socket& operator= (Socket && s);
                    nonvirtual  const Socket& operator= (const Socket& s);

                public:
                    /**
                     *  Once a PlatformNativeHandle is attached to Socket object, it will be automatically closed
                     *  when the last reference to the socket disappears (or when someone calls close).
                     *
                     *  To prevent that behavior, you can Detatch the PlatformNativeHandle.
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
                     * API must be ammended to include most bind properites, like the flags...
                     */
                    nonvirtual void   Bind (const SocketAddress& sockAddr, BindFlags bindFlags = BindFlags ());


                public:
                    /**
                     *  @todo   Need timeout on this API? Or global (for instance) timeout?
                     *
                     */
                    nonvirtual  Socket  Accept ();


                public:
                    /**
                     *  @todo   Need timeout on this API? Or global (for instance) timeout?
                     *
                     *   throws on error, and otherwise means should call accept
                     */
                    nonvirtual  void    Listen (unsigned int backlog);

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
                    nonvirtual  size_t    ReceiveFrom (Byte* intoStart, Byte* intoEnd, const SocketAddress& sockAddr);

                public:
                    /**
                     * Note that Socket is an envelope class, and there could be multiple references to
                     * the same underlying socket. But this closes ALL of them. It also removes the reference
                     * to the underlying rep (meaning that some Socket envelopes COULD have a rep with an underlying
                     * closed socket).
                     */
                    nonvirtual  void    Close ();

                public:
                    /**
                     *  A socket can be open or closed.
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
                    virtual void    Close () = 0;
                    virtual size_t  Read (Byte* intoStart, Byte* intoEnd) = 0;
                    virtual void    Write (const Byte* start, const Byte* end) = 0;
                    virtual void    SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr) = 0;
                    virtual size_t  ReceiveFrom (Byte* intoStart, Byte* intoEnd, const SocketAddress& sockAddr) = 0;
                    virtual void    Listen (unsigned int backlog) = 0;
                    virtual Socket  Accept () = 0;
                    virtual PlatformNativeHandle    GetNativeSocket () const = 0;
                };


            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_Socket_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Socket.inl"
