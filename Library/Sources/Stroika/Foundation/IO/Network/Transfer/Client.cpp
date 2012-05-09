/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../../StroikaPreComp.h"

#if	qPlatform_Windows
	#include	<atlbase.h>

	#include	<Windows.h>
	#include	<URLMon.h>
#endif

#include	"../../../Characters/Format.h"
#include	"../../../Execution/Exceptions.h"

#include	"Client.h"

using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;
using	namespace	Stroika::Foundation::IO;
using	namespace	Stroika::Foundation::IO::Network;
using	namespace	Stroika::Foundation::IO::Network::Transfer;






/*
 ********************************************************************************
 ******************************* Transfer::Fetch ********************************
 ********************************************************************************
 */
vector<Byte>	Transfer::Fetch (const wstring& url)
{
	return vector<Byte> ();	//tmphack
}






/*
 ********************************************************************************
 ******************************* Transfer::Request *****************************
 ********************************************************************************
 */
Request::Request ()
	: fMethod ()
	, fOverrideHeaders ()
	, fData ()
	, fContentType ()
{
}





/*
 ********************************************************************************
 ******************************* Transfer::Response *****************************
 ********************************************************************************
 */
Response::Response ()
	: fData ()
	, fHeaders ()
	, fStatus ()
{
}






/*
 ********************************************************************************
 ******************************* Transfer::Session ******************************
 ********************************************************************************
 */
Response	Session::Get (const map<String,String>& extraHeaders)
{
	///// TODO:  ADD HTTP::METHODS module (like with Headers) to enumerate common/basic METHODS GET/POST/ETC...
	Request	r;
	r.fMethod = L"GET";
	r.fOverrideHeaders = extraHeaders;
	return SendAndRequest (r);
}

Response	Session::Post (const vector<Byte>& data, const InternetMediaType& contentType, const map<String,String>& extraHeaders)
{
	///// TODO:  ADD HTTP::METHODS module (like with Headers) to enumerate common/basic METHODS GET/POST/ETC...
	Request	r;
	r.fMethod = L"POST";
	r.fOverrideHeaders = extraHeaders;
	r.fData = data;
	r.fContentType = contentType;
	return SendAndRequest (r);
}

Response	Session::Delete (const map<String,String>& extraHeaders)
{
	///// TODO:  ADD HTTP::METHODS module (like with Headers) to enumerate common/basic METHODS GET/POST/ETC...
	Request	r;
	r.fMethod = L"DELETE";
	r.fOverrideHeaders = extraHeaders;
	return SendAndRequest (r);
}

Response	Session::Put (const vector<Byte>& data, const InternetMediaType& contentType, const map<String,String>& extraHeaders)
{
	///// TODO:  ADD HTTP::METHODS module (like with Headers) to enumerate common/basic METHODS GET/POST/ETC...
	Request	r;
	r.fMethod = L"POST";
	r.fOverrideHeaders = extraHeaders;
	r.fData = data;
	r.fContentType = contentType;
	return SendAndRequest (r);
}


