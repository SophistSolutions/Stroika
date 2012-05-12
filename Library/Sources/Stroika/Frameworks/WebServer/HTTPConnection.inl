/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Frameworks_WebServer_HTTPConnection_inl_
#define	_Stroika_Frameworks_WebServer_HTTPConnection_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../../Foundation/Containers/Common.h"

namespace	Stroika {
	namespace	Frameworks	{
		namespace	WebServer {

			// class	HTTPConnection
			inline	Socket	HTTPConnection::GetSocket () const		{ return fSocket_; }
			inline	const HTTPRequest&	HTTPConnection::GetRequest () const { return fRequest_; }
			inline	HTTPRequest&	HTTPConnection::GetRequest () { return fRequest_; }
			inline	HTTPResponse&	HTTPConnection::GetResponse () { return fResponse_; }
			inline	const HTTPResponse&	HTTPConnection::GetResponse () const { return fResponse_; }

		}
	}
}
#endif	/*_Stroika_Frameworks_WebServer_HTTPConnection_inl_*/
