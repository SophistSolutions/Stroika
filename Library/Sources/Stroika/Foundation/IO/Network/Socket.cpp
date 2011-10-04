/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../StroikaPreComp.h"

#include	<cstdlib>
#include	<sys/types.h>

#if		qPlatform_Windows
	#include	<winsock2.h>
	#include	<ws2tcpip.h>
	#include	<io.h>
#elif	qPlatform_POSIX
	#include	<unistd.h>
	#include	<sys/socket.h>
	#include	<netdb.h>
	#include	<sys/ioctl.h>
#endif

#include	"../../Execution/Sleep.h"
#include	"../../Execution/Thread.h"
#include	"../../Execution/ErrNoException.h"

#include	"Socket.h"




using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Memory;
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
		void	Listen (unsigned int backlog)
			{
				Execution::ThrowErrNoIfNegative (::listen (fSD_, backlog));
			}

	public:
		Socket	Accept ()
			{
				// HACK - not right.... - Need to find a way to get interupted when abort is called
				#if		qPlatform_POSIX && 0
				fd_set	fs;
				FD_ZERO (&fs);
				FD_SET (fSD_, &fs);
				timeval	timeout;
				memset (&timeout, 0, sizeof (timeout));
				timeout.tv_sec  = 5;
				int	maxSD	=	fSD_;
				#endif
				
				sockaddr	peer;
				memset (&peer, 0, sizeof (peer));

AGAIN:
				socklen_t	sz	=	sizeof (peer);
				int r = accept(fSD_, &peer, &sz);
// must update Socket object so CTOR also takes (optional) sockaddr (for the peer - mostly to answer  other quesiutona later)
				if (r < 0) {
					// HACK - so we get interuptabilitiy.... MUST IMPROVE!!!
					if (errno == EAGAIN or errno == EWOULDBLOCK)
					{
						Execution::Sleep(1.0);
						Execution::CheckForThreadAborting();
						goto AGAIN;
					}
				}
				Execution::ThrowErrNoIfNegative (r);
				return Socket (r);
			}

};


const	String	Socket::BindProperties::kANYHOST;



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
	// Should this throw if already has something bound - already non-null!???


//	fRep_->Bind (bindProperties);
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

	sockaddr_in	useAddr;
	memset (&useAddr, 0, sizeof (useAddr));
	useAddr.sin_family = AF_INET;
	useAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	useAddr.sin_port = htons((short)bindProperties.fPort);

	NativeSocket sd;
	Execution::ThrowErrNoIfNegative (sd = socket(AF_INET, SOCK_STREAM, 0));


	// Allow socket descriptor to be reuseable
	{
		int    on = 1;
		Execution::ThrowErrNoIfNegative (::setsockopt(sd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on)));
	}

#if		qPlatform_POSIX
	{
		// Set socket to be non-blocking.  All of the sockets for the incoming connections will also be non-blocking since
		// they will inherit that state from the listening socket
		int    on = 1;
		Execution::ThrowErrNoIfNegative (::ioctl (sd, FIONBIO, (char *)&on));
	}
#endif

	Execution::ThrowErrNoIfNegative (::bind (sd, (sockaddr*)&useAddr, sizeof (useAddr)));

	fRep_  = SharedPtr<Rep_> (new Rep_ (sd));
}

void	Socket::Listen (unsigned int backlog)
{
	fRep_->Listen (backlog);
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
