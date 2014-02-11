/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <cstdlib>

#include    "../../Foundation/Characters/String_Constant.h"
#include    "../../Foundation/Characters/Tokenize.h"
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
    , fRequest_ (fSocketStream_)
    , fResponse_ (s, fSocketStream_, DataExchange::PredefinedInternetMediaType::OctetStream_CT ())
{
}

Connection::~Connection ()
{
    if (fResponse_.GetState () != Response::State::eCompleted) {
        IgnoreExceptionsForCall (fResponse_.Abort ());
    }
}

void    Connection::ReadHeaders ()
{
    // @todo - DONT use TextStream::ReadLine - because that asserts SEEKABLE - which may not be true (and probably isn't here anymore)
    // Instead - we need a special variant that looks for CRLF - which doesn't require backtracking...!!!

    Foundation::IO::Network::HTTP::MessageStartTextInputStreamBinaryAdapter inTextStream (fRequest_.fInputStream);
    {
        // Read METHOD line
        String line = inTextStream.ReadLine ();
        vector<String> tokens = Characters::Tokenize<String> (line, L" ");
        if (tokens.size () < 3) {
            Execution::DoThrow (Execution::StringException (String_Constant (L"Bad METHOD REQUEST HTTP line")));
        }
        fRequest_.fMethod = tokens[0];
        if (tokens[1].empty ()) {
            // should check if GET/PUT/DELETE etc...
            Execution::DoThrow (Execution::StringException (String_Constant (L"Bad HTTP REQUEST line - missing host-relative URL")));
        }
        fRequest_.fURL = IO::Network::URL::ParseHostRelativeURL (tokens[1]);
        if (fRequest_.fMethod.empty ()) {
            // should check if GET/PUT/DELETE etc...
            Execution::DoThrow (Execution::StringException (String_Constant (L"Bad METHOD in REQUEST HTTP line")));
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
            Execution::DoThrow (Execution::StringException (String_Constant (L"Bad HTTP REQUEST missing colon in headers")));
        }
        else {
            String  hdr     =   line.SubString (0, i).Trim ();
            String  value   =   line.SubString (i + 1).Trim ();
            fRequest_.fHeaders.insert (map<String, String>::value_type (hdr, value));
        }
    }
}

void    Connection::Close ()
{
    fResponse_.Flush ();
    fSocket_.Close ();
}

