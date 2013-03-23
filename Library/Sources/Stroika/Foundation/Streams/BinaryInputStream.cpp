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
    // Inefficient implementation
    vector<Byte>    r;
    size_t          n;
    Byte            buf[10 * 1024];
    while ( (n = this->Read (std::begin (buf), std::end (buf))) != 0) {
        r.insert (r.end (), std::begin (buf), std::begin (buf) + n);
    }
    return Memory::BLOB (r);
}
