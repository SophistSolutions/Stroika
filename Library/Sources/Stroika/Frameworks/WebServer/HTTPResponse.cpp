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
namespace	{
	// Based on looking at a handlful of typical file sizes...5k to 80k, but ave around
	// 25 and median abit above 32k. Small, not very representative sampling. And the more we use
	// subscripts (script src=x) this number could shrink.
	//
	// MAY want to switch to using SmallStackBuffer<Byte> - but before doing so, do some cleanups of its bugs and make sure
	// its optimized about how much it copies etc. Its really only tuned for POD-types (OK here but not necessarily good about reallocs).
	//
	//		-- LGP 2011-07-06
	const	size_t	kMinResponseBufferReserve_				=	32 * 1024;
	const	size_t	kResponseBufferReallocChunkSizeReserve_	=	16 * 1024;
}

HTTPResponse::HTTPResponse (const IO::Network::Socket& s,  Streams::BinaryOutputStream& outStream, const InternetMediaType& ct)
	: fSocket_ (s)
	, fOutStream_ (outStream)
	, fState_ (eInProgress)
	, fStatus_ (StatusCodes::kOK)
	, fHeaders_ ()
	, fContentType_ (ct)
	, fBytes_ ()
{
	AddHeader (L"Server", L"Stroka-Based-Web-Server");
}

void	HTTPResponse::SetContentType (const InternetMediaType& contentType)
{
	Require (fState_ == eInProgress);
	fContentType_ = contentType;
}

void	HTTPResponse::SetStatus (Status newStatus)
{
	Require (fState_ == eInProgress);
	fStatus_ = newStatus;
}

void	HTTPResponse::AddHeader (String headerName, String value)
{
	Require (fState_ == eInProgress);
	ClearHeader (headerName);
	fHeaders_.insert (map<String,String>::value_type (headerName, value));
}

void	HTTPResponse::ClearHeader ()
{
	Require (fState_ == eInProgress);
	fHeaders_.clear ();
}

void	HTTPResponse::ClearHeader (String headerName)
{
	Require (fState_ == eInProgress);
	map<String,String>::iterator i = fHeaders_.find (headerName);
	if (i != fHeaders_.end ()) {
		fHeaders_.erase (i);
	}
}

void	HTTPResponse::Flush ()
{
	if (fState_ == eInProgress) {
		{
			wstring	statusMsg;
			switch (fStatus_) {
				case StatusCodes::kOK:					statusMsg = L"OK"; break;
				case StatusCodes::kMovedPermanently:	statusMsg = L"Moved Permanently"; break;
			}
			wstring	version	=	L"1.1";
			wstring	tmp	=	Characters::Format (L"HTTP/%s %d %s\r\n", version.c_str (), fStatus_, statusMsg.c_str ());
			string	utf8	=	String (tmp).AsUTF8 ();
			fOutStream_.Write (reinterpret_cast<const Byte*> (Containers::Start (utf8)), reinterpret_cast<const Byte*> (Containers::End (utf8)));
		}

		for (map<String,String>::const_iterator i = fHeaders_.begin (); i != fHeaders_.end (); ++i) {
			wstring	tmp	=	Characters::Format (L"%s: %s\r\n", i->first.As<wstring> ().c_str (), i->second.As<wstring> ().c_str ());
			string	utf8	=	String (tmp).AsUTF8 ();
			fOutStream_.Write (reinterpret_cast<const Byte*> (Containers::Start (utf8)), reinterpret_cast<const Byte*> (Containers::End (utf8)));
		}

		const char	kCRLF[]	=	"\r\n";
		fOutStream_.Write (reinterpret_cast<const Byte*> (kCRLF), reinterpret_cast<const Byte*> (kCRLF + 2));
		fState_ = eInProgressHeaderState;
	}
	// write BYTES to fOutStream
	if (not fBytes_.empty ()) {
		Assert (not fState_ == eCompleted);	// We PREVENT any writes when completed
		fOutStream_.Write (Containers::Start (fBytes_), Containers::End (fBytes_));
		fBytes_.clear ();
	}
}

void	HTTPResponse::End ()
{
	Require ((fState_ == eInProgress) or (fState_ == eInProgressHeaderState));
	Flush ();
	fState_ = eCompleted;
}

void	HTTPResponse::Kill ()
{
	if (fState_ != eCompleted) {
		fState_ = eCompleted;
		fSocket_.Close ();
	}
}

void	HTTPResponse::Redirect (const wstring& url)
{
	Require (fState_ == eInProgress);
	fBytes_.clear ();

	// PERHAPS should clear some header values???
	AddHeader (L"Connection", L"close");	// needed for redirect
	AddHeader (L"Location", url);			// needed for redirect
	SetStatus (StatusCodes::kMovedPermanently);
	Flush ();
	fState_ = eCompleted;
}

void	HTTPResponse::write (const Byte* s, const Byte* e)
{
	Require ((fState_ == eInProgress) or (fState_ == eInProgressHeaderState));
	Require (s <= e);
	if (s < e) {
		Containers::ReserveSpeedTweekAddN (fBytes_, (e - s), kResponseBufferReallocChunkSizeReserve_);
		fBytes_.insert (fBytes_.end (), s, e);
	}
}

void	HTTPResponse::write (const wchar_t* s, const wchar_t* e)
{
	Require ((fState_ == eInProgress) or (fState_ == eInProgressHeaderState));
	Require (s <= e);
	if (s < e) {
		wstring tmp = wstring (s,e);
		string utf8 = String (tmp).AsUTF8 ();
		if (not utf8.empty ()) {
			fBytes_.insert (fBytes_.end (), reinterpret_cast<const Byte*> (utf8.c_str ()), reinterpret_cast<const Byte*> (utf8.c_str () + utf8.length ()));
		}
	}
}
