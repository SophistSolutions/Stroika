/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"../Containers/Common.h"

#include	"BufferedBinaryOutputStream.h"




using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Streams;


namespace	{
	const	size_t	kMinBufSize_	=	1 * 1024;
	const	size_t	kDefaultBufSize	=	16 * 1024;
}

/*
 ********************************************************************************
 *********************** Streams::BinaryOutputStream ****************************
 ********************************************************************************
 */
BufferedBinaryOutputStream::BufferedBinaryOutputStream (BinaryOutputStream& realOut)
	: fBuffer_ ()
	, fRealOut_ (realOut)
	#if		qDebug
	, fAborted_ (false)
	#endif
{
	fBuffer_.reserve (kDefaultBufSize);
}

BufferedBinaryOutputStream::~BufferedBinaryOutputStream ()
{
	Assert (fBuffer_.size () == 0);		// require FLUSHED
}

size_t	BufferedBinaryOutputStream::GetBufferSize () const
{
	return (fBuffer_.capacity ());
}

void	BufferedBinaryOutputStream::SetBufferSize (size_t bufSize)
{
	bufSize = max (bufSize, kMinBufSize_);
	if (bufSize < fBuffer_.size ()) {
		Flush ();
	}
	fBuffer_.reserve (bufSize);
}

void	BufferedBinaryOutputStream::Abort ()
{
	#if		qDebug
	fAborted_ = true;	// for debug sake track this
	#endif
	fBuffer_.clear ();
}

void	BufferedBinaryOutputStream::Flush ()
{
	Require (not fAborted_);
	if (fBuffer_.empty ()) {
		fRealOut_.Write (Containers::Start (fBuffer_), Containers::End (fBuffer_));
		fBuffer_.clear ();
	}
}

void	BufferedBinaryOutputStream::_Write (const Byte* start, const Byte* end) override
{
	Require (start < end);	// for BinaryOutputStream - this funciton requires non-empty write
	Require (not fAborted_);
	/*
	 * Minimize the number of writes at the possible cost of extra copying.
	 *
	 * See if there is room in the buffer, and use it up. Only when no more room do we flush.
	 */
	size_t	bufSpaceRemaining	=	fBuffer_.capacity () - fBuffer_.size ();
	size_t	size2WriteRemaining	=	end - start;
	
	size_t	copy2Buffer	=	min (bufSpaceRemaining, size2WriteRemaining);
	#if		qDebug
		size_t	oldCap	=	fBuffer_.capacity ();
	#endif
	fBuffer_.insert (fBuffer_.end (), start, start + copy2Buffer);
	Assert (oldCap == fBuffer_.capacity ());

	Assert (size2WriteRemaining >= copy2Buffer);
	size2WriteRemaining -= copy2Buffer;

	/*
	 * At this point - either the buffer is full, OR we are done writing. EITHER way - if the buffer is full - we may as well write it now.
	 */
	if (fBuffer_.capacity () == fBuffer_.size ()) {
		Flush ();
		Assert (fBuffer_.empty ());
	}
	Assert (oldCap == fBuffer_.capacity ());

	// If the remaining will fit in the buffer, then buffer. But if it won't - no point in using the buffer - just write directly to avoid the copy.
	// And no point - even if equal to buffer size - since it won't save any writes...
	if (size2WriteRemaining == 0) {
		// DONE
	}
	else if (size2WriteRemaining < fBuffer_.capacity ()) {
		fBuffer_.insert (fBuffer_.end (), start + copy2Buffer, end);
	}
	else {
		fRealOut_.Write (start + copy2Buffer, end);
	}
}

