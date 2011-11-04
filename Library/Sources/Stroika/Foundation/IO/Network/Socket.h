/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_Network_Socket_h_
#define	_Stroika_Foundation_IO_Network_Socket_h_	1

#include	"../../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<WinSock.h>
#endif

#include	"../../Characters/String.h"
#include	"../../Configuration/Common.h"



namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {
			namespace	Network {
				

				using	Characters::String;


				// Platform Socket descriptor - file descriptor on unix (something like this on windoze)
				#if		qPlatform_Windows
					typedef	SOCKET	NativeSocket;
				#else
					typedef	int	NativeSocket;
				#endif


				class	Socket {
					protected:
						class	_Rep;
					public:
						// Note - socket is CLOSED (filesystem close for now) in DTOR 
						// TODO:
						//			We will need an abstract Socket object, and maybe have  it refernce counted so close can happen when last refernce goes
						//	away!
						//
						Socket ();
						Socket (const Socket& s);

						// TODO: By default - when refcount goes to zero - this socket auto-closed. MAYBE offer overload where that is not so?
						explicit Socket (NativeSocket sd);
						~Socket ();
						const Socket& operator= (const Socket& s);

					public:
						struct	BindProperties {
							static	const	String	kANYHOST;
							static	const	int				kANYPORT				=	0;
							static	const	int				kDefaultListenBacklog	=	100;
							String			fHostName;
							int				fPort;
							unsigned int	fListenBacklog;
							unsigned int	fExtraBindFlags;		// eg. SO_REUSEADDR
						};
						// throws if socket already bound or valid - only legal on empty socket
						nonvirtual	void	Bind (const BindProperties& bindProperties);
						nonvirtual	Socket	Accept ();

						// throws on error, and otherwise means should call accept
						nonvirtual	void	Listen (unsigned int backlog);

					public:
						nonvirtual	size_t	Read (Byte* intoStart, Byte* intoEnd);
						nonvirtual	void	Write (const Byte* start, const Byte* end);

					public:
						nonvirtual	void	Close ();

					public:
						nonvirtual	NativeSocket	GetNativeSocket () const;

					private:
						Memory::SharedPtr<_Rep>	fRep_;
				};


				class	Socket::_Rep {
					public:
						virtual ~_Rep ();
						virtual	void	Close () = 0;
						virtual	size_t	Read (Byte* intoStart, Byte* intoEnd) = 0;
						virtual	void	Write (const Byte* start, const Byte* end) = 0;
						virtual	void	Listen (unsigned int backlog) = 0;
						virtual	Socket	Accept () = 0;
						virtual	NativeSocket	GetNativeSocket () const = 0;
				};

			}
		}
	}
}
#endif	/*_Stroika_Foundation_IO_Network_Socket_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Socket.inl"
