/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Execution/OperationNotSupportedException.h"

#include    "TextInputStreamFromIStreamAdapter.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;
using   namespace   Stroika::Foundation::Streams::iostream;



// TODO: HORRIBLE IMPL - DO LIKE BinaryInputStreamFromIStreamAdapter - but wait until
// we do refactoring of TextInputStream
//      -- LGP 2012-12-02




/*
 ********************************************************************************
 ********* Streams::iostream::BinaryInputStreamFromIStreamAdapter ***************
 ********************************************************************************
 */

TextInputStreamFromIStreamAdapter::TextInputStreamFromIStreamAdapter (std::wistream& originalStream)
    : fOriginalStream_ (originalStream)
{
}

size_t          TextInputStreamFromIStreamAdapter::_Read (Character* intoStart, Character* intoEnd)
{
    RequireNotNull (intoStart);
    RequireNotNull (intoEnd);
    Require (intoStart < intoEnd);

    if (fOriginalStream_.eof ()) {
        return 0;
    }
    size_t  maxToRead   =   intoEnd - intoStart;
    fOriginalStream_.read (reinterpret_cast<wchar_t*> (intoStart), maxToRead);
    size_t  n   =    static_cast<size_t> (fOriginalStream_.gcount ());      // cast safe cuz amount asked to read was also size_t

    // apparently based on http://www.cplusplus.com/reference/iostream/istream/read/ EOF sets the EOF bit AND the fail bit
    if (not fOriginalStream_.eof () and fOriginalStream_.fail ()) {
        Execution::DoThrow (Execution::StringException (L"Failed to read from wistream"));
    }
    return n;
}
