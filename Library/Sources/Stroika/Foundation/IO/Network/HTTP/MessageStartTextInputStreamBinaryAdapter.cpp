/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include	<mutex>

#include    "../../../Execution/OperationNotSupportedException.h"
#include    "../../../Execution/OperationNotSupportedException.h"
#include    "../../../Memory/SmallStackBuffer.h"

#include    "MessageStartTextInputStreamBinaryAdapter.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;
using   namespace   Stroika::Foundation::IO::Network::HTTP;
using   namespace   Stroika::Foundation::Streams;




namespace {
	const size_t	kDefaultBufSize_ = 1024;	// guess enough for http headers?
}



//@todo - NOTE - NOT RIGHT - RE_READ RFP - maybe need to do mime decoding???
// http://stackoverflow.com/questions/4400678/http-header-should-use-what-character-encoding
// but for now this seems and adequate hack


class   MessageStartTextInputStreamBinaryAdapter::IRep_ : public TextInputStream::_IRep, public Seekable::_IRep {
public:
    IRep_ (const BinaryInputStream& src)
        : fCriticalSection_ ()
        , fSource_ (src)
		, fReadDataBuf_ (kDefaultBufSize_)
        , fOffset_ (0)
		, fBufferFilledUpValidBytes_ (0)
    {
    }

protected:
	virtual size_t    _Read (Character* intoStart, Character* intoEnd) override
	{
		Require ((intoStart == intoEnd) or (intoStart != nullptr));
		Require ((intoStart == intoEnd) or (intoEnd != nullptr));

		if (intoStart == nullptr) {
			return 0;
		}
		AssertNotNull (intoStart);
		lock_guard<recursive_mutex>  critSec (fCriticalSection_);
		Assert (fBufferFilledUpValidBytes_ >= fOffset_);				// limitation/feature of current implemetnation
		if (fBufferFilledUpValidBytes_ == fOffset_) {
			size_t roomLeftInBuf = fReadDataBuf_.GetSize () - fBufferFilledUpValidBytes_;
			if (roomLeftInBuf == 0) {
				// should be quite rare
				fReadDataBuf_.GrowToSize (fBufferFilledUpValidBytes_ + kDefaultBufSize_);
				roomLeftInBuf = fReadDataBuf_.GetSize () - fBufferFilledUpValidBytes_;
			}
			Assert (roomLeftInBuf > 0);
			Byte* startReadAt = fReadDataBuf_.begin () + fBufferFilledUpValidBytes_;
			size_t  n = fSource_.Read (startReadAt, startReadAt + roomLeftInBuf);
			Assert (n <= roomLeftInBuf);
			// if n == 0, OK, just means EOF
			fBufferFilledUpValidBytes_ += n;
		}

		// At this point - see if we can fullfill the request. If not - its cuz we got EOF
		size_t  outN = 0;
		for (Character* outChar = intoStart; outChar != intoEnd; ++outChar) {
			if (fOffset_ < fBufferFilledUpValidBytes_) {
				*outChar = Characters::Character ((char)* (fReadDataBuf_.begin () + fOffset_));
				fOffset_++;
				outN++;
			}
		}
        Ensure (outN <= static_cast<size_t> (intoEnd - intoStart));
        return outN;
    }

    virtual SeekOffsetType  GetOffset () const override
    {
        lock_guard<recursive_mutex>  critSec (fCriticalSection_);
        return fOffset_;
    }

    virtual SeekOffsetType  Seek (Whence whence, SignedSeekOffsetType offset) override
    {
        lock_guard<recursive_mutex>  critSec (fCriticalSection_);
        switch (whence) {
            case    Whence::eFromStart: {
                    if (offset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
					if (offset > fBufferFilledUpValidBytes_) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    // Note - warning here  legit - our caching strategy wtih string is bogus and wont work with large streams
                    fOffset_ = offset;
                }
                break;
            case    Whence::eFromCurrent: {
                    Streams::SeekOffsetType         curOffset   =   fOffset_;
                    Streams::SignedSeekOffsetType   newOffset   =   curOffset + offset;
                    if (newOffset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
					if (newOffset > fBufferFilledUpValidBytes_) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    // Note - warning here  legit - our caching strategy wtih string is bogus and wont work wtih large streams
                    fOffset_ = newOffset;
                }
                break;
            case    Whence::eFromEnd: {
                    Streams::SeekOffsetType         curOffset   =   fOffset_;
					Streams::SignedSeekOffsetType   newOffset	=	fBufferFilledUpValidBytes_ + offset;
                    if (newOffset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
					if (newOffset > fBufferFilledUpValidBytes_) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    // Note - warning here  legit - our caching strategy wtih string is bogus and wont work wtih large streams
                    fOffset_ = newOffset;
                }
                break;
        }
		Ensure ((0 <= fOffset_) and (fOffset_ <= fBufferFilledUpValidBytes_));
        return GetOffset ();
    }

private:
    mutable recursive_mutex			fCriticalSection_;
    BinaryInputStream				fSource_;
	Memory::SmallStackBuffer<Byte>	fReadDataBuf_;					// OK cuz typically this will be very small (1k) and not really grow...
																	// but it can if we must
	size_t							fOffset_;						// text stream offset
	size_t							fBufferFilledUpValidBytes_;		// nbytes of valid text in fReadDataBuf_
};




/*
 ********************************************************************************
 ******* IO::Network::HTTP::MessageStartTextInputStreamBinaryAdapter ************
 ********************************************************************************
 */
MessageStartTextInputStreamBinaryAdapter::MessageStartTextInputStreamBinaryAdapter (const BinaryInputStream& src)
	: TextInputStream (shared_ptr<_IRep> (new IRep_ (src)))
{
}