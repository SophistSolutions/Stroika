/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/UTFConvert.h"
#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "../Execution/Common.h"
#include "../Execution/OperationNotSupportedException.h"
#include "../Memory/InlineBuffer.h"
#include "../Memory/StackBuffer.h"
#include "MemoryStream.h"

#include "TextToByteReader.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;

using Characters::String;
using Debug::AssertExternallySynchronizedMutex;
using Memory::InlineBuffer;
using Memory::StackBuffer;

class TextToByteReader::Rep_ : public InputStream<byte>::_IRep {
public:
    Rep_ (const InputStream<Character>::Ptr& src)
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

    virtual size_t Read (byte* intoStart, byte* intoEnd) override
    {
        Require ((intoStart == intoEnd) or (intoStart != nullptr));
        Require ((intoStart == intoEnd) or (intoEnd != nullptr));
        Require (IsOpenRead ());
        span<byte> intoSpan{intoStart, intoEnd}; // soon change Stream API to pass in span instead
        // first see if any partially translated bytes to return
        if (not fSrcBufferedSpan_.empty ()) [[unlikely]] {
            auto copiedIntoSpan = Memory::CopySpanData (fSrcBufferedSpan_.subspan (0, min (fSrcBufferedSpan_.size (), intoSpan.size ())), intoSpan);
            Assert (copiedIntoSpan.size () >= 1);
            fSrcBufferedSpan_ = fSrcBufferedSpan_.subspan (copiedIntoSpan.size ()); // skip copied bytes
            _fOffset += copiedIntoSpan.size ();
            return copiedIntoSpan.size ();
        }
        // more likely - KISS for now - read one character from upstream and return appropriate number of bytes
        Assert (fSrcBufferedSpan_.empty ());
        Character readBuf[1];
        if (size_t nChars = fSrc_.Read (begin (readBuf), end (readBuf))) {
            char8_t       buf[10];
            span<char8_t> convertedSpan  = Characters::UTFConvert::kThe.ConvertSpan (span{readBuf, nChars}, span{buf, sizeof (buf)});
            auto          copiedIntoSpan = Memory::CopySpanData_StaticCast (convertedSpan, intoSpan);
            if (copiedIntoSpan.size () < convertedSpan.size ()) {
                // save extra bytes in fSrcBufferedSpan_
                fSrcBufferedSpan_ =
                    Memory::CopySpanData_StaticCast (convertedSpan.subspan (copiedIntoSpan.size ()), span<byte>{fSrcBufferedRawBytes_});
                Assert (1 <= fSrcBufferedSpan_.size () and fSrcBufferedSpan_.size () <= sizeof (fSrcBufferedRawBytes_));
            }
            _fOffset += copiedIntoSpan.size ();
            return copiedIntoSpan.size ();
        }
        // if we got here, nothing in our buf, and nothing upstream - EOF
        return 0;
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
    InputStream<Character>::Ptr fSrc_;
    byte                        fSrcBufferedRawBytes_[4]; // not used directly, but always through fSrcBufferedSpan_
    span<byte>                  fSrcBufferedSpan_;
    SeekOffsetType              _fOffset{0};
    [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
};

auto TextToByteReader::New (InputStream<Character>::Ptr srcStream) -> Ptr
{
    return _mkPtr (make_shared<Rep_> (srcStream));
}