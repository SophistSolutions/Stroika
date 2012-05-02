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
	, fDataType ()
{
}




/*
 ********************************************************************************
 ******************************* Transfer::Response *****************************
 ********************************************************************************
 */
Response::Response ()
	: fData ()
	, fDataType ()
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
	AssertNotImplemented ();
	Request	r;
	return SendAndRequest (r);
}

Response	Session::POST (const vector<Byte>& data, const InternetMediaType& contentType, const map<String,String>& extraHeaders)
{
	AssertNotImplemented ();
	Request	r;
	return SendAndRequest (r);
}

Response	Session::Delete (const map<String,String>& extraHeaders)
{
	AssertNotImplemented ();
	Request	r;
	return SendAndRequest (r);
}

Response	Session::Put (const map<String,String>& extraHeaders)
{
	AssertNotImplemented ();
	Request	r;
	return SendAndRequest (r);
}


