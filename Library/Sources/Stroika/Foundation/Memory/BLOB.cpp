/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Character.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Cryptography/Encoding/Algorithm/Base64.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Streams/InputStream.h"

#include "BLOB.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Streams;

using Debug::AssertExternallySynchronizedMutex;
using Memory::BLOB;

#if qCompilerAndStdLib_specializeDeclarationRequiredSometimesToGenCode_Buggy
template <>
Characters::String Stroika::Foundation::Memory::BLOB::AsBase64 () const;
#endif

/*
 ********************************************************************************
 ************************* Memory::BLOB::BasicRep_ ******************************
 ********************************************************************************
 */
namespace {
    size_t len_ (const initializer_list<pair<const byte*, const byte*>>& startEndPairs)
    {
        size_t sz = 0;
        for (auto i : startEndPairs) {
            sz += (i.second - i.first);
        }
        return sz;
    }
    size_t len_ (const initializer_list<BLOB>& list2Concatenate)
    {
        size_t sz = 0;
        for (const auto& i : list2Concatenate) {
            sz += i.GetSize ();
        }
        return sz;
    }
}

BLOB::BasicRep_::BasicRep_ (const initializer_list<pair<const byte*, const byte*>>& startEndPairs)
    : fData{len_ (startEndPairs)}
{
    byte* pb = fData.begin ();
    for (auto i : startEndPairs) {
        auto itemSize{i.second - i.first};
        if (itemSize != 0) {
            (void)::memcpy (pb, i.first, itemSize);
            pb += itemSize;
        }
    }
    Ensure (pb == fData.end ());
}

BLOB::BasicRep_::BasicRep_ (const initializer_list<BLOB>& list2Concatenate)
    : fData{len_ (list2Concatenate)}
{
    byte* pb = fData.begin ();
    for (const auto& i : list2Concatenate) {
        auto itemSize{i.GetSize ()};
        if (itemSize != 0) {
            (void)::memcpy (pb, i.begin (), itemSize);
            pb += itemSize;
        }
    }
    Ensure (pb == fData.end ());
}

span<const byte> BLOB::BasicRep_::GetBounds () const
{
    Ensure (fData.begin () <= fData.end ());
#if qCompilerAndStdLib_span_requires_explicit_type_for_BLOBCVT_Buggy
    return span<const byte>{fData.begin (), fData.end ()};
#else
    return span{fData.begin (), fData.end ()};
#endif
}

/*
 ********************************************************************************
 ************************** Memory::BLOB::ZeroRep_ ******************************
 ********************************************************************************
 */
span<const byte> BLOB::ZeroRep_::GetBounds () const
{
    return span<const byte>{};
}

/*
 ********************************************************************************
 ************************* Memory::BLOB::AdoptRep_ ******************************
 ********************************************************************************
 */
BLOB::AdoptRep_::AdoptRep_ (const byte* start, const byte* end)
    : fStart{start}
    , fEnd{end}
{
    Require (start <= end);
}

span<const byte> BLOB::AdoptRep_::GetBounds () const
{
    Ensure (fStart <= fEnd);
    return span<const byte>{fStart, fEnd};
}

/*
 ********************************************************************************
 ******************* Memory::BLOB::AdoptAndDeleteRep_ ***************************
 ********************************************************************************
 */
BLOB::AdoptAndDeleteRep_::AdoptAndDeleteRep_ (const byte* start, const byte* end)
    : fStart{start}
    , fEnd{end}
{
    Require (start <= end);
}

BLOB::AdoptAndDeleteRep_::~AdoptAndDeleteRep_ ()
{
    delete[] fStart;
}

span<const byte> BLOB::AdoptAndDeleteRep_::GetBounds () const
{
    Ensure (fStart <= fEnd);
    return span<const byte>{fStart, fEnd};
}

/*
 ********************************************************************************
 ********************************* Memory::BLOB *********************************
 ********************************************************************************
 */
BLOB BLOB::FromHex (span<const char> s)
{
    auto HexChar2Num_ = [] (char c) -> byte {
        if ('0' <= c and c <= '9') [[likely]] {
            return byte (c - '0');
        }
        if ('A' <= c and c <= 'F') [[likely]] {
            return byte ((c - 'A') + 10);
        }
        if ('a' <= c and c <= 'f') [[likely]] {
            return byte ((c - 'a') + 10);
        }
        static const Execution::RuntimeErrorException kException_{"Invalid HEX character in BLOB::Hex"sv};
        Execution::Throw (kException_);
    };
    StackBuffer<byte> buf;
    const char*       e = s.data () + s.size ();
    for (const char* i = s.data (); i < e; ++i) {
        if (isspace (*i)) [[unlikely]] {
            continue;
        }
        byte b = HexChar2Num_ (*i);
        ++i;
        if (i == e) [[unlikely]] {
            static const Execution::RuntimeErrorException kException_{"Invalid partial HEX character in BLOB::Hex"sv};
            Execution::Throw (kException_);
        }
        b = byte (uint8_t (b << 4) + uint8_t (HexChar2Num_ (*i)));
        buf.push_back (b);
    }
    return BLOB{buf.begin (), buf.end ()};
}

BLOB BLOB::FromHex (const Characters::String& s)
{
    if (optional<span<const Characters::ASCII>> ps = s.PeekData<Characters::ASCII> ()) [[likely]] {
        return BLOB::FromHex (*ps);
    }
    return BLOB::FromHex (span<const char>{s.AsASCII ()}); // will throw in this case cuz if not ascii... oops...
}

BLOB BLOB::FromBase64 (span<const char> s)
{
    return Cryptography::Encoding::Algorithm::Base64::Decode (s);
}

BLOB BLOB::FromBase64 (const Characters::String& s)
{
    if (optional<span<const Characters::ASCII>> ps = s.PeekData<Characters::ASCII> ()) [[likely]] {
        return BLOB::FromBase64 (*ps);
    }
    return BLOB::FromBase64 (span<const char>{s.AsASCII ()}); // will throw in this case cuz if not ascii... oops...
}

namespace {
    using namespace Streams;
    struct BLOBBINSTREAM_ : InputStream::Ptr<byte> {
        BLOBBINSTREAM_ (const BLOB& b)
            : InputStream::Ptr<byte>{Memory::MakeSharedPtr<REP> (b)}
        {
        }
        struct REP : InputStream::IRep<byte>, public Memory::UseBlockAllocationIfAppropriate<REP> {
            bool                                                    fIsOpenForRead_{true};
            [[no_unique_address]] AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
            REP (const BLOB& b)
                : fCur{b.begin ()}
                , fStart{b.begin ()}
                , fEnd{b.end ()}
            {
            }
            virtual bool IsSeekable () const override
            {
                return true;
            }
            virtual void CloseRead () override
            {
                Require (IsOpenRead ());
                fIsOpenForRead_ = false;
            }
            virtual bool IsOpenRead () const override
            {
                return fIsOpenForRead_;
            }
            virtual optional<size_t> AvailableToRead () override
            {
                Require (IsOpenRead ());
                AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                return fEnd - fCur;
            }
            virtual optional<SeekOffsetType> RemainingLength () override
            {
                AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                return fEnd - fCur;
            }
            virtual optional<span<byte>> Read (span<byte> intoBuffer, [[maybe_unused]] NoDataAvailableHandling blockFlag) override
            {
                Require (not intoBuffer.empty ());
                AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                size_t                                          bytesToRead = intoBuffer.size ();
                size_t                                          bytesLeft   = fEnd - fCur;
                bytesToRead                                                 = min (bytesLeft, bytesToRead);
                if (bytesToRead != 0) {
                    // see http://stackoverflow.com/questions/16362925/can-i-pass-a-null-pointer-to-memcmp -- illegal to pass nullptr to memcmp() even if size 0 (aka for memcpy)
                    (void)::memcpy (intoBuffer.data (), fCur, bytesToRead);
                    fCur += bytesToRead;
                }
                return intoBuffer.subspan (0, bytesToRead);
            }
            virtual SeekOffsetType GetReadOffset () const override
            {
                Require (IsOpenRead ());
                AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                return fCur - fStart;
            }
            virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                static const auto                               kException_ = range_error{"seek"};
                AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                switch (whence) {
                    case Whence::eFromStart: {
                        if (offset < 0) [[unlikely]] {
                            Execution::Throw (kException_);
                        }
                        if (offset > (fEnd - fStart)) [[unlikely]] {
                            Execution::Throw (kException_);
                        }
                        fCur = fStart + offset;
                    } break;
                    case Whence::eFromCurrent: {
                        Streams::SeekOffsetType       curOffset = fCur - fStart;
                        Streams::SignedSeekOffsetType newOffset = curOffset + offset;
                        if (newOffset < 0) [[unlikely]] {
                            Execution::Throw (kException_);
                        }
                        if (newOffset > (fEnd - fStart)) [[unlikely]] {
                            Execution::Throw (kException_);
                        }
                        fCur = fStart + newOffset;
                    } break;
                    case Whence::eFromEnd: {
                        Streams::SignedSeekOffsetType newOffset = (fEnd - fStart) + offset;
                        if (newOffset < 0) [[unlikely]] {
                            Execution::Throw (kException_);
                        }
                        if (newOffset > (fEnd - fStart)) [[unlikely]] {
                            Execution::Throw (kException_);
                        }
                        fCur = fStart + newOffset;
                    } break;
                }
                Ensure ((fStart <= fCur) and (fCur <= fEnd));
                return GetReadOffset ();
            }
            const byte* fCur;
            const byte* fStart;
            const byte* fEnd;
        };
    };
}

template <>
Streams::InputStream::Ptr<byte> BLOB::As () const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    return BLOBBINSTREAM_{*this};
}

template <>
Characters::String Stroika::Foundation::Memory::BLOB::AsHex (size_t maxBytesToShow) const
{
    // @todo Could be more efficient
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    StringBuilder                                  sb;
    size_t                                         cnt{};
    for (byte b : *this) {
        if (cnt++ > maxBytesToShow) {
#if qCompilerAndStdLib_crash_compiling_break_in_forLoop_Buggy
            return sb.str ();
#else
            break;
#endif
        }
        sb << Characters::Format ("{:02x}"_f, static_cast<unsigned int> (b));
    }
    return sb;
}

template <>
Characters::String Stroika::Foundation::Memory::BLOB::AsBase64 () const
{
    return AsBase64 ({});
}
template <>
Characters::String Stroika::Foundation::Memory::BLOB::AsBase64 (const Cryptography::Encoding::Algorithm::Base64::Options& o) const
{
    return Cryptography::Encoding::Algorithm::Base64::Encode (*this, o);
}

BLOB BLOB::Repeat (unsigned int count) const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    if (count == 0) {
        return BLOB{};
    }
    else if (count <= 2) {
        BLOB tmp = *this;
        for (unsigned int i = 1; i < count; ++i) {
            tmp = tmp + *this;
        }
        return tmp;
    }
    else {
        // speed tweak
        span<const byte>   b2c = As<span<const byte>> ();
        InlineBuffer<byte> target{};
        target.reserve (count * b2c.size ());
        for (unsigned int i = 0; i < count; ++i) {
            target.push_back (b2c);
        }
        return BLOB{span{target}};
    }
}

BLOB BLOB::Slice (size_t startAt, size_t endAt) const
{
    Require (startAt <= endAt);
    Require (endAt < size ());
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    return BLOB{begin () + startAt, begin () + endAt};
}

String BLOB::ToString (size_t maxBytesToShow) const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    bool allBytesAscii = [this] () { return Character::IsASCII (Memory::SpanReInterpretCast<const char> (this->As<span<const byte>> ())); }();
    auto quoteAscii4Display = [] (const String& s) {
        return s.ReplaceAll ("\n"sv, "\\n"sv).ReplaceAll ("\r"sv, "\\r"sv); // todo more such
    };
    if (size () > maxBytesToShow) {
        if (allBytesAscii) {
            return Characters::Format ("[{} bytes: '{}' ...]"_f, size (), quoteAscii4Display (String{this->As<string> ()}));
        }
        else {
            String hexStr    = AsHex (maxBytesToShow + 1); // so we can replace/ellipsis with LimitLength ()
            size_t maxStrLen = maxBytesToShow < numeric_limits<size_t>::max () / 2 ? maxBytesToShow * 2 : maxBytesToShow;
            return Characters::Format ("[{} bytes: {}]"_f, size (), hexStr.LimitLength (maxStrLen));
        }
    }
    else {
        if (allBytesAscii) {
            return Characters::Format ("[{} bytes: '{}']"_f, size (), quoteAscii4Display (String{this->As<string> ()}));
        }
        else {
            return Characters::Format ("[{} bytes: {}]"_f, size (), AsHex ());
        }
    }
}
