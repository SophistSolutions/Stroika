/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>

#include	"ExternallyOwnedMemoryBinaryInputStream.h"




using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Streams;






/*
 ********************************************************************************
 ***************** Streams::ExternallyOwnedMemoryBinaryInputStream **************
 ********************************************************************************
 */
ExternallyOwnedMemoryBinaryInputStream::ExternallyOwnedMemoryBinaryInputStream (const Byte* start, const Byte* end)
	: fCriticalSection_ ()
	, fCursor_ (start)
	, fStart_ (start)
	, fEnd_ (end)
{
}

size_t	ExternallyOwnedMemoryBinaryInputStream::_Read (Byte* intoStart, Byte* intoEnd)
{
	RequireNotNull (intoStart);
	RequireNotNull (intoEnd);
	Require (intoStart < intoEnd);
	size_t	nRequested	=	intoEnd - intoStart;
	Execution::AutoCriticalSection	critSec (fCriticalSection_);
	Assert ((fStart_ <= fCursor_) and (fCursor_ <= fEnd_));
	size_t	nAvail		=	fEnd_ - fCursor_;
	size_t	nCopied		=	min (nAvail, nRequested);
	memcpy (intoStart, fCursor_, nCopied);
	fCursor_ += nCopied;
	return nCopied;	// this can be zero on EOF
}
