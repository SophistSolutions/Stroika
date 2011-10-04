/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_Network_Socket_h_
#define	_Stroika_Foundation_IO_Network_Socket_h_	1

#include	"../../StroikaPreComp.h"

#include	"../../Characters/String.h"
#include	"../../Configuration/Common.h"
#include	"../../Streams/BinaryInputStream.h"
#include	"../../Streams/BinaryOutputStream.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {
			namespace	Network {
				

				using	Characters::String;


				// Platform Socket descriptor - file descriptor on unix (something like this on windoze)
				typedef	int	NativeSocket;


				class	Socket {
					private:
						class	Rep_;
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
						void	Bind (const BindProperties& bindProperties);
						Socket	Accept ();

						// throws on error, and otherwise means should call accept
						void	Listen (unsigned int backlog);

					public:
						nonvirtual	size_t	Read (Byte* buffer, size_t bufSize);
						nonvirtual	void	Write (const Byte* buffer, size_t bufSize);

					public:
						nonvirtual	void	Close ();

					private:
						Memory::SharedPtr<Rep_>	fRep_;
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
