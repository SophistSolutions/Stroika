/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<windows.h>
	#include	<Psapi.h>
#endif

#include	<new>

#include	"MemoryAllocator.h"

#include	"Common.h"


using	namespace	Stroika;
using	namespace	Stroika::Foundation;

using	Debug::TraceContextBumper;


// Very primitive memleak detector to help me debug memleak issues
//
//		Probably unneeded, since I think I can use the MSVC builtin leak detector (at least for DEBUG builds).
//
//		One advantage of writing my own - is that I can turn it on/off indepedent of the _DEBUG define (which can
//		be important for performance reasons - since the DEBUG macro turns on lots of checking that make lots of
//		things quite slow).
//
//		Basic idea here - is that I would use a map<> - like I did in the XMLDB Xerces mem-mgr class - to track memory.
//		But I would need to be extra careful (using a stack-based flag setter/unsetter) so that the map<> class
//		memory allocations didn't get tracked.
//
//			-- LGP 2009-05-25
//#define	qOverrideOpNewDeleteForAccounting	1
#ifndef	qOverrideOpNewDeleteForAccounting
//	For DEBUG builds - do NOT turn this on - because the builtin support in MSVC lib works better.
//	and if we wanted to include this - we would have to move these #defines to the .h file - and disable
//	the DEBUG_NEW macro if we ever tried to use this (since it conflicts). That - or we need to find a way
//	to avoid the conflicts from the DBNEW library...(for method with extra params for filename/line#).
//	MAYBE we could include that override here too - but I think that just adds to the conflicts with that
//	library.
//		-- LGP 2009-05-26
//
//	#if		qDebug
//		#define	qOverrideOpNewDeleteForAccounting	1
//	#endif
	#if		qDebug
		#define	qOverrideOpNewDeleteForAccounting	0
	#endif
#endif
#ifndef	qOverrideOpNewDeleteForAccounting
	// for LOGGING builds - which are RELEASE builds - turn on our override op-new
	#if		defined (qTraceToFile) && !qDebug
		#if		qTraceToFile
			//#define	qOverrideOpNewDeleteForAccounting	1
			#define	qOverrideOpNewDeleteForAccounting	0
		#endif
	#endif
#endif
#ifndef	qOverrideOpNewDeleteForAccounting
	// for DEBUG-MEM-LEAK builds - turn on our override op-new
	#if		defined (qAllowBlockAllocation)
		#if		qAllowBlockAllocation==0
			#define	qOverrideOpNewDeleteForAccounting	1
		#endif
	#endif
#endif
#ifndef		qOverrideOpNewDeleteForAccounting
	#define	qOverrideOpNewDeleteForAccounting	0
#endif



#if		qOverrideOpNewDeleteForAccounting
	//#define	qOverrideOpNew_EXTRA_LEAK_DETECTION	0
	//#define	qOverrideOpNew_EXTRA_LEAK_DETECTION	1
	#ifndef	qOverrideOpNew_EXTRA_LEAK_DETECTION
	#define	qOverrideOpNew_EXTRA_LEAK_DETECTION	1
	#endif
#endif


#if		qOverrideOpNewDeleteForAccounting
namespace	{
	bool	sInitialized	=	false;

	#if		qOverrideOpNew_EXTRA_LEAK_DETECTION
		typedef	Memory::LeakTrackingGeneralPurposeAllocator		_USE_ALLOCATOR_;
	#else
		typedef	Memory::SimpleSizeCountingGeneralPurposeAllocator	_USE_ALLOCATOR_;
	#endif

	Byte	sAllocatorBuf_[sizeof (_USE_ALLOCATOR_)];		// BSS until intiailized in Memory::Private::INIT::INIT ()

	inline	_USE_ALLOCATOR_&	GetAllocator_ ()
		{
			return *reinterpret_cast<_USE_ALLOCATOR_*> (&sAllocatorBuf_);
		}
}
#endif





/*
 ********************************************************************************
 *************************** Memory::Private::INIT ******************************
 ********************************************************************************
 */
Memory::Private::INIT::INIT ()
{
	#if		qOverrideOpNewDeleteForAccounting
		sInitialized  = true;
		new (&sAllocatorBuf_) _USE_ALLOCATOR_ ();
	#endif
}

Memory::Private::INIT::~INIT ()
{
	#if		qOverrideOpNewDeleteForAccounting
		sInitialized = false;
		#if		qOverrideOpNew_EXTRA_LEAK_DETECTION
			reinterpret_cast<_USE_ALLOCATOR_*> (&sAllocatorBuf_)->~LeakTrackingGeneralPurposeAllocator ();
		#else
			reinterpret_cast<_USE_ALLOCATOR_*> (&sAllocatorBuf_)->~SimpleSizeCountingGeneralPurposeAllocator ();
		#endif
	#endif
}






#if		qOverrideOpNewDeleteForAccounting
/*
 ********************************************************************************
 ******************** ::operator new/::operator delete ETC **********************
 ********************************************************************************
 */
#pragma warning (push)
#pragma warning (4: 4290)
void*	operator new (size_t size) throw (std::bad_alloc)
{
	// not perfrect - cuz doesn't call 'new handler' and if uninitialized - doesn't do null-->bad_alloc () mapping,
	// but should be VERY RARELY called uninitilized (just very early in app startup).
	//	-- LGP 2009-05-29
	return sInitialized? GetAllocator_ ().Allocate (size): malloc (size);
}

void*	operator new[] (size_t size) throw (std::bad_alloc)
{
	// not perfrect - cuz doesn't call 'new handler' and if uninitialized - doesn't do null-->bad_alloc () mapping,
	// but should be VERY RARELY called uninitilized (just very early in app startup).
	//	-- LGP 2009-05-29
	return sInitialized? GetAllocator_ ().Allocate (size): malloc (size);
}
#pragma warning (pop)

void	operator delete (void* pUserData)
{
	if (pUserData != NULL) {
		if (sInitialized) {
			GetAllocator_ ().Deallocate (pUserData);
		}
		else {
			free (pUserData);
		}
	}
}

void	operator delete[] (void* pUserData)
{
	if (pUserData != NULL) {
		if (sInitialized) {
			GetAllocator_ ().Deallocate (pUserData);
		}
		else {
			free (pUserData);
		}
	}
}
#endif







/*
 ********************************************************************************
 ******************* Memory::GetGlobalAllocationStatistics **********************
 ********************************************************************************
 */
Memory::GlobalAllocationStatistics	Memory::GetGlobalAllocationStatistics ()
{
	GlobalAllocationStatistics	s;
	#if		qPlatform_Windows
	HANDLE	hProcess = ::OpenProcess (PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ::GetCurrentProcessId ());
	if (hProcess != NULL) {
		#pragma comment (lib, "Psapi.lib")
		PROCESS_MEMORY_COUNTERS	pmc;
		memset (&pmc, 0, sizeof (pmc));
		pmc.cb = sizeof (pmc);
		if (::GetProcessMemoryInfo (hProcess, &pmc, sizeof (pmc))) {
			s.fPageFaultCount = pmc.PageFaultCount;
			s.fPagefileUsage = pmc.PagefileUsage;
			s.fWorkingSetSize = pmc.WorkingSetSize;
			#if		qOverrideOpNewDeleteForAccounting
				s.fTotalOutstandingAllocations = GetAllocator_ ().GetNetAllocationCount ();
				s.fTotalOutstandingBytesAllocated = GetAllocator_ ().GetNetAllocatedByteCount ();
			#elif	qDebug
				_CrtMemState	memState;
				_CrtMemCheckpoint (&memState);
				s.fTotalOutstandingAllocations = memState.lCounts[_NORMAL_BLOCK] + memState.lCounts[_CLIENT_BLOCK];
				s.fTotalOutstandingBytesAllocated = memState.lSizes[_NORMAL_BLOCK] + memState.lSizes[_CLIENT_BLOCK];
			#endif
		}
		::CloseHandle (hProcess);
	}
	#endif
	return s;
}










/*
 ********************************************************************************
 ******************************* Memory::LeakChecker ****************************
 ********************************************************************************
 */
#if		qOverrideOpNewDeleteForAccounting && qOverrideOpNew_EXTRA_LEAK_DETECTION
namespace	{
	// no critical sections - only used for debugging - and caller can easily avoid trouble by being careful
	Memory::LeakTrackingGeneralPurposeAllocator::Snapshot	sLastSnapshot;
}
#endif
#if		qMSVisualStudioCRTMemoryDebug
namespace	{
	_CrtMemState	sLastSnapshot;
}
#endif
void	Memory::LeakChecker::ForceCheckpoint ()
{
	#if		qOverrideOpNewDeleteForAccounting && qOverrideOpNew_EXTRA_LEAK_DETECTION
		sLastSnapshot = GetAllocator_ ().GetSnapshot ();
	#elif	qMSVisualStudioCRTMemoryDebug
		_CrtMemCheckpoint (&sLastSnapshot);
	#else
		Assert (false);
	#endif
}

void	Memory::LeakChecker::DumpLeaksSinceLastCheckpoint ()
{
	TraceContextBumper	ctx (_T ("LeakChecker::DumpLeaksSinceLastCheckpoint"));
	#if		qOverrideOpNewDeleteForAccounting && qOverrideOpNew_EXTRA_LEAK_DETECTION
		GetAllocator_ ().DUMPCurMemStats (sLastSnapshot);
	#elif	qMSVisualStudioCRTMemoryDebug
		_CrtMemState	memState;
		_CrtMemCheckpoint (&memState);
		_CrtMemState	diffState;
		DbgTrace ("<<prevMemState>>:");
		_CrtMemDumpStatistics (&sLastSnapshot);	
		DbgTrace ("<<curMemState>>:");
		_CrtMemDumpStatistics (&memState);	
		if ( _CrtMemDifference (&diffState, &sLastSnapshot, &memState)) {
			DbgTrace ("<<LEAKED BLOCKS>>:");
			_CrtMemDumpStatistics (&diffState);
			//USELESS - seems to dump way too much....
			//_CrtMemDumpAllObjectsSince (&sLastSnapshot);
		}
	#else
		Assert (false);
	#endif
}

