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
}

size_t	SocketStream::_Read (Byte* intoStart, Byte* intoEnd) override
{
	return fSD_.Read (intoStart, intoEnd);
}

void	SocketStream::_Write (const Byte* start, const Byte* end) override
{
	fSD_.Write (start, end);
}

