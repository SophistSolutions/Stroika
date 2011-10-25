/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>
#include	<cstdlib>

#include	"../../Foundation/Characters/Tokenize.h"
#include	"../../Foundation/Containers/Common.h"
#include	"../../Foundation/DataExchangeFormat/BadFormatException.h"
#include	"../../Foundation/Debug/Assertions.h"
#include	"../../Foundation/Execution/Exceptions.h"
#include	"../../Foundation/Memory/SmallStackBuffer.h"

#include	"HTTPConnection.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Containers;
using	namespace	Stroika::Foundation::Memory;

using	namespace	Stroika::Frameworks;
using	namespace	Stroika::Frameworks::WebServer;





/*
 ********************************************************************************
 ************************* WebServer::HTTPConnection ****************************
 ********************************************************************************
 */
HTTPConnection::HTTPConnection (Socket s)
	: fSocket_ (s)
	, fSocketStream_ (s)
	, fRequest_ (fSocketStream_)
	, fResponse_ (s, fSocketStream_, DataExchangeFormat::PredefinedInternetMediaType::OctetStream_CT ())
{
}

HTTPConnection::~HTTPConnection ()
{
	if (fResponse_.GetState () != HTTPResponse::eCompleted) {
		IgnoreExceptionsForCall (fResponse_.Abort ());
	}
}

void	HTTPConnection::ReadHeaders ()
{
	{
		// Read METHOD line
		wstring	line	=	fRequest_.fInputTextStream.ReadLine ();
		vector<wstring> tokens = Characters::Tokenize<wstring> (line, L" ");
		if (tokens.size () < 3) {
			Execution::DoThrow (Execution::StringException (L"Bad METHOD REQUEST HTTP line"));
		}
		fRequest_.fMethod = tokens[0];
		if (tokens[1].empty ()) {
			// should check if GET/PUT/DELETE etc...
			Execution::DoThrow (Execution::StringException (L"Bad HTTP REQUEST line - missing host-relative URL"));
		}
		fRequest_.fURL = IO::Network::URL::ParseHostRelativeURL (tokens[1]);
		if (fRequest_.fMethod.empty ()) {
			// should check if GET/PUT/DELETE etc...
			Execution::DoThrow (Execution::StringException (L"Bad METHOD in REQUEST HTTP line"));
		}
	}
	while (true) {
		wstring	line	=	fRequest_.fInputTextStream.ReadLine ();
		if (line == L"\r\n" or line.empty ()) {
			return;	// done
		}

		// add subsequent items to the header map
		size_t	i	=	line.find (':');
		if (i == string::npos) {
			Execution::DoThrow (Execution::StringException (L"Bad HTTP REQUEST missing colon in headers"));
		}
		else {
			String	hdr		=	String (line.substr (0, i)).Trim ();
			String	value	=	String (line.substr (i+1)).Trim ();
			fRequest_.fHeaders.insert (map<String,String>::value_type (hdr, value));
		}
	}
}

void	HTTPConnection::Close ()
{
	fResponse_.Flush ();
	fSocket_.Close ();
}

