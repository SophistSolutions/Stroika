/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Framework_WebServer_ConnectionManager_h_
#define	_Stroika_Framework_WebServer_ConnectionManager_h_	1

#include	"../StroikaPreComp.h"

#include	"../../Foundation/Execution/ThreadPool.h"
#include	"../../Foundation/Memory/SharedPtr.h"

#include	"HTTPRequest.h"
#include	"HTTPRequestHandler.h"
#include	"HTTPResponse.h"
#include	"HTTPConnection.h"



/*
 * TODO:
 *
 *		o	First get basics working
 *
 *		o	Handle thread safety (very tricky - watch adding connection handlers!)
 *
 *		o	Do a single background thread first to run handlers.
 *
 *		o	THen do threadpooling.
 */

namespace	Stroika {	
	namespace	Frameworks {
		namespace	WebServer {

			using	namespace	Stroika::Foundation;
			using	namespace	Stroika::Foundation::IO;
			using	Characters::String;


			/*
			 * This class is a useful helper for managing a set of connections. You can start it and stop it (it maintains internal threads).
			 * And you can hand it HTTPConnections, along with a set of handlers, and it will monitor the connections, and when any is ready
			 * with more input, it will assign the appropriate handler to handle the request, and produce the response.
			 *
			 * This doesn;t CURRENTLY support keepalives or thread pooling - but those functions will be builtin here (eventiually) or a subclass.
			 */
			class	ConnectionManager {
				private:
					NO_COPY_CONSTRUCTOR (ConnectionManager);
					NO_ASSIGNMENT_OPERATOR (ConnectionManager);
				public:
					ConnectionManager ();
					~ConnectionManager ();
				
				public:
					nonvirtual	void	Start ();
					nonvirtual	void	Abort ();
					nonvirtual	void	WaitUnitlDone (int timeoutGetRightTime);
					nonvirtual	void	AbortAndWaitUnitlDone (int timeoutGetRightTime);

				public:
					nonvirtual	void	AddHandler (Memory::SharedPtr<HTTPRequestHandler> h);
					nonvirtual	void	RemoveHandler (Memory::SharedPtr<HTTPRequestHandler> h);

				public:
					nonvirtual	void	AddConnection (Memory::SharedPtr<HTTPConnection> c);
					nonvirtual	void	AbortConnection (Memory::SharedPtr<HTTPConnection> c);
			
				public:
					/* 
					 * We need some sort of status flag on connections - saying of they are OPEN or not - or done. But this will return just those
					 * which are not 'done'. Of course - due to asyncrhony, by the time one looks at the list, some may already be done.
					 */
					nonvirtual	vector<Memory::SharedPtr<HTTPConnection>> GetConnections () const;

				private:
					// REALLY could use Stroika threadsafe lists here!!! - so could just iterate and forget!
					vector<Memory::SharedPtr<HTTPRequestHandler> >	fHandlers_;
					vector<Memory::SharedPtr<HTTPConnection> >		fActiveConnections_;

					// we may eventually want two thread pools - one for managing bookkeeping/monitoring harvests, and one for actually handling
					// connections. Or maybe a single thread for the bookkeeping, and the pool for handling ongoing connections?
					//
					// But for now - KISS
					Execution::ThreadPool							fThreads_;
			};


		}
	}
}
#endif	/*_Stroika_Framework_WebServer_ConnectionManager_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"ConnectionManager.inl"
