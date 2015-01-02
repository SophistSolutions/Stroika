/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "Seekable.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;



/*
 ********************************************************************************
 ********************************* Streams::Seekable ****************************
 ********************************************************************************
 */
SeekOffsetType  Seekable::GetOffsetToEndOfStream () const
{
    SeekOffsetType  savedReadFrom = GetOffset ();
    SeekOffsetType  size =  Seek (Whence::eFromEnd, 0);
    Seek (Whence::eFromStart, savedReadFrom);
    Assert (size >= savedReadFrom);
    size -= savedReadFrom;
    return size;
}
