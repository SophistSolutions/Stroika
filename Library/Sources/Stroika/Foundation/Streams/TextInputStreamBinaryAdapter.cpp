/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"



#include    "../Characters/CodePage.h"
#include    "../Containers/Common.h"
#include    "../Execution/OperationNotSupportedException.h"
#include    "../Memory/SmallStackBuffer.h"

#include    "TextInputStreamBinaryAdapter.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;


/*
 ********************************************************************************
 ************************** Streams::TextInputStream ****************************
 ********************************************************************************
 */
TextInputStreamBinaryAdapter::TextInputStreamBinaryAdapter (BinaryInputStream src)
    : fSource_ (src)
    , fOffset_ (0)
{
}

size_t  TextInputStreamBinaryAdapter::_Read (Character* intoStart, Character* intoEnd)
{
    Require ((intoStart == intoEnd) or (intoStart != nullptr));
    Require ((intoStart == intoEnd) or (intoEnd != nullptr));
#if 1
    if (fTmpHackTextRemaining_.empty ()) {
        // only happens once
        Assert (fOffset_ == 0);
        Memory::BLOB    b   =   fSource_.ReadAll ();
        fTmpHackTextRemaining_ = Characters::MapUNICODETextWithMaybeBOMTowstring ((char*) (b.begin ()), (char*) (b.end ()));
    }
    Character* ci = intoStart;
    for (; ci != intoEnd; ) {
        if (fOffset_ >= fTmpHackTextRemaining_.length ()) {
            return (ci - intoStart);
        }
        else {
            *ci = fTmpHackTextRemaining_[fOffset_];
            fOffset_++;
            ++ci;
        }
    }
    return (ci - intoStart);
#else
    Memory::SmallStackBuffer<Byte>  buf (intoEnd - intoStart);
    size_t  n   =   fSource_.Read (buf.begin (), buf.end ());
    size_t  outN    =   0;
    for (size_t i = 0; i < n; ++i) {
        intoStart[i] = Characters::Character ((char) * (buf.begin () + i));
        outN++;
    }
    Ensure (outN <= static_cast<size_t> (intoEnd - intoStart));
    return outN;
#endif
}

Streams::SeekOffsetType TextInputStreamBinaryAdapter::_GetOffset () const
{
    return fOffset_;
}

SeekOffsetType    TextInputStreamBinaryAdapter::_Seek (Streams::Whence whence, Streams::SignedSeekOffsetType offset)
{
    AssertNotImplemented ();    // easy todo with current hack impl, but trickier with a real one...
    return 0;
}
