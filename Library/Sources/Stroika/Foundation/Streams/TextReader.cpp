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

    public:
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
            Require (IsOpenRead ());
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            // Tricky todo. Must grab ENUF bytes (non-blocking) to assure we can create at least one character.
            // Note we cannot SEEK the _fSource stream, which is what makes it hard...
            // Just means read ahead a bit, and store whatever we needed into _fReadAheadCache
            byte   inByteBuf[10]; // just enuf to hold a single encoded character
            size_t currentByteGoal = 1;
        again:
            if (auto o = PreReadUpstreamInto_ (span{inByteBuf}, currentByteGoal, eDontBlock)) {
                span<const byte> binarySrcSpan{*o};
                Character        ignoredChar;
                span<Character>  targetBuf{&ignoredChar, 1};
                Assert (_fCharConverter.ComputeTargetCharacterBufferSize (binarySrcSpan.size ()) <= targetBuf.size ()); // just because _fCharConverter will require this
                span<Character> convertedCharacters = _fCharConverter.Bytes2Characters (&binarySrcSpan, targetBuf);
                if (convertedCharacters.empty ()) {
                    // We have zero convertedCharacters, so apparently not enough bytes read. Read one more, and try again.
                    ++currentByteGoal;
                    Prepend2ReadAheadCache_ (binarySrcSpan);
                    goto again;
                }
                else {
                    Assert (_fReadAheadCache == nullopt); // may not always be true, but if not true we need todo more work here...
                    Assert (not binarySrcSpan.empty () and not convertedCharacters.empty ());
                    Assert (convertedCharacters.size () <= targetBuf.size ());
                    Prepend2ReadAheadCache_ (binarySrcSpan);
                    return 1;
                }
            }
            return nullopt; // insufficient binary data known available to form a single character
        }
        virtual optional<span<Character>> Read (span<Character> intoBuffer, NoDataAvailableHandling blockFlag) override
        {
            Require (not intoBuffer.empty ());
            Require (IsOpenRead ());
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            /*
             *  Try to minimize # of calls to underlying fSource binary stream per call this this Read () - efficiency.
             *
             *  Only need to read one character, and once we have that much, don't block on more from upstream binary stream.
             *
             *  But always just return at least 1 char if we can, so if partial, and no full chars read, keep reading.
             *
             *  Since number of Characters filled always <= number of bytes read, we can read up to that # of bytes from upstream binary stream.
             */
            size_t readAtMostCharacters = intoBuffer.size (); // must read at least one character, and no more than (intoEnd - intoStart)
            StackBuffer<byte, 8 * 1024> inByteBuf{Memory::eUninitialized, readAtMostCharacters};
            size_t                      currentByteGoal = 1;
        again:
            if (auto o = PreReadUpstreamInto_ (span{inByteBuf}, currentByteGoal, blockFlag)) {
                span<const byte> binarySrcSpan{*o};
                span<Character>  targetBuf{intoBuffer};
                Assert (_fCharConverter.ComputeTargetCharacterBufferSize (inByteBuf.size ()) <= targetBuf.size ()); // just because _fCharConverter will require this
                span<Character> convertedCharacters = _fCharConverter.Bytes2Characters (&binarySrcSpan, targetBuf);
                if (binarySrcSpan.empty ()) {
                    // Great case! Then we got data - already copied into intoStart (part of target span) - so just return the characters read
                    Assert (convertedCharacters.size () <= targetBuf.size ());
                    _fOffset += convertedCharacters.size ();
                    return intoBuffer.subspan (0, convertedCharacters.size ());
                }
                else if (convertedCharacters.empty ()) {
                    // We have zero convertedCharacters, so apparently not enough bytes read. Read one more, and try again.
                    ++currentByteGoal;
                    Prepend2ReadAheadCache_ (binarySrcSpan); // PreReadUpstreamInto_ removed from readahead cache so put back to try a second byte
                    goto again;
                }
                else {
                    Assert (not binarySrcSpan.empty () and not convertedCharacters.empty ());
                    Assert (convertedCharacters.size () <= targetBuf.size ());
                    _fOffset += convertedCharacters.size (); // complete the read
                    // Save the extra bytes for next time (with the offset where those bytes come from)
                    Prepend2ReadAheadCache_ (binarySrcSpan);
                    return intoBuffer.subspan (0, convertedCharacters.size ());
                }
            }
            else {
                // if we failed to meet our byte goal, any already read bytes have been squirreled away and we just return saying not enuf data
                Assert (blockFlag == eDontBlock);
                return nullopt;
            }
            AssertNotReached ();
            return nullopt;
        }
        template <size_t EXTENT>
        span<byte> ReadFromAndRemoveFromReadAheadCache_ (span<byte, EXTENT> intoSpan)
        {
            size_t bytes2Copy = intoSpan.size ();
            Require (_fReadAheadCache and _fReadAheadCache->fData.size () >= bytes2Copy);
            Require (this->_fOffset == _fReadAheadCache->fFrom);
            Memory::CopySpanData (span{_fReadAheadCache->fData}, intoSpan);
            if (bytes2Copy == _fReadAheadCache->fData.size ()) {
                _fReadAheadCache.reset ();
            }
            else {
                _fReadAheadCache.emplace (_ReadAheadCache{.fFrom = _fOffset + bytes2Copy, .fData = span{_fReadAheadCache->fData}.subspan (bytes2Copy)});
            }
            return intoSpan;
        }
        template <size_t EXTENT>
        void Prepend2ReadAheadCache_ (span<const byte, EXTENT> data2Append)
        {
            if (_fReadAheadCache) {
                Require (_fOffset == _fReadAheadCache->fFrom - data2Append.size ());
                StackBuffer<byte> combined{data2Append};
                combined.push_back (span{_fReadAheadCache->fData});
                _fReadAheadCache.emplace (_ReadAheadCache{.fFrom = _fOffset, .fData = combined});
            }
            else {
                _fReadAheadCache.emplace (_ReadAheadCache{.fFrom = _fOffset, .fData = data2Append});
            }
        }
        // use (and empty) _fReadAheadCache, and read goalSizeAtLeast bytes (if possible) into intoBuf from upstream source.
        // Return nullopt if fails due to EWouldBlock (in which case leave _fReadAheadCache unchanged)
        // intoBuf assumed had no real data on entry, and filled in with good data on exit (except nullopt case).
        // return span says how much good data read.
        // If goal is met:
        //      o   Return value is valid span of at least goalSizeAtLeast bytes
        //      o   Any excess read data is stored in _fReadAheadCache (say because intoBuf was too small)
        // If goal not met either:
        //      o   return value is nullopt (and _fReadAheadCache saves any actually read bytes < goalsize);
        //      o   return value is shorter buffer (iff EOF) - and that is known to be the end.
        nonvirtual optional<span<const byte>> PreReadUpstreamInto_ (span<byte> intoBuf, size_t goalSizeAtLeast, NoDataAvailableHandling blockFlag)
        {
            Require (goalSizeAtLeast <= intoBuf.size ());
            span<const byte> result; // always some subset of intoBuf
            if (_fReadAheadCache and _fReadAheadCache->fFrom == this->_fOffset) {
                // If _fReadAheadCache present, may contain enuf bytes for a full character, so don't do another
                // blocking read as it MAY not be necessary, and if it is, will be caught in existing logic for 'again' not enuf data
                //
                // NOTE - for non-seekable case, this _fReadAheadCache->fFrom == this->_fOffset will always work/match. Only way to fail
                // is if we got seeked (subclass). But then subclass better have adjusted seek offset of _fSource magically too
                Assert (intoBuf.size () >= _fReadAheadCache->fData.size ());
                size_t amtToCopy = min (_fReadAheadCache->fData.size (), intoBuf.size ());
                result           = ReadFromAndRemoveFromReadAheadCache_ (intoBuf.subspan (0, amtToCopy));
            }
            while (result.size () < goalSizeAtLeast) {
                switch (blockFlag) {
                    case eBlockIfNoDataAvailable: {
                        auto r = _fSource.Read (intoBuf.subspan (result.size ()));
                        if (r.size () == 0) {
                            // we are done - got EOF, and so just return as much as we have
                            return result;
                        }
                        else {
                            // add the new data to result (its already been read into the right place of intoBuf, just adjust size of result)
                            result = intoBuf.subspan (0, result.size () + r.size ());
                        }
                    } break;
                    case eDontBlock: {
                        if (auto o = _fSource.ReadNonBlocking (intoBuf.subspan (result.size ()))) {
                            if (o->size () == 0) {
                                // we are done - got EOF, and so just return as much as we have
                                return result;
                            }
                            else {
                                // add the new data to result (its already been read into the right place of intoBuf, just adjust size of result)
                                result = intoBuf.subspan (0, result.size () + o->size ());
                            }
                        }
                        else {
                            // if we have no raw bytes to work with, cannot produce characters (yet)
                            // so must PUT BACK accumulated text for next time...
                            Prepend2ReadAheadCache_ (result);
                            return nullopt;
                        }
                    } break;
                }
            }
            return result;
        }
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
                    for (auto o = Read (span{&c, 1}, NoDataAvailableHandling::eDEFAULT); o && o->size () == 1;
                         o      = Read (span{&c, 1}, NoDataAvailableHandling::eDEFAULT)) {
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
                if (auto o = Read (span{&c, 1}, NoDataAvailableHandling::eDEFAULT); o && o->size () == 0) [[unlikely]] {
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
