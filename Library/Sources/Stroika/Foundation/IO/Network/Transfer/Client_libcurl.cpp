/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../../StroikaPreComp.h"

#if		qHasFeature_libcurl
#include	<curl/curl.h>
#endif

#include	"../../../Characters/Format.h"
#include	"../../../Execution/Exceptions.h"

#include	"Client_libcurl.h"

using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;
using	namespace	Stroika::Foundation::IO;
using	namespace	Stroika::Foundation::IO::Network;
using	namespace	Stroika::Foundation::IO::Network::Transfer;





#if		qHasFeature_libcurl
namespace	{
	struct	ModuleInit_ {
		ModuleInit_ ()
		{
			curl_global_init (CURL_GLOBAL_ALL);
		}
	};
}
#endif




#if		qHasFeature_libcurl
namespace	{
	wstring	mkExceptMsg_ (LibCurlException::CURLcode ccode)
		{
			return String::FromUTF8 (curl_easy_strerror (static_cast<::CURLcode> (ccode))).As<wstring> ();
		}
}

/*
 ********************************************************************************
 ************************ Transfer::LibCurlException ****************************
 ********************************************************************************
 */
LibCurlException::LibCurlException (CURLcode ccode)
	: StringException (mkExceptMsg_ (ccode))
	, fCurlCode_ (ccode)
{
}

void	LibCurlException::DoThrowIfError (CURLcode status)
{
	if (status != 0) {
		Execution::DoThrow (LibCurlException (status));
	}
}
#endif








#if		qHasFeature_libcurl
/*
 ********************************************************************************
 ********************* Transfer::IConnection_LibCurl ****************************
 ********************************************************************************
 */
IConnection_LibCurl::IConnection_LibCurl ()
	: fCurlHandle_ (curl_easy_init ())
	, fCURLCache_URL_ ()
	, fResponseData_ ()
	, fSavedHeaders_ (nullptr)
{
}

IConnection_LibCurl::~IConnection_LibCurl ()
{
	if (fCurlHandle_ != nullptr) {
		curl_easy_cleanup (fCurlHandle_);
	}
	if (fSavedHeaders_ != nullptr) {
		curl_slist_free_all (fSavedHeaders_);
		fSavedHeaders_ = nullptr;
	}
}

URL		IConnection_LibCurl::GetURL () const override
{
	// needs work... - not sure this is safe - may need to cache orig... instead of reparsing...
	return URL (String::FromUTF8 (fCURLCache_URL_).As<wstring> ());
}

void	IConnection_LibCurl::SetURL (const URL& url) override
{
	MakeHandleIfNeeded_ ();
	fCURLCache_URL_ = String (url.GetURL ()).AsUTF8 ();
	LibCurlException::DoThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_URL, fCURLCache_URL_.c_str ()));
}

void	IConnection_LibCurl::Close ()	override
{
	if (fCurlHandle_ != nullptr) {
		LibCurlException::DoThrowIfError (::curl_easy_cleanup (fCurlHandle_));
		fCurlHandle_ = nullptr;
	}
}

size_t	IConnection_LibCurl::ResponseWriteHandler_ (void* ptr, size_t size, size_t nmemb, void* userP)
{
	return reinterpret_cast<IConnection_LibCurl*> (userP)->ResponseWriteHandler_ (reinterpret_cast<const Byte*> (ptr), size * nmemb);
}

size_t	IConnection_LibCurl::ResponseWriteHandler_ (const Byte* ptr, size_t nBytes)
{
	fResponseData_.insert (ptr, ptr + nBytes);
	return nBytes;
}

size_t	IConnection_LibCurl::ResponseHeaderWriteHandler_ (void* ptr, size_t size, size_t nmemb, void* userP)
{
	return reinterpret_cast<IConnection_LibCurl*> (userP)->ResponseHeaderWriteHandler_ (reinterpret_cast<const Byte*> (ptr), size * nmemb);
}

size_t	IConnection_LibCurl::ResponseHeaderWriteHandler_ (const Byte* ptr, size_t nBytes)
{
	string tmp (reinterpret_cast<const char*> (ptr), nBytes);
	string::size_type i = tmp.find (':');
	String	from;
	String	to;
	if (i == string::npos) {
		from = String::FromUTF8 (tmp);
	}
	else {
		from = String::FromUTF8 (tmp.substr (0, i));
		to = String::FromUTF8 (tmp.substr (i+1));
	}
	from = from.Trim ();
	to = to.Trim ();
	fResponseHeaders_[from] = to;
	return nBytes;
}

Response	IConnection_LibCurl::SendAndRequest (const Request& request)	override
{
	MakeHandleIfNeeded_ ();
	fResponseData_.clear ();
	fResponseHeaders_.clear ();

	//grab useragent from request headers...
	//curl_easy_setopt (fCurlHandle_, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	// grab initial headers and do POST/etc based on args in request...
	curl_slist*	tmpH	=	curl_slist_append (nullptr, "Expect: ");	//tmphack - really iterate over requst headers - with a few exceptions...
	LibCurlException::DoThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_HTTPHEADER, tmpH));
	if (fSavedHeaders_ != nullptr) {
		curl_slist_free_all (fSavedHeaders_);
		fSavedHeaders_ = nullptr;
	}
	fSavedHeaders_ = tmpH;

	LibCurlException::DoThrowIfError (:: curl_easy_perform (fCurlHandle_));

	Response	response;
	response.fData = fResponseData_;

	long	resultCode	=	0;
	LibCurlException::DoThrowIfError (::curl_easy_getinfo (fCurlHandle_, CURLINFO_RESPONSE_CODE, &resultCode));
	response.fStatus = resultCode;
	response.fHeaders = fResponseHeaders_;
	response.fData = fResponseData_;
	return response;
}

void	IConnection_LibCurl::MakeHandleIfNeeded_ ()
{
	if (fCurlHandle_ == nullptr) {
		fCurlHandle_  = curl_easy_init ();
		LibCurlException::DoThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_URL, fCURLCache_URL_.c_str ()));
	}
}
#endif

