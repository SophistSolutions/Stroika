/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Memory/BLOB.h"

#include    "BinaryInputStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;





/*
 ********************************************************************************
 ************************ Streams::BinaryInputStream ****************************
 ********************************************************************************
 */
Memory::BLOB BinaryInputStream::ReadAll () const
{
    if (IsSeekable ()) {
        size_t  savedReadFrom = this->GetOffset ();
        size_t  size =  this->Seek (Whence::eFromEnd, 0);
        Seek (Whence::eFromStart, savedReadFrom);
        Assert (size >= savedReadFrom);
        size -= savedReadFrom;
        Byte* b = new Byte[size];   // if this fails, we had no way to create the BLOB
        size_t n = this->Read (b, b + size);
        Assert (n <= size);
        return Memory::BLOB (b, b + n, Memory::BLOB::AdoptFlag::eAdopt);
    }
    else {
        // Less efficient implementation
        vector<Byte>    r;
        size_t          n;
        Byte            buf[32 * 1024];
        while ( (n = this->Read (std::begin (buf), std::end (buf))) != 0) {
            r.insert (r.end (), std::begin (buf), std::begin (buf) + n);
        }
        return Memory::BLOB (r);
    }
}
