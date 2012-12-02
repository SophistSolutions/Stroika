/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Execution/CriticalSection.h"
#include    "../Streams/BinaryInputStream.h"

#include    "BLOB.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;





Memory::BLOB::BasicRep_::BasicRep_ (const Byte* start, const Byte* end)
    : fData (end - start)
{
    memcpy (fData.begin (), start, end - start);
}

pair<const Byte*, const Byte*>   Memory::BLOB::BasicRep_::GetBounds () const
{
    return pair<const Byte*, const Byte*> (fData.begin (), fData.end ());
}






pair<const Byte*, const Byte*>   Memory::BLOB::ZeroRep_::GetBounds () const
{
    return pair<const Byte*, const Byte*> (nullptr, nullptr);
}








int  Memory::BLOB::compare (const BLOB& rhs) const
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
    // @todo - add seekability to this helper!!!
    struct BLOBBINSTREAM_ : BinaryInputStream {
        BLOBBINSTREAM_ (const Memory::BLOB& b)
            : BinaryInputStream (_SharedIRep (DEBUG_NEW REP (b))) {
        }
        struct REP : BinaryInputStream::_IRep  {
            REP (const Memory::BLOB& b)
                : fCur (b.begin ())
                , fEnd (b.end ()) {
            }
            virtual size_t  Read (Byte* intoStart, Byte* intoEnd)  override {
                RequireNotNull (intoStart);
                RequireNotNull (intoEnd);
                Require (intoStart < intoEnd);
                size_t bytesToRead  =   intoEnd - intoStart;
                size_t bytesLeft    =   fEnd - fCur;
                bytesToRead = min (bytesLeft, bytesToRead);
                (void)::memcpy (intoStart, fCur, bytesToRead);
                fCur += bytesToRead;
                return bytesToRead;
            }
            const Byte* fCur;
            const Byte* fEnd;

            DECLARE_USE_BLOCK_ALLOCATION (REP);
        };
    };

}

template    <>
Streams::BinaryInputStream Memory::BLOB::As () const
{
    return BLOBBINSTREAM_ (*this);
}
