/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/CodePage.h"
#include "../Characters/String_Constant.h"
#include "../Containers/Common.h"
#include "../Debug/AssertExternallySynchronizedLock.h"
#include "../Execution/Common.h"
#include "../Execution/OperationNotSupportedException.h"
#include "../Memory/SmallStackBuffer.h"

#include "TextReader.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;

using Characters::String;
using Characters::String_Constant;
using Execution::make_unique_lock;
using Memory::Byte;

namespace {
    using MyWCharTConverterType_ = codecvt<wchar_t, char, mbstate_t>;
}

class TextReader::FromBinaryStreamBaseRep_ : public InputStream<Character>::_IRep, protected Debug::AssertExternallySynchronizedLock {
public:
    FromBinaryStreamBaseRep_ (const InputStream<Byte>& src, const MyWCharTConverterType_& charConverter)
        : fSource_ (src)
        , fCharConverter_ (charConverter)
        , fOffset_ (0)
    {
    }

protected:
    virtual bool IsSeekable () const override
    {
        return false; // but many subclasses implement seekability
    }

// at least on windows, fCharCoverter with utf8 converter appeared to not mutate the mbState. Just reconverting
// whole thing worked, so try that for now... Slow/inefficeint, but at least it works
// -- LGP 2017-06-10
#ifndef qMaintainingMBShiftStateNotWorking_
#define qMaintainingMBShiftStateNotWorking_ 1
#endif
    virtual size_t Read (Character* intoStart, Character* intoEnd) override
    {
        Require ((intoStart == intoEnd) or (intoStart != nullptr));
        Require ((intoStart == intoEnd) or (intoEnd != nullptr));

        /*
         *  Try to minimize # of calls to underlying fSource binary stream per call this this Read () - efficiency.
         *
         *  Only need to read one character, and once we have that much, don't block on more from upstream binary stream.
         *
         *  But always just return at least 1 char if we can, so if partial, and no full chars read, keep reading.
         *
         *  Since number of wchar_ts filled always <= number of bytes read, we can read up to that # of bytes from upstream binary stream.
         */
        Memory::SmallStackBuffer<wchar_t>                  outBuf{size_t (intoEnd - intoStart)};
        wchar_t*                                           outCursor = begin (outBuf);
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        {
            Memory::SmallStackBuffer<Byte> inBuf{size_t (intoEnd - intoStart)}; // wag at size
            size_t                         inBytes = fSource_.Read (begin (inBuf), end (inBuf));
        again:
            const char* firstB = reinterpret_cast<const char*> (begin (inBuf));
            const char* endB   = firstB + inBytes;
            Assert (endB <= reinterpret_cast<const char*> (end (inBuf)));
            const char* cursorB = firstB;
#if qMaintainingMBShiftStateNotWorking_
            mbstate_t                     mbState_ = mbstate_t{};
            codecvt_utf8<wchar_t>::result r        = fCharConverter_.in (mbState_, firstB, endB, cursorB, std::begin (outBuf), std::end (outBuf), outCursor);
#else
            codecvt_utf8<wchar_t>::result r              = fCharConverter_.in (fMBState_, firstB, endB, cursorB, std::begin (outBuf), std::end (outBuf), outCursor);
#endif
            Assert (std::begin (outBuf) <= outCursor and outCursor <= std::end (outBuf));
            switch (r) {
                case codecvt_utf8<wchar_t>::partial: {
// see if we can read more from binary source
#if qMaintainingMBShiftStateNotWorking_
                    size_t prevInBufSize = inBuf.size ();
                    inBuf.GrowToSize (prevInBufSize + 1);
                    size_t thisReadNBytes = fSource_.Read (begin (inBuf) + prevInBufSize, end (inBuf));
                    if (thisReadNBytes != 0) {
                        outCursor = begin (outBuf);
                        inBytes += thisReadNBytes;
                        goto again;
                    }
#else
                    size_t                thisReadNBytes = fSource_.Read (begin (inBuf), begin (inBuf) + 1);
                    if (thisReadNBytes != 0) {
                        inBytes = thisReadNBytes;
                        goto again;
                    }
#endif
                } break;
                case codecvt_utf8<wchar_t>::error: {
                    Execution::Throw (Execution::StringException (String_Constant (L"Error converting characters codepage")));
                }
            }
            Assert (end (outBuf) - outCursor <= (intoEnd - intoStart)); // no overflow of output buffer - must be able to store all the results into arguments
        }
        Character* resultCharP = intoStart;
        for (const wchar_t* p = std::begin (outBuf); p < outCursor; ++p) {
            Assert (resultCharP < intoEnd);
            *resultCharP = *p;
            ++resultCharP;
        }
        size_t n = resultCharP - intoStart;
        fOffset_ += n;
        Ensure (n != 0 or fSource_.Read ().IsMissing ()); // if we read no characters, upstream binary source must be at EOF
        return n;
    }

    virtual Memory::Optional<size_t> ReadSome (Character* intoStart, Character* intoEnd) override
    {
        // https://stroika.atlassian.net/browse/STK-567 EXPERIMENTAL DRAFT API
        Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
        WeakAssert (false);
        // @todo - FIX TO REALLY CHECK
        return {};
    }

    virtual SeekOffsetType GetReadOffset () const override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        return fOffset_;
    }

    virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        AssertNotReached ();
        return fOffset_;
    }

protected:
    InputStream<Byte>             fSource_;
    const MyWCharTConverterType_& fCharConverter_;
#if !qMaintainingMBShiftStateNotWorking_
    mbstate_t fMBState_{};
#endif
    SeekOffsetType fOffset_;
};

class TextReader::UnseekableBinaryStreamRep_ : public FromBinaryStreamBaseRep_ {
    using inherited = FromBinaryStreamBaseRep_;

public:
    UnseekableBinaryStreamRep_ (const InputStream<Byte>& src, const MyWCharTConverterType_& charConverter)
        : inherited (src, charConverter)
    {
    }
};

class TextReader::CachingSeekableBinaryStreamRep_ : public FromBinaryStreamBaseRep_ {
    using inherited = FromBinaryStreamBaseRep_;

public:
    CachingSeekableBinaryStreamRep_ (const InputStream<Byte>& src, const MyWCharTConverterType_& charConverter)
        : FromBinaryStreamBaseRep_ (src, charConverter)
    {
    }

protected:
    virtual bool IsSeekable () const override
    {
        return true;
    }
    virtual size_t Read (Character* intoStart, Character* intoEnd) override
    {
        Require ((intoStart == intoEnd) or (intoStart != nullptr));
        Require ((intoStart == intoEnd) or (intoEnd != nullptr));

        // if already cached, return from cache. If not already cached, add to cache
        if (fOffset_ < fCache_.size ()) {
            // return data from cache
            size_t nToRead     = intoEnd - intoStart;
            size_t nInBufAvail = fCache_.size () - static_cast<size_t> (fOffset_);
            nToRead            = min (nToRead, nInBufAvail);
            Assert (nToRead > 0);
            for (size_t i = 0; i < nToRead; ++i) {
                intoStart[i] = fCache_[i + static_cast<size_t> (fOffset_)];
            }
            fOffset_ += nToRead;
            return nToRead;
        }
        // if not a cache hit, use inherited Read (), and fill the cache
        SeekOffsetType origOffset = fOffset_;
        size_t         n          = inherited::Read (intoStart, intoEnd);
        if (n != 0) {
            if (origOffset + n > numeric_limits<size_t>::max ()) {
                // size_t can be less bits than SeekOffsetType, in which case we cannot cahce all in RAM
                Execution::Throw (std::range_error ("seek past max size for size_t"));
            }
            size_t newCacheSize = static_cast<size_t> (origOffset + n);
            Assert (fCache_.size () == static_cast<size_t> (origOffset));
            Assert (newCacheSize > fCache_.size ());
            Containers::ReserveSpeedTweekAddN (fCache_, n);
            fCache_.resize (newCacheSize);
            for (size_t i = 0; i < n; ++i) {
                fCache_[i + static_cast<size_t> (origOffset)] = intoStart[i].As<wchar_t> ();
            }
        }
        return n;
    }
    virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        switch (whence) {
            case Whence::eFromStart: {
                if (offset < 0) {
                    Execution::Throw (std::range_error ("seek"));
                }
                SeekTo_ (static_cast<SeekOffsetType> (offset));
            } break;
            case Whence::eFromCurrent: {
                Streams::SeekOffsetType       curOffset = fOffset_;
                Streams::SignedSeekOffsetType newOffset = curOffset + offset;
                if (newOffset < 0) {
                    Execution::Throw (std::range_error ("seek"));
                }
                SeekOffsetType uNewOffset = static_cast<SeekOffsetType> (newOffset);
                SeekTo_ (static_cast<size_t> (uNewOffset));
            } break;
            case Whence::eFromEnd: {
                Character c;
                while (Read (&c, &c + 1) == 1) {
                    break; // read til EOF
                }
                SeekTo_ (fOffset_ + offset);
            } break;
        }
        return fOffset_;
    }

private:
    void SeekFowardTo_ (SeekOffsetType offset)
    {
        // easy - keep reading
        while (fOffset_ < offset) {
            Character c;
            if (Read (&c, &c + 1) == 0) {
                Execution::Throw (std::range_error ("seek"));
            }
        }
        Ensure (fOffset_ == offset);
    }
    void SeekBackwardTo_ (SeekOffsetType offset)
    {
        fOffset_ = offset;
        // no need to adjust seekpos of base FromBinaryStreamBaseRep_ readpos, because that is always left at end of cache
    }
    void SeekTo_ (SeekOffsetType offset)
    {
        if (offset > fOffset_) {
            SeekFowardTo_ (offset);
        }
        else if (offset < fOffset_) {
            SeekBackwardTo_ (offset);
        }
        Ensure (fOffset_ == offset);
    }

private:
    vector<wchar_t> fCache_;
};

class TextReader::IterableAdapterStreamRep_ : public InputStream<Character>::_IRep, private Debug::AssertExternallySynchronizedLock {
public:
    IterableAdapterStreamRep_ (const Traversal::Iterable<Character>& src)
        : fSource_ (src)
        , fSrcIter_ (fSource_.begin ())
    {
    }

protected:
    virtual bool IsSeekable () const override
    {
        return true;
    }
    virtual size_t Read (Character* intoStart, Character* intoEnd) override
    {
        Require (intoEnd - intoStart >= 1);
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Character*                                         outI = intoStart;
        for (; fSrcIter_ != fSource_.end () and outI != intoEnd; ++fSrcIter_, outI++) {
            *outI = *fSrcIter_;
            fOffset_++;
        }
        return outI - intoStart;
    }
    virtual Memory::Optional<size_t> ReadSome (Character* intoStart, Character* intoEnd) override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        // https://stroika.atlassian.net/browse/STK-567 EXPERIMENTAL DRAFT API
        ///      @todo  NOTE - we may need CTOR flag saying that iterating is non-blocking on type passed in!!!!
        Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
        if (intoStart == nullptr) {
            // Don't read (so dont update fOffset_) - just see how much available
            Traversal::Iterator<Character> srcIt = fSrcIter_;
            size_t                         cnt{};
            for (; srcIt != fSource_.end (); ++srcIt, ++cnt)
                ;
            return srcIt;
        }
        else {
            return Read (intoStart, intoEnd); // safe because implemtation of Read () in this type of stream doesnt block
        }
    }
    virtual SeekOffsetType GetReadOffset () const override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        return fOffset_;
    }
    virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        size_t                                             sourceLen = fSource_.GetLength ();
        SeekOffsetType                                     newOffset{};
        switch (whence) {
            case Whence::eFromStart: {
                if (offset < 0) {
                    Execution::Throw (std::range_error ("seek"));
                    if (offset > sourceLen) {
                        Execution::Throw (std::range_error ("seek"));
                    }
                }
                newOffset = static_cast<SeekOffsetType> (offset);
            } break;
            case Whence::eFromCurrent: {
                Streams::SignedSeekOffsetType tmpOffset = fOffset_ + offset;
                if (tmpOffset < 0) {
                    Execution::Throw (std::range_error ("seek"));
                }
                if (tmpOffset > sourceLen) {
                    Execution::Throw (std::range_error ("seek"));
                }
                newOffset = static_cast<SeekOffsetType> (tmpOffset);
            } break;
            case Whence::eFromEnd: {
                Streams::SignedSeekOffsetType tmpOffset = fSource_.GetLength () + offset;
                if (tmpOffset < 0) {
                    Execution::Throw (std::range_error ("seek"));
                }
                if (tmpOffset > sourceLen) {
                    Execution::Throw (std::range_error ("seek"));
                }
                newOffset = static_cast<SeekOffsetType> (tmpOffset);
            } break;
        }

        if (newOffset < fOffset_) {
            fSrcIter_ = fSource_.begin ();
            fOffset_  = 0;
        }
        while (fOffset_ < newOffset) {
            if (fSrcIter_.Done ()) {
                AssertNotReached (); // because we checked within maxlen above
                //Execution::Throw (Execution::StringException (L"Seek past end of input")); // @todo clarify - docuemnt - not sure if/how to handle this
            }
            fSrcIter_++;
            fOffset_++;
        }
        return 0;
    }

private:
    Traversal::Iterable<Character> fSource_;
    Traversal::Iterator<Character> fSrcIter_;
    size_t                         fOffset_{};
};

/*
 ********************************************************************************
 ******************************* Streams::TextReader ****************************
 ********************************************************************************
 */
namespace {
    const codecvt_utf8<wchar_t> kUTF8Converter_; // safe to keep static because only read-only const methods used
}

namespace {
    const MyWCharTConverterType_& LookupCharsetConverter_ (const Memory::Optional<Characters::String>& charset)
    {
        if (charset.IsMissing ()) {
            return kUTF8Converter_; // not sure this is best? HTTP 1.1 spec says to default to ISO-8859-1
        }
        return Characters::LookupCodeConverter<wchar_t> (*charset);
    }
}

TextReader::TextReader (const InputStream<Byte>& src, bool seekable)
    : TextReader (src, kUTF8Converter_, seekable)
{
}

TextReader::TextReader (const InputStream<Byte>& src, const Memory::Optional<Characters::String>& charset, bool seekable)
    : TextReader (src, LookupCharsetConverter_ (charset), seekable)
{
}

TextReader::TextReader (const InputStream<Byte>& src, const codecvt<wchar_t, char, mbstate_t>& codeConverter, bool seekable)
    : InputStream<Character> (seekable ? static_cast<InputStream::_SharedIRep> (make_shared<CachingSeekableBinaryStreamRep_> (src, codeConverter)) : static_cast<InputStream::_SharedIRep> (make_shared<UnseekableBinaryStreamRep_> (src, codeConverter)))
{
    Ensure (this->IsSeekable () == seekable);
}

TextReader::TextReader (const Memory::BLOB& src)
    : TextReader (src.As<InputStream<Byte>> (), true)
{
}

TextReader::TextReader (const Traversal::Iterable<Character>& src)
    : InputStream<Character> (make_shared<IterableAdapterStreamRep_> (src))
{
}
