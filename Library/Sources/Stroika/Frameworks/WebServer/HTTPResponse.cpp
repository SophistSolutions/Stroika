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
HTTPResponse::HTTPResponse (Streams::BinaryOutputStream& outStream, const InternetMediaType& ct)
	: fOutStream_ (outStream)
	, fAnyWritesDone_ (false)
	, fContentType_ (ct)
	, fBytes_ ()
{
}

void	HTTPResponse::SetContentType (const InternetMediaType& contentType)
{
	fContentType_ = contentType;
}

void	HTTPResponse::Flush ()
{
	// write BYTES to fOutStream
	if (not fBytes_.empty ()) {
		fOutStream_.Write (Containers::Start (fBytes_), Containers::End (fBytes_));
		fAnyWritesDone_ = true;
		fBytes_.clear ();
	}
}

void	HTTPResponse::Redirect (const wstring& url)
{
	if (fAnyWritesDone_) {
		Execution::DoThrow (Execution::StringException (L"Redirect after data flushed"));
	}
	fBytes_.clear ();
	writeln (L"HTTP/1.1 301 Moved Permanently\r\n");
	writeln (L"Server: SPECIFYSERVERFROMHEADERSREDCORD\r\n");
	writeln (L"Connection: close\r\n");
	printf (L"Location: %s\r\n", url.c_str ());
	writeln (L"\r\n");
	Flush ();
}
