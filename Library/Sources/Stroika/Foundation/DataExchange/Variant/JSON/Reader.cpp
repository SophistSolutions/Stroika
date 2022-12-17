/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include <optional>

#include "../../../Characters/FloatConversion.h"
#include "../../../Characters/Format.h"
#include "../../../Characters/String2Int.h"
#include "../../../Characters/StringBuilder.h"
#include "../../../Containers/Support/ReserveTweaks.h"
#include "../../../Memory/InlineBuffer.h"
#include "../../../Streams/TextReader.h"
#include "../../../Traversal/Range.h"
#include "../../BadFormatException.h"

#include "Reader.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Traversal;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {

    /*
     * Parse strategy:
     *      o   Pre-pass to map all input to UNICODE - and then just handle a sequence of unicode strings
     *      o   Classic resursive decent parser.
     *      o   Inline lexical analysis (cuz very simple)
     */

    enum ReadState_ {
        eNeutral_,
        eInObject_,
        eInArray_,
        eInNumber_,
        eInString_,
    };

    // @todo generalize this so templated and can use in header file
    // or merge this buffering logic into Ptr class? No - issue with that is we mess up other access to Ptr,
    // so must be clear this an adopt (or have detach method)
    // Maybe have "Syncrhonize" method, so this forces THIS seek ptr to same position as
    // underlying stream?
    struct BufferedStreamReader_ {
        using SeekOffsetType       = Streams::SeekOffsetType;
        using SignedSeekOffsetType = Streams::SignedSeekOffsetType;
        using Whence               = Streams::Whence;

        // may want to tune these - unclear
        static constexpr size_t kDefaultReadBufferSize = 8 * 1024;
        static constexpr size_t kMaxBufferedChunkSize  = 64 * 1024;
        static constexpr size_t kCountPingPingBufs     = 2;

        using ElementType = Character;
        BufferedStreamReader_ (const Streams::InputStream<ElementType>::Ptr& underlyingReadFromStreamAdopted)
            : fStrm_{underlyingReadFromStreamAdopted}
            , fOffset_{underlyingReadFromStreamAdopted.GetOffset ()}
        {
        }
        BufferedStreamReader_ ()                             = delete;
        BufferedStreamReader_ (const BufferedStreamReader_&) = delete;
        ~BufferedStreamReader_ ()
        {
            // @todo perhaps CTOR flag to control if we do this or not
            try {
                this->SyncrhonizeWithUnderlyingStream ();
            }
            catch (...) {
            }
        }


        optional<ElementType> Read ()
        {
            if (auto p = Read1FromCache_ ()) { // usually will get hit - else default to standard algorithm
                return p;
            }
            ElementType b{};
            return (Read_Slow_Case_ (&b, &b + 1) == 0) ? optional<ElementType>{} : b;
        }
        optional<ElementType> Peek ()
        {
            // @todo OPTIMIZE
            // Require (this->IsSeekable ());
            //  Require (IsOpen ());
            if (auto p = Peek1FromCache_ ()) {  // usually will get hit - else default to standard algorithm
                return p;
            }
            SeekOffsetType saved  = fOffset_;
            auto           result = this->Read ();
            fOffset_ = saved;
            return result;
        }

        nonvirtual SeekOffsetType Seek (SeekOffsetType offset)
        {
            Require (offset < static_cast<SeekOffsetType> (numeric_limits<SignedSeekOffsetType>::max ()));
            return Seek (Whence::eFromStart, static_cast<SignedSeekOffsetType> (offset));
        }

        size_t ReadAll (ElementType* intoStart, ElementType* intoEnd)
        {
            size_t elementsRead{};
            for (ElementType* readCursor = intoStart; readCursor < intoEnd;) {
                size_t eltsReadThisTime = Read (readCursor, intoEnd);
                Assert (eltsReadThisTime <= static_cast<size_t> (intoEnd - readCursor));
                if (eltsReadThisTime == 0) {
                    // irrevocable EOF
                    break;
                }
                elementsRead += eltsReadThisTime;
                readCursor += eltsReadThisTime;
            }
            return elementsRead;
        }

        // Since all we do is read from that stream, our OUR offset may DIVERGE from that stream - otherwise
        // we are the smae. So this just means - set that stream to have the same seek offset as our caching reader
        void SyncrhonizeWithUnderlyingStream ()
        {
            fStrm_.Seek (GetOffset ());
        }
        inline bool IsAtEOF ()
        {
            if (fOffset_ < fFarthestReadInUnderlyingStream_) {
                return false; // not logically needed, but optimization
            }
            return not Peek ().has_value ();
        }
        nonvirtual SeekOffsetType GetOffset () const
        {
            return fOffset_;
        }
        nonvirtual SeekOffsetType Seek (Whence whence, SignedSeekOffsetType offset)
        {
            switch (whence) {
                case Whence::eFromCurrent:
                    fOffset_ += offset;
                    break;
                case Whence::eFromStart:
                    fOffset_ = offset;
                    break;
                case Whence::eFromEnd:
                    fStrm_.Seek (Whence::eFromEnd, offset);
                    fOffset_ = fStrm_.GetOffset ();
                    break;
            }
            return fOffset_;
        }
        size_t Read (ElementType* intoStart, ElementType* intoEnd)
        {
            // if already cached, return from cache. Note - even if only one element is in the Cache, thats enough to return
            // and not say 'eof'
            if (optional<size_t> o = ReadFromCache_ (intoStart, intoEnd)) {
                return *o;
            }
            return Read_Slow_Case_ (intoStart, intoEnd);
        }

        private:
        using InlineBufferElementType_ = conditional_t<is_same_v<Characters::Character, ElementType>, wchar_t, ElementType>;
        size_t Read_Slow_Case_ (ElementType* intoStart, ElementType* intoEnd)
        {
            ElementType buf[kDefaultReadBufferSize];
            fStrm_.Seek (fOffset_); // check if getoffset not same in case not seekable) - or handle not seekable case
            size_t nRecordsRead = fStrm_.Read (begin (buf), end (buf));
            if (nRecordsRead == 0) {
                // not much point in caching - at eof
                return 0;
            }
            fFarthestReadInUnderlyingStream_ = max (fFarthestReadInUnderlyingStream_, fStrm_.GetOffset ());
            FillCacheWith_ (fOffset_, begin (buf), begin (buf) + nRecordsRead);
            return Memory::ValueOf (ReadFromCache_ (intoStart, intoEnd)); // we just cached bytes a the right offset so this must succeed
        }


    private:
        Streams::InputStream<ElementType>::Ptr fStrm_;

        Streams::SeekOffsetType fOffset_{0};
        Streams::SeekOffsetType fFarthestReadInUnderlyingStream_{0};

        struct CacheBlock_ {
            CacheBlock_ ()
            {
            }
            size_t GetSize () const
            {
                return fCacheWindowBuf_.GetSize ();
            }
            SeekOffsetType GetStart () const
            {
                return fCacheWindowBufStart_;
            }
            SeekOffsetType GetEnd () const
            {
                return fCacheWindowBufStart_ + fCacheWindowBuf_.GetSize ();
            }
            nonvirtual optional<ElementType> Peek1FromCache (const SeekOffsetType* actualOffset)
            {
                SeekOffsetType              offset          = *actualOffset;
                size_t                      cacheWindowSize = fCacheWindowBuf_.size ();
                if (fCacheWindowBufStart_ <= offset and offset < fCacheWindowBufStart_ + cacheWindowSize) {
                    return fCacheWindowBuf_[offset - fCacheWindowBufStart_];
                }
                return nullopt;
            }
            nonvirtual optional<ElementType> Read1FromCache (SeekOffsetType* actualOffset)
            {
                auto result = Peek1FromCache (actualOffset);
                if (result) {
                    ++(*actualOffset);
                }
                return result;
            }
            nonvirtual optional<size_t> ReadFromCache (SeekOffsetType* actualOffset, ElementType* intoStart, ElementType* intoEnd)
            {
                size_t cacheWindowSize = fCacheWindowBuf_.size ();
                if (cacheWindowSize != 0) {
                    Range<SignedSeekOffsetType> cacheWindow{
                        static_cast<SignedSeekOffsetType> (fCacheWindowBufStart_),
                        static_cast<SignedSeekOffsetType> (fCacheWindowBufStart_ + cacheWindowSize),
                        Openness::eClosed,
                        Openness::eOpen,
                    };
                    if (cacheWindow.Contains (*actualOffset)) {
                        // then we can return at least some data from the cache - do that now
                        size_t nToRead = intoEnd - intoStart;
                        if (nToRead != 1) {
                            size_t nInBufAvail = static_cast<size_t> (cacheWindow.GetUpperBound () - *actualOffset);
                            nToRead            = min (nToRead, nInBufAvail);
                        }
                        Assert (nToRead > 0); // because contained _fOffset
                        size_t curSeekPosOffsetIntoCache = *actualOffset - cacheWindow.GetLowerBound ();
                        Assert (0 <= curSeekPosOffsetIntoCache and curSeekPosOffsetIntoCache < fCacheWindowBuf_.size ());
                        std::copy (fCacheWindowBuf_ + curSeekPosOffsetIntoCache, fCacheWindowBuf_ + curSeekPosOffsetIntoCache + nToRead, intoStart);
                        *actualOffset += nToRead;
                        return nToRead;
                    }
                }
                return nullopt;
            }
            nonvirtual void FillCacheWith (SeekOffsetType s, const InlineBufferElementType_* intoStart, const InlineBufferElementType_* intoEnd)
            {
                // adjust so smarter to not make cache too big...
                size_t         oldCacheSize = fCacheWindowBuf_.GetSize ();
                SeekOffsetType currentEnd   = fCacheWindowBufStart_ + oldCacheSize;
                size_t         nToWrite   = intoEnd - intoStart;
                Require (nToWrite > 0);
                if (currentEnd == s) {
                    // extend the cache

                    // resize_uninitialized showed up alot in windows profile running 'large-xxx' so figured
                    // if we are going to actually allocate memory anyhow, then do it once, by grabbing largest chunk we
                    // (are ever likely to) ask for.
                    if (oldCacheSize + nToWrite > fCacheWindowBuf_.kMinCapacity) {
                        fCacheWindowBuf_.reserve (kMaxBufferedChunkSize);
                    }
                    fCacheWindowBuf_.resize_uninitialized (oldCacheSize + nToWrite);
                    std::copy (intoStart, intoEnd, fCacheWindowBuf_.begin () + oldCacheSize);
                }
                else {
                    fCacheWindowBuf_.resize_uninitialized (nToWrite); // CAN shrink
                    fCacheWindowBufStart_ = s;
                    std::copy (intoStart, intoEnd, fCacheWindowBuf_.begin ());
                }
            }

        private:
            // Cache uses wchar_t instead of Character so can use resize_uninitialized () - requires is_trivially_constructible
            SeekOffsetType                fCacheWindowBufStart_{0}; // buffer a 'window' around the current data. Not infinite the whole file, but can be pretty big
            Memory::InlineBuffer<InlineBufferElementType_> fCacheWindowBuf_;         
        };
        CacheBlock_ fCacheBlocks_[kCountPingPingBufs];
        size_t      fCacheBlockLastFilled_{0};

        nonvirtual optional<ElementType> Peek1FromCache_ ()
        {
            // first try last filled - generally will be the right one
            for (size_t i = fCacheBlockLastFilled_; i < Memory::NEltsOf (fCacheBlocks_); ++i) {
                if (auto r = fCacheBlocks_[i].Peek1FromCache (&this->fOffset_)) {
                    return r;
                }
            }
            for (size_t i = 0; i < fCacheBlockLastFilled_; ++i) {
                if (auto r = fCacheBlocks_[i].Peek1FromCache (&this->fOffset_)) {
                    return r;
                }
            }
            return nullopt;
        }

        nonvirtual optional<ElementType> Read1FromCache_ ()
        {
            // first try last filled - generally will be the right one
            for (size_t i = fCacheBlockLastFilled_; i < Memory::NEltsOf (fCacheBlocks_); ++i) {
                if (auto r = fCacheBlocks_[i].Read1FromCache (&this->fOffset_)) {
                    return r;
                }
            }
            for (size_t i = 0; i < fCacheBlockLastFilled_; ++i) {
                if (auto r = fCacheBlocks_[i].Read1FromCache (&this->fOffset_)) {
                    return r;
                }
            }
            return nullopt;
        }

        nonvirtual optional<size_t> ReadFromCache_ (ElementType* intoStart, ElementType* intoEnd)
        {
            // first try last filled - generally will be the right one
            for (size_t i = fCacheBlockLastFilled_; i < Memory::NEltsOf (fCacheBlocks_); ++i) {
                if (auto r = fCacheBlocks_[i].ReadFromCache (&this->fOffset_, intoStart, intoEnd)) {
                    return r;
                }
            }
            for (size_t i = 0; i < fCacheBlockLastFilled_; ++i) {
                if (auto r = fCacheBlocks_[i].ReadFromCache (&this->fOffset_, intoStart, intoEnd)) {
                    return r;
                }
            }
            return nullopt;
        }
        nonvirtual void FillCacheWith_ (SeekOffsetType s, const InlineBufferElementType_* intoStart, const InlineBufferElementType_* intoEnd)
        {
            // pingpong buffers
            // try not to overfill any one cache block, but if the amount being read will fit, append to the current cache block
            // dont start a new one
            size_t thisFillSize = intoEnd - intoStart;
            if (fCacheBlocks_[fCacheBlockLastFilled_].GetEnd () != this->fOffset_ or fCacheBlocks_[fCacheBlockLastFilled_].GetSize () + thisFillSize > kMaxBufferedChunkSize) {
                ++fCacheBlockLastFilled_;
                if (fCacheBlockLastFilled_ >= Memory::NEltsOf (fCacheBlocks_)) {
                    fCacheBlockLastFilled_ = 0;
                }
            }
            fCacheBlocks_[fCacheBlockLastFilled_].FillCacheWith (s, intoStart, intoEnd);
        }
        nonvirtual void FillCacheWith_ (SeekOffsetType s, const ElementType* intoStart, const ElementType* intoEnd)
        {
            FillCacheWith_ (s, reinterpret_cast<const InlineBufferElementType_*> (intoStart), reinterpret_cast<const InlineBufferElementType_*> (intoEnd));
        }
    };

    // @todo generalize this so templated and can use in header file
    // or merge this buffering logic into Ptr class? No - issue with that is we mess up other access to Ptr,
    // so must be clear this an adopt (or have detach method)
    // Maybe have "Syncrhonize" method, so this forces THIS seek ptr to same position as
    // underlying stream?
    struct MyBufferedStreamReader_ : BufferedStreamReader_ {
        MyBufferedStreamReader_ (const Streams::InputStream<ElementType>::Ptr& underlyingReadFromStreamAdopted)
            : BufferedStreamReader_{underlyingReadFromStreamAdopted}
        {
        }

        [[nodiscard]] inline wchar_t NextChar ()
        {
            Require (not IsAtEOF ());
            return Read ()->As<wchar_t> ();
        }
        inline void AdvanceOne ()
        {
            Require (not IsAtEOF ());
            Seek (Whence::eFromCurrent, 1);
        }
    };

    VariantValue Reader_value_ (MyBufferedStreamReader_& in);

    // throw if bad hex digit
    uint8_t HexChar2Num_ (char c)
    {
        if ('0' <= c and c <= '9') {
            return static_cast<uint8_t> (c - '0');
        }
        if ('A' <= c and c <= 'F') {
            return static_cast<uint8_t> ((c - 'A') + 10);
        }
        if ('a' <= c and c <= 'f') {
            return static_cast<uint8_t> ((c - 'a') + 10);
        }
        Execution::Throw (BadFormatException{L"JSON: bad hex digit after \\u"sv});
    }

    // 'in' is positioned to the start of string, and we read, leaving in possitioned just after end of string
    VariantValue Reader_String_ (MyBufferedStreamReader_& in)
    {
        Require (not in.IsAtEOF ());
        wchar_t c = in.NextChar ();
        if (c != '\"') [[unlikely]] {
            Execution::Throw (BadFormatException{L"JSON: Expected quoted string"sv});
        }
        // accumulate chars, and check for close-quote
        StringBuilder result;
        while (true) {
            if (in.IsAtEOF ()) [[unlikely]] {
                Execution::Throw (BadFormatException{L"JSON: Unexpected EOF reading string (looking for close quote)"sv});
            }
            c = in.NextChar ();
            if (c == '\"') [[unlikely]] {
                return VariantValue{result.str ()};
            }
            else if (c == '\\') {
                // quoted character read...
                if (in.IsAtEOF ()) [[unlikely]] {
                    Execution::Throw (BadFormatException{L"JSON: Unexpected EOF reading string (looking for close quote)"sv});
                }
                c = in.NextChar ();
                switch (c) {
                    case 'b':
                        c = '\b';
                        break;
                    case 'f':
                        c = '\f';
                        break;
                    case 'n':
                        c = '\n';
                        break;
                    case 'r':
                        c = '\r';
                        break;
                    case 't':
                        c = '\t';
                        break;
                    case 'u': {
                        // Not sure this is right -- But I hope so ... -- LGP 2012-11-29
                        wchar_t newC = '\0';
                        for (int n = 0; n < 4; ++n) {
                            if (in.IsAtEOF ()) [[unlikely]] {
                                Execution::Throw (BadFormatException{L"JSON: Unexpected EOF reading string (looking for close quote)"sv});
                            }
                            newC += HexChar2Num_ (static_cast<char> (in.NextChar ()));
                            if (n != 3) {
                                newC <<= 4;
                            }
                        }
                        c = newC;
                    } break;
                    default: {
                        // if we have \N for any unrecognized N, just treat it as N
                    }
                }
            }
            result += c; // must handle other character quoting (besides \u which was preflighted)
        }
    }

    // 'in' is positioned to the second character of number (first passed as arg), and we read, leaving in positioned just after end of number
    static constexpr Character kDash_{'-'};
    VariantValue               Reader_Number_ (wchar_t initialChar, MyBufferedStreamReader_& in)
    {
        Require (initialChar == '-' or iswdigit (initialChar));

        bool containsDot = false;
        // ACCUMULATE STRING, and then call builtin number parsing functions...
        // This accumulation is NOT as restrictive as it could be - but should accept all valid numbers
        StringBuilder tmp;
        for (wchar_t c = initialChar; c != '\0'; c = in.Read ().value_or ('\0').As<wchar_t> ()) {
            if (iswdigit (c) or c == '.' or c == 'e' or c == 'E' or c == '+' or c == '-') [[likely]] {
                tmp += c;
                if (c == '.') [[unlikely]] {
                    containsDot = true;
                }
            }
            else {
                // any other character signals end of number (not a syntax error)
                // but backup - don't consume next character - not part of number
                Assert (not tmp.empty ()); // at least consumed 'initialChar'
                in.Seek (Streams::Whence::eFromCurrent, -1);
                break;
            }
        }
        Assert (not tmp.empty ());
        if (containsDot) {
            return VariantValue{Characters::FloatConversion::ToFloat<long double> (tmp.begin (), tmp.end ())};
        }
        else {
            // if no - use unsigned since has wider range (if no -)
            return (initialChar == kDash_)
                       ? VariantValue{Characters::String2Int<long long int> (tmp.begin (), tmp.end ())}
                       : VariantValue{Characters::String2Int<unsigned long long int> (tmp.begin (), tmp.end ())};
        }
    }

    // NOTE: THIS STARTS SEEKED JUST PAST OPENING '{'
    VariantValue Reader_Object_ (MyBufferedStreamReader_& in)
    {
        Mapping<String, VariantValue> result;

        // accumulate elements, and check for close-array
        enum LookingFor { eName,
                          eValue,
                          eColon,
                          eComma };
        LookingFor lf = eName;

        optional<String> curName;
        while (true) {
            optional<Character> oNextChar = in.Read ();
            if (not oNextChar.has_value ()) [[unlikely]] {
                in.Seek (Streams::Whence::eFromCurrent, -1);
                Execution::Throw (BadFormatException{L"JSON: Unexpected EOF reading string (looking for '}')"sv});
            }
            wchar_t nextChar = oNextChar->As<wchar_t> ();
            if (nextChar == '}') {
                if (lf == eName or lf == eComma) {
                    // skip char
                    return VariantValue{result};
                }
                else {
                    in.Seek (Streams::Whence::eFromCurrent, -1);
                    Execution::Throw (BadFormatException{L"JSON: Unexpected '}' reading object"sv});
                }
            }
            else if (nextChar == ',') {
                if (lf == eComma) [[likely]] {
                    // skip char
                    lf = eName; // next elt
                }
                else {
                    in.Seek (Streams::Whence::eFromCurrent, -1);
                    Execution::Throw (BadFormatException{L"JSON: Unexpected ',' reading object"sv});
                }
            }
            else if (nextChar == ':') {
                if (lf == eColon) [[likely]] {
                    // skip char
                    lf = eValue; // next elt
                }
                else {
                    in.Seek (Streams::Whence::eFromCurrent, -1);
                    Execution::Throw (BadFormatException{L"JSON: Unexpected ':' reading object"sv});
                }
            }
            else if (iswspace (nextChar)) {
                // skip char
            }
            else {
                in.Seek (Streams::Whence::eFromCurrent, -1);
                if (lf == eName) {
                    curName = Reader_String_ (in).As<wstring> ();
                    lf      = eColon;
                }
                else if (lf == eValue) [[likely]] {
                    result.Add (Memory::ValueOf (curName), Reader_value_ (in));
                    curName = nullopt;
                    lf      = eComma;
                }
                else {
                    Execution::Throw (BadFormatException{L"JSON: Unexpected character looking for colon or comma reading object"sv});
                }
            }
        }
    }

    // NOTE - called with OPENING '[' already read
    VariantValue Reader_Array_ (MyBufferedStreamReader_& in)
    {
        vector<VariantValue> result;

        // accumulate elements, and check for close-array
        bool lookingForElt = true;
        while (true) {
            if (in.IsAtEOF ()) [[unlikely]] {
                Execution::Throw (BadFormatException{L"JSON: Unexpected EOF reading string (looking for ']')"sv});
            }
            if (in.Peek () == ']') {
                if (lookingForElt) {
                    // allow ending ',' - harmless - could  be more aggressive - but if so - careful of zero-sized array special case
                }
                in.AdvanceOne ();
                return VariantValue{result};
            }
            else if (in.Peek () == ',') {
                if (lookingForElt) [[unlikely]] {
                    Execution::Throw (BadFormatException{L"JSON: Unexpected second ',' in reading array"sv});
                }
                else {
                    lookingForElt = true;
                }
                in.AdvanceOne ();
            }
            else if (iswspace (in.Peek ()->As<wchar_t> ())) {
                in.AdvanceOne ();
            }
            else {
                // not looking at whitespace, in midst of array, and array not terminated, so better be looking at a value
                if (lookingForElt) [[likely]] {
                    Containers::Support::ReserveTweaks::Reserve4Add1 (result);
                    result.push_back (Reader_value_ (in));
                    lookingForElt = false;
                }
                else {
                    Execution::Throw (BadFormatException{L"JSON: Unexpected character (missing ',' ?) in reading array"sv});
                }
            }
        }
    }

    VariantValue Reader_SpecialToken_ (wchar_t initialChar, MyBufferedStreamReader_& in)
    {
        Streams::SeekOffsetType savedPos = in.GetOffset ();
        switch (initialChar) {
            case 'f': {
                Character buf[4];
                if (in.ReadAll (begin (buf), end (buf)) == 4 and
                    buf[0] == 'a' and
                    buf[1] == 'l' and
                    buf[2] == 's' and
                    buf[3] == 'e') {
                    return VariantValue{false};
                }
            } break;
            case 't': {
                Character buf[3];
                if (in.ReadAll (begin (buf), end (buf)) == 3 and
                    buf[0] == 'r' and
                    buf[1] == 'u' and
                    buf[2] == 'e') {
                    return VariantValue{true};
                }
            } break;
            case 'n': {
                Character buf[3];
                if (in.ReadAll (begin (buf), end (buf)) == 3 and
                    buf[0] == 'u' and
                    buf[1] == 'l' and
                    buf[2] == 'l') {
                    return VariantValue{};
                }
            } break;
        }
        in.Seek (savedPos - 1); // because handed initial char, and seeked past it
        Execution::Throw (BadFormatException{L"JSON: Unrecognized token"sv});
    }

    VariantValue Reader_value_ (MyBufferedStreamReader_& in)
    {
        // Skip initial whitespace, and look for any value:
        //      string
        //      number
        //      object
        //      array
        //      true
        //      false
        //      null
        for (optional<Character> oc = in.Read (); oc; oc = in.Read ()) {
            switch (oc->As<wchar_t> ()) {
                case '\"':
                    in.Seek (Streams::Whence::eFromCurrent, -1);
                    return Reader_String_ (in);

                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case '-':
                    return Reader_Number_ (oc->As<wchar_t> (), in);

                case '{':
                    return Reader_Object_ (in);
                case '[':
                    return Reader_Array_ (in);

                case 't':
                case 'f':
                case 'n':
                    return Reader_SpecialToken_ (oc->As<wchar_t> (), in);

                default: {
                    if (iswspace (oc->As<wchar_t> ())) [[likely]] {
                        // ignore
                    }
                    else {
                        Execution::Throw (BadFormatException{L"JSON: Unexpected character looking for start of value"sv});
                    }
                }
            }
        }
        // if we get here - nothing found
        Execution::Throw (BadFormatException{L"JSON: Unexpected EOF looking for value"sv});
    }
}

/*
 ********************************************************************************
 ************************* Variant::JSON::Reader ********************************
 ********************************************************************************
 */
class Variant::JSON::Reader::Rep_ : public Variant::Reader::_IRep {
public:
    virtual _SharedPtrIRep Clone () const override
    {
        return make_shared<Rep_> (); // no instance data
    }
    virtual String GetDefaultFileSuffix () const override
    {
        static const String kResult_ = L".json"sv;
        return kResult_;
    }
    virtual VariantValue Read (const Streams::InputStream<byte>::Ptr& in) override
    {
        using namespace Streams;
        return Read (TextReader::New (in, SeekableFlag::eSeekable));
    }
    virtual VariantValue Read (const Streams::InputStream<Characters::Character>::Ptr& in) override
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"DataExchange::JSON::Reader::Rep_::Read"};
#endif
        Require (in.IsSeekable ());
        MyBufferedStreamReader_ reader{in};
        return Reader_value_ (reader);
    }
};
Variant::JSON::Reader::Reader ()
    : inherited{make_shared<Rep_> ()}
{
}
