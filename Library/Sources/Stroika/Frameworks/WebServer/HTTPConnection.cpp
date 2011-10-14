/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>
#include	<cstdlib>

#include	"../..//Foundation/Characters/Tokenize.h"
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
	, fResponse_ (fSocketStream_, InternetMediaType ())
{
}

void	HTTPConnection::ReadHeaders ()
{
	while (true) {
		wstring	line	=	fRequest_.fInputTextStream.ReadLine ();
		if (line == L"\r\n") {
			return;	// done
		}
		vector<wstring> tokens = Characters::Tokenize<wstring> (line, L" ");
		if (tokens.size () > 2 ) {
			fRequest_.fHostRelativeURL = tokens[1];
		}

		// We SHOULD add subsequent items to the header map!!!
	}
}

void	HTTPConnection::Close ()
{
	fResponse_.Flush ();
	fSocket_.Close ();
}

