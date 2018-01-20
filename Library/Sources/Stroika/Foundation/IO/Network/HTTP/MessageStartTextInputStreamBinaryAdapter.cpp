/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include <mutex>

#include "../../../Execution/Common.h"
#include "../../../Execution/OperationNotSupportedException.h"
#include "../../../Memory/SmallStackBuffer.h"

#include "MessageStartTextInputStreamBinaryAdapter.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;
using namespace Stroika::Foundation::Streams;

using Characters::Character;

namespace {
    constexpr size_t kDefaultBufSize_ = 1024; // guess enough for http headers?
}

//@todo - NOTE - NOT RIGHT - RE_READ RFP - maybe need to do mime decoding???
// http://stackoverflow.com/questions/4400678/http-header-should-use-what-character-encoding
// but for now this seems and adequate hack

class MessageStartTextInputStreamBinaryAdapter::Rep_ : public InputStream<Character>::_IRep {
public:
    Rep_ (const InputStream<Byte>::Ptr& src)
        : fCriticalSection_ ()
        , fSource_ (src)
        , fReadDataBuf_ (kDefaultBufSize_)
        , fOffset_ (0)
        , fBufferFilledUpValidBytes_ (0)
    {
    }

protected:
    virtual bool IsSeekable () const override
    {
        return true;
    }
    virtual void CloseRead () override
    {
        Require (IsOpenRead ());
        fSource_.Close ();
        Assert (fSource_ == nullptr);
    }
    virtual bool IsOpenRead () const override
    {
        return fSource_ != nullptr;
    }
    virtual size_t Read (Character* intoStart, Character* intoEnd) override
    {
        Require ((intoStart == intoEnd) or (intoStart != nullptr));
        Require ((intoStart == intoEnd) or (intoEnd != nullptr));
        Require (IsOpenRead ());

        if (intoStart == nullptr) {
            return 0;
        }
        AssertNotNull (intoStart);
        auto critSec{make_unique_lock (fCriticalSection_)};
        Assert (fBufferFilledUpValidBytes_ >= fOffset_); // limitation/feature of current implemetnation
        if (fBufferFilledUpValidBytes_ == fOffset_) {
            size_t roomLeftInBuf = fReadDataBuf_.GetSize () - fBufferFilledUpValidBytes_;
            if (roomLeftInBuf == 0) {
                // should be quite rare
                fReadDataBuf_.GrowToSize (fBufferFilledUpValidBytes_ + kDefaultBufSize_);
                roomLeftInBuf = fReadDataBuf_.GetSize () - fBufferFilledUpValidBytes_;
            }
            Assert (roomLeftInBuf > 0);

            //tmphack
            {
                // this code is crap and needs to be thrown out/rewritten - but this kludge may get us limping along
                size_t nBytesNeeded = intoEnd - intoStart;
                if (roomLeftInBuf > nBytesNeeded) {
                    roomLeftInBuf = nBytesNeeded;
                }
            }

            Byte*  startReadAt = fReadDataBuf_.begin () + fBufferFilledUpValidBytes_;
            size_t n           = fSource_.Read (startReadAt, startReadAt + roomLeftInBuf);
            Assert (n <= roomLeftInBuf);
            // if n == 0, OK, just means EOF
            fBufferFilledUpValidBytes_ += n;
        }

        // At this point - see if we can fullfill the request. If not - its cuz we got EOF
        size_t outN = 0;
        for (Character* outChar = intoStart; outChar != intoEnd; ++outChar) {
            if (fOffset_ < fBufferFilledUpValidBytes_) {
                *outChar = Characters::Character ((char)*(fReadDataBuf_.begin () + fOffset_));
                fOffset_++;
                outN++;
            }
        }
        Ensure (outN <= static_cast<size_t> (intoEnd - intoStart));
        return outN;
    }
    virtual Memory::Optional<size_t> ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) override
    {
        Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
        Require (IsOpenRead ());
        auto critSec{make_unique_lock (fCriticalSection_)};
        // See if data already in fReadDataBuf_. If yes, then data available. If no, ReadNonBlocking () from upstream, and return that result.
        if (fOffset_ < fBufferFilledUpValidBytes_) {
            return _ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (intoStart, intoEnd, fBufferFilledUpValidBytes_ - fOffset_);
        }
        if (Memory::Optional<size_t> n = fSource_.ReadNonBlocking ()) {
            return _ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (intoStart, intoEnd, *n);
        }
        return {};
    }
    virtual SeekOffsetType GetReadOffset () const override
    {
        auto critSec{make_unique_lock (fCriticalSection_)};
        Require (IsOpenRead ());
        return fOffset_;
    }
    virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        auto critSec{make_unique_lock (fCriticalSection_)};
        Require (IsOpenRead ());
        switch (whence) {
            case Whence::eFromStart: {
                if (offset < 0) {
                    Execution::Throw (std::range_error ("seek"));
                }
                SeekOffsetType uOffset = static_cast<SeekOffsetType> (offset);
                if (uOffset > fBufferFilledUpValidBytes_) {
                    Execution::Throw (std::range_error ("seek"));
                }
                // Note - warning here  legit - our caching strategy wtih string is bogus and wont work with large streams
                fOffset_ = static_cast<size_t> (offset);
            } break;
            case Whence::eFromCurrent: {
                Streams::SeekOffsetType       curOffset = fOffset_;
                Streams::SignedSeekOffsetType newOffset = curOffset + offset;
                if (newOffset < 0) {
                    Execution::Throw (std::range_error ("seek"));
                }
                SeekOffsetType uNewOffset = static_cast<SeekOffsetType> (newOffset);
                if (uNewOffset > fBufferFilledUpValidBytes_) {
                    Execution::Throw (std::range_error ("seek"));
                }
                // Note - warning here  legit - our caching strategy wtih string is bogus and wont work wtih large streams
                fOffset_ = static_cast<size_t> (newOffset);
            } break;
            case Whence::eFromEnd: {
                Streams::SeekOffsetType       curOffset = fOffset_;
                Streams::SignedSeekOffsetType newOffset = fBufferFilledUpValidBytes_ + offset;
                if (newOffset < 0) {
                    Execution::Throw (std::range_error ("seek"));
                }
                SeekOffsetType uNewOffset = static_cast<SeekOffsetType> (newOffset);
                if (uNewOffset > fBufferFilledUpValidBytes_) {
                    Execution::Throw (std::range_error ("seek"));
                }
                // Note - warning here  legit - our caching strategy wtih string is bogus and wont work wtih large streams
                fOffset_ = static_cast<size_t> (newOffset);
            } break;
        }
        Ensure ((0 <= fOffset_) and (fOffset_ <= fBufferFilledUpValidBytes_));
        return GetReadOffset ();
    }

private:
    mutable recursive_mutex        fCriticalSection_;
    InputStream<Byte>::Ptr         fSource_;
    Memory::SmallStackBuffer<Byte> fReadDataBuf_; // OK cuz typically this will be very small (1k) and not really grow...
    // but it can if we must
    size_t fOffset_;                   // text stream offset
    size_t fBufferFilledUpValidBytes_; // nbytes of valid text in fReadDataBuf_
};

/*
 ********************************************************************************
 ******* IO::Network::HTTP::MessageStartTextInputStreamBinaryAdapter ************
 ********************************************************************************
 */
MessageStartTextInputStreamBinaryAdapter::MessageStartTextInputStreamBinaryAdapter (const InputStream<Byte>::Ptr& src)
    : InputStream<Character>::Ptr (make_shared<Rep_> (src))
{
}
