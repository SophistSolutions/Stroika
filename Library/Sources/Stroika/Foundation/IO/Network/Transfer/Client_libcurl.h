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
#include	"../../../Execution/StringException.h"
#include	"../../../Memory/SharedPtr.h"
#include	"../URL.h"
#include	"../HTTP/Status.h"

#include	"Client.h"


#if		qHasFeature_libcurl
struct	curl_slist;
#endif



/*
 * TODO:
 *		(o)		Very rough draft. This code is completely untested, and really only about 1/2 written. Even the high level
 *				usage (container) pattern is just a draft.
 *
 *		(o)		Must do basic METHOD supprot - setting CURLOPT_GET for gets, etc.
 *				Need extended MEHTOD support for DELETE (just specify string)
 *				And for PUT/POST special setopt(CURLOPT_POST/PUT, and do reader function for pUT ajnd direct data pass for Post -
 *				since I THINK thats what curl requires - use common API if I can find a way).
 *
 *		(o)		Handle pass in of headers. Treat special headers like content type proeprly. Same for content-length.
 *
 *
 */


namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {
			namespace	Network {
				namespace	Transfer {

					#if		qHasFeature_libcurl
						class	LibCurlException : public Execution::StringException {
							public:
								typedef	int	CURLcode;		// tried directly to reference libcurl CURLcode but tricky cuz its an enum -- LGP 2012-05-08
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
							private:
								NO_COPY_CONSTRUCTOR (IConnection_LibCurl);
								NO_ASSIGNMENT_OPERATOR (IConnection_LibCurl);
							public:
								IConnection_LibCurl ();
								virtual ~IConnection_LibCurl ();
						
							public:
								virtual	URL			GetURL () const	override;
								virtual	void		SetURL (const URL& url)	override;
								virtual	void		Close ()	override;
								virtual	Response	SendAndRequest (const Request& request)	override;

							private:
								nonvirtual	void	MakeHandleIfNeeded_ ();

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


					// Just object-slice the smart pointer to get a regular connection object - this is just a factory for
					// LibCurl connection rep objects
					class	LibCurlConnection : public Connection {
						public:
							LibCurlConnection ();
					};

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
