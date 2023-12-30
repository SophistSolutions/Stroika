/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/UTFConvert.h"
#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "../Execution/Common.h"
#include "../Memory/StackBuffer.h"

#include "IterableToInputStream.h"
#include "MemoryStream.h"

#include "TextToByteReader.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;

using std::byte;

using Characters::Character;
using Characters::String;
using Debug::AssertExternallySynchronizedMutex;
using Memory::StackBuffer;

namespace {
    class Rep_ final : public InputStream::IRep<byte>, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    public:
        Rep_ (const InputStream::Ptr<Character>& src)
            : fSrc_{src}
        {
        }

    protected:
        virtual bool IsSeekable () const override
        {
            return false; // avoid seekability at first cuz makes impl much more costly/complex
        }
        virtual void CloseRead () override
        {
            Require (IsOpenRead ());
            fSrc_.Close ();
            Assert (fSrc_ == nullptr);
        }
        virtual bool IsOpenRead () const override
        {
            return fSrc_ != nullptr;
        }

        virtual optional<span<byte>> Read (span<byte> intoBuffer, NoDataAvailableHandling blockFlag) override
        {
            Require (IsOpenRead ());
            Require (not intoBuffer.empty ());
            // first see if any partially translated bytes to return
            if (not fSrcBufferedSpan_.empty ()) [[unlikely]] {
                auto copiedIntoSpan =
                    Memory::CopySpanData (fSrcBufferedSpan_.subspan (0, min (fSrcBufferedSpan_.size (), intoBuffer.size ())), intoBuffer);
                Assert (copiedIntoSpan.size () >= 1);
                fSrcBufferedSpan_ = fSrcBufferedSpan_.subspan (copiedIntoSpan.size ()); // skip copied bytes
                _fOffset += copiedIntoSpan.size ();
                return intoBuffer.subspan (0, copiedIntoSpan.size ());
            }
            // more likely - KISS for now - read one character from upstream and return appropriate number of bytes
            Assert (fSrcBufferedSpan_.empty ());
            Character readBuf[1];
            if (size_t nChars = fSrc_.Read (span{readBuf}, blockFlag).size ()) {
                char8_t       buf[10];
                span<char8_t> convertedSpan  = Characters::UTFConvert::kThe.ConvertSpan (span{readBuf, nChars}, span{buf, sizeof (buf)});
                auto          copiedIntoSpan = Memory::CopySpanData_StaticCast (convertedSpan, intoBuffer);
                if (copiedIntoSpan.size () < convertedSpan.size ()) {
                    // save extra bytes in fSrcBufferedSpan_
                    fSrcBufferedSpan_ =
                        Memory::CopySpanData_StaticCast (convertedSpan.subspan (copiedIntoSpan.size ()), span<byte>{fSrcBufferedRawBytes_});
                    Assert (1 <= fSrcBufferedSpan_.size () and fSrcBufferedSpan_.size () <= sizeof (fSrcBufferedRawBytes_));
                }
                _fOffset += copiedIntoSpan.size ();
                return intoBuffer.subspan (0, copiedIntoSpan.size ());
            }
            // if we got here, nothing in our buf, and nothing upstream - EOF
            return span<byte>{};
        }

        virtual optional<size_t> ReadNonBlocking (byte* intoStart, byte* intoEnd) override
        {
            Require ((intoStart == intoEnd) or (intoStart != nullptr));
            Require ((intoStart == intoEnd) or (intoEnd != nullptr));
            Require (IsOpenRead ());
            return _ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (intoStart, intoEnd, fSrcBufferedSpan_.size ());
        }

        virtual SeekOffsetType GetReadOffset () const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
            Require (IsOpenRead ());
            return _fOffset;
        }

        virtual SeekOffsetType SeekRead (Whence /*whence*/, SignedSeekOffsetType /*offset*/) override
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            Require (IsOpenRead ());
            AssertNotReached (); // not seekable
            return _fOffset;
        }

    protected:
        InputStream::Ptr<Character> fSrc_;
        byte                        fSrcBufferedRawBytes_[4]; // not used directly, but always through fSrcBufferedSpan_
        span<byte>                  fSrcBufferedSpan_;
        SeekOffsetType              _fOffset{0};
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
    };
}

/*
 ********************************************************************************
 *********************** Streams::TextToByteReader::New *************************
 ********************************************************************************
 */
auto TextToByteReader::New (const InputStream::Ptr<Character>& srcStream) -> InputStream::Ptr<byte>
{
    return InputStream::Ptr<byte>{make_shared<Rep_> (srcStream)};
}

auto TextToByteReader::New (const Traversal::Iterable<Character>& srcText) -> InputStream::Ptr<byte>
{
    // @todo - Could make this more efficient (by combining into one object) - but for now KISS
    return New (IterableToInputStream::New<Character> (srcText));
}