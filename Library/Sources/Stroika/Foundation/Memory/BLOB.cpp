/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <mutex>

#include "../Characters/Format.h"
#include "../Characters/StringBuilder.h"
#include "../Execution/Exceptions.h"
#include "../Execution/StringException.h"
#include "../Streams/InputStream.h"

#include "BLOB.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Streams;

using Memory::BLOB;

/*
 ********************************************************************************
 ************************* Memory::BLOB::BasicRep_ ******************************
 ********************************************************************************
 */
namespace {
    size_t len_ (const initializer_list<pair<const Byte*, const Byte*>>& startEndPairs)
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
        for (auto i : list2Concatenate) {
            sz += i.GetSize ();
        }
        return sz;
    }
}

BLOB::BasicRep_::BasicRep_ (const initializer_list<pair<const Byte*, const Byte*>>& startEndPairs)
    : fData{len_ (startEndPairs)}
{
    Byte* pb = fData.begin ();
    for (auto i : startEndPairs) {
        (void)::memcpy (pb, i.first, i.second - i.first);
        pb += (i.second - i.first);
    }
    Ensure (pb == fData.end ());
}

BLOB::BasicRep_::BasicRep_ (const initializer_list<BLOB>& list2Concatenate)
    : fData{len_ (list2Concatenate)}
{
    Byte* pb = fData.begin ();
    for (auto i : list2Concatenate) {
        (void)::memcpy (pb, i.begin (), i.GetSize ());
        pb += i.GetSize ();
    }
    Ensure (pb == fData.end ());
}

pair<const Byte*, const Byte*> BLOB::BasicRep_::GetBounds () const
{
    Ensure (fData.begin () <= fData.end ());
    return pair<const Byte*, const Byte*> (fData.begin (), fData.end ());
}

/*
 ********************************************************************************
 ************************** Memory::BLOB::ZeroRep_ ******************************
 ********************************************************************************
 */
pair<const Byte*, const Byte*> BLOB::ZeroRep_::GetBounds () const
{
    return pair<const Byte*, const Byte*> (nullptr, nullptr);
}

/*
 ********************************************************************************
 ************************* Memory::BLOB::AdoptRep_ ******************************
 ********************************************************************************
 */
BLOB::AdoptRep_::AdoptRep_ (const Byte* start, const Byte* end)
    : fStart (start)
    , fEnd (end)
{
    Require (start <= end);
}

BLOB::AdoptRep_::~AdoptRep_ ()
{
    delete[] fStart;
}

pair<const Byte*, const Byte*> BLOB::AdoptRep_::GetBounds () const
{
    Ensure (fStart <= fEnd);
    return pair<const Byte*, const Byte*> (fStart, fEnd);
}

/*
 ********************************************************************************
 ******************* Memory::BLOB::AdoptAppLifetimeRep_ *************************
 ********************************************************************************
 */
BLOB::AdoptAppLifetimeRep_::AdoptAppLifetimeRep_ (const Byte* start, const Byte* end)
    : fStart (start)
    , fEnd (end)
{
    Require (start <= end);
}

pair<const Byte*, const Byte*> BLOB::AdoptAppLifetimeRep_::GetBounds () const
{
    Ensure (fStart <= fEnd);
    return pair<const Byte*, const Byte*> (fStart, fEnd);
}

/*
 ********************************************************************************
 ********************************* Memory::BLOB *********************************
 ********************************************************************************
 */
namespace {
    Byte HexChar2Num_ (char c)
    {
        if ('0' <= c and c <= '9') {
            return Byte (c - '0');
        }
        if ('A' <= c and c <= 'F') {
            return Byte ((c - 'A') + 10);
        }
        if ('a' <= c and c <= 'f') {
            return Byte ((c - 'a') + 10);
        }
        Execution::Throw (Execution::StringException (L"Invalid HEX character in BLOB::Hex"));
    }
}
BLOB BLOB::Hex (const char* s, const char* e)
{
    SmallStackBuffer<Byte> buf;
    for (const char* i = s; i < e; ++i) {
        if (isspace (*i)) {
            continue;
        }
        Byte b = HexChar2Num_ (*i);
        ++i;
        if (i == e) {
            Execution::Throw (Execution::StringException (L"Invalid partial HEX character in BLOB::Hex"));
        }
        b = Byte (uint8_t (b << 4) + uint8_t (HexChar2Num_ (*i)));
        buf.push_back (b);
    }
    return BLOB (buf.begin (), buf.end ());
}

int BLOB::Compare (const BLOB& rhs) const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    pair<const Byte*, const Byte*>                      l = fRep_->GetBounds ();
    pair<const Byte*, const Byte*>                      r = rhs.fRep_->GetBounds ();

    size_t lSize        = l.second - l.first;
    size_t rSize        = r.second - r.first;
    size_t nCommonBytes = min (lSize, rSize);
    int    tmp          = ::memcmp (l.first, r.first, nCommonBytes);
    if (tmp != 0) {
        return tmp;
    }
    // if tmp is zero, and same size - its really zero. But if lhs shorter than right, say lhs < right
    if (lSize == rSize) {
        return 0;
    }
    return (lSize < rSize) ? -1 : 1;
}

namespace {
    using namespace Streams;
    struct BLOBBINSTREAM_ : InputStream<Byte>::Ptr {
        BLOBBINSTREAM_ (const BLOB& b)
            : InputStream<Byte>::Ptr (make_shared<REP> (b))
        {
        }
        struct REP : InputStream<Byte>::_IRep, private Debug::AssertExternallySynchronizedLock {
            bool fIsOpenForRead_{true};
            REP (const BLOB& b)
                : fCur (b.begin ())
                , fStart (b.begin ())
                , fEnd (b.end ())
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
            virtual size_t Read (Byte* intoStart, Byte* intoEnd) override
            {
                RequireNotNull (intoStart);
                RequireNotNull (intoEnd);
                Require (intoStart < intoEnd);
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                size_t                                             bytesToRead = intoEnd - intoStart;
                size_t                                             bytesLeft   = fEnd - fCur;
                bytesToRead                                                    = min (bytesLeft, bytesToRead);
                if (bytesToRead != 0) {
                    // see http://stackoverflow.com/questions/16362925/can-i-pass-a-null-pointer-to-memcmp -- illegal to pass nullptr to memcmp() even if size 0 (aka for memcpy)
                    (void)::memcpy (intoStart, fCur, bytesToRead);
                    fCur += bytesToRead;
                }
                return bytesToRead;
            }
            virtual optional<size_t> ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) override
            {
                Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                return _ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (intoStart, intoEnd, fEnd - fCur);
            }
            virtual SeekOffsetType GetReadOffset () const override
            {
                Require (IsOpenRead ());
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                return fCur - fStart;
            }
            virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                Require (IsOpenRead ());
                switch (whence) {
                    case Whence::eFromStart: {
                        if (offset < 0) {
                            Execution::Throw (range_error ("seek"));
                        }
                        if (offset > (fEnd - fStart)) {
                            Execution::Throw (range_error ("seek"));
                        }
                        fCur = fStart + offset;
                    } break;
                    case Whence::eFromCurrent: {
                        Streams::SeekOffsetType       curOffset = fCur - fStart;
                        Streams::SignedSeekOffsetType newOffset = curOffset + offset;
                        if (newOffset < 0) {
                            Execution::Throw (range_error ("seek"));
                        }
                        if (newOffset > (fEnd - fStart)) {
                            Execution::Throw (range_error ("seek"));
                        }
                        fCur = fStart + newOffset;
                    } break;
                    case Whence::eFromEnd: {
                        Streams::SignedSeekOffsetType newOffset = (fEnd - fStart) + offset;
                        if (newOffset < 0) {
                            Execution::Throw (range_error ("seek"));
                        }
                        if (newOffset > (fEnd - fStart)) {
                            Execution::Throw (range_error ("seek"));
                        }
                        fCur = fStart + newOffset;
                    } break;
                }
                Ensure ((fStart <= fCur) and (fCur <= fEnd));
                return GetReadOffset ();
            }
            const Byte* fCur;
            const Byte* fStart;
            const Byte* fEnd;
        };
    };
}

template <>
Streams::InputStream<Byte>::Ptr BLOB::As () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    return BLOBBINSTREAM_{*this};
}

Characters::String BLOB::AsHex (size_t maxBytesToShow) const
{
    // @todo Could be more efficient
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    StringBuilder                                       sb;
    size_t                                              cnt{};
    for (Byte b : *this) {
        if (cnt++ > maxBytesToShow) {
            break;
        }
        sb += Characters::Format (L"%02x", b);
    }
    return sb.str ();
}

BLOB BLOB::Repeat (unsigned int count) const
{
    // @todo - re-implement using powers of 2 - so fewer concats (maybe - prealloc / reserve so only one - using vector)
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    BLOB                                                tmp = *this;
    for (unsigned int i = 1; i < count; ++i) {
        tmp = tmp + *this;
    }
    return tmp;
}

BLOB BLOB::Slice (size_t startAt, size_t endAt) const
{
    Require (startAt <= endAt);
    Require (endAt < size ());
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    return BLOB (begin () + startAt, begin () + endAt);
}

String BLOB::ToString (size_t maxBytesToShow) const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    if (size () > maxBytesToShow) {
        String hexStr    = AsHex (maxBytesToShow + 1); // so we can replace/elispis with LimitLength ()
        size_t maxStrLen = maxBytesToShow < numeric_limits<size_t>::max () / 2 ? maxBytesToShow * 2 : maxBytesToShow;
        return Characters::Format (L"[%d bytes: ", size ()) + hexStr.LimitLength (maxStrLen) + L"]";
    }
    else {
        return Characters::Format (L"[%d bytes: ", size ()) + AsHex () + L"]";
    }
}
