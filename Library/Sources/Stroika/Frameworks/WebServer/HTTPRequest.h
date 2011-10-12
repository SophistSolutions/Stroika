/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Framework_WebServer_HTTPRequest_h_
#define	_Stroika_Framework_WebServer_HTTPRequest_h_	1

#include	"../StroikaPreComp.h"

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
