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
#include	"HTTPRequest.h"
#include	"HTTPResponse.h"



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


					// Must rethink this organization -but for now - call this once at start of connection to fill in details in
					// the HTTP Request object
					void	ReadHeaders ();
// not sure we want this
					void	Close ();


				public:
					Socket	GetSocket () const		{ return fSocket_; }
					//tmphack - not sure if/what we want todo here
					const HTTPRequest&	GetRequest () const { return fRequest_; }
					HTTPRequest&	GetRequest () { return fRequest_; }
					//tmphack - not sure if/what we want todo here
					HTTPResponse&	GetResponse () { return fResponse_; }
					const HTTPResponse&	GetResponse () const { return fResponse_; }

				private:
					SocketStream	fSocketStream_;
					HTTPRequest		fRequest_;
					HTTPResponse	fResponse_;

				private:
					//SocketStream	fSocketStream_;
					Socket	fSocket_;
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
