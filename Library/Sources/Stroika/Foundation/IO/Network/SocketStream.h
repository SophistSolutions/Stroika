/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_Network_SocketStream_h_
#define	_Stroika_Foundation_IO_Network_SocketStream_h_	1

#include	"../../StroikaPreComp.h"

#include	"../../Configuration/Common.h"
#include	"../../Streams/BinaryInputStream.h"
#include	"../../Streams/BinaryOutputStream.h"
#include	"Socket.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {
			namespace	Network {

				// Platform Socket descriptor - file descriptor on unix (something like this on windoze)
				typedef	int	SocketDescriptor;

				class	SocketStream : public Streams::BinaryInputStream, public Streams::BinaryOutputStream {
					public:
						// Note - socket is CLOSED (filesystem close for now) in DTOR 
						// TODO:
						//			We will need an abstract Socket object, and maybe have  it refernce counted so close can happen when last refernce goes
						//	away!
						//
						explicit SocketStream (Socket sd);
						~SocketStream ();
	
					public:
						virtual	size_t	_Read (Byte* buffer, size_t bufSize) override;
						virtual	void	_Write (const Byte* buffer, size_t bufSize) override;

					private:
						Socket	fSD_;
				};

			}
		}
	}
}
#endif	/*_Stroika_Foundation_IO_Network_SocketStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"SocketStream.inl"
