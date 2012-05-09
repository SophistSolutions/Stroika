/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_Network_Transfer_Client_WinHTTP_h_
#define	_Stroika_Foundation_IO_Network_Transfer_Client_WinHTTP_h_	1

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






/*
 * TODO:
 *		(o)		Very rough draft. This code is completely untested, and really only about 1/2 written. Even the high level
 *				usage (container) pattern is just a draft. Its based on HTTPSupport.cpp code in R4LLib (2012-05-08)
 *
 *
 */


namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {
			namespace	Network {
				namespace	Transfer {

					#if		qHasFeature_WinHTTP
						// Just object-slice the smart pointer to get a regular connection object - this is just a factory for
						// LibCurl connection rep objects
						class	WinHTTPConnection : public Connection {
							public:
								WinHTTPConnection ();

							private:
								class	Rep_;
						};

						class	WinHTTPConnection::Rep_ : public IConnection {
							private:
								NO_COPY_CONSTRUCTOR (Rep_);
								NO_ASSIGNMENT_OPERATOR (Rep_);
							public:
								Rep_ ();
								virtual ~Rep_ ();
						
							public:
								virtual	URL			GetURL () const	override;
								virtual	void		SetURL (const URL& url)	override;
								virtual	void		Close ()	override;
								virtual	Response	SendAndRequest (const Request& request)	override;

							private:
								nonvirtual	void	AssureHasHandle_ ();
							
							private:
								void*		fSessionHandle_;
								void*		fConnectionHandle_;
						};
					#endif
				}
			}
		}
	}
}
#endif	/*_Stroika_Foundation_IO_Network_Transfer_Client_WinHTTP_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Client_WinHTTP.inl"
