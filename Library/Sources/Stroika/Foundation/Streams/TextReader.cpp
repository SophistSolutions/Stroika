/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/CodeCvt.h"
#include "../Characters/TextConvert.h"
#include "../Containers/Support/ReserveTweaks.h"
#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "../Execution/Common.h"
#include "../Execution/OperationNotSupportedException.h"
#include "../Memory/InlineBuffer.h"
#include "../Memory/StackBuffer.h"

#include "IterableToInputStream.h"

#include "TextReader.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::Streams::TextReader;

using Characters::String;
using Debug::AssertExternallySynchronizedMutex;
using Memory::InlineBuffer;
using Memory::StackBuffer;

namespace {
    const auto kReadPartialCharacterAtEndOfBinaryStreamException_ =
        Execution::RuntimeErrorException{"TextReader read partial character at end of binary input stream"sv};

    class FromBinaryStreamBaseRep_ : public InputStream::IRep<Character> {
    public:
        FromBinaryStreamBaseRep_ (const InputStream::Ptr<byte>& src, const Characters::CodeCvt<>& charConverter)
            : _fSource{src}
            , _fCharConverter{charConverter}
        {
        }

    protected:
        /*
         *  In Read (Character* intoStart, Character* intoEnd) override,
         *  if we have converted at least one character, but have leftover bytes read,
         *  we must somehow backup binary read amount for next time - save 'multibyte' state
         *
         *  Two strategies: 
         *      o   SEEK source back: quick/easiy buy maybe impossible (if source not seekable)
         *      o   OR track 'extra stuff to prepend to next read' - and use that on next read.
         * 
         *  We COULD use the 'seek' approach by creating a wrapper class that introduced seekability so
         *  we could assume it. But that would be costly - because it over-generalizes what we need. We need just very
         *  limited seekability, and that is best captured by the second approach. And little point in two implementations
         *  so just go with the second - since it will always work, and work fairly well (downside is a bit of extra code complexity here)
         */
        struct _ReadAheadCache {
            SeekOffsetType fFrom; // save offset cuz cache only valid on reads from this offset (if subclass allowed seek)
            InlineBuffer<byte, 8> fData; // @todo UPDATE DOCS - AND LOGIC - CAN BE MORE THAN ONE CHARACTERS WORTH DUE TO READ NON BLOCKING WITH nullptr args!!!
        };
        optional<_ReadAheadCache> _fReadAheadCache;

    protected:
        virtual bool IsSeekable () const override
        {
            return false; // but subclasses may implement seekability
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
        virtual optional<size_t> AvailableToRead () override
        {
            // todo
            AssertNotImplemented ();
            return 0; // not sure used right now - but review ReadNonBlocking code below and fix Read to handle nonblcoig case
        }
        virtual optional<span<Character>> Read (span<Character> intoBuffer, NoDataAvailableHandling blockFlag) override
        {
            Require (not intoBuffer.empty ());
            Require (IsOpenRead ());

            /*
             *  Try to minimize # of calls to underlying fSource binary stream per call this this Read () - efficiency.
             *
             *  Only need to read one character, and once we have that much, don't block on more from upstream binary stream.
             *
             *  But always just return at least 1 char if we can, so if partial, and no full chars read, keep reading.
             *
             *  Since number of Characters filled always <= number of bytes read, we can read up to that # of bytes from upstream binary stream.
             */
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            {
                size_t readAtMostCharacters = intoBuffer.size (); // must read at least one character, and no more than (intoEnd - intoStart)
                StackBuffer<byte, 8 * 1024> inBuf{Memory::eUninitialized, readAtMostCharacters};
                if (_fReadAheadCache and _fReadAheadCache->fFrom == this->_fOffset) {
                    // If _fReadAheadCache present, may contain enuf bytes for a full character, so don't do another
                    // blocking read as it MAY not be necessary, and if it is, will be caught in existing logic for 'again' not enuf data
                    inBuf = span<byte>{_fReadAheadCache->fData};
                }
                else {
                    inBuf.ShrinkTo (_fSource.Read (span{inBuf}).size ()); // @todo WRONG FOR NONBLOCKING CASE
                }
            again:
                span<const byte> binarySrcSpan{inBuf};
                span<Character>  targetBuf{intoBuffer};
                Assert (_fCharConverter.ComputeTargetCharacterBufferSize (inBuf.size ()) <= targetBuf.size ()); // just because _fCharConverter will require this
                span<Character> convertedCharacters = _fCharConverter.Bytes2Characters (&binarySrcSpan, targetBuf);
                if (binarySrcSpan.empty ()) {
                    // then we got data - already copied into intoStart (part of target span) - so just return the number of characters read
                    Assert (convertedCharacters.size () <= targetBuf.size ());
                    _fOffset += convertedCharacters.size ();
                    return intoBuffer.subspan (0, convertedCharacters.size ());
                }
                else if (convertedCharacters.empty ()) {
                    // @todo - we only NEED one byte here, but probably more efficient to grab more. Either wrap _fSource in streamReader or cache more directly here!
                    // We have zero convertedCharacters, so apparently not enough bytes read. Read one more, and try again.
                    byte b;
                    if (_fSource.Read (span{&b, 1}, blockFlag).size () == 1) {
                        inBuf.push_back (b);
                        goto again;
                    }
                    else {
                        // if we read EOF, and have a partial character read, thats an error - so throw
                        Execution::Throw (kReadPartialCharacterAtEndOfBinaryStreamException_);
                    }
                }
                else {
                    Assert (not binarySrcSpan.empty () and not convertedCharacters.empty ());
                    Assert (convertedCharacters.size () <= targetBuf.size ());
                    _fOffset += convertedCharacters.size (); // complete the read
                    // Save the extra bytes for next time (with the offset where those bytes come from)
                    _fReadAheadCache.emplace (_ReadAheadCache{.fFrom = _fOffset, .fData = binarySrcSpan});
                    return intoBuffer.subspan (0, convertedCharacters.size ());
                }
            }
            return span<Character>{};
        }
#if 0
        virtual optional<size_t> ReadNonBlocking (Character* intoStart, Character* intoEnd) override
        {
            /// @todo hANDLE intoStart==nullptr case!!!
            Require ((intoStart == intoEnd) or (intoStart != nullptr));
            Require ((intoStart == intoEnd) or (intoEnd != nullptr));
            Require (IsOpenRead ());
#if 1
            AssertNotReached ();
            return nullopt;
#else
            // Plan:
            //      o   ReadNonBlocking upstream (or grab from _fReadAheadCache)
            //      o   decode and see if at least one character
            //      o   if need more data to decode, try 'again' and grab more bytes.
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            {
                size_t readAtMostCharacters = size_t (intoEnd - intoStart); // must read at least one character, and no more than (intoEnd - intoStart)
                StackBuffer<byte, 8 * 1024> inBuf{Memory::eUninitialized, readAtMostCharacters};
                {
                    bool atEOF = false;
                    if (_fReadAheadCache and _fReadAheadCache->fFrom == this->_fOffset) {
                        inBuf = span<byte>{_fReadAheadCache->fData};
                        // ReadNonBlocking probably needed, but may not be. But since non blocking, its harmless, so try it
                        if (readAtMostCharacters > inBuf.size ()) {
                            StackBuffer<byte> buf{Memory::eUninitialized, readAtMostCharacters - inBuf.size ()};
                            if (auto o = _fSource.ReadNonBlocking (begin (buf), end (buf))) {
                                atEOF = *o == 0;
                                inBuf.push_back (span{buf.data (), *o});
                            }
                        }
                    }
                    else {
                        if (auto o = _fSource.ReadNonBlocking (begin (inBuf), end (inBuf))) {
                            atEOF = *o == 0;
                            inBuf.ShrinkTo (*o);
                        }
                    }
                    if (atEOF) {
                        if (inBuf.size () != 0) {
                            Execution::Throw (kReadPartialCharacterAtEndOfBinaryStreamException_);
                        }
                        return 0;
                    }
                }
            again:
                span<const byte> binarySrcSpan{inBuf};
                if (intoStart == nullptr) {
                    // tricky case. Must possibly do several binary reads til we have enuf for a character, and then return
                    // one for that case, but must save away the read bits (_fReadAheadCache).
                    auto cs = _fCharConverter.Bytes2Characters (binarySrcSpan);
                    if (cs == 0) {
                        byte b{};
                        if (auto o = _fSource.ReadNonBlocking (&b, &b + 1)) {
                            inBuf.push_back (b);
                            goto again;
                        }
                        else {
                            // (re-)save _fReadAheadCache and return null cuz no character ready yet
                            _fReadAheadCache.emplace (_ReadAheadCache{.fFrom = _fOffset, .fData = binarySrcSpan});
                            return nullopt;
                        }
                    }
                    return cs;
                }
                else {
                    span<Character> targetBuf{intoStart, intoEnd};
                    Assert (_fCharConverter.ComputeTargetCharacterBufferSize (inBuf.size ()) <= targetBuf.size ()); // just because _fCharConverter will require this
                    span<Character> convertedCharacters = _fCharConverter.Bytes2Characters (&binarySrcSpan, targetBuf);
                    if (binarySrcSpan.empty ()) {
                        // then we got data - already copied into intoStart (part of target span) - so just return the number of characters read
                        Assert (convertedCharacters.size () <= targetBuf.size ());
                        _fOffset += convertedCharacters.size ();
                        return convertedCharacters.size ();
                    }
                    else if (convertedCharacters.empty ()) {
                        // We have zero convertedCharacters, so apparently not enough bytes read. Read one more, and try again.
                        byte b;
                        if (auto o = _fSource.ReadNonBlocking (&b, &b + 1)) {
                            if (*o == 1) {
                                inBuf.push_back (b);
                                goto again;
                            }
                            else {
                                // if we read EOF, and have a partial character read, thats an error - so throw
                                Execution::Throw (kReadPartialCharacterAtEndOfBinaryStreamException_);
                            }
                        }
                        else {
                            return nullopt; // need more bytes, not at EOF, and bytes not ready
                        }
                    }
                    else {
                        Assert (not binarySrcSpan.empty () and not convertedCharacters.empty ());
                        Assert (convertedCharacters.size () <= targetBuf.size ());
                        _fOffset += convertedCharacters.size (); // complete the read
                        // Save the extra bytes for next time (with the offset where those bytes come from)
                        _fReadAheadCache.emplace (_ReadAheadCache{.fFrom = _fOffset, .fData = binarySrcSpan});
                        return convertedCharacters.size ();
                    }
                }
            }
#endif
        }
#endif
        virtual SeekOffsetType GetReadOffset () const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
            Require (IsOpenRead ());
            return _fOffset;
        }

    protected:
        InputStream::Ptr<byte>                                         _fSource;
        const Characters::CodeCvt<Character>                           _fCharConverter;
        SeekOffsetType                                                 _fOffset{0};
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
    };

    class UnseekableBinaryStreamRep_ final : public FromBinaryStreamBaseRep_ {
        using inherited = FromBinaryStreamBaseRep_;

    public:
        UnseekableBinaryStreamRep_ (const InputStream::Ptr<byte>& src, const Characters::CodeCvt<char32_t>& charConverter)
            : inherited{src, charConverter}
        {
        }
    };

    /*
     *  Note this CAN be used with either a seekable or unseekable byte stream.
     *  But this always produces a SEEKABLE TextStream, so can be used to produce
     *  a seekable TextStream from a non-seekable binary stream.
     * 
     *  But note - it does this at the cost of KEEPING a copy of the entire stream in memory.
     */
    class CachingSeekableBinaryStreamRep_ final : public FromBinaryStreamBaseRep_ {
        using inherited = FromBinaryStreamBaseRep_;

    public:
        CachingSeekableBinaryStreamRep_ (const InputStream::Ptr<byte>& src, const Characters::CodeCvt<char32_t>& charConverter, ReadAhead readAhead)
            : FromBinaryStreamBaseRep_{src, charConverter}
            , fReadAheadAllowed_{readAhead == ReadAhead::eReadAheadAllowed}
        {
        }

    protected:
        virtual bool IsSeekable () const override
        {
            return true;
        }
        virtual optional<span<Character>> Read (span<Character> intoBuffer, NoDataAvailableHandling blockFlag) override
        {
            Require (not intoBuffer.empty ());
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            Require (IsOpenRead ());

            // if already cached, return from cache. Note - even if only one element is in the Cache, thats enough to return
            // and not say 'eof'
            if (_fOffset < fCache_.size ()) {
                // return data from cache
                size_t nToRead     = intoBuffer.size ();
                size_t nInBufAvail = fCache_.size () - static_cast<size_t> (_fOffset);
                nToRead            = min (nToRead, nInBufAvail);
                Assert (nToRead > 0);
                for (size_t i = 0; i < nToRead; ++i) {
                    intoBuffer[i] = fCache_[i + static_cast<size_t> (_fOffset)];
                }
                _fOffset += nToRead;
                return intoBuffer.subspan (0, nToRead);
            }

            // if not already cached, add to cache, and then return the data
            SeekOffsetType origOffset       = _fOffset;
            auto           pushIntoCacheBuf = [origOffset, this] (Character* bufStart, Character* bufEnd) {
                size_t n            = bufEnd - bufStart;
                size_t newCacheSize = static_cast<size_t> (origOffset + n);
                Assert (fCache_.size () == static_cast<size_t> (origOffset));
                Assert (newCacheSize > fCache_.size ());
                Containers::Support::ReserveTweaks::Reserve4AddN (fCache_, n);
                fCache_.resize_uninitialized (newCacheSize);
                for (size_t i = 0; i < n; ++i) {
                    fCache_[i + static_cast<size_t> (origOffset)] = bufStart[i];
                }
            };
            // if not a cache hit, use inherited Read (), and fill the cache.
            // If the calling read big enough, re-use that buffer.
            constexpr size_t kMinCachedReadSize_{512};
            if (intoBuffer.size () >= kMinCachedReadSize_ or not fReadAheadAllowed_) {
                auto result = inherited::Read (intoBuffer, blockFlag);
                if (result == nullopt) {
                    Execution::Throw (EWouldBlock::kThe);
                }
                if (result->size () != 0) {
                    if (origOffset + result->size () > numeric_limits<size_t>::max ()) [[unlikely]] {
                        // size_t can be less bits than SeekOffsetType, in which case we cannot cahce all in RAM
                        Execution::Throw (range_error{"seek past max size for size_t"});
                    }
                    pushIntoCacheBuf (intoBuffer.data (), intoBuffer.data () + result->size ());
                }
                return result;
            }
            else {
                // if argument buffer not big enough, read into a temporary buffer
                constexpr size_t kUseCacheSize_ = 8 * kMinCachedReadSize_;
                Character        buf[kUseCacheSize_]; // use wchar_t and cast to Character* so we get this array uninitialized
                auto             result =
                    inherited::Read (span{reinterpret_cast<Character*> (std::begin (buf)), reinterpret_cast<Character*> (std::end (buf))}, blockFlag);
                if (result == nullopt) {
                    Execution::Throw (EWouldBlock::kThe);
                }
                if (result->size () != 0) {
                    if (origOffset + result->size () > numeric_limits<size_t>::max ()) [[unlikely]] {
                        // size_t can be less bits than SeekOffsetType, in which case we cannot cahce all in RAM
                        Execution::Throw (range_error{"seek past max size for size_t"});
                    }
                    pushIntoCacheBuf (std::begin (buf), std::begin (buf) + result->size ());
                    result = result->subspan (0, intoBuffer.size ());
                    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wclass-memaccess\"");
                    (void)::memcpy (intoBuffer.data (), std::begin (buf), result->size () * sizeof (Character));
                    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wclass-memaccess\"");
                    _fOffset = origOffset + result->size ();
                }
                return result;
            }
        }
        virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            Require (IsOpenRead ());
            switch (whence) {
                case Whence::eFromStart: {
                    if (offset < 0) [[unlikely]] {
                        Execution::Throw (range_error{"seek"});
                    }
                    SeekTo_ (static_cast<SeekOffsetType> (offset));
                } break;
                case Whence::eFromCurrent: {
                    Streams::SeekOffsetType       curOffset = _fOffset;
                    Streams::SignedSeekOffsetType newOffset = curOffset + offset;
                    if (newOffset < 0) [[unlikely]] {
                        Execution::Throw (range_error{"seek"});
                    }
                    SeekOffsetType uNewOffset = static_cast<SeekOffsetType> (newOffset);
                    SeekTo_ (static_cast<size_t> (uNewOffset));
                } break;
                case Whence::eFromEnd: {
                    // @todo DECIDE IF SeekRead needs blockFlag???
                    // and fix/simpify code here!!! for blockflag
                    Character c;
                    for (auto o = Read (span{&c, 1}, NoDataAvailableHandling::eDefault); o && o->size () == 1;
                         o      = Read (span{&c, 1}, NoDataAvailableHandling::eDefault)) {
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
                // @todo Seek may require NoDataAvailableHandling flag!!!
                // @todo fix data missing logic... - need datanotavailhandling flag arg
                if (auto o = Read (span{&c, 1}, NoDataAvailableHandling::eDefault); o && o->size () == 0) [[unlikely]] {
                    Execution::Throw (range_error{"seek"});
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
        bool fReadAheadAllowed_{false};
        InlineBuffer<Character> fCache_; // Cache uses wchar_t instead of Character so can use resize_uninitialized () - requires is_trivially_constructible
        // @todo for larger streams, a different data structure than InlineBuffer would be appropriate - E.G. ChunkedArray! probably best.
    };
}

/*
 ********************************************************************************
 ************************** Streams::TextReader::New ****************************
 ********************************************************************************
 */
namespace {
    auto New_ (const InputStream::Ptr<byte>& src, const Characters::CodeCvt<>& codeConverter, SeekableFlag seekable, ReadAhead readAhead)
        -> TextReader::Ptr
    {
        using TextReader::Ptr;
        Ptr p = (seekable == SeekableFlag::eSeekable) ? Ptr{make_shared<CachingSeekableBinaryStreamRep_> (src, codeConverter, readAhead)}
                                                      : Ptr{make_shared<UnseekableBinaryStreamRep_> (src, codeConverter)};
        Ensure (p.IsSeekable () == (seekable == SeekableFlag::eSeekable));
        return p;
    }
}

#if 0
auto TextReader::New (const InputStream::Ptr<byte>& src, optional<SeekableFlag> seekable, ReadAhead readAhead) -> Ptr
{
    if (seekable == nullopt) {
        seekable = src.IsSeekable () ? SeekableFlag::eSeekable : SeekableFlag::eNotSeekable;
    }
    Ptr p = TextReader::New (src, Characters::UnicodeExternalEncodings::eUTF8, *seekable, readAhead);
    Ensure (src.IsSeekable () == (seekable == SeekableFlag::eSeekable));
    return p;
}
#endif

auto TextReader::New (const InputStream::Ptr<byte>& src, const optional<AutomaticCodeCvtFlags> codeCvtFlags,
                      optional<SeekableFlag> seekable, ReadAhead readAhead) -> Ptr
{
    if (seekable == nullopt) {
        seekable = src.IsSeekable () ? SeekableFlag::eSeekable : SeekableFlag::eNotSeekable;
    }
    using namespace Characters;
    CodeCvt<> codeConverter = [&] () {
        if (src.IsSeekable ()) {
            auto savedSeek = src.GetOffset ();
            // read possible BOM, and then chose CodeCvt according to codeCvtFlags
            byte                                                          bomData[Characters::kMaxBOMSize];
            optional<tuple<Characters::UnicodeExternalEncodings, size_t>> bomInfo;
            if (src.ReadAll (span{bomData}).size () == Memory::NEltsOf (bomData) and
                (bomInfo = Characters::ReadByteOrderMark (span{bomData})).has_value ()) {
                // adjust amount read from input stream if we over-read
                src.Seek (savedSeek + get<size_t> (*bomInfo)); // adjust amount read from input stream if we read anything (could be a no-op seek)
                return CodeCvt<>{get<UnicodeExternalEncodings> (*bomInfo)};
            }
            else {
                src.Seek (savedSeek); // adjust amount read from input stream if we read anything
                switch (codeCvtFlags.value_or (AutomaticCodeCvtFlags::eDEFAULT)) {
                    case AutomaticCodeCvtFlags::eReadBOMAndIfNotPresentUseUTF8:
                        return CodeCvt<>{UnicodeExternalEncodings::eUTF8};
                    case AutomaticCodeCvtFlags::eReadBOMAndIfNotPresentUseCurrentLocale:
                        return CodeCvt<>{locale{}};
                    default:
                        AssertNotReached ();
                        return CodeCvt<>{};
                }
            }
        }
        else {
            return CodeCvt<>{locale{}};
        }
    }();
    return New_ (src, codeConverter, *seekable, readAhead);
}

auto TextReader::New (const InputStream::Ptr<byte>& src, const Characters::CodeCvt<>& codeConverter, optional<SeekableFlag> seekable,
                      ReadAhead readAhead) -> Ptr
{
    if (seekable == nullopt) {
        seekable = src.IsSeekable () ? SeekableFlag::eSeekable : SeekableFlag::eNotSeekable;
    }
    return New_ (src, codeConverter, *seekable, readAhead);
}

auto TextReader::New (const Traversal::Iterable<Character>& src) -> Ptr
{
    Ptr p = IterableToInputStream::New<Character> (src);
    Ensure (p.IsSeekable ());
    return p;
}
