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

namespace	{
	#if		qPlatform_Windows
		const	NativeSocket	kINVALID_NATIVE_HANDLE	=	INVALID_SOCKET;
	#elif	qPlatform_POSIX
		const	NativeSocket	kINVALID_NATIVE_HANDLE	=	-1;	// right value??
	#endif
}

class	Socket::Rep_ {
	public:
		NativeSocket	fSD_;
	public:
		Rep_ (NativeSocket sd)
			: fSD_ (sd)
			{
			}
		~Rep_ ()
			{
				if (fSD_ != kINVALID_NATIVE_HANDLE) {
					Close ();
				}
			}

	public:
		void	Close ()
			{
				if (fSD_ != kINVALID_NATIVE_HANDLE) {
					#if		qPlatform_Windows
						::closesocket (fSD_);
					#elif	qPlatform_POSIX
						::close (fSD_);
					#else
						AssertNotImplemented ();
					#endif
					fSD_ = kINVALID_NATIVE_HANDLE;
				}
			}
	public:
		size_t	Read (Byte* intoStart, Byte* intoEnd) override
			{
				// Must do erorr checking and throw exceptions!!!
				#if		qPlatform_Windows
					AssertNotImplemented ();
					return 0;
					//return ::_read (fSD_, intoStart, intoEnd - intoStart);
				#elif	qPlatform_POSIX
					return Execution::Handle_ErrNoResultInteruption ([this, &intoStart, &intoEnd] () -> int { return ::read (fSD_, intoStart, intoEnd - intoStart); });
				#else
					AssertNotImplemented ();
				#endif
			}
	public:
		void	Write (const Byte* start, const Byte* end) override
			{
				// Must do erorr checking and throw exceptions!!!
				#if		qPlatform_Windows
					AssertNotImplemented ();
					//int		n	=	::_write (fSD_, start, end - start);
				#elif	qPlatform_POSIX
					int		n	=	Execution::Handle_ErrNoResultInteruption ([this, &start, &end] () -> int { return ::write (fSD_, start, end - start); });
				#else
					AssertNotImplemented ();
				#endif
			}

	public:
		void	Listen (unsigned int backlog)
			{
				Execution::Handle_ErrNoResultInteruption ([this, &backlog] () -> int { return ::listen (fSD_, backlog); });
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
				NativeSocket	r = accept (fSD_, &peer, &sz);
// must update Socket object so CTOR also takes (optional) sockaddr (for the peer - mostly to answer  other quesiutona later)
				if (r < 0) {
					// HACK - so we get interuptabilitiy.... MUST IMPROVE!!!
					if (errno == EAGAIN or errno == EINTR or errno == EWOULDBLOCK)
					{
						Execution::Sleep(1.0);
						Execution::CheckForThreadAborting();
						goto AGAIN;
					}
				}
				#if		qPlatform_Windows
					AssertNotImplemented ();
				#elif	qPlatform_POSIX
					Execution::ThrowErrNoIfNegative (r);
				#endif
				return Socket (r);
			}

};


const	String	Socket::BindProperties::kANYHOST;



Socket::Socket ()
	: fRep_ ()
{
}

Socket::Socket (const Socket& s)
	: fRep_ (s.fRep_)
{
}

Socket::Socket (NativeSocket sd)
	: fRep_ (SharedPtr<Rep_> (new Rep_ (sd)))
{
}

Socket::~Socket ()
{
}

const Socket& Socket::operator= (const Socket& s)
{
	if (fRep_ != s.fRep_) {
		fRep_ = s.fRep_;
	}
	return *this;
}

void	Socket::Bind (const BindProperties& bindProperties)
{
	// Should this throw if already has something bound - already non-null!???
	if (not fRep_.IsNull () and fRep_->fSD_ != kINVALID_NATIVE_HANDLE) {
		throw Execution::StringException (L"Cannot bind an already bound socket");
	}

	addrinfo hints;
	addrinfo* res = nullptr;
	memset ((void*)&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	string	tmp	=	bindProperties.fHostName.AsUTF8<string> ();	// BAD - SB tstring - or??? not sure what...
	try {
		Execution::Handle_ErrNoResultInteruption ([&tmp, &hints, &res] () -> int { return getaddrinfo (tmp.c_str (), nullptr, &hints, &res);});
	}
	catch (...) {
		// MUST FIX THIS - BROKEN - BUT LEAVE IGNORING ERRORS FOR NOW...
	}

	sockaddr_in	useAddr;
	memset (&useAddr, 0, sizeof (useAddr));
	useAddr.sin_family = AF_INET;
	useAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	useAddr.sin_port = htons((short)bindProperties.fPort);

	NativeSocket sd;
#if		qPlatform_POSIX
	sd = Execution::Handle_ErrNoResultInteruption ([]() -> int { return socket (AF_INET, SOCK_STREAM, 0); });
#else
	sd = 0;
	AssertNotImplemented ();
#endif


	// Allow socket descriptor to be reuseable
	{
		int    on = 1;
		Execution::Handle_ErrNoResultInteruption ([&sd, &on] () -> int { return ::setsockopt(sd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on)); });
	}


#if 0
	// Now that we have signal/interuption - we can use blocking IO - A BIG improvement... but test carefully
#if		qPlatform_POSIX
	{
		// Set socket to be non-blocking.  All of the sockets for the incoming connections will also be non-blocking since
		// they will inherit that state from the listening socket
		int    on = 1;
		Execution::ThrowErrNoIfNegative (::ioctl (sd, FIONBIO, (char *)&on));
	}
#endif
#endif

	Execution::Handle_ErrNoResultInteruption ([&sd, &useAddr] () -> int { return ::bind (sd, (sockaddr*)&useAddr, sizeof (useAddr));});

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

size_t	Socket::Read (Byte* intoStart, Byte* intoEnd)
{
	return fRep_->Read (intoStart, intoEnd);
}

void	Socket::Write (const Byte* start, const Byte* end)
{
	fRep_->Write (start, end);
}

void	Socket::Close ()
{
	// not importnat to null-out, but may as well...
	fRep_->Close ();
	fRep_.clear ();
}
