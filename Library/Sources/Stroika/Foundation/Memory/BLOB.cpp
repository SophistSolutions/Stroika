/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <mutex>

#include    "../Characters/Format.h"
#include    "../Characters/StringBuilder.h"
#include    "../Execution/Exceptions.h"
#include    "../Execution/StringException.h"
#include    "../Streams/InputStream.h"

#include    "BLOB.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Streams;


using   Memory::BLOB;



/*
 ********************************************************************************
 ************************* Memory::BLOB::BasicRep_ ******************************
 ********************************************************************************
 */
namespace {
    size_t  len_ (const initializer_list<pair<const Byte*, const Byte*>>& startEndPairs)
    {
        size_t  sz = 0;
        for (auto i : startEndPairs) {
            sz += (i.second - i.first);
        }
        return sz;
    }
    size_t  len_ (const initializer_list<BLOB>& list2Concatenate)
    {
        size_t  sz = 0;
        for (auto i : list2Concatenate) {
            sz += i.GetSize ();
        }
        return sz;
    }
}


BLOB::BasicRep_::BasicRep_ (const initializer_list<pair<const Byte*, const Byte*>>& startEndPairs)
    : fData { len_ (startEndPairs) }
{
    Byte*   pb  =   fData.begin ();
    for (auto i : startEndPairs) {
        (void)::memcpy (pb, i.first, i.second - i.first);
        pb += (i.second - i.first);
    }
    Ensure (pb == fData.end ());
}

BLOB::BasicRep_::BasicRep_ (const initializer_list<BLOB>& list2Concatenate)
    : fData { len_ (list2Concatenate) }
{
    Byte*   pb  =   fData.begin ();
    for (auto i : list2Concatenate) {
        (void)::memcpy (pb, i.begin (), i.GetSize ());
        pb += i.GetSize ();
    }
    Ensure (pb == fData.end ());
}

pair<const Byte*, const Byte*>   BLOB::BasicRep_::GetBounds () const
{
    Ensure (fData.begin () <= fData.end ());
    return pair<const Byte*, const Byte*> (fData.begin (), fData.end ());
}






/*
 ********************************************************************************
 ************************** Memory::BLOB::ZeroRep_ ******************************
 ********************************************************************************
 */
pair<const Byte*, const Byte*>   BLOB::ZeroRep_::GetBounds () const
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

pair<const Byte*, const Byte*>   BLOB::AdoptRep_::GetBounds () const
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

pair<const Byte*, const Byte*>   BLOB::AdoptAppLifetimeRep_::GetBounds () const
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
    unsigned int    HexChar2Num_ (char c)
    {
        if ('0' <= c and c <= '9') {
            return c - '0';
        }
        if ('A' <= c and c <= 'F') {
            return (c - 'A') + 10;
        }
        if ('a' <= c and c <= 'f') {
            return (c - 'a') + 10;
        }
        Execution::Throw (Execution::StringException (L"Invalid HEX character in BLOB::Hex"));
    }
}
BLOB    BLOB::Hex (const char* s, const char* e)
{
    SmallStackBuffer<Byte>  buf (0);
    for (const char* i = s; i < e; ++i) {
        if (isspace (*i)) {
            continue;
        }
        Byte    b   =   HexChar2Num_ (*i);
        ++i;
        if (i == e) {
            Execution::Throw (Execution::StringException (L"Invalid partial HEX character in BLOB::Hex"));
        }
        b = (b << 4) + HexChar2Num_ (*i);
        buf.push_back (b);
    }
    return BLOB (buf.begin (), buf.end ());
}

int  BLOB::Compare (const BLOB& rhs) const
{
    pair<const Byte*, const Byte*>   l =   fRep_->GetBounds ();
    pair<const Byte*, const Byte*>   r =   rhs.fRep_->GetBounds ();

    size_t  lSize = l.second - l.first;
    size_t  rSize = r.second - r.first;
    size_t  nCommonBytes = min (lSize, rSize);
    int tmp = ::memcmp (l.first, r.first, nCommonBytes);
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
    struct BLOBBINSTREAM_ : InputStream<Byte> {
        BLOBBINSTREAM_ (const BLOB& b)
            : InputStream<Byte> (make_shared<REP> (b))
        {
        }
        struct REP : InputStream<Byte>::_IRep  {
            REP (const BLOB& b)
                : fCur (b.begin ())
                , fStart (b.begin ())
                , fEnd (b.end ())
            {
            }
            virtual bool    IsSeekable () const override
            {
                return true;
            }
            virtual size_t  Read (Byte* intoStart, Byte* intoEnd) override
            {
                RequireNotNull (intoStart);
                RequireNotNull (intoEnd);
                Require (intoStart < intoEnd);
                size_t bytesToRead  =   intoEnd - intoStart;
                size_t bytesLeft    =   fEnd - fCur;
                bytesToRead = min (bytesLeft, bytesToRead);
                if (bytesToRead != 0) {
                    // see http://stackoverflow.com/questions/16362925/can-i-pass-a-null-pointer-to-memcmp -- illegal to pass nullptr to memcmp() even if size 0 (aka for memcpy)
                    (void)::memcpy (intoStart, fCur, bytesToRead);
                    fCur += bytesToRead;
                }
                return bytesToRead;
            }
            virtual Memory::Optional<size_t>  ReadSome (ElementType* intoStart, ElementType* intoEnd) override
            {
                // @todo - FIX TO REALLY CHECK
                return {};
#if 0
                if (intoStart == nullptr) {
                    return fEnd - fCur;
                }
                else {
                    return Read (intoStart, intoEnd);
                }
#endif
            }
            virtual SeekOffsetType  GetReadOffset () const override
            {
                return fCur - fStart;
            }
            virtual SeekOffsetType  SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                switch (whence) {
                    case    Whence::eFromStart: {
                            if (offset < 0) {
                                Execution::Throw (std::range_error ("seek"));
                            }
                            if (offset > (fEnd - fStart)) {
                                Execution::Throw (std::range_error ("seek"));
                            }
                            fCur = fStart + offset;
                        }
                        break;
                    case    Whence::eFromCurrent: {
                            Streams::SeekOffsetType         curOffset   =   fCur - fStart;
                            Streams::SignedSeekOffsetType   newOffset   =   curOffset + offset;
                            if (newOffset < 0) {
                                Execution::Throw (std::range_error ("seek"));
                            }
                            if (newOffset > (fEnd - fStart)) {
                                Execution::Throw (std::range_error ("seek"));
                            }
                            fCur = fStart + newOffset;
                        }
                        break;
                    case    Whence::eFromEnd: {
                            Streams::SeekOffsetType         curOffset   =   fCur - fStart;
                            Streams::SignedSeekOffsetType   newOffset   =   (fEnd - fStart) + offset;
                            if (newOffset < 0) {
                                Execution::Throw (std::range_error ("seek"));
                            }
                            if (newOffset > (fEnd - fStart)) {
                                Execution::Throw (std::range_error ("seek"));
                            }
                            fCur = fStart + newOffset;
                        }
                        break;
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

template    <>
Streams::InputStream<Byte> BLOB::As () const
{
    return BLOBBINSTREAM_ (*this);
}

Characters::String    BLOB::AsHex () const
{
    // @todo Could be more efficient
    StringBuilder   sb;
    for (Byte b : *this) {
        sb += Characters::Format (L"%02x", b);
    }
    return sb.str ();
}

BLOB    BLOB::Repeat (unsigned int count) const
{
    // @todo - re-implement using powers of 2 - so fewer concats (maybe - prealloc / reserve so only one - using vector)
    BLOB    tmp = *this;
    for (unsigned int i = 1; i < count; ++i) {
        tmp = tmp + *this;
    }
    return tmp;
}

BLOB    BLOB::Slice (size_t startAt, size_t endAt) const
{
    Require (startAt <= endAt);
    Require (endAt < size ());
    return BLOB (begin () + startAt, begin () + endAt);
}

String    BLOB::ToString () const
{
    // @todo Consider if we should 'LimitLength on the AsHex() string?
    return Characters::Format (L"[%d bytes: ", size ()) + AsHex () + L"]";
}
