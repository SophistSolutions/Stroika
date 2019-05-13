/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
#include "../../Foundation/Execution/Throw.h"
#include "../../Foundation/IO/Network/HTTP/Headers.h"
#include "../../Foundation/IO/Network/HTTP/Versions.h"
#include "../../Foundation/Memory/SmallStackBuffer.h"
#include "../../Foundation/Streams/InputSubStream.h"
#include "../../Foundation/Streams/MemoryStream.h"

#include "Request.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Streams;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ************************* WebServer::Request ***********************************
 ********************************************************************************
 */
Request::Request (const Streams::InputStream<byte>::Ptr& inStream)
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
    auto                                               versionStringComparer = String::EqualsComparer{Characters::CompareOptions::eCaseInsensitive};
    if (versionOrVersionLabel == kLabeled_11_ or versionOrVersionLabel == IO::Network::HTTP::Versions::kOnePointOne or versionStringComparer (versionOrVersionLabel, kLabeled_11_)) {
        fHTTPVersion_ = IO::Network::HTTP::Versions::kOnePointOne;
    }
    else if (versionOrVersionLabel == kLabeled_10_ or versionOrVersionLabel == IO::Network::HTTP::Versions::kOnePointZero or versionStringComparer (versionOrVersionLabel, kLabeled_10_)) {
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
    if (not fBody_.has_value ()) {
        fBody_ = GetBodyStream ().ReadAll ();
    }
    return *fBody_;
}

optional<uint64_t> Request::GetContentLength () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    if (auto ci = fHeaders_.Lookup (IO::Network::HTTP::HeaderName::kContentLength)) {
        return Characters::String2Int<uint64_t> (*ci);
    }
    else {
        return nullopt;
    }
}

optional<InternetMediaType> Request::GetContentType () const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    if (auto ci = fHeaders_.Lookup (IO::Network::HTTP::HeaderName::kContentType)) {
        return InternetMediaType{*ci};
    }
    else {
        return nullopt;
    }
}

Streams::InputStream<byte>::Ptr Request::GetBodyStream ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"Request::GetBodyStream");
#endif
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    if (fBodyInputStream_ == nullptr) {
        /*
         *  According to https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html:
         *      The presence of a message-body in a request is signaled by the inclusion of
         *      a Content-Length or Transfer-Encoding header field in the request's message-headers
         */
        // if we have a content-length, read that many bytes.
        if (optional<uint64_t> contentLength = GetContentLength ()) {
            fBodyInputStream_ = InputSubStream<byte>::New (fInputStream_, {}, fInputStream_.GetOffset () + static_cast<size_t> (*contentLength));
        }
        else {
            /*
             *  @todo FIX - WRONG!!!! - MUST SUPPORT TRANSFER ENCODING IN THIS CASE OR NOTHING
             *
             *  https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html
             *      The rules for when a message-body is allowed in a message differ for requests and responses.
             *
             *      The presence of a message-body in a request is signaled by the inclusion of a Content-Length 
             *      or Transfer-Encoding header field in the request's message-headers/
             *
             *  For now - EMPTY is a better failure mode than reading everything and hanging...
             */
            fBodyInputStream_ = MemoryStream<byte>::New ();
        }
    }
    return fBodyInputStream_;
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
