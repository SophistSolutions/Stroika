/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Framework_WebServer_HTTPRequestHandler_h_
#define	_Stroika_Framework_WebServer_HTTPRequestHandler_h_	1

#include	"../StroikaPreComp.h"

#include	<string>
#include	<vector>

#include	"../../Foundation/Characters/String.h"
#include	"../../Foundation/Configuration/Common.h"
#include	"../../Foundation/DataExchangeFormat/InternetMediaType.h"
#include	"../../Foundation/Memory/SharedPtr.h"

#include	"HTTPConnection.h"


/*
 * TODO:
 *		(o)		
 */

namespace	Stroika {	
	namespace	Frameworks {
		namespace	WebServer {

			using	namespace	Stroika::Foundation;
			using	Characters::String;
			using	DataExchangeFormat::InternetMediaType;


			/*
			 * A request handler should be understood to be stateless - as far as the connection is concerned.
			 * ??? Maybe - or maybe have add/remove or notication so assocaited?? For now - assume stateless - and just called
			 * with HandleRequest ...
			 */
			struct	HTTPRequestHandler {
				protected:
					HTTPRequestHandler ();
				public:
					virtual	~HTTPRequestHandler ();

				public:
					// CanHandleRequest() can peek at data in the connection - but cannot read, or directly or indirecly modify anything
					// NB: Its called after the HTTP Header has been read (thats already embodied in teh connection Request object)
					virtual	bool	CanHandleRequest (const HTTPConnection& connection) const = 0;

					// This will write an answer to the connection Response object
					virtual	void	HandleRequest (HTTPConnection& connection) = 0;
			};

		}
	}
}
#endif	/*_Stroika_Framework_WebServer_HTTPRequestHandler_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"HTTPRequest.inl"
