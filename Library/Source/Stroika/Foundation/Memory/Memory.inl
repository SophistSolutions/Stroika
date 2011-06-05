/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Memory_inl
#define	_Memory_inl	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Debug/Assertions.h"
#include	"../ModuleInit.h"



namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {


	//	class	Memory::StackBasedHandleLocker
		inline	StackBasedHandleLocker::StackBasedHandleLocker (HANDLE h):
			fHandle (h)
			{
				RequireNotNil (h);
				fPointer = reinterpret_cast<Byte*> (::GlobalLock (h));
			}
		inline	StackBasedHandleLocker::~StackBasedHandleLocker ()
			{
				::GlobalUnlock (fHandle);
			}
		inline	Byte*	StackBasedHandleLocker::GetPointer () const
			{
				return fPointer;
			}
		inline	size_t	StackBasedHandleLocker::GetSize () const
			{
				return ::GlobalSize (fHandle);
			}


	//	class	Memory::GlobalAllocationStatistics
		inline	GlobalAllocationStatistics::GlobalAllocationStatistics ():
			fTotalOutstandingAllocations (0),
			fTotalOutstandingBytesAllocated (0),
			fPageFaultCount (0),
			fWorkingSetSize (0),
			fPagefileUsage (0)
			{
			}


// Module initialization
		namespace	Private {
			struct	INIT {
				INIT ();
				~INIT ();
			};
		}


		}

	}
}


namespace	{
	Stroika::Foundation::ModuleInit::ModuleInitializer<Stroika::Foundation::Memory::Private::INIT>	_StroikaFoundationMemoryPrivateINIT_;	// this object constructed for the CTOR/DTOR per-module side-effects
}


#endif	/*_Memory_inl*/
