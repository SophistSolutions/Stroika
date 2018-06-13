/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
using Memory::Byte;
using Memory::Optional;

namespace {
    using MyWCharTConverterType_ = codecvt<wchar_t, char, mbstate_t>;
}

class TextReader::FromBinaryStreamBaseRep_ : public InputStream<Character>::_IRep, protected Debug::AssertExternallySynchronizedLock {
public:
    FromBinaryStreamBaseRep_ (const InputStream<Byte>::Ptr& src, const MyWCharTConverterType_& charConverter)
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
        Require (IsOpenRead ());

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
            codecvt_utf8<wchar_t>::result r = fCharConverter_.in (fMBState_, firstB, endB, cursorB, std::begin (outBuf), std::end (outBuf), outCursor);
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
                    size_t thisReadNBytes = fSource_.Read (begin (inBuf), begin (inBuf) + 1);
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
        Ensure (n != 0 or not fSource_.Read ().has_value ()); // if we read no characters, upstream binary source must be at EOF
        return n;
    }

    virtual Optional<size_t> ReadNonBlocking (Character* intoStart, Character* intoEnd) override
    {
        Require ((intoStart == intoEnd) or (intoStart != nullptr));
        Require ((intoStart == intoEnd) or (intoEnd != nullptr));
        Require (IsOpenRead ());
        // Plan:
        //      o   ReadNonBlocking upstream
        //      o   save existing decode state
        //      o   decode and see if at least one character
        //      o   fall through to _ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream
        Memory::SmallStackBuffer<Byte> inBuf{10}; // enuf to get at least one charcter decoded (wag at number - but enuf for BOM+one char)
        Optional<size_t>               inBytes = fSource_.ReadNonBlocking (begin (inBuf), end (inBuf));
        if (inBytes) {
            if (*inBytes == 0) {
                return 0; // EOF - other than zero read bytes COULD mean unknown if EOF or not
            }
            const char* firstB = reinterpret_cast<const char*> (begin (inBuf));
            const char* endB   = firstB + *inBytes;
            Assert (endB <= reinterpret_cast<const char*> (end (inBuf)));
            const char* cursorB = firstB;
#if qMaintainingMBShiftStateNotWorking_
            mbstate_t mbState_ = mbstate_t{};
#else
            mbstate_t mbState_ = fMBState_;
#endif
            wchar_t                                        outChar;
            wchar_t*                                       outCursor = &outChar;
            [[maybe_unused]] codecvt_utf8<wchar_t>::result r         = fCharConverter_.in (mbState_, firstB, endB, cursorB, &outChar, &outChar + 1, outCursor);
            // we could read one byte upstream, but not ENOUGH to get a full character output!
            if (outCursor != &outChar) {
                return _ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (intoStart, intoEnd, 1);
            }
        }
        return {};
    }

    virtual SeekOffsetType GetReadOffset () const override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpenRead ());
        return fOffset_;
    }

    virtual SeekOffsetType SeekRead (Whence /*whence*/, SignedSeekOffsetType /*offset*/) override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpenRead ());
        AssertNotReached (); // not seekable
        return fOffset_;
    }

protected:
    InputStream<Byte>::Ptr        fSource_;
    const MyWCharTConverterType_& fCharConverter_;
#if !qMaintainingMBShiftStateNotWorking_
    mbstate_t fMBState_{};
#endif
    SeekOffsetType fOffset_;
};

class TextReader::UnseekableBinaryStreamRep_ : public FromBinaryStreamBaseRep_ {
    using inherited = FromBinaryStreamBaseRep_;

public:
    UnseekableBinaryStreamRep_ (const InputStream<Byte>::Ptr& src, const MyWCharTConverterType_& charConverter)
        : inherited (src, charConverter)
    {
    }
};

class TextReader::CachingSeekableBinaryStreamRep_ : public FromBinaryStreamBaseRep_ {
    using inherited = FromBinaryStreamBaseRep_;

public:
    CachingSeekableBinaryStreamRep_ (const InputStream<Byte>::Ptr& src, const MyWCharTConverterType_& charConverter)
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
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpenRead ());

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
        Require (IsOpenRead ());
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

private:
    bool fIsOpen_{true};

protected:
    virtual bool IsSeekable () const override
    {
        return true;
    }
    virtual void CloseRead () override
    {
        Require (IsOpenRead ());
        fIsOpen_ = false;
    }
    virtual bool IsOpenRead () const override
    {
        return fIsOpen_;
    }
    virtual size_t Read (Character* intoStart, Character* intoEnd) override
    {
        Require (intoEnd - intoStart >= 1);
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpenRead ());
        Character* outI = intoStart;
        if (fPutBack_) {
            *outI = *fPutBack_;
            fPutBack_.clear ();
            outI++;
            // fOffset_ doesn't take into account putback
        }
        for (; fSrcIter_ != fSource_.end () and outI != intoEnd; ++fSrcIter_, outI++) {
            *outI = *fSrcIter_;
            fOffset_++;
        }
        if (outI > intoStart) {
            fPrevCharCached_ = *(outI - 1);
        }
        else {
            fPrevCharCached_.clear ();
        }
        return outI - intoStart;
    }
    virtual Optional<size_t> ReadNonBlocking (Character* intoStart, Character* intoEnd) override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
        Require (IsOpenRead ());
        if (intoStart == nullptr) {
            // Don't read (so dont update fOffset_) - just see how much available
            Traversal::Iterator<Character> srcIt = fSrcIter_;
            size_t                         cnt{};
            if (fPutBack_) {
                ++cnt;
            }
            for (; srcIt != fSource_.end (); ++srcIt, ++cnt)
                ;
            return cnt;
        }
        else {
            return Read (intoStart, intoEnd); // safe because implemtation of Read () in this type of stream doesn't block
        }
    }
    virtual SeekOffsetType GetReadOffset () const override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpenRead ());
        if (fPutBack_) {
            Assert (fOffset_ >= 1);
            return fOffset_ - 1;
        }
        return fOffset_;
    }
    virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        Require (IsOpenRead ());
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        size_t                                             sourceLen = fSource_.GetLength ();
        SeekOffsetType                                     newOffset{};
        switch (whence) {
            case Whence::eFromStart: {
                if (offset < 0) {
                    Execution::Throw (std::range_error ("seek"));
                }
                if (static_cast<SeekOffsetType> (offset) > sourceLen) {
                    Execution::Throw (std::range_error ("seek"));
                }
                newOffset = static_cast<SeekOffsetType> (offset);
            } break;
            case Whence::eFromCurrent: {
                Streams::SignedSeekOffsetType tmpOffset = fOffset_ + offset;
                if (tmpOffset < 0) {
                    Execution::Throw (std::range_error ("seek"));
                }
                if (static_cast<SeekOffsetType> (tmpOffset) > sourceLen) {
                    Execution::Throw (std::range_error ("seek"));
                }
                newOffset = static_cast<SeekOffsetType> (tmpOffset);
            } break;
            case Whence::eFromEnd: {
                Streams::SignedSeekOffsetType tmpOffset = fSource_.GetLength () + offset;
                if (tmpOffset < 0) {
                    Execution::Throw (std::range_error ("seek"));
                }
                if (static_cast<SeekOffsetType> (tmpOffset) > sourceLen) {
                    Execution::Throw (std::range_error ("seek"));
                }
                newOffset = static_cast<SeekOffsetType> (tmpOffset);
            } break;
        }

        if (newOffset == fOffset_ - 1 and fPrevCharCached_) {
            fPutBack_ = fPrevCharCached_;
            fPrevCharCached_.clear ();
            return GetReadOffset ();
        }
        else if (newOffset < fOffset_) {
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
        return fOffset_;
    }

private:
    Traversal::Iterable<Character> fSource_;
    Traversal::Iterator<Character> fSrcIter_;
    size_t                         fOffset_{};
    Optional<Character>            fPrevCharCached_{}; // fPrevCharCached_/fPutBack_ speed hack to support IsAtEOF (), and Peek () more efficiently, little cost, big cost avoidance for seek
    Optional<Character>            fPutBack_{};
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
    const MyWCharTConverterType_& LookupCharsetConverter_ (const Optional<Characters::String>& charset)
    {
        if (not charset.has_value ()) {
            return kUTF8Converter_; // not sure this is best? HTTP 1.1 spec says to default to ISO-8859-1
        }
        return Characters::LookupCodeConverter<wchar_t> (*charset);
    }
}

auto TextReader::New (const Memory::BLOB& src, const Optional<Characters::String>& charset) -> Ptr
{
    Ptr p = TextReader::New (src.As<InputStream<Byte>::Ptr> (), charset, true);
    Ensure (p.IsSeekable ());
    return p;
}

auto TextReader::New (const InputStream<Byte>::Ptr& src, bool seekable) -> Ptr
{
    Ptr p = TextReader::New (src, kUTF8Converter_, seekable);
    Ensure (p.IsSeekable () == seekable);
    return p;
}

auto TextReader::New (const InputStream<Byte>::Ptr& src, const Optional<Characters::String>& charset, bool seekable) -> Ptr
{
    Ptr p = TextReader::New (src, LookupCharsetConverter_ (charset), seekable);
    Ensure (p.IsSeekable () == seekable);
    return p;
}

auto TextReader::New (const InputStream<Byte>::Ptr& src, const codecvt<wchar_t, char, mbstate_t>& codeConverter, bool seekable) -> Ptr
{
    Ptr p = seekable ? Ptr{make_shared<CachingSeekableBinaryStreamRep_> (src, codeConverter)} : Ptr{make_shared<UnseekableBinaryStreamRep_> (src, codeConverter)};
    Ensure (p.IsSeekable () == seekable);
    return p;
}

auto TextReader::New (const Traversal::Iterable<Character>& src) -> Ptr
{
    Ptr p{make_shared<IterableAdapterStreamRep_> (src)};
    Ensure (p.IsSeekable ());
    return p;
}

auto TextReader::New (Execution::InternallySyncrhonized internallySyncrhonized, const InputStream<Character>::Ptr& src) -> Ptr
{
    switch (internallySyncrhonized) {
        case Execution::eInternallySynchronized:
            AssertNotImplemented ();
            //return InternalSyncRep_::New ();
            return src;
        case Execution::eNotKnownInternallySynchronized:
            return src;
        default:
            RequireNotReached ();
            return src;
    }
}

auto TextReader::New (Execution::InternallySyncrhonized internallySyncrhonized, const Memory::BLOB& src, const Optional<Characters::String>& charset) -> Ptr
{
    switch (internallySyncrhonized) {
        case Execution::eInternallySynchronized:
            AssertNotImplemented ();
            //return InternalSyncRep_::New ();
            return New (src, charset);
        case Execution::eNotKnownInternallySynchronized:
            return New (src, charset);
        default:
            RequireNotReached ();
            return New (src, charset);
    }
}

auto TextReader::New (Execution::InternallySyncrhonized internallySyncrhonized, const InputStream<Byte>::Ptr& src, bool seekable) -> Ptr
{
    switch (internallySyncrhonized) {
        case Execution::eInternallySynchronized:
            AssertNotImplemented ();
            //return InternalSyncRep_::New ();
            return New (src, seekable);
        case Execution::eNotKnownInternallySynchronized:
            return New (src, seekable);
        default:
            RequireNotReached ();
            return New (src, seekable);
    }
}

auto TextReader::New (Execution::InternallySyncrhonized internallySyncrhonized, const InputStream<Byte>::Ptr& src, const Optional<Characters::String>& charset, bool seekable) -> Ptr
{
    switch (internallySyncrhonized) {
        case Execution::eInternallySynchronized:
            AssertNotImplemented ();
            //return InternalSyncRep_::New ();
            return New (src, charset, seekable);
        case Execution::eNotKnownInternallySynchronized:
            return New (src, charset, seekable);
        default:
            RequireNotReached ();
            return New (src, charset, seekable);
    }
}

auto TextReader::New (Execution::InternallySyncrhonized internallySyncrhonized, const InputStream<Byte>::Ptr& src, const codecvt<wchar_t, char, mbstate_t>& codeConverter, bool seekable) -> Ptr
{
    switch (internallySyncrhonized) {
        case Execution::eInternallySynchronized:
            AssertNotImplemented ();
            //return InternalSyncRep_::New ();
            return New (src, codeConverter, seekable);
        case Execution::eNotKnownInternallySynchronized:
            return New (src, codeConverter, seekable);
        default:
            RequireNotReached ();
            return New (src, codeConverter, seekable);
    }
}

auto TextReader::New (Execution::InternallySyncrhonized internallySyncrhonized, const Traversal::Iterable<Character>& src) -> Ptr
{
    switch (internallySyncrhonized) {
        case Execution::eInternallySynchronized:
            AssertNotImplemented ();
            //return InternalSyncRep_::New ();
            return New (src);
        case Execution::eNotKnownInternallySynchronized:
            return New (src);
        default:
            RequireNotReached ();
            return New (src);
    }
}