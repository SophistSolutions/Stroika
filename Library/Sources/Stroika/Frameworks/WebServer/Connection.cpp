/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <cstdlib>

#include    "../../Foundation/Characters/String_Constant.h"
#include    "../../Foundation/Containers/Common.h"
#include    "../../Foundation/DataExchange/BadFormatException.h"
#include    "../../Foundation/Debug/Assertions.h"
#include    "../../Foundation/Execution/Exceptions.h"
#include    "../../Foundation/Memory/SmallStackBuffer.h"
#include    "../../Foundation/IO/Network/HTTP/MessageStartTextInputStreamBinaryAdapter.h"

#include    "Connection.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::WebServer;





/*
 ********************************************************************************
 ***************************** WebServer::Connection ****************************
 ********************************************************************************
 */
#if     qCompilerAndStdLib_copy_elision_Warning_too_aggressive_when_not_copyable_Buggy
DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wpessimizing-move\"");
#endif
Connection::Connection (Socket s)
    : fSocket_ (s)
    , fSocketStream_ (s)
    , fMessage_ {
    move (Request (fSocketStream_,  s.GetPeerAddress ())),
    move (Response (s, fSocketStream_, DataExchange::PredefinedInternetMediaType::OctetStream_CT ())),
    s.GetPeerAddress ()
}
{
}
#if     qCompilerAndStdLib_copy_elision_Warning_too_aggressive_when_not_copyable_Buggy
DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wpessimizing-move\"");
#endif

Connection::~Connection ()
{
    if (fMessage_.PeekResponse ()->GetState () != Response::State::eCompleted) {
        IgnoreExceptionsForCall (fMessage_.PeekResponse ()->Abort ());
    }
    Require (fMessage_.PeekResponse ()->GetState () == Response::State::eCompleted);
}

void    Connection::ReadHeaders ()
{
    // @todo - DONT use TextStream::ReadLine - because that asserts SEEKABLE - which may not be true (and probably isn't here anymore)
    // Instead - we need a special variant that looks for CRLF - which doesn't require backtracking...!!!

    Foundation::IO::Network::HTTP::MessageStartTextInputStreamBinaryAdapter inTextStream (fMessage_.PeekRequest ()->fInputStream);
    {
        // Read METHOD line
        String line = inTextStream.ReadLine ();
        Sequence<String>    tokens  { line.Tokenize (Set<Character> { ' ' }) };
        if (tokens.size () < 3) {
            Execution::Throw (Execution::StringException (String_Constant (L"Bad METHOD REQUEST HTTP line")));
        }
        fMessage_.PeekRequest ()->fMethod = tokens[0];
        if (tokens[1].empty ()) {
            // should check if GET/PUT/DELETE etc...
            Execution::Throw (Execution::StringException (String_Constant (L"Bad HTTP REQUEST line - missing host-relative URL")));
        }
        using   IO::Network::URL;
        fMessage_.PeekRequest ()->fURL = URL::Parse (tokens[1], URL::eAsRelativeURL);
        if (fMessage_.PeekRequest ()->fMethod.empty ()) {
            // should check if GET/PUT/DELETE etc...
            Execution::Throw (Execution::StringException (String_Constant (L"Bad METHOD in REQUEST HTTP line")));
        }
    }
    while (true) {
        String line = inTextStream.ReadLine ();
        if (line == String_Constant (L"\r\n") or line.empty ()) {
            return; // done
        }

        // add subsequent items to the header map
        size_t  i   =   line.find (':');
        if (i == string::npos) {
            Execution::Throw (Execution::StringException (String_Constant (L"Bad HTTP REQUEST missing colon in headers")));
        }
        else {
            String  hdr     =   line.SubString (0, i).Trim ();
            String  value   =   line.SubString (i + 1).Trim ();
            fMessage_.PeekRequest ()->fHeaders.Add (hdr, value);
        }
    }
}

void    Connection::Close ()
{
    fMessage_.PeekResponse ()->Flush ();
    fSocket_.Close ();
}

