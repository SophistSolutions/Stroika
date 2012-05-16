/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_Network_Transfer_Client_h_
#define	_Stroika_Foundation_IO_Network_Transfer_Client_h_	1

#include	"../../../StroikaPreComp.h"

#include	<map>
#include	<string>

#include	"../../../Configuration/Common.h"
#include	"../../../Characters/String.h"
#include	"../../../DataExchangeFormat/InternetMediaType.h"
#include	"../../../Memory/SharedPtr.h"
#include	"../URL.h"
#include	"../HTTP/Status.h"



// *		(o)		Draft API



/*
 * TODO:
 *
 *		(o)		Fixup Requqest/Response code to just be driven off fHeaders  list - and have Get/Set ContentType, etc - and other fields - 
 *				which map to that list. MAYBE move request/repsonse stuff to HTTP module- so re-usable in framework code which does webserver??
 *
 *		(o)		Probably princiapply a wrapper on CURL - but also support wrapper on WinHTTP
 *		(o)		Redo response / src API using streams?
 *		(o)		Add Client side certs
 *		(o)		Add server-side-cert checking mechanism (review LIBCURL to see what makes sense)
 *		(o)		Add TIMEOUT property on Connection object as background for requests - or maybe make it part of request object? (param to requests)?
 *
 */


namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {
			namespace	Network {
				namespace	Transfer {

					using	Characters::String;
					using	DataExchangeFormat::InternetMediaType;
					using	Time::DurationSecondsType;


					struct	Request {
						Request ();

						String				fMethod;
						map<String,String>	fOverrideHeaders;
						vector<Byte>		fData;	// usually empty, but provided for some methods like POST
						InternetMediaType	fContentType;
						
					};
					struct	Response {
						Response ();

						vector<Byte>		fData;	// usually empty, but provided for some methods like POST
						map<String,String>	fHeaders;
						HTTP::Status		fStatus;

						nonvirtual	InternetMediaType	GetContentType () const;	// scans headers
					};
					

					// TODO:
					//		Unclear about copyability - maybe if its a smartpr OK to copy - but would be copy-by-reference?
					//		Could be confusing! CONSIDER
					class	Connection {
						protected:
							class	_IRep {
								private:
									NO_COPY_CONSTRUCTOR (_IRep);
									NO_ASSIGNMENT_OPERATOR (_IRep);
						
								public:
									_IRep ();
									virtual ~_IRep ();
						
								public:
									virtual	URL					GetURL () const								=	0;
									virtual	void				SetURL (const URL& url)						=	0;
									virtual	DurationSecondsType	GetTimeout () const							=	0;
									virtual	void				SetTimeout (DurationSecondsType timeout)	=	0;
									virtual	void				Close ()									=	0;
									virtual	Response			SendAndRequest (const Request& r)			=	0;
							};

						protected:
							Connection (const Memory::SharedPtr<_IRep>& rep);

						public:
							// SendAndRequest should timeout after this amount of time. Note - the initial SendAndRequest may do
							// much more work (nslookup and tcp connect) than subsequent ones, and this same timeout is used for the combined time.
							nonvirtual	DurationSecondsType		GetTimeout () const;
							nonvirtual	void					SetTimeout (DurationSecondsType timeout);

						public:
							nonvirtual	URL		GetURL () const;
							nonvirtual	void	SetURL (const URL& url);

						public:
							// force closed Connection. Can still call Send again, but that autocreates new Connection
							nonvirtual	void	Close ();

						public:
							nonvirtual	Response	SendAndRequest (const Request& r);
						
						// Simple wrappers, with hardwired methods
						public:
							nonvirtual	Response	Get (const map<String,String>& extraHeaders = map<String,String> ());
							nonvirtual	Response	Post (const vector<Byte>& data, const InternetMediaType& contentType, const map<String,String>& extraHeaders = map<String,String> ());
							nonvirtual	Response	Delete (const map<String,String>& extraHeaders = map<String,String> ());
							nonvirtual	Response	Put (const vector<Byte>& data, const InternetMediaType& contentType, const map<String,String>& extraHeaders = map<String,String> ());
							nonvirtual	Response	Options (const map<String,String>& extraHeaders = map<String,String> ());

						private:
							Memory::SharedPtr<_IRep>	fRep_;
					};



					/*
					 * Simple connection factory object. If you don't care what backend to use for remote connections, use this API
					 * to construct an unconnected object.
					 */
					Connection	CreateConnection ();
				}
			}
		}
	}
}
#endif	/*_Stroika_Foundation_IO_Network_Transfer_Client_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Client.inl"
