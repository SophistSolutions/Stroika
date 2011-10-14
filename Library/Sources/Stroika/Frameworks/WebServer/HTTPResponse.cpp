/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>
#include	<cstdlib>

#include	"../../Foundation/Containers/Common.h"
#include	"../../Foundation/DataExchangeFormat/BadFormatException.h"
#include	"../../Foundation/Debug/Assertions.h"
#include	"../../Foundation/Execution/Exceptions.h"
#include	"../../Foundation/Memory/SmallStackBuffer.h"

#include	"HTTPResponse.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Containers;
using	namespace	Stroika::Foundation::Memory;

using	namespace	Stroika::Frameworks;
using	namespace	Stroika::Frameworks::WebServer;







/*
 ********************************************************************************
 ************************ WebServer::HTTPResponse *******************************
 ********************************************************************************
 */
HTTPResponse::HTTPResponse (const InternetMediaType& ct)
	: fContentType_ (ct)
	,_fBytes ()
{
}

void	HTTPResponse::Redirect (const wstring& url)
{
	_fBytes.clear ();
	writeln (L"HTTP/1.1 301 Moved Permanently\r\n");
	writeln (L"Server: SPECIFYSERVERFROMHEADERSREDCORD\r\n");
	writeln (L"Connection: close\r\n");
	printf (L"Location: %s\r\n", url.c_str ());
	writeln (L"\r\n");
}
