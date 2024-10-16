/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Execution/Common.h"
#include "Stroika/Foundation/Execution/OperationNotSupportedException.h"
#include "Stroika/Foundation/Memory/InlineBuffer.h"

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

class MessageStartTextInputStreamBinaryAdapter::Rep_ : public InputStream::IRep<Character> {
    using inherited = InputStream::IRep<Character>;

public:
    Rep_ (const InputStream::Ptr<byte>& src)
        : fSource_{src}
        , fAllDataReadBuf_{kDefaultBufSize_}
        , fOffset_{0}
        , fBufferFilledUpValidBytes_{0}
    {
    }

public:
    bool AssureHeaderSectionAvailable ()
    {
        // @todo fix - inefficient implementation - LGP 2023-12-30
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"MessageStartTextInputStreamBinaryAdapter::AssureHeaderSectionAvailable"};
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
        while (optional<span<Character>> o = Read (span{&c, &c + 1}, NoDataAvailableHandling::eDontBlock)) {
            if (o->size () == 0) {
                return true; // tricky corner case - EOF in header - treat as available so we process whole header
            }
            Assert (o->size () == 1);
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
                            DbgTrace ("Looks like bad HTTP header (\\r)"_f);
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
                            DbgTrace ("Looks like bad HTTP header (\\n)"_f);
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
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        StringBuilder                                         sb;
        sb << "{"sv;
        sb << "Offset: "sv << fOffset_ << ", "sv;
        sb << "HighWaterMark: "sv << fBufferFilledUpValidBytes_ << ", "sv;
        sb << "TEXT: "sv;
        switch (format) {
            case ToStringFormat::eAsBytes: {
                for (size_t i = 0; i < fBufferFilledUpValidBytes_; ++i) {
                    sb << Characters::Format ("x{:x}, "_f, fAllDataReadBuf_[i]);
                }
            } break;
            case ToStringFormat::eAsString: {
                sb << "'"sv;
                for (Character c : String::FromLatin1 (span{reinterpret_cast<const char*> (begin (fAllDataReadBuf_)), fBufferFilledUpValidBytes_})) {
                    switch (c.GetCharacterCode ()) {
                        case '\r':
                            sb << "\\r"sv;
                            break;
                        case '\n':
                            sb << "\\n"sv;
                            break;
                        default:
                            sb << c.GetCharacterCode ();
                            break;
                    }
                }
                sb << "'"sv;
            } break;
        }
        sb << "}"sv;
        return sb;
    }

protected:
    virtual bool IsSeekable () const override
    {
        return true;
    }
    virtual void CloseRead () override
    {
        Require (IsOpenRead ());
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        fSource_.Close ();
        Assert (fSource_ == nullptr);
    }
    virtual bool IsOpenRead () const override
    {
        return fSource_ != nullptr;
    }
    virtual optional<size_t> AvailableToRead () override
    {
        Require (IsOpenRead ());
        if (fOffset_ < fBufferFilledUpValidBytes_) {
            return fBufferFilledUpValidBytes_ - fOffset_;
        }
        // default impl handles this case since we are seekable
        return inherited::AvailableToRead ();
    }
    virtual optional<SeekOffsetType> RemainingLength () override
    {
        Require (IsOpenRead ());
        return nullopt; // could do a bit better, but not  important here - generally can do no better
    }
    virtual optional<span<Character>> Read (span<Character> intoBuffer, NoDataAvailableHandling blockFlag) override
    {
        Require (not intoBuffer.empty ());
        Require (IsOpenRead ());
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
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
                size_t nBytesNeeded = intoBuffer.size ();
                if (roomLeftInBuf > nBytesNeeded) {
                    roomLeftInBuf = nBytesNeeded;
                }
            }

            byte*  startReadAt = fAllDataReadBuf_.begin () + fBufferFilledUpValidBytes_;
            size_t n           = fSource_.Read (span{startReadAt, roomLeftInBuf}, blockFlag).size ();
            Assert (n <= roomLeftInBuf);
            // if n == 0, OK, just means EOF
            fBufferFilledUpValidBytes_ += n;
        }

        // At this point - see if we can fullfill the request. If not - its cuz we got EOF
        size_t outN = 0;
        for (auto outChar = intoBuffer.begin (); outChar != intoBuffer.end (); ++outChar) {
            if (fOffset_ < fBufferFilledUpValidBytes_) {
                // SEE http://stroika-bugs.sophists.com/browse/STK-969 - treat incoming chars as ascii for now
                *outChar = Characters::Character{(char32_t)*(fAllDataReadBuf_.begin () + fOffset_)};
                fOffset_++;
                outN++;
            }
        }
        Ensure (outN <= intoBuffer.size ());
        return intoBuffer.subspan (0, outN);
    }
    virtual SeekOffsetType GetReadOffset () const override
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        Require (IsOpenRead ());
        return fOffset_;
    }
    virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        Require (IsOpenRead ());
        static const auto kException_ = range_error{"seek"};
        switch (whence) {
            case Whence::eFromStart: {
                if (offset < 0) [[unlikely]] {
                    Execution::Throw (kException_);
                }
                SeekOffsetType uOffset = static_cast<SeekOffsetType> (offset);
                if (uOffset > fBufferFilledUpValidBytes_) [[unlikely]] {
                    Execution::Throw (kException_);
                }
                // Note - warning here  legit - our caching strategy wtih string is bogus and wont work with large streams
                fOffset_ = static_cast<size_t> (offset);
            } break;
            case Whence::eFromCurrent: {
                Streams::SeekOffsetType       curOffset = fOffset_;
                Streams::SignedSeekOffsetType newOffset = curOffset + offset;
                if (newOffset < 0) [[unlikely]] {
                    Execution::Throw (kException_);
                }
                SeekOffsetType uNewOffset = static_cast<SeekOffsetType> (newOffset);
                if (uNewOffset > fBufferFilledUpValidBytes_) [[unlikely]] {
                    Execution::Throw (kException_);
                }
                // Note - warning here  legit - our caching strategy wtih string is bogus and wont work wtih large streams
                fOffset_ = static_cast<size_t> (newOffset);
            } break;
            case Whence::eFromEnd: {
                Streams::SignedSeekOffsetType newOffset = fBufferFilledUpValidBytes_ + offset;
                if (newOffset < 0) [[unlikely]] {
                    Execution::Throw (kException_);
                }
                SeekOffsetType uNewOffset = static_cast<SeekOffsetType> (newOffset);
                if (uNewOffset > fBufferFilledUpValidBytes_) [[unlikely]] {
                    Execution::Throw (kException_);
                }
                // Note - warning here  legit - our caching strategy wtih string is bogus and wont work wtih large streams
                fOffset_ = static_cast<size_t> (newOffset);
            } break;
        }
        Ensure ((0 <= fOffset_) and (fOffset_ <= fBufferFilledUpValidBytes_));
        return GetReadOffset ();
    }

private:
    [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
    InputStream::Ptr<byte>                                         fSource_;
    Memory::InlineBuffer<byte> fAllDataReadBuf_; // OK cuz typically this will be very small (1k) and not really grow...but it can if we must
    size_t fOffset_;                             // text stream offset
    size_t fBufferFilledUpValidBytes_;           // nbytes of valid text in fAllDataReadBuf_
};

/*
 ********************************************************************************
 ********* IO::Network::HTTP::MessageStartTextInputStreamBinaryAdapter **********
 ********************************************************************************
 */
MessageStartTextInputStreamBinaryAdapter::Ptr MessageStartTextInputStreamBinaryAdapter::New (const InputStream::Ptr<byte>& src)
{
    return Ptr{make_shared<Rep_> (src)};
}

/*
 ********************************************************************************
 ******** IO::Network::HTTP::MessageStartTextInputStreamBinaryAdapter::Ptr ******
 ********************************************************************************
 */
MessageStartTextInputStreamBinaryAdapter::Ptr::Ptr (const shared_ptr<InputStream::IRep<Character>>& from)
    : inherited{from}
{
}

bool MessageStartTextInputStreamBinaryAdapter::Ptr::AssureHeaderSectionAvailable ()
{
    return Debug::UncheckedDynamicCast<Rep_&> (GetRepRWRef ()).AssureHeaderSectionAvailable ();
}

String MessageStartTextInputStreamBinaryAdapter::Ptr::ToString (ToStringFormat format) const
{
    return Debug::UncheckedDynamicCast<const Rep_&> (GetRepConstRef ()).ToString (format);
}
