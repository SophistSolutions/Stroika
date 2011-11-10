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

#include	"../../Execution/Exceptions.h"
#include	"../../Execution/OperationNotSupportedException.h"

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

Streams::SeekOffsetType	SocketStream::_GetOffset () const override
{
	Execution::DoThrow (Execution::OperationNotSupportedException (L"SocketStream::GetOffset"));
}

bool	SocketStream::_CanSeek (Streams::Whence whence) const override
{
	// Socket streams not seekable
	return false;
}

void	SocketStream::_Seek (Streams::Whence whence, Streams::SeekOffsetType offset) override
{
	Execution::DoThrow (Execution::OperationNotSupportedException (L"SocketStream::Seek"));
}
