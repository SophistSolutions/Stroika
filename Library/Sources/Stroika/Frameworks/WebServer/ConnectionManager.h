/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Framework_WebServer_ConnectionManager_h_
#define	_Stroika_Framework_WebServer_ConnectionManager_h_	1

#include	"../StroikaPreComp.h"

#include	<list>

#include	"../../Foundation/Execution/Lockable.h"
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
			using	Memory::SharedPtr;


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
					nonvirtual	void	WaitForDone (Time::DurationSecondsType timeout = Time::kInfinite);
					nonvirtual	void	AbortAndWaitForDone (Time::DurationSecondsType timeout = Time::kInfinite);

				public:
					nonvirtual	void	AddHandler (const SharedPtr<HTTPRequestHandler>& h);
					nonvirtual	void	RemoveHandler (const SharedPtr<HTTPRequestHandler>& h);

				public:
					nonvirtual	void	AddConnection (const SharedPtr<HTTPConnection>& conn);
					nonvirtual	void	AbortConnection (const SharedPtr<HTTPConnection>& conn);
			
				public:
					/* 
					 * We need some sort of status flag on connections - saying of they are OPEN or not - or done. But this will return just those
					 * which are not 'done'. Of course - due to asyncrhony, by the time one looks at the list, some may already be done.
					 */
					nonvirtual	vector<SharedPtr<HTTPConnection>> GetConnections () const;

				private:
					//VERY VERY SUPER PRIMITIVE FIFRST DRAFT OF CONNECTION HANDLING
					nonvirtual	void	DoMainConnectionLoop_ ();
					nonvirtual	void	DoOneConnection_ (SharedPtr<HTTPConnection> c);
				
				private:
					// REALLY could use Stroika threadsafe lists here!!! - so could just iterate and forget!
					Execution::Lockable<list<SharedPtr<HTTPRequestHandler>>>	fHandlers_;
					Execution::Lockable<list<SharedPtr<HTTPConnection>>>		fActiveConnections_;

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
