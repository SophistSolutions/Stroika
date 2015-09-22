/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
        auto ci = fHeaders.find (IO::Network::HTTP::HeaderName::kContentLength);
        if (ci == fHeaders.end ()) {
            fBody_ = fInputStream.ReadAll ();
        }
        else {
            size_t contentLength = Characters::String2Int<size_t> (ci->second);
            Memory::SmallStackBuffer<Memory::Byte>  buf (contentLength);
            size_t  nRead = fInputStream.Read (buf.begin (), buf.end ());
            if (contentLength != nRead) {
                Execution::DoThrow (Execution::StringException (L"unexpeced wrong number of bytes returned in HTTP body"));
            }
            fBody_ = Memory::BLOB (buf.begin (), buf.end ());
        }
    }
    return *fBody_;
}
