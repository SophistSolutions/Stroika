/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>

#include "../../Foundation/Characters/Format.h"
#include "../../Foundation/Characters/String_Constant.h"
#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/Containers/Common.h"
#include "../../Foundation/DataExchange/BadFormatException.h"
#include "../../Foundation/Debug/Assertions.h"
#include "../../Foundation/Execution/Exceptions.h"
#include "../../Foundation/IO/Network/HTTP/Headers.h"
#include "../../Foundation/IO/Network/HTTP/MessageStartTextInputStreamBinaryAdapter.h"
#include "../../Foundation/Memory/SmallStackBuffer.h"

#include "ClientErrorException.h"

#include "Connection.h"

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
 ***************************** WebServer::Connection ****************************
 ********************************************************************************
 */
#if qCompilerAndStdLib_copy_elision_Warning_too_aggressive_when_not_copyable_Buggy
DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wpessimizing-move\"");
#endif
Connection::Connection (const ConnectionOrientedSocket::Ptr& s, const InterceptorChain& interceptorChain)
    : fInterceptorChain_{interceptorChain}
    , fSocket_ (s)
    , fSocketStream_ (SocketStream::New (s))
    , fMessage_{
          move (Request (fSocketStream_)),
          move (Response (s, fSocketStream_, DataExchange::PredefinedInternetMediaType::OctetStream_CT ())),
          s.GetPeerAddress ()}
{
}
#if qCompilerAndStdLib_copy_elision_Warning_too_aggressive_when_not_copyable_Buggy
DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wpessimizing-move\"");
#endif

Connection::~Connection ()
{
    if (fMessage_.PeekResponse ()->GetState () != Response::State::eCompleted) {
        IgnoreExceptionsForCall (fMessage_.PeekResponse ()->Abort ());
    }
    Require (fMessage_.PeekResponse ()->GetState () == Response::State::eCompleted);
}

void Connection::ReadHeaders ()
{
    // @todo - DONT use TextStream::ReadLine - because that asserts SEEKABLE - which may not be true (and probably isn't here anymore)
    // Instead - we need a special variant that looks for CRLF - which doesn't require backtracking...!!!

    Foundation::IO::Network::HTTP::MessageStartTextInputStreamBinaryAdapter inTextStream (fMessage_.PeekRequest ()->GetInputStream ());
    {
        // Read METHOD line
        String           line = inTextStream.ReadLine ();
        Sequence<String> tokens{line.Tokenize (Set<Character>{' '})};
        if (tokens.size () < 3) {
            DbgTrace (L"tokens=%s, line='%s'", Characters::ToString (tokens).c_str (), line.c_str ());
            Execution::Throw (ClientErrorException (Characters::Format (L"Bad METHOD REQUEST HTTP line (%s)", line.c_str ())));
        }
        fMessage_.PeekRequest ()->SetHTTPMethod (tokens[0]);
        if (tokens[1].empty ()) {
            // should check if GET/PUT/DELETE etc...
            DbgTrace (L"tokens=%s, line='%s'", Characters::ToString (tokens).c_str (), line.c_str ());
            Execution::Throw (ClientErrorException (String_Constant (L"Bad HTTP REQUEST line - missing host-relative URL")));
        }
        using IO::Network::URL;
        fMessage_.PeekRequest ()->SetURL (URL::Parse (tokens[1], URL::eAsRelativeURL));
        if (fMessage_.PeekRequest ()->GetHTTPMethod ().empty ()) {
            // should check if GET/PUT/DELETE etc...
            DbgTrace (L"tokens=%s, line='%s'", Characters::ToString (tokens).c_str (), line.c_str ());
            Execution::Throw (ClientErrorException (String_Constant (L"Bad METHOD in REQUEST HTTP line")));
        }
    }
    while (true) {
        String line = inTextStream.ReadLine ();
        if (line == String_Constant (L"\r\n") or line.empty ()) {
            return; // done
        }

        // add subsequent items to the header map
        size_t i = line.find (':');
        if (i == string::npos) {
            DbgTrace (L"line=%s", Characters::ToString (line).c_str ());
            Execution::Throw (ClientErrorException (String_Constant (L"Bad HTTP REQUEST missing colon in headers")));
        }
        else {
            String hdr   = line.SubString (0, i).Trim ();
            String value = line.SubString (i + 1).Trim ();
            fMessage_.PeekRequest ()->AddHeader (hdr, value);
        }
    }
}

void Connection::Close ()
{
    fMessage_.PeekResponse ()->Flush ();
    fSocket_.Close ();
}

bool Connection::ReadAndProcessMessage ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"Connection::ReadAndProcessMessage");
#endif
    constexpr bool kSupportHTTPKeepAlives_{false}; // @todo - support - now structured so maybe not too hard -- LGP 2016-09-02
    // @todo but be sure REsponse::End () doesnt close socket - just flushes response!

    ReadHeaders (); // bad API. Must rethink...

    if (not kSupportHTTPKeepAlives_) {
        // From https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
        //      HTTP/1.1 applications that do not support persistent connections MUST include the "close" connection option in every message.
        GetResponse ().AddHeader (IO::Network::HTTP::HeaderName::kConnection, String_Constant{L"close"});
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    String url = GetRequest ().GetURL ().GetFullURL ();
    DbgTrace (L"Serving page %s", url.c_str ());
#endif
    try {
        fInterceptorChain_.HandleMessage (&fMessage_);
    }
    catch (...) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"Interceptor-Chain caught exception handling message: %s", Characters::ToString (current_exception ()).c_str ());
#endif
    }
    GetResponse ().End ();
    return kSupportHTTPKeepAlives_;
}
