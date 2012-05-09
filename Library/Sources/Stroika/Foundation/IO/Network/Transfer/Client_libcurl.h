/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_Network_Transfer_Client_libcurl_h_
#define	_Stroika_Foundation_IO_Network_Transfer_Client_libcurl_h_	1

#include	"../../../StroikaPreComp.h"

#include	<map>
#include	<string>

#include	"../../../Configuration/Common.h"
#include	"../../../Characters/String.h"
#include	"../../../DataExchangeFormat/InternetMediaType.h"
#include	"../../../Memory/SharedPtr.h"
#include	"../URL.h"
#include	"../HTTP/Status.h"

#include	"Client.h"


#if		qHasFeature_libcurl
struct	curl_slist;
typedef	int	CURLcode;
#endif



/*
 * TODO:
 *		(o)		Very rough draft
 *
 */


namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {
			namespace	Network {
				namespace	Transfer {

					#if		qHasFeature_libcurl
						class	LibCurlException : public StringException {
							public:
								LibCurlException (CURLcode ccode);

							public:
								// throw Exception () type iff the status indicates a real exception code. This MAY not throw an exception of type LibCurlException,
								// but MAY map to any other exception type
								static	void	DoThrowIfError (CURLcode status);

							public:
								nonvirtual	CURLcode	GetCode () const;
						
							private:
								CURLcode	fCurlCode_;
						};


						class	IConnection_LibCurl : public IConnection {
							public:
								IConnection_LibCurl ();
								virtual ~IConnection_LibCurl ();
						
							public:
								virtual	URL			GetURL () const	override;
								virtual	void		SetURL (const URL& url)	override;
								virtual	void		Close ()	override;
								virtual	Response	SendAndRequest (const Request& request)	override;

							private:
								static		size_t	ResponseWriteHandler_ (void* ptr, size_t size, size_t nmemb, void* userP);
								nonvirtual	size_t	ResponseWriteHandler_ (const Byte* ptr, size_t nBytes);

							private:
								static		size_t	ResponseHeaderWriteHandler_ (void* ptr, size_t size, size_t nmemb, void* userP);
								nonvirtual	size_t	ResponseHeaderWriteHandler_ (const Byte* ptr, size_t nBytes);

							private:
								void*				fCurlHandle_;
								string				fCURLCache_URL_;	// cuz of quirky memory management policies of libcurl
								vector<Byte>		fResponseData_;
								map<String,String>	fResponseHeaders_;
								curl_slist*			fSavedHeaders_;
						};
					#endif

				}
			}
		}
	}
}
#endif	/*_Stroika_Foundation_IO_Network_Transfer_Client_libcurl_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Client_libcurl.inl"
