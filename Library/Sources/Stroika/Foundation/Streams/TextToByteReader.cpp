/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
private:
    InputStream<Character>::Ptr fSrc_;
    byte                        fSrcBufferedBytes_[4];
    size_t                      fCountAvailableBufferedBytes_{0};

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
    virtual bool IsOpenRead () const override { return fSrc_ != nullptr; }

    virtual size_t Read (byte* intoStart, byte* intoEnd) override
    {
        Require ((intoStart == intoEnd) or (intoStart != nullptr));
        Require ((intoStart == intoEnd) or (intoEnd != nullptr));
        Require (IsOpenRead ());

        if (fCountAvailableBufferedBytes_ != 0) [[unlikely]] {
            size_t nToReadHere = min (fCountAvailableBufferedBytes_, static_cast<size_t> (intoEnd - intoStart));
            Assert (nToReadHere >= 1);
            copy (begin (fSrcBufferedBytes_), begin (fSrcBufferedBytes_) + nToReadHere, intoStart);
            fCountAvailableBufferedBytes_ -= nToReadHere;
            if (fCountAvailableBufferedBytes_ != 0) [[unlikely]] {
                // must slide chars down
                memmove (begin (fSrcBufferedBytes_), begin (fSrcBufferedBytes_) + nToReadHere, fCountAvailableBufferedBytes_);
            }
            _fOffset += nToReadHere;
            return nToReadHere;
        }

        // more likely - KISS for now - read one character from upstream and return appropriate number of bytes
        Assert (fCountAvailableBufferedBytes_ == 0);
        Character readBuf[1];
        if (size_t nChars = fSrc_.Read (begin (readBuf), end (readBuf))) {
            char8_t       buf[10];
            span<char8_t> cspan      = Characters::UTFConverter::kThe.ConvertSpan (span{readBuf, 1}, span{buf, sizeof (buf)});
            size_t        nToCopyNow = min (cspan.size (), static_cast<size_t> (intoEnd - intoStart));
            //copy (buf, buf + nToCopyNow, intoStart);
            std::memcpy (intoStart, buf, nToCopyNow); // copy out result to caller
            if (nToCopyNow < cspan.size ()) {         // if any left, copy into fSrcBufferedBytes_
                size_t cacheBytes = cspan.size () - nToCopyNow;
                copy (reinterpret_cast<const byte*> (cspan.data ()) + nToCopyNow,
                      reinterpret_cast<const byte*> (cspan.data ()) + cspan.size (), fSrcBufferedBytes_);
                fCountAvailableBufferedBytes_ = cacheBytes;
            }
            _fOffset += nToCopyNow;
            return nToCopyNow;
        }
        // if we got here, nothing in our buf, and nothing upstream
        return 0;
    }

    virtual optional<size_t> ReadNonBlocking (byte* intoStart, byte* intoEnd) override
    {
        Require ((intoStart == intoEnd) or (intoStart != nullptr));
        Require ((intoStart == intoEnd) or (intoEnd != nullptr));
        Require (IsOpenRead ());
        return _ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (intoStart, intoEnd, fCountAvailableBufferedBytes_);
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
    InputStream<Character>::Ptr                                    _fSource;
    mbstate_t                                                      fMBState_{};
    SeekOffsetType                                                 _fOffset{0};
    [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
};

auto TextToByteReader::New (InputStream<Character>::Ptr srcStream) -> Ptr
{
#if 1

    return _mkPtr (make_shared<Rep_> (srcStream));
#else
    // quick hack but in adequate (assuming we can read to EOF, which is not true) in general so this is bad
    auto s = srcStream.ReadAll ().AsUTF8<string> ();
    return Streams::MemoryStream<std::byte>::New (reinterpret_cast<const std::byte*> (s.data ()),
                                                  reinterpret_cast<const std::byte*> (s.data ()) + s.size ());
#endif
}