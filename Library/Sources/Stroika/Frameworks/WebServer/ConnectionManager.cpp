/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>
#include	<cstdlib>

#include	"../../Foundation/Characters/Tokenize.h"
#include	"../../Foundation/Containers/Common.h"
#include	"../../Foundation/DataExchangeFormat/BadFormatException.h"
#include	"../../Foundation/Debug/Assertions.h"
#include	"../../Foundation/Execution/Exceptions.h"
#include	"../../Foundation/Execution/SimpleRunnable.h"
#include	"../../Foundation/Execution/Sleep.h"
#include	"../../Foundation/IO/Network/HTTP/Exception.h"
#include	"../../Foundation/Memory/SmallStackBuffer.h"

#include	"ConnectionManager.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Containers;
using	namespace	Stroika::Foundation::Memory;

using	namespace	Stroika::Frameworks;
using	namespace	Stroika::Frameworks::WebServer;





/*
 ********************************************************************************
 ************************* WebServer::ConnectionManager *************************
 ********************************************************************************
 */
ConnectionManager::ConnectionManager ()
{
}

ConnectionManager::~ConnectionManager ()
{
}

void	ConnectionManager::Start ()
{
	fThreads_.AddTask (Execution::SimpleObjRunnable<ConnectionManager>::MAKE (&ConnectionManager::DoMainConnectionLoop_, this));
}

void	ConnectionManager::Abort ()
{
	fThreads_.Abort ();
}

void	ConnectionManager::WaitForDone (Time::DurationSecondsType timeout)
{
	fThreads_.WaitForDone (timeout);
}

void	ConnectionManager::AbortAndWaitForDone (Time::DurationSecondsType timeout)
{
	fThreads_.AbortAndWaitForDone ();
}

void	ConnectionManager::AddHandler (const SharedPtr<HTTPRequestHandler>& h)
{
	Execution::AutoCriticalSection	critSec (fHandlers_);
	fHandlers_.push_back (h);
}

void	ConnectionManager::RemoveHandler (const SharedPtr<HTTPRequestHandler>& h)
{
	Execution::AutoCriticalSection	critSec (fHandlers_);
	for (list<SharedPtr<HTTPRequestHandler> >::iterator i = fHandlers_.begin (); i != fHandlers_.end (); ++i) {
		if (*i == h) {
			fHandlers_.erase (i);
			return;
		}
	}
	RequireNotReached ();	// you must specify a valid handler to remove
}

void	ConnectionManager::AddConnection (const SharedPtr<HTTPConnection>& conn)
{
	Execution::AutoCriticalSection	critSec (fActiveConnections_);
	fActiveConnections_.push_back (conn);
}

void	ConnectionManager::AbortConnection (const SharedPtr<HTTPConnection>& conn)
{
	AssertNotImplemented ();
}

void	ConnectionManager::DoMainConnectionLoop_ ()
{
	// MUST DO MAJOR CRITICAL SECTION WORK HERE
	while (true) {
		Execution::Sleep (0.1);	// hack - need smarter wait on available data
		SharedPtr<HTTPConnection>	conn;
		{
			Execution::AutoCriticalSection	critSec (fActiveConnections_);
			if (fActiveConnections_.empty ()) {
				conn = fActiveConnections_.front ();
			}
		}

		if (not conn.IsNull ()) {
			
// REALLY should create NEW TASK we subbit to the threadpool...
			DoOneConnection_ (conn);

			Execution::AutoCriticalSection	critSec (fActiveConnections_);
			for (list<SharedPtr<HTTPConnection>>::iterator i = fActiveConnections_.begin (); i != fActiveConnections_.end (); ++i) {
				if (*i == conn) {
					fActiveConnections_.erase (i);
					break;
				}
			}
		}

		// now go again...
	}
}

void	ConnectionManager::DoOneConnection_ (SharedPtr<HTTPConnection> c)
{
	// prevent exceptions for now cuz outer code not handling them...
	try {
		c->ReadHeaders ();

		SharedPtr<HTTPRequestHandler>	h;
		{
			Execution::AutoCriticalSection	critSec (fHandlers_);
			for (list<SharedPtr<HTTPRequestHandler> >::iterator i = fHandlers_.begin (); i != fHandlers_.end (); ++i) {
				if ((*i)->CanHandleRequest (*c)) {
					h = *i;
				}
			}
		}
		if (not h.IsNull ()) {
			h->HandleRequest (*c);
			c->GetResponse ().End ();
			c->Close ();//tmphack
			return;
		}
		Execution::DoThrow (Network::HTTP::Exception (StatusCodes::kNotFound));
	}
	catch (...) {
		c->GetResponse ().End ();
		c->Close ();//tmphack
	}
}
