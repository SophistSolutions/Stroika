/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Framework_WebServer_HTTPRequest_h_
#define	_Stroika_Framework_WebServer_HTTPRequest_h_	1

#include	"../StroikaPreComp.h"

#include	<map>
#include	<string>
#include	<vector>

#include	"../../Foundation/Characters/String.h"
#include	"../../Foundation/Configuration/Common.h"
#include	"../../Foundation/DataExchangeFormat/InternetMediaType.h"
#include	"../../Foundation/Memory/SharedPtr.h"



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

			// Maybe associated TextStream, and maybe readline method goes here
			struct	HTTPRequest {
				public:
					HTTPRequest ();

				public:
					String				fHTTPVersion;
					String				fHostRelativeURL;	// includes query-string, but not hostname, not port
					map<String,String>	fHeaders;
			};

		}
	}
}
#endif	/*_Stroika_Framework_WebServer_HTTPRequest_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"HTTPRequest.inl"
