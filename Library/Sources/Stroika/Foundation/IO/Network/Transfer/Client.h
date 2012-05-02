/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
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
 *		(o)		Probably princiapply a wrapper on CURL - but also support wrapper on WinHTTP
 *		(o)		Redo response / src API using streams?
 *		(o)		Add Client side certs
 *		(o)		Add server-side-cert checking mechanism (review LIBCURL to see what makes sense)
 *		(o)		Add TIMEOUT property on Session object as background for requests - or maybe make it part of request object? (param to requests)?
 *
 */


namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {
			namespace	Network {
				namespace	Transfer {

					using	Characters::String;
					using	DataExchangeFormat::InternetMediaType;


					// SUPER PRIMITIVE fetcher - must do MUCH better, much more elaborate!
					vector<Byte>	Fetch (const wstring& url);

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
						InternetMediaType	fContentType;
						map<String,String>	fHeaders;
						HTTP::Status		fStatus;
					};
					

// DO REP STUFF - SO CAN HAVE _WinHTTP and _LibCurl implemenations
//	
// Unclear about copyability - maybe if its a smartpr OK to copy - but would be copy-by-reference? Could be confusing! CONSIDER
					class	ISession {
						private:
							NO_COPY_CONSTRUCTOR (ISession);
							NO_ASSIGNMENT_OPERATOR (ISession);
						
						public:
							ISession ();
							virtual ~ISession ();
						
						public:
							virtual	URL		GetURL () const							=	0;
							virtual	void	SetURL (const URL& url)					=	0;
							virtual	void	Close ()								=	0;
							virtual	Response	SendAndRequest (const Request& r)	=	0;
					};

					class	Session {
						private:
							NO_COPY_CONSTRUCTOR (Session);
							NO_ASSIGNMENT_OPERATOR (Session);
						protected:
							Session (const Memory::SharedPtr<ISession>& rep);

						public:
							nonvirtual	URL		GetURL () const;
							nonvirtual	void	SetURL (const URL& url);

						public:
							// force closed session. Can still call Send again, but that autocreates new session
							nonvirtual	void	Close ();

						public:
							nonvirtual	Response	SendAndRequest (const Request& r);
						
						// Simple wrappers, with hardwired methods
						public:
							nonvirtual	Response	Get (const map<String,String>& extraHeaders = map<String,String> ());
							nonvirtual	Response	Post (const vector<Byte>& data, const InternetMediaType& contentType, const map<String,String>& extraHeaders = map<String,String> ());
							nonvirtual	Response	Delete (const map<String,String>& extraHeaders = map<String,String> ());
							nonvirtual	Response	Put (const vector<Byte>& data, const InternetMediaType& contentType, const map<String,String>& extraHeaders = map<String,String> ());

						private:
							Memory::SharedPtr<ISession>	fRep_;
					};



					// Do ISession API - pure virutal, and then concreete subclasses for and _WinHTTP and _LibCurl
					// Uncear if winhttp and libcurl versions go in diff files? Could also do native socket impl?
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
