/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Framework_WebServer_HTTPListener_h_
#define	_Stroika_Framework_WebServer_HTTPListener_h_	1

#include	"../StroikaPreComp.h"

#include	<string>
#include	<vector>

#include	"../../Foundation/Characters/String.h"
#include	"../../Foundation/Configuration/Common.h"
#include	"../../Foundation/Memory/SharedPtr.h"
#include	"../../Foundation/IO/Network/SocketStream.h"



/*
 * TODO:
 *		Very early stage draft.
 *
 *		(o)	
 */

namespace	Stroika {	
	namespace	Frameworks {
		namespace	WebServer {

			using	namespace	Stroika::Foundation;
			using	namespace	Stroika::Foundation::IO;
			using	Characters::String;

			using	Stroika::Foundation::IO::Network::Socket;
			using	Stroika::Foundation::IO::Network::SocketStream;

			class	HTTPConnection {
				public:
					HTTPConnection (Socket s);
					HTTPConnection (SocketStream s);


				private:
					SocketStream	fSocketStream_;
			};
		}
	}
}
#endif	/*_Stroika_Framework_WebServer_HTTPListener_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"HTTPConnection.inl"
