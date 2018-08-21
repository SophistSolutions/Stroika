/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/Format.h"
#include "../../../Characters/ToString.h"
#include "../../../Execution/Common.h"
#include "../../../Execution/OperationNotSupportedException.h"
#include "../../../Memory/SmallStackBuffer.h"

#include "MessageStartTextInputStreamBinaryAdapter.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;
using namespace Stroika::Foundation::Streams;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

namespace {
    constexpr size_t kDefaultBufSize_ = 2 * 1024; // guess enough for http headers (typically around .8K but little cost in reserving a bit more)
}

//@todo - NOTE - NOT RIGHT - RE_READ RFP - maybe need to do mime decoding???
// http://stackoverflow.com/questions/4400678/http-header-should-use-what-character-encoding
// but for now this seems and adequate hack

class MessageStartTextInputStreamBinaryAdapter::Rep_ : public InputStream<Character>::_IRep, protected Debug::AssertExternallySynchronizedLock {
public:
    Rep_ (const InputStream<Byte>::Ptr& src)
        : fSource_ (src)
        , fAllDataReadBuf_ (kDefaultBufSize_)
        , fOffset_ (0)
        , fBufferFilledUpValidBytes_ (0)
    {
    }

public:
    bool AssureHeaderSectionAvailable ()
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx (Stroika_Foundation_Debug_OptionalizeTraceArgs (L"MessageStartTextInputStreamBinaryAdapter::AssureHeaderSectionAvailable"));
#endif
        this->SeekRead (Whence::eFromStart, 0);
        Character c;
        enum state {
            gotCR,
            gotCRLF,
            gotCRLFCR,
            gotNOTHING,
        };
        state s = gotNOTHING;
        while (optional<size_t> o = ReadNonBlocking (&c, &c + 1)) {
            if (*o == 0) {
                return true; // tricky corner case - EOF in header - treat as available so we process whole header
            }
            Assert (*o == 1);
            switch (c.GetCharacterCode ()) {
                case '\r': {
                    switch (s) {
                        case gotNOTHING: {
                            s = gotCR;
                        } break;
                        case gotCRLF: {
                            s = gotCRLFCR;
                        } break;
                        default: {
                            DbgTrace (L"Looks like bad HTTP header");
                            s = gotNOTHING;
                        } break;
                    }
                } break;
                case '\n': {
                    switch (s) {
                        case gotCR: {
                            s = gotCRLF;
                        } break;
                        case gotCRLFCR: {
                            this->SeekRead (Whence::eFromStart, 0);
                            return true;
                        } break;
                        default: {
                            DbgTrace (L"Looks like bad HTTP header");
                            s = gotNOTHING;
                        } break;
                    }
                } break;
                default: {
                    s = gotNOTHING;
                } break;
            }
        }
        return false;
    }

public:
    nonvirtual Characters::String ToString (ToStringFormat format) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        StringBuilder                                       sb;
        sb += L"{";
        sb += L"Offset: " + Characters::Format (L"%d", fOffset_) + L", ";
        sb += L"HighWaterMark: " + Characters::Format (L"%d", fBufferFilledUpValidBytes_) + L", ";
        sb += L"TEXT: ";
        switch (format) {
            case ToStringFormat::eAsBytes: {
                for (size_t i = 0; i < fBufferFilledUpValidBytes_; ++i) {
                    sb += Characters::Format (L"x%x, ", fAllDataReadBuf_[i]);
                }
            } break;
            case ToStringFormat::eAsString: {
                sb += L"'";
                for (Character c : String::FromISOLatin1 (reinterpret_cast<const char*> (begin (fAllDataReadBuf_)), reinterpret_cast<const char*> (begin (fAllDataReadBuf_) + fBufferFilledUpValidBytes_))) {
                    switch (c.GetCharacterCode ()) {
                        case '\r':
                            sb += L"\\r";
                            break;
                        case '\n':
                            sb += L"\\n";
                            break;
                        default:
                            sb += c.GetCharacterCode ();
                            break;
                    }
                }
                sb += L"'";
            } break;
        }
        sb += L"}";
        return sb.str ();
    }

protected:
    virtual bool IsSeekable () const override
    {
        return true;
    }
    virtual void CloseRead () override
    {
        Require (IsOpenRead ());
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
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
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Assert (fBufferFilledUpValidBytes_ >= fOffset_); // limitation/feature of current implemetnation
        if (fBufferFilledUpValidBytes_ == fOffset_) {
            size_t roomLeftInBuf = fAllDataReadBuf_.GetSize () - fBufferFilledUpValidBytes_;
            if (roomLeftInBuf == 0) {
                // should be quite rare
                fAllDataReadBuf_.GrowToSize_uninitialized (fBufferFilledUpValidBytes_ + kDefaultBufSize_);
                roomLeftInBuf = fAllDataReadBuf_.GetSize () - fBufferFilledUpValidBytes_;
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

            Byte*  startReadAt = fAllDataReadBuf_.begin () + fBufferFilledUpValidBytes_;
            size_t n           = fSource_.Read (startReadAt, startReadAt + roomLeftInBuf);
            Assert (n <= roomLeftInBuf);
            // if n == 0, OK, just means EOF
            fBufferFilledUpValidBytes_ += n;
        }

        // At this point - see if we can fullfill the request. If not - its cuz we got EOF
        size_t outN = 0;
        for (Character* outChar = intoStart; outChar != intoEnd; ++outChar) {
            if (fOffset_ < fBufferFilledUpValidBytes_) {
                *outChar = Characters::Character ((char)*(fAllDataReadBuf_.begin () + fOffset_));
                fOffset_++;
                outN++;
            }
        }
        Ensure (outN <= static_cast<size_t> (intoEnd - intoStart));
        return outN;
    }
    virtual optional<size_t> ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) override
    {
        Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
        Require (IsOpenRead ());
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        // See if data already in fAllDataReadBuf_. If yes, then data available. If no, ReadNonBlocking () from upstream, and return that result.
        if (fOffset_ < fBufferFilledUpValidBytes_) {
            return _ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (intoStart, intoEnd, fBufferFilledUpValidBytes_ - fOffset_);
        }
        if (optional<size_t> n = fSource_.ReadNonBlocking ()) {
            return _ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (intoStart, intoEnd, *n);
        }
        return {};
    }
    virtual SeekOffsetType GetReadOffset () const override
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpenRead ());
        return fOffset_;
    }
    virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpenRead ());
        switch (whence) {
            case Whence::eFromStart: {
                if (offset < 0) {
                    Execution::Throw (range_error ("seek"));
                }
                SeekOffsetType uOffset = static_cast<SeekOffsetType> (offset);
                if (uOffset > fBufferFilledUpValidBytes_) {
                    Execution::Throw (range_error ("seek"));
                }
                // Note - warning here  legit - our caching strategy wtih string is bogus and wont work with large streams
                fOffset_ = static_cast<size_t> (offset);
            } break;
            case Whence::eFromCurrent: {
                Streams::SeekOffsetType       curOffset = fOffset_;
                Streams::SignedSeekOffsetType newOffset = curOffset + offset;
                if (newOffset < 0) {
                    Execution::Throw (range_error ("seek"));
                }
                SeekOffsetType uNewOffset = static_cast<SeekOffsetType> (newOffset);
                if (uNewOffset > fBufferFilledUpValidBytes_) {
                    Execution::Throw (range_error ("seek"));
                }
                // Note - warning here  legit - our caching strategy wtih string is bogus and wont work wtih large streams
                fOffset_ = static_cast<size_t> (newOffset);
            } break;
            case Whence::eFromEnd: {
                Streams::SignedSeekOffsetType newOffset = fBufferFilledUpValidBytes_ + offset;
                if (newOffset < 0) {
                    Execution::Throw (range_error ("seek"));
                }
                SeekOffsetType uNewOffset = static_cast<SeekOffsetType> (newOffset);
                if (uNewOffset > fBufferFilledUpValidBytes_) {
                    Execution::Throw (range_error ("seek"));
                }
                // Note - warning here  legit - our caching strategy wtih string is bogus and wont work wtih large streams
                fOffset_ = static_cast<size_t> (newOffset);
            } break;
        }
        Ensure ((0 <= fOffset_) and (fOffset_ <= fBufferFilledUpValidBytes_));
        return GetReadOffset ();
    }

private:
    InputStream<Byte>::Ptr         fSource_;
    Memory::SmallStackBuffer<Byte> fAllDataReadBuf_;           // OK cuz typically this will be very small (1k) and not really grow...but it can if we must
    size_t                         fOffset_;                   // text stream offset
    size_t                         fBufferFilledUpValidBytes_; // nbytes of valid text in fAllDataReadBuf_
};

/*
 ********************************************************************************
 ********* IO::Network::HTTP::MessageStartTextInputStreamBinaryAdapter **********
 ********************************************************************************
 */
MessageStartTextInputStreamBinaryAdapter::Ptr MessageStartTextInputStreamBinaryAdapter::New (const InputStream<Byte>::Ptr& src)
{
    return Ptr (make_shared<Rep_> (src));
}

/*
 ********************************************************************************
 ******** IO::Network::HTTP::MessageStartTextInputStreamBinaryAdapter::Ptr ******
 ********************************************************************************
 */
MessageStartTextInputStreamBinaryAdapter::Ptr::Ptr (const shared_ptr<InputStream<Character>::_IRep>& from)
    : inherited (from)
{
}

bool MessageStartTextInputStreamBinaryAdapter::Ptr::AssureHeaderSectionAvailable ()
{
    AssertMember (&_GetRepRWRef (), Rep_);
    return reinterpret_cast<Rep_*> (&_GetRepRWRef ())->AssureHeaderSectionAvailable ();
}

String MessageStartTextInputStreamBinaryAdapter::Ptr::ToString (ToStringFormat format) const
{
    AssertMember (&_GetRepConstRef (), Rep_);
    return reinterpret_cast<const Rep_*> (&_GetRepConstRef ())->ToString (format);
}
