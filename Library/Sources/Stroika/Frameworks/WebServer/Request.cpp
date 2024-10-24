/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/HTTP/Versions.h"
#include "Stroika/Foundation/Streams/InputSubStream.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"

#include "Request.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Streams;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

using Debug::AssertExternallySynchronizedMutex;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 *************************** WebServer::Request *********************************
 ********************************************************************************
 */
Request::Request (Request&& src)
    : Request{src.fInputStream_}
{
    fBodyInputStream_ = move (src.fBodyInputStream_);
    fBody_            = move (src.fBody_);
}

Request::Request (const Streams::InputStream::Ptr<byte>& inStream)
    : keepAliveRequested{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
        const Request* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Request::keepAliveRequested);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->_fThisAssertExternallySynchronized};
        using ConnectionValue = IO::Network::HTTP::Headers::ConnectionValue;
        if (thisObj->httpVersion == IO::Network::HTTP::Versions::kOnePointZero) {
            return thisObj->headers ().connection ().value_or (ConnectionValue::eClose) == ConnectionValue::eKeepAlive;
        }
        if (thisObj->httpVersion == IO::Network::HTTP::Versions::kOnePointOne) {
            return thisObj->headers ().connection ().value_or (ConnectionValue::eKeepAlive) == ConnectionValue::eKeepAlive;
        }
        return true; // for HTTP 2.0 and later, keep alive is always assumed (double check/reference?)
    }}
    , fInputStream_{inStream}
{
}

Memory::BLOB Request::GetBody ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Request::GetBody"};
#endif
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
    if (not fBody_.has_value ()) {
        fBody_ = GetBodyStream ().ReadAll ();
    }
    return *fBody_;
}

DataExchange::VariantValue Request::GetBodyVariantValue ()
{
    if (auto oct = contentType ()) {
        if (DataExchange::InternetMediaTypeRegistry::sThe->IsA (DataExchange::InternetMediaTypes::kJSON, *oct)) {
            return DataExchange::Variant::JSON::Reader{}.Read (GetBody ());
        }
    }
    static const auto kExcept_ = Execution::RuntimeErrorException{"Unrecognized content type"sv};
    Execution::Throw (kExcept_);
}

Streams::InputStream::Ptr<byte> Request::GetBodyStream ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Request::GetBodyStream"};
#endif
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
    if (fBodyInputStream_ == nullptr) {
        /*
         *  According to https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html:
         *      The presence of a message-body in a request is signaled by the inclusion of
         *      a Content-Length or Transfer-Encoding header field in the request's message-headers
         */
        // if we have a content-length, read that many bytes.
        if (optional<uint64_t> cl = headers ().contentLength ()) {
            fBodyInputStream_ = InputSubStream::New<byte> (fInputStream_, {}, fInputStream_.GetOffset () + static_cast<size_t> (*cl));
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
            fBodyInputStream_ = MemoryStream::New<byte> ();
        }
        // @todo See http://stroika-bugs.sophists.com/browse/STK-758 - zip compression support
    }
    return fBodyInputStream_;
}

String Request::ToString () const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{_fThisAssertExternallySynchronized};
    StringBuilder                                  sb = inherited::ToString ().SubString (0, -1); // strip trialing '{'
    // @todo add stuff about body
    sb << "}"sv;
    return sb;
}
