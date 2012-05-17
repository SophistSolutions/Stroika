/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Execution/OperationNotSupportedException.h"

#include    "BinaryInputStreamFromIStreamAdapter.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;
using   namespace   Stroika::Foundation::Streams::iostream;






/*
 ********************************************************************************
 ********* Streams::iostream::BinaryInputStreamFromIStreamAdapter ***************
 ********************************************************************************
 */
BinaryInputStreamFromIStreamAdapter::BinaryInputStreamFromIStreamAdapter (std::istream& originalStream)
    : fOriginalStream_ (originalStream) {
}

size_t  BinaryInputStreamFromIStreamAdapter::_Read (Byte* intoStart, Byte* intoEnd) {
    RequireNotNull (intoStart);
    RequireNotNull (intoEnd);
    Require (intoStart < intoEnd);

    if (fOriginalStream_.eof ()) {
        return 0;
    }
    size_t  maxToRead   =   intoEnd - intoStart;
    fOriginalStream_.read (reinterpret_cast<char*> (intoStart), maxToRead);
    size_t  n   =    static_cast<size_t> (fOriginalStream_.gcount ());      // cast safe cuz amount asked to read was also size_t

    // apparently based on http://www.cplusplus.com/reference/iostream/istream/read/ EOF sets the EOF bit AND the fail bit
    if (not fOriginalStream_.eof () and fOriginalStream_.fail ()) {
        Execution::DoThrow (Execution::StringException (L"Failed to read from istream"));
    }
    return n;
}

Streams::SeekOffsetType BinaryInputStreamFromIStreamAdapter::_GetOffset () const override {
    // instead of tellg () - avoids issue with EOF where fail bit set???
    return fOriginalStream_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::in);
}

void    BinaryInputStreamFromIStreamAdapter::_Seek (Streams::Whence whence, Streams::SeekOffsetType offset) override {
    switch (whence) {
        case    FromStart_W:
            fOriginalStream_.seekg (offset, ios::beg);
            break;
        case    FromCurrent_W:
            fOriginalStream_.seekg (offset, ios::cur);
            break;
        case    FromEnd_W:
            fOriginalStream_.seekg (offset, ios::end);
            break;
    }
}
