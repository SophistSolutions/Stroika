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
Connection::Connection (Socket s)
    : fSocket_ (s)
    , fSocketStream_ (s)
    , fMessage_ {
    move (Request (fSocketStream_)),
    move (Response (s, fSocketStream_, DataExchange::PredefinedInternetMediaType::OctetStream_CT ())),
    s.GetPeerAddress ()
}
{
}

Connection::~Connection ()
{
    if (fMessage_.GetResponse ()->GetState () != Response::State::eCompleted) {
        IgnoreExceptionsForCall (fMessage_.GetResponse ()->Abort ());
    }
}

void    Connection::ReadHeaders ()
{
    // @todo - DONT use TextStream::ReadLine - because that asserts SEEKABLE - which may not be true (and probably isn't here anymore)
    // Instead - we need a special variant that looks for CRLF - which doesn't require backtracking...!!!

    Foundation::IO::Network::HTTP::MessageStartTextInputStreamBinaryAdapter inTextStream (fMessage_.GetRequest ()->fInputStream);
    {
        // Read METHOD line
        String line = inTextStream.ReadLine ();
        Sequence<String>    tokens  { line.Tokenize (Set<Character> { ' ' }) };
        if (tokens.size () < 3) {
            Execution::Throw (Execution::StringException (String_Constant (L"Bad METHOD REQUEST HTTP line")));
        }
        fMessage_.GetRequest ()->fMethod = tokens[0];
        if (tokens[1].empty ()) {
            // should check if GET/PUT/DELETE etc...
            Execution::Throw (Execution::StringException (String_Constant (L"Bad HTTP REQUEST line - missing host-relative URL")));
        }
        using   IO::Network::URL;
        fMessage_.GetRequest ()->fURL = URL::Parse (tokens[1], URL::eAsRelativeURL);
        if (fMessage_.GetRequest ()->fMethod.empty ()) {
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
            fMessage_.GetRequest ()->fHeaders.Add (hdr, value);
        }
    }
}

void    Connection::Close ()
{
    fMessage_.GetResponse ()->Flush ();
    fSocket_.Close ();
}

