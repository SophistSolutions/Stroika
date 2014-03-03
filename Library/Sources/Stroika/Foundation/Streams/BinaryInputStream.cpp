/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Execution/Exceptions.h"
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
        SeekOffsetType  size = GetOffsetToEndOfStream ();
        if (size >= numeric_limits<size_t>::max ()) {
            Execution::DoThrow<bad_alloc> (bad_alloc ());
        }
        size_t sb = static_cast<size_t> (size);
        if (sb == 0) {
            return Memory::BLOB ();
        }
        Byte* b = new Byte[sb];   // if this fails, we had no way to create the BLOB
        size_t n = Read (b, b + sb);
        Assert (n <= sb);
        return Memory::BLOB::Attach (b, b + n);
    }
    else {
        // Less efficient implementation
        vector<Byte>    r;
        size_t          n;
        Byte            buf[32 * 1024];
        while ( (n = Read (std::begin (buf), std::end (buf))) != 0) {
            r.insert (r.end (), std::begin (buf), std::begin (buf) + n);
        }
        return Memory::BLOB (r);
    }
}
