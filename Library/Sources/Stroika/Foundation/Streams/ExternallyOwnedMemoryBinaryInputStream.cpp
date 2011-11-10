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

Streams::SeekOffsetType	ExternallyOwnedMemoryBinaryInputStream::_GetOffset () const override
{
	return fCursor_ - fStart_;
}

void	ExternallyOwnedMemoryBinaryInputStream::_Seek (Streams::Whence whence, Streams::SeekOffsetType offset) override
{
	Execution::AutoCriticalSection	critSec (fCriticalSection_);
	switch (whence) {
		case	FromStart_W: {
			if (offset < 0) {
				Execution::DoThrow (std::range_error ("seek"));
			}
			if (offset > (fEnd_ - fStart_)) {
				Execution::DoThrow (std::range_error ("seek"));
			}
			fCursor_ = fStart_ + offset;
		}
		break;
		case	FromCurrent_W: {
			Streams::SeekOffsetType curOffset	=	fCursor_ - fStart_;
			Streams::SeekOffsetType newOffset	=	curOffset + offset;
			if (newOffset < 0) {
				Execution::DoThrow (std::range_error ("seek"));
			}
			if (newOffset > (fEnd_ - fStart_)) {
				Execution::DoThrow (std::range_error ("seek"));
			}
			fCursor_ = fStart_ + newOffset;
		}
		break;
		case	FromEnd_W: {
			Streams::SeekOffsetType curOffset	=	fCursor_ - fStart_;
			Streams::SeekOffsetType newOffset	=	(fEnd_ - fStart_) + offset;
			if (newOffset < 0) {
				Execution::DoThrow (std::range_error ("seek"));
			}
			if (newOffset > (fEnd_ - fStart_)) {
				Execution::DoThrow (std::range_error ("seek"));
			}
			fCursor_ = fStart_ + newOffset;
		}
		break;
	}
	Ensure ((fStart_ <= fCursor_) and (fCursor_ <= fEnd_));
}
