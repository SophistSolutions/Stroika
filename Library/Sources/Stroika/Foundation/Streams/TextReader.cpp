/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/String_Constant.h"
#include "../Containers/Common.h"
#include "../Debug/AssertExternallySynchronizedLock.h"
#include "../Execution/Common.h"
#include "../Execution/OperationNotSupportedException.h"
#include "../Memory/SmallStackBuffer.h"

#include "TextReader.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;

using Characters::String;
using Characters::String_Constant;
using Memory::SmallStackBuffer;
using Memory::SmallStackBufferCommon;

namespace {
    using MyWCharTConverterType_ = codecvt<wchar_t, char, mbstate_t>;
}

class TextReader::FromBinaryStreamBaseRep_ : public InputStream<Character>::_IRep, protected Debug::AssertExternallySynchronizedLock {
public:
    FromBinaryStreamBaseRep_ (const InputStream<byte>::Ptr& src, const MyWCharTConverterType_& charConverter)
        : _fSource (src)
        , _fCharConverter (charConverter)
        , _fOffset (0)
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
        _fSource.Close ();
        Assert (_fSource == nullptr);
    }
    virtual bool IsOpenRead () const override
    {
        return _fSource != nullptr;
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
        SmallStackBuffer<wchar_t, 8 * 1024>                outBuf{SmallStackBufferCommon::eUninitialized, size_t (intoEnd - intoStart)};
        wchar_t*                                           outCursor = begin (outBuf);
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        {
            SmallStackBuffer<byte, 8 * 1024> inBuf{SmallStackBufferCommon::eUninitialized, size_t (intoEnd - intoStart)}; // wag at size
            size_t                           inBytes = _fSource.Read (begin (inBuf), end (inBuf));
        again:
            const char* firstB = reinterpret_cast<const char*> (begin (inBuf));
            const char* endB   = firstB + inBytes;
            Assert (endB <= reinterpret_cast<const char*> (end (inBuf)));
            const char* cursorB = firstB;
#if qMaintainingMBShiftStateNotWorking_
            mbstate_t                     mbState = mbstate_t{};
            codecvt_utf8<wchar_t>::result r       = _fCharConverter.in (mbState, firstB, endB, cursorB, std::begin (outBuf), std::end (outBuf), outCursor);
#else
            codecvt_utf8<wchar_t>::result r = _fCharConverter.in (fMBState_, firstB, endB, cursorB, std::begin (outBuf), std::end (outBuf), outCursor);
#endif
            Assert (std::begin (outBuf) <= outCursor and outCursor <= std::end (outBuf));
            switch (r) {
                case codecvt_utf8<wchar_t>::partial: {
// see if we can read more from binary source
#if qMaintainingMBShiftStateNotWorking_
                    size_t prevInBufSize = inBuf.size ();
                    inBuf.GrowToSize_uninitialized (prevInBufSize + 1);
                    size_t thisReadNBytes = _fSource.Read (begin (inBuf) + prevInBufSize, end (inBuf));
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
        _fOffset += n;
        Ensure (n != 0 or not _fSource.Read ().has_value ()); // if we read no characters, upstream binary source must be at EOF
        return n;
    }

    virtual optional<size_t> ReadNonBlocking (Character* intoStart, Character* intoEnd) override
    {
        Require ((intoStart == intoEnd) or (intoStart != nullptr));
        Require ((intoStart == intoEnd) or (intoEnd != nullptr));
        Require (IsOpenRead ());
        // Plan:
        //      o   ReadNonBlocking upstream
        //      o   save existing decode state
        //      o   decode and see if at least one character
        //      o   fall through to _ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream
        SmallStackBuffer<byte> inBuf{SmallStackBufferCommon::eUninitialized, 10}; // enuf to get at least one charcter decoded (wag at number - but enuf for BOM+one char)
        optional<size_t>       inBytes = _fSource.ReadNonBlocking (begin (inBuf), end (inBuf));
        if (inBytes) {
            if (*inBytes == 0) {
                return 0; // EOF - other than zero read bytes COULD mean unknown if EOF or not
            }
            const char* firstB = reinterpret_cast<const char*> (begin (inBuf));
            const char* endB   = firstB + *inBytes;
            Assert (endB <= reinterpret_cast<const char*> (end (inBuf)));
            const char* cursorB = firstB;
#if qMaintainingMBShiftStateNotWorking_
            mbstate_t mbState = mbstate_t{};
#else
            mbstate_t mbState = fMBState_;
#endif
            wchar_t                                        outChar;
            wchar_t*                                       outCursor = &outChar;
            [[maybe_unused]] codecvt_utf8<wchar_t>::result r         = _fCharConverter.in (mbState, firstB, endB, cursorB, &outChar, &outChar + 1, outCursor);
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
        return _fOffset;
    }

    virtual SeekOffsetType SeekRead (Whence /*whence*/, SignedSeekOffsetType /*offset*/) override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpenRead ());
        AssertNotReached (); // not seekable
        return _fOffset;
    }

protected:
    InputStream<byte>::Ptr        _fSource;
    const MyWCharTConverterType_& _fCharConverter;
#if !qMaintainingMBShiftStateNotWorking_
    mbstate_t fMBState_{};
#endif
    SeekOffsetType _fOffset;
};

class TextReader::UnseekableBinaryStreamRep_ : public FromBinaryStreamBaseRep_ {
    using inherited = FromBinaryStreamBaseRep_;

public:
    UnseekableBinaryStreamRep_ (const InputStream<byte>::Ptr& src, const MyWCharTConverterType_& charConverter)
        : inherited (src, charConverter)
    {
    }
};

class TextReader::CachingSeekableBinaryStreamRep_ : public FromBinaryStreamBaseRep_ {
    using inherited = FromBinaryStreamBaseRep_;

public:
    CachingSeekableBinaryStreamRep_ (const InputStream<byte>::Ptr& src, const MyWCharTConverterType_& charConverter, ReadAhead readAhead)
        : FromBinaryStreamBaseRep_ (src, charConverter)
        , fReadAheadAllowed_ (readAhead == ReadAhead::eReadAheadAllowed)
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

        // if already cached, return from cache. Note - even if only one element is in the Cache, thats enough to return
        // and not say 'eof'
        if (_fOffset < fCache_.size ()) {
            // return data from cache
            size_t nToRead     = intoEnd - intoStart;
            size_t nInBufAvail = fCache_.size () - static_cast<size_t> (_fOffset);
            nToRead            = min (nToRead, nInBufAvail);
            Assert (nToRead > 0);
            for (size_t i = 0; i < nToRead; ++i) {
                intoStart[i] = fCache_[i + static_cast<size_t> (_fOffset)];
            }
            _fOffset += nToRead;
            return nToRead;
        }

        // if not already cached, add to cache, and then return the data
        SeekOffsetType origOffset       = _fOffset;
        auto           pushIntoCacheBuf = [origOffset, this](Character* bufStart, Character* bufEnd) {
            size_t n            = bufEnd - bufStart;
            size_t newCacheSize = static_cast<size_t> (origOffset + n);
            Assert (fCache_.size () == static_cast<size_t> (origOffset));
            Assert (newCacheSize > fCache_.size ());
            Containers::ReserveSpeedTweekAddN (fCache_, n);
            fCache_.resize_uninitialized (newCacheSize);
            for (size_t i = 0; i < n; ++i) {
                fCache_[i + static_cast<size_t> (origOffset)] = bufStart[i].As<wchar_t> ();
            }
        };
        // if not a cache hit, use inherited Read (), and fill the cache.
        // If the calling read big enough, re-use that buffer.
        constexpr size_t kMinCachedReadSize_{512};
        if (intoEnd - intoStart >= kMinCachedReadSize_ or not fReadAheadAllowed_) {
            size_t n = inherited::Read (intoStart, intoEnd);
            if (n != 0) {
                if (origOffset + n > numeric_limits<size_t>::max ())
                    [[UNLIKELY_ATTR]]
                    {
                        // size_t can be less bits than SeekOffsetType, in which case we cannot cahce all in RAM
                        Execution::Throw (range_error ("seek past max size for size_t"));
                    }
                pushIntoCacheBuf (intoStart, intoStart + n);
            }
            return n;
        }
        else {
            // if argument buffer not big enough, read into a temporary buffer
            constexpr size_t kUseCacheSize_ = 8 * kMinCachedReadSize_;
            static_assert (sizeof (wchar_t) == sizeof (Character));
            wchar_t buf[kUseCacheSize_]; // use wchar_t and cast to Character* so we get this array uninitialized
            size_t  n = inherited::Read (reinterpret_cast<Character*> (std::begin (buf)), reinterpret_cast<Character*> (std::end (buf)));
            if (n != 0) {
                if (origOffset + n > numeric_limits<size_t>::max ())
                    [[UNLIKELY_ATTR]]
                    {
                        // size_t can be less bits than SeekOffsetType, in which case we cannot cahce all in RAM
                        Execution::Throw (range_error ("seek past max size for size_t"));
                    }
                pushIntoCacheBuf (reinterpret_cast<Character*> (std::begin (buf)), reinterpret_cast<Character*> (std::begin (buf)) + n);
                n = intoEnd - intoStart;
                (void)::memcpy (intoStart, std::begin (buf), n * sizeof (wchar_t));
                _fOffset = origOffset + n;
            }
            return n;
        }
    }
    virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (IsOpenRead ());
        switch (whence) {
            case Whence::eFromStart: {
                if (offset < 0)
                    [[UNLIKELY_ATTR]]
                    {
                        Execution::Throw (range_error ("seek"));
                    }
                SeekTo_ (static_cast<SeekOffsetType> (offset));
            } break;
            case Whence::eFromCurrent: {
                Streams::SeekOffsetType       curOffset = _fOffset;
                Streams::SignedSeekOffsetType newOffset = curOffset + offset;
                if (newOffset < 0)
                    [[UNLIKELY_ATTR]]
                    {
                        Execution::Throw (range_error ("seek"));
                    }
                SeekOffsetType uNewOffset = static_cast<SeekOffsetType> (newOffset);
                SeekTo_ (static_cast<size_t> (uNewOffset));
            } break;
            case Whence::eFromEnd: {
                Character c;
                while (Read (&c, &c + 1) == 1) {
                    break; // read til EOF
                }
                SeekTo_ (_fOffset + offset);
            } break;
        }
        return _fOffset;
    }

private:
    void SeekFowardTo_ (SeekOffsetType offset)
    {
        // easy - keep reading
        while (_fOffset < offset) {
            Character c;
            if (Read (&c, &c + 1) == 0)
                [[UNLIKELY_ATTR]]
                {
                    Execution::Throw (range_error ("seek"));
                }
        }
        Ensure (_fOffset == offset);
    }
    void SeekBackwardTo_ (SeekOffsetType offset)
    {
        _fOffset = offset;
        // no need to adjust seekpos of base FromBinaryStreamBaseRep_ readpos, because that is always left at end of cache
    }
    void SeekTo_ (SeekOffsetType offset)
    {
        if (offset > _fOffset) {
            SeekFowardTo_ (offset);
        }
        else if (offset < _fOffset) {
            SeekBackwardTo_ (offset);
        }
        Ensure (_fOffset == offset);
    }

private:
    bool                      fReadAheadAllowed_{false};
    SmallStackBuffer<wchar_t> fCache_; // Cache uses wchar_t instead of Character so can use resize_uninitialized () - requires is_trivially_constructible
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
            *outI     = *fPutBack_;
            fPutBack_ = nullopt;
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
            fPrevCharCached_ = nullopt;
        }
        return outI - intoStart;
    }
    virtual optional<size_t> ReadNonBlocking (Character* intoStart, Character* intoEnd) override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
        Require (IsOpenRead ());
        if (intoStart == nullptr) {
            // Don't read (so don't update fOffset_) - just see how much available
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
                if (offset < 0)
                    [[UNLIKELY_ATTR]]
                    {
                        Execution::Throw (range_error ("seek"));
                    }
                if (static_cast<SeekOffsetType> (offset) > sourceLen)
                    [[UNLIKELY_ATTR]]
                    {
                        Execution::Throw (range_error ("seek"));
                    }
                newOffset = static_cast<SeekOffsetType> (offset);
            } break;
            case Whence::eFromCurrent: {
                Streams::SignedSeekOffsetType tmpOffset = fOffset_ + offset;
                if (tmpOffset < 0)
                    [[UNLIKELY_ATTR]]
                    {
                        Execution::Throw (range_error ("seek"));
                    }
                if (static_cast<SeekOffsetType> (tmpOffset) > sourceLen)
                    [[UNLIKELY_ATTR]]
                    {
                        Execution::Throw (range_error ("seek"));
                    }
                newOffset = static_cast<SeekOffsetType> (tmpOffset);
            } break;
            case Whence::eFromEnd: {
                Streams::SignedSeekOffsetType tmpOffset = fSource_.GetLength () + offset;
                if (tmpOffset < 0)
                    [[UNLIKELY_ATTR]]
                    {
                        Execution::Throw (range_error ("seek"));
                    }
                if (static_cast<SeekOffsetType> (tmpOffset) > sourceLen)
                    [[UNLIKELY_ATTR]]
                    {
                        Execution::Throw (range_error ("seek"));
                    }
                newOffset = static_cast<SeekOffsetType> (tmpOffset);
            } break;
        }

        if (newOffset == fOffset_ - 1 and fPrevCharCached_) {
            fPutBack_        = fPrevCharCached_;
            fPrevCharCached_ = nullopt;
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
    optional<Character>            fPrevCharCached_{}; // fPrevCharCached_/fPutBack_ speed hack to support IsAtEOF (), and Peek () more efficiently, little cost, big cost avoidance for seek
    optional<Character>            fPutBack_{};
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
    const MyWCharTConverterType_& LookupCharsetConverter_ (const optional<Characters::String>& charset)
    {
        if (not charset.has_value ()) {
            return kUTF8Converter_; // not sure this is best? HTTP 1.1 spec says to default to ISO-8859-1
        }
        return Characters::LookupCodeConverter<wchar_t> (*charset);
    }
}

auto TextReader::New (const Memory::BLOB& src, const optional<Characters::String>& charset) -> Ptr
{
    Ptr p = TextReader::New (src.As<InputStream<byte>::Ptr> (), charset, SeekableFlag::eSeekable);
    Ensure (p.IsSeekable ());
    return p;
}

auto TextReader::New (const InputStream<byte>::Ptr& src, SeekableFlag seekable, ReadAhead readAhead) -> Ptr
{
    Ptr p = TextReader::New (src, kUTF8Converter_, seekable, readAhead);
    Ensure (p.GetSeekability () == seekable);
    return p;
}

auto TextReader::New (const InputStream<byte>::Ptr& src, const optional<Characters::String>& charset, SeekableFlag seekable, ReadAhead readAhead) -> Ptr
{
    Ptr p = TextReader::New (src, LookupCharsetConverter_ (charset), seekable, readAhead);
    Ensure (p.GetSeekability () == seekable);
    return p;
}

auto TextReader::New (const InputStream<byte>::Ptr& src, const codecvt<wchar_t, char, mbstate_t>& codeConverter, SeekableFlag seekable, ReadAhead readAhead) -> Ptr
{
    Ptr p = (seekable == SeekableFlag::eSeekable) ? Ptr{make_shared<CachingSeekableBinaryStreamRep_> (src, codeConverter, readAhead)} : Ptr{make_shared<UnseekableBinaryStreamRep_> (src, codeConverter)};
    Ensure (p.GetSeekability () == seekable);
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

auto TextReader::New (Execution::InternallySyncrhonized internallySyncrhonized, const Memory::BLOB& src, const optional<Characters::String>& charset) -> Ptr
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

auto TextReader::New (Execution::InternallySyncrhonized internallySyncrhonized, const InputStream<byte>::Ptr& src, SeekableFlag seekable, ReadAhead readAhead) -> Ptr
{
    switch (internallySyncrhonized) {
        case Execution::eInternallySynchronized:
            AssertNotImplemented ();
            //return InternalSyncRep_::New ();
            return New (src, seekable);
        case Execution::eNotKnownInternallySynchronized:
            return New (src, seekable, readAhead);
        default:
            RequireNotReached ();
            return New (src, seekable, readAhead);
    }
}

auto TextReader::New (Execution::InternallySyncrhonized internallySyncrhonized, const InputStream<byte>::Ptr& src, const optional<Characters::String>& charset, SeekableFlag seekable, ReadAhead readAhead) -> Ptr
{
    switch (internallySyncrhonized) {
        case Execution::eInternallySynchronized:
            AssertNotImplemented ();
            //return InternalSyncRep_::New ();
            return New (src, charset, seekable, readAhead);
        case Execution::eNotKnownInternallySynchronized:
            return New (src, charset, seekable, readAhead);
        default:
            RequireNotReached ();
            return New (src, charset, seekable, readAhead);
    }
}

auto TextReader::New (Execution::InternallySyncrhonized internallySyncrhonized, const InputStream<byte>::Ptr& src, const codecvt<wchar_t, char, mbstate_t>& codeConverter, SeekableFlag seekable, ReadAhead readAhead) -> Ptr
{
    switch (internallySyncrhonized) {
        case Execution::eInternallySynchronized:
            AssertNotImplemented ();
            //return InternalSyncRep_::New ();
            return New (src, codeConverter, seekable, readAhead);
        case Execution::eNotKnownInternallySynchronized:
            return New (src, codeConverter, seekable, readAhead);
        default:
            RequireNotReached ();
            return New (src, codeConverter, seekable, readAhead);
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

[[deprecated ("use SeekableFlag overload since version 2.1d6")]] TextReader::Ptr TextReader::New (const InputStream<byte>::Ptr& src, bool seekable)
{
    return New (src, seekable ? SeekableFlag::eSeekable : SeekableFlag::eNotSeekable);
}

[[deprecated ("use SeekableFlag overload since version 2.1d6")]] TextReader::Ptr TextReader::New (const InputStream<byte>::Ptr& src, const optional<Characters::String>& charset, bool seekable)
{
    return New (src, charset, seekable ? SeekableFlag::eSeekable : SeekableFlag::eNotSeekable);
}

[[deprecated ("use SeekableFlag overload since version 2.1d6")]] TextReader::Ptr TextReader::New (const InputStream<byte>::Ptr& src, const codecvt<wchar_t, char, mbstate_t>& codeConverter, bool seekable)
{
    return New (src, codeConverter, seekable ? SeekableFlag::eSeekable : SeekableFlag::eNotSeekable);
}

[[deprecated ("use SeekableFlag overload since version 2.1d6")]] TextReader::Ptr TextReader::New (Execution::InternallySyncrhonized internallySyncrhonized, const InputStream<byte>::Ptr& src, bool seekable)
{
    return New (internallySyncrhonized, src, seekable ? SeekableFlag::eSeekable : SeekableFlag::eNotSeekable);
}
#if 0
// no idea why wont compile on msvc, but unimportant, since deprecated, and could easily be never used...
[[deprecated ("use SeekableFlag overload since version 2.1d6")]] TextReader::TextReader::Ptr New (Execution::InternallySyncrhonized internallySyncrhonized, const InputStream<byte>::Ptr& src, const optional<Characters::String>& charset, bool seekable)
{
    return New (internallySyncrhonized, src, charset, (seekable ? SeekableFlag::eSeekable : SeekableFlag::eNotSeekable));
}
#endif

[[deprecated ("use SeekableFlag overload since version 2.1d6")]] TextReader::Ptr TextReader::New (Execution::InternallySyncrhonized internallySyncrhonized, const InputStream<byte>::Ptr& src, const codecvt<wchar_t, char, mbstate_t>& codeConverter, bool seekable)
{
    return New (internallySyncrhonized, src, codeConverter, seekable ? SeekableFlag::eSeekable : SeekableFlag::eNotSeekable);
}
