/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>
#include	<cstdlib>

#include	"../../Foundation/Characters/Tokenize.h"
#include	"../../Foundation/Containers/Common.h"
#include	"../../Foundation/DataExchangeFormat/BadFormatException.h"
#include	"../../Foundation/Debug/Assertions.h"
#include	"../../Foundation/Execution/Exceptions.h"
#include	"../../Foundation/Memory/SmallStackBuffer.h"

#include	"ConnectionManager.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Containers;
using	namespace	Stroika::Foundation::Memory;

using	namespace	Stroika::Frameworks;
using	namespace	Stroika::Frameworks::WebServer;





/*
 ********************************************************************************
 ************************* WebServer::HTTPConnection ****************************
 ********************************************************************************
 */
void	ConnectionManager::AddHandler (SharedPtr<HTTPRequestHandler> h)
{
	fHandlers_.push_back (h);
}

void	ConnectionManager::RemoveHandler (SharedPtr<HTTPRequestHandler> h)
{
	for (vector<SharedPtr<HTTPRequestHandler> >::iterator i = fHandlers_.begin (); i != fHandlers_.end (); ++i) {
		if (*i == h) {
			fHandlers_.erase (i);
			return;
		}
	}
	RequireNotReached ();	// you must specify a valid handler to remove

}
