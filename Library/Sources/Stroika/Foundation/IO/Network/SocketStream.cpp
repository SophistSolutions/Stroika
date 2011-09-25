/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<io.h>
#elif	qPlatform_POSIX
	#include	<unistd.h>
#endif
#include	<cstdlib>

#include	"SocketStream.h"




using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::IO;
using	namespace	Stroika::Foundation::IO::Network;



// Very primitive, and unsatistifacotry - but perhaps usable -draft implemenation
//		-- LGP 2011-09-24



SocketStream::~SocketStream ()
{
#if		qPlatform_Windows
	::_close (fSD_);
#elif	qPlatform_POSIX
	::close (fSD_);
#else
	AssertNotImplemented ();
#endif
}

size_t	SocketStream::_Read (Byte* buffer, size_t bufSize) override
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

void	SocketStream::_Write (const Byte* buffer, size_t bufSize) override
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

