/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>

#include	"../Execution/OperationNotSupportedException.h"

#include	"MemoryBinaryInputStream.h"




using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Streams;


/*
 ********************************************************************************
 ************************ Streams::MemoryBinaryInputStream **********************
 ********************************************************************************
 */
MemoryBinaryInputStream::MemoryBinaryInputStream (const Byte* start, const Byte* end)
	: fCriticalSection_ ()
	, fData_ (end-start)
	, fCursor_ ()
{
	memcpy (fData_.begin (), start, end-start);
	fCursor_ = fData_.begin ();
}

size_t	MemoryBinaryInputStream::_Read (Byte* intoStart, Byte* intoEnd)
{
	RequireNotNull (intoStart);
	RequireNotNull (intoEnd);
	Require (intoStart < intoEnd);
	size_t	nRequested	=	intoEnd - intoStart;
	Execution::AutoCriticalSection	critSec (fCriticalSection_);
	Assert ((fData_.begin () <= fCursor_) and (fCursor_ <= fData_.end ()));
	size_t	nAvail		=	fData_.end () - fCursor_;
	size_t	nCopied		=	min (nAvail, nRequested);
	memcpy (intoStart, fCursor_, nCopied);
	fCursor_ += nCopied;
	return nCopied;	// this can be zero on EOF
}

Streams::SeekOffsetType	MemoryBinaryInputStream::_GetOffset () const override
{
	// REALLY JUST NOT YET IMPLEMENTED - BUT IT SHOULD BE
	Execution::DoThrow (Execution::OperationNotSupportedException (L"SocketStream::GetOffset"));
}

bool	MemoryBinaryInputStream::_CanSeek (Streams::Whence whence) const override
{
	// REALLY JUST NOT YET IMPLEMENTED - BUT IT SHOULD BE
	return false;
}

void	MemoryBinaryInputStream::_Seek (Streams::Whence whence, Streams::SeekOffsetType offset) override
{
	// REALLY JUST NOT YET IMPLEMENTED - BUT IT SHOULD BE
	Execution::DoThrow (Execution::OperationNotSupportedException (L"SocketStream::Seek"));
}
