/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>

#include "../../Foundation/Characters/Format.h"
#include "../../Foundation/Characters/String2Int.h"
#include "../../Foundation/Characters/StringBuilder.h"
#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/Containers/Common.h"
#include "../../Foundation/DataExchange/BadFormatException.h"
#include "../../Foundation/Debug/Assertions.h"
#include "../../Foundation/Debug/Trace.h"
#include "../../Foundation/Execution/Exceptions.h"
#include "../../Foundation/IO/Network/HTTP/Headers.h"
#include "../../Foundation/IO/Network/HTTP/Versions.h"
#include "../../Foundation/Memory/SmallStackBuffer.h"

#include "Request.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ************************* WebServer::Request ***********************************
 ********************************************************************************
 */
Request::Request (const Streams::InputStream<Byte>::Ptr& inStream)
    : fInputStream_ (inStream)
    , fHTTPVersion_ ()
    , fURL_ ()
    , fHeaders_ ()
{
}

void Request::SetHTTPVersion (const String& versionOrVersionLabel)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    static const String_Constant                       kLabeled_10_{L"HTTP/1.0"};
    static const String_Constant                       kLabeled_11_{L"HTTP/1.1"};
    if (versionOrVersionLabel == kLabeled_11_ or versionOrVersionLabel == IO::Network::HTTP::Versions::kOnePointOne or versionOrVersionLabel.Equals (kLabeled_11_, Characters::CompareOptions::eCaseInsensitive)) {
        fHTTPVersion_ = IO::Network::HTTP::Versions::kOnePointOne;
    }
    else if (versionOrVersionLabel == kLabeled_10_ or versionOrVersionLabel == IO::Network::HTTP::Versions::kOnePointZero or versionOrVersionLabel.Equals (kLabeled_10_, Characters::CompareOptions::eCaseInsensitive)) {
        fHTTPVersion_ = IO::Network::HTTP::Versions::kOnePointZero;
    }
    else if (versionOrVersionLabel.StartsWith (L"HTTP/", Characters::CompareOptions::eCaseInsensitive)) {
        fHTTPVersion_ = versionOrVersionLabel.SubString (5);
    }
    else {
        fHTTPVersion_ = versionOrVersionLabel;
    }
}

bool Request::GetKeepAliveRequested () const
{
    if (auto connectionHdr = this->fHeaders_.Lookup (IO::Network::HTTP::HeaderName::kConnection)) {
        return connectionHdr->Equals (L"Keep-Alive", Characters::CompareOptions::eCaseInsensitive);
    }
    // @todo convert version to number and compare that way
    return fHTTPVersion_ == IO::Network::HTTP::Versions::kOnePointOne;
}

Memory::BLOB Request::GetBody ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"Request::GetBody");
#endif
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    if (fBody_.IsMissing ()) {
        // if we have a content-length, read that many bytes. otherwise, read til EOF
        if (Optional<uint64_t> contentLength = GetContentLength ()) {
            // assumes content can fit in RAM
            Memory::SmallStackBuffer<Memory::Byte> buf (static_cast<size_t> (*contentLength));
            if (*contentLength != 0) {
                size_t n = fInputStream_.ReadAll (begin (buf), end (buf));
                Assert (n <= *contentLength);
                if (n != *contentLength) {
                    Execution::Throw (Execution::StringException (Characters::Format (L"Unexpected wrong number of bytes returned in HTTP body (found %d, but content-length %d)", n, *contentLength)));
                }
            }
            fBody_ = Memory::BLOB (begin (buf), end (buf));
        }
        else {
            /*
             *  @todo FIX - WRONG!!!! - MUST TO TRANSFER ENCODING IN THIS CASE OR NOTHING
             *
             *  https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html
             *      The rules for when a message-body is allowed in a message differ for requests and responses.
             *
             *      The presence of a message-body in a request is signaled by the inclusion of a Content-Length 
             *      or Transfer-Encoding header field in the request's message-headers/
             *
             *  For now - EMPTY is a better failure mode than reading eveythign and hanging...
             */
#if 1
            fBody_ = Memory::BLOB{};
#else
            fBody_ = fInputStream_.ReadAll ();
#endif
        }
    }
    return *fBody_;
}

Optional<uint64_t> Request::GetContentLength () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    if (auto ci = fHeaders_.Lookup (IO::Network::HTTP::HeaderName::kContentLength)) {
        return Characters::String2Int<uint64_t> (*ci);
    }
    else {
        return Optional<uint64_t>{};
    }
}

Memory::Optional<InternetMediaType> Request::GetContentType () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    if (auto ci = fHeaders_.Lookup (IO::Network::HTTP::HeaderName::kContentType)) {
        return InternetMediaType{*ci};
    }
    else {
        return Optional<InternetMediaType>{};
    }
}

String Request::ToString () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    StringBuilder                                       sb;
    sb += L"{";
    sb += L"HTTPVersion: " + fHTTPVersion_ + L", ";
    sb += L"Method: " + fMethod_ + L", ";
    sb += L"URL: '" + Characters::ToString (fURL_) + L"', ";
    sb += L"Headers: " + Characters::ToString (fHeaders_) + L", ";
    sb += L"}";
    return sb.str ();
}
