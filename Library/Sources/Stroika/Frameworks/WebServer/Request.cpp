/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <cstdlib>

#include    "../../Foundation/Characters/String2Int.h"
#include    "../../Foundation/Containers/Common.h"
#include    "../../Foundation/DataExchange/BadFormatException.h"
#include    "../../Foundation/Debug/Assertions.h"
#include    "../../Foundation/Execution/Exceptions.h"
#include    "../../Foundation/Memory/SmallStackBuffer.h"
#include    "../../Foundation/IO/Network/HTTP/Headers.h"

#include    "Request.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::WebServer;





/*
 ********************************************************************************
 ************************* WebServer::Request ***********************************
 ********************************************************************************
 */
Request::Request (const Streams::InputStream<Byte>& inStream)
    : fInputStream (inStream)
//    , fInputTextStream (fInputStream)
    , fHTTPVersion ()
    , fURL ()
    , fHeaders ()
{
}

Memory::BLOB    Request::GetBody ()
{
    if (fBody_.IsMissing ()) {
        // if we have a content-length, read that many bytes. otherwise, read til EOF
        if (auto ci = fHeaders.Lookup (IO::Network::HTTP::HeaderName::kContentLength)) {
            size_t contentLength = Characters::String2Int<size_t> (*ci);
            Memory::SmallStackBuffer<Memory::Byte>  buf (contentLength);
            if (contentLength != 0) {
                size_t  nRead = fInputStream.Read (buf.begin (), buf.end ());
                if (contentLength != nRead) {
                    Execution::Throw (Execution::StringException (L"unexpected wrong number of bytes returned in HTTP body"));
                }
            }
            fBody_ = Memory::BLOB (buf.begin (), buf.end ());
        }
        else {
            fBody_ = fInputStream.ReadAll ();
        }
    }
    return *fBody_;
}
