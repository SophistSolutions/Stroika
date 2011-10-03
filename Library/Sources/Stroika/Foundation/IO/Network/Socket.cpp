/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../StroikaPreComp.h"

#include	<sys/types.h>

#if		qPlatform_Windows
	#include	<winsock2.h>
	#include	<ws2tcpip.h>
	#include	<io.h>
#elif	qPlatform_POSIX
	#include	<unistd.h>
	#include	<sys/socket.h>
	#include	<netdb.h>
#endif



#include	<cstdlib>

#include	"Socket.h"




using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::IO;
using	namespace	Stroika::Foundation::IO::Network;



class	Socket::Rep_ {
	public:
		NativeSocket	fSD_;
	public:
		Rep_ ()
			{
			}
		Rep_ (NativeSocket sd)
			: fSD_ (sd)
			{
			}

	public:
		void	Close ()
			{
				#if		qPlatform_Windows
					::_close (fSD_);
				#elif	qPlatform_POSIX
					::close (fSD_);
				#else
					AssertNotImplemented ();
				#endif
			}
	public:
		size_t	Read (Byte* buffer, size_t bufSize) override
			{
				// Must do erorr checking and throw exceptions!!!
				#if		qPlatform_Windows
					return ::_read (fSD_, buffer, bufSize);
				#elif	qPlatform_POSIX
					return ::read (fSD_, buffer, bufSize);
				#else
					AssertNotImplemented ();
				#endif
			}
	public:
		void	Write (const Byte* buffer, size_t bufSize) override
			{
				// Must do erorr checking and throw exceptions!!!
				#if		qPlatform_Windows
					int		n	=	::_write (fSD_, buffer, bufSize);
				#elif	qPlatform_POSIX
					int		n	=	::write (fSD_, buffer, bufSize);
				#else
					AssertNotImplemented ();
				#endif
			}
	public:
		void	Bind (const Socket::BindProperties& bindProperties)
			{
				addrinfo hints;
				addrinfo* res = nullptr;
				memset ((void*)&hints, 0, sizeof(hints));
				hints.ai_family = AF_UNSPEC;
			    hints.ai_socktype = SOCK_STREAM;
				hints.ai_flags = AI_PASSIVE;
				string	tmp	=	bindProperties.fHostName.AsUTF8<string> ();	// BAD - SB tstring - or??? not sure what...
				if (getaddrinfo (tmp.c_str (), nullptr, &hints, &res) < 0) {
					// throw
				}
				fSD_ = socket(AF_INET, SOCK_STREAM, 0);

				if (::bind (fSD_, res->ai_addr, res->ai_addrlen) < 0) {
					// throw
				}
			}

	public:
		Socket	Accept ()
			{
				sockaddr	peer;
				memset (&peer, 0, sizeof (peer));

				int	sz	=	sizeof (peer);
				int r = accept(fSD_, &peer, &sz);
// must update Socket object so CTOR also takes (optional) sockaddr (for the peer - mostly to answer  other quesiutona later)
				if (r < 0) {
					// throw...
				}
				return Socket (r);
			}

};





Socket::Socket ()
{
}

Socket::Socket (const Socket& s)
{
}

Socket::Socket (NativeSocket sd)
{
}

Socket::~Socket ()
{
}

const Socket& Socket::operator= (const Socket& s)
{
	return *this;
}

void	Socket::Bind (const BindProperties& bindProperties)
{
	fRep_->Bind (bindProperties);
}

Socket	Socket::Accept ()
{
	return fRep_->Accept ();
}

size_t	Socket::Read (Byte* buffer, size_t bufSize)
{
	return fRep_->Read (buffer, bufSize);
}

void	Socket::Write (const Byte* buffer, size_t bufSize)
{
	fRep_->Write (buffer, bufSize);
}

void	Socket::Close ()
{
	fRep_->Close ();
}
