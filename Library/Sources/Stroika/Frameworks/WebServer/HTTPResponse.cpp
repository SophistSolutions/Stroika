/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>
#include	<cstdlib>

#include	"../../Foundation/Characters/Format.h"
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
	, fHeaders_ ()
	, fAnyWritesDone_ (false)
	, fContentType_ (ct)
	, fBytes_ ()
{
}

void	HTTPResponse::SetContentType (const InternetMediaType& contentType)
{
	fContentType_ = contentType;
}

void	HTTPResponse::AddHeader (String headerName, String value)
{
	Require (not fAnyWritesDone_);
	fHeaders_.insert (map<String,String>::value_type (headerName, value));
}

void	HTTPResponse::ClearHeader ()
{
	fHeaders_.clear ();
}

void	HTTPResponse::ClearHeader (String headerName)
{
	map<String,String>::iterator i = fHeaders_.find (headerName);
	if (i != fHeaders_.end ()) {
		fHeaders_.erase (i);
	}
}

void	HTTPResponse::Flush ()
{
	if (!fAnyWritesDone_) {
		for (map<String,String>::const_iterator i = fHeaders_.begin (); i != fHeaders_.end (); ++i) {
			wstring	tmp	=	Characters::Format (L"%s: %s\r\n");
			string	utf8	=	String (tmp).AsUTF8 ();
			fOutStream_.Write (reinterpret_cast<const Byte*> (Containers::Start (utf8)), reinterpret_cast<const Byte*> (Containers::End (utf8)));
		}

		////TMPHJACK SO REDIRECT BELOW WORKS - REALLY MUST HAVE "HEADLINE" OR "STATUSLINE" fields - and use those to gen that first line of response...
		if (!fHeaders_.empty ()) {
			const char	kCRLF[]	=	"\r\n";
			fOutStream_.Write (reinterpret_cast<const Byte*> (kCRLF), reinterpret_cast<const Byte*> (kCRLF + 2));
		}
	}
	// write BYTES to fOutStream
	if (not fBytes_.empty ()) {
		fOutStream_.Write (Containers::Start (fBytes_), Containers::End (fBytes_));
		fBytes_.clear ();
	}
	fAnyWritesDone_ = true;
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
