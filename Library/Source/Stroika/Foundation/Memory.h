/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__Memory_h__
#define	__Memory_h__	1

#include	"StroikaPreComp.h"

#include	<Windows.h>

#include	<memory>

#include	"Support.h"




namespace	Stroika {	
	namespace	Foundation {



namespace	Memory {
	class	StackBasedHandleLocker {
		public:
			StackBasedHandleLocker (HANDLE h);
			~StackBasedHandleLocker ();
		private:
			StackBasedHandleLocker (const StackBasedHandleLocker&);
			const StackBasedHandleLocker& operator= (const StackBasedHandleLocker&);

		public:
			nonvirtual	Byte*	GetPointer () const;
			nonvirtual	size_t	GetSize () const;

		private:
			HANDLE	fHandle;
			Byte*	fPointer;
	};



	/* 
	 * Support for using the MSVC CRT memleak detector. If not available, then DBG_NEW simply expands
	 * to 'new' so it can be safely used in places where the global new operator would have been used.
	 *		-- LGP 2009-05-25
	 */
	#if		defined (_DEBUG)
		#define		_CRTDBG_MAP_ALLOC
		#include	<crtdbg.h>
		#define DEBUG_NEW	new (_NORMAL_BLOCK, __FILE__, __LINE__)
	#else
	   #define DEBUG_NEW	new
	#endif // _DEBUG




	/*
	 *	API to return memory allocation statistics. Generally - these will be inaccurate,
	 *	unless certain defines are set in Memory.cpp - but at least some stats can be
	 *	returned in either case.
	 */
	struct	GlobalAllocationStatistics {
		GlobalAllocationStatistics ();

		size_t	fTotalOutstandingAllocations;
		size_t	fTotalOutstandingBytesAllocated;
		size_t	fPageFaultCount;
		size_t	fWorkingSetSize;
		size_t	fPagefileUsage;
	};
	GlobalAllocationStatistics	GetGlobalAllocationStatistics ();



	namespace	LeakChecker {
		// Note - this functionality is just for debugging, and may be disabled inside the Memory leakchecker module, in which
		// case it will link but do nothing...
		void	ForceCheckpoint ();
		void	DumpLeaksSinceLastCheckpoint ();
	}
}


	}
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Memory.inl"

#endif	/*__Memory_h__*/
