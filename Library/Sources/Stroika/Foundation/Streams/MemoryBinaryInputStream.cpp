/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
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
	return fCursor_ - fData_.begin ();
}

void	MemoryBinaryInputStream::_Seek (Streams::Whence whence, Streams::SeekOffsetType offset) override
{
	Execution::AutoCriticalSection	critSec (fCriticalSection_);
	switch (whence) {
		case	FromStart_W: {
			if (offset < 0) {
				Execution::DoThrow (std::range_error ("seek"));
			}
			if (static_cast<size_t> (offset) > fData_.GetSize ()) {
				Execution::DoThrow (std::range_error ("seek"));
			}
			fCursor_ = fData_.begin () + offset;
		}
		break;
		case	FromCurrent_W: {
			Streams::SeekOffsetType curOffset	=	fCursor_ - fData_.begin ();
			Streams::SeekOffsetType newOffset	=	curOffset + offset;
			if (newOffset < 0) {
				Execution::DoThrow (std::range_error ("seek"));
			}
			if (static_cast<size_t> (newOffset) > fData_.GetSize ()) {
				Execution::DoThrow (std::range_error ("seek"));
			}
			fCursor_ = fData_.begin () + newOffset;
		}
		break;
		case	FromEnd_W: {
			Streams::SeekOffsetType curOffset	=	fCursor_ - fData_.begin ();
			Streams::SeekOffsetType newOffset	=	fData_.GetSize () + offset;
			if (newOffset < 0) {
				Execution::DoThrow (std::range_error ("seek"));
			}
			if (static_cast<size_t> (newOffset) > fData_.GetSize ()) {
				Execution::DoThrow (std::range_error ("seek"));
			}
			fCursor_ = fData_.begin () + newOffset;
		}
		break;
	}
	Ensure ((fData_.begin () <= fCursor_) and (fCursor_ <= fData_.end ()));
}
