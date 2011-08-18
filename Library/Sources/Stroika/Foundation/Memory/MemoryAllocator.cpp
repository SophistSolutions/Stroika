/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<new>
#include	<set>

#include	"../Execution/AtomicOperations.h"
#include	"../Execution/ModuleInit.h"

#include	"MemoryAllocator.h"



using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Memory;


using	Execution::AutoCriticalSection;

using	Debug::TraceContextBumper;



namespace	{
	Execution::StaticSingletonObjectConstructionHelper<SimpleAllocator_CallLIBCMallocFree>	sDefaultAllocator;
}


// Since this code frequently gets used with 'DEBUG' turned off - and so no assert checking, we may
// sometimes want to 'force asserts on' at least for this modules checking.
//#define	qSuperAssertChecks_MemAllocator	0
//#define	qSuperAssertChecks_MemAllocator	1
#ifndef	qSuperAssertChecks_MemAllocator
#define	qSuperAssertChecks_MemAllocator	qDebug
#endif

namespace	{
	#if		qSuperAssertChecks_MemAllocator
	inline	void	SUPER_ASSERT_ (bool t)
		{
			#if		qDebug
				Assert (t);
			#else
				if (not t) {
					DebugBreak ();
				}
			#endif
		}
	#else
		#define	SUPER_ASSERT_(x)
	#endif
}





/*
 ********************************************************************************
 ********************* Memory::SimpleAllocator_CallLIBCMallocFree ***************
 ********************************************************************************
 */
void*	SimpleAllocator_CallLIBCMallocFree::Allocate (size_t size)
{
	void*	p	=	malloc (size);
	if (p == NULL) {
		throw bad_alloc ();
	}
	return p;
}

void	SimpleAllocator_CallLIBCMallocFree::Deallocate (void* p)
{
	RequireNotNil (p);
	free (p);
}






/*
 ********************************************************************************
 ******************* Memory::SimpleAllocator_CallLIBCNewDelete ******************
 ********************************************************************************
 */
void*	SimpleAllocator_CallLIBCNewDelete::Allocate (size_t size)
{
	return ::operator new (size);
}

void	SimpleAllocator_CallLIBCNewDelete::Deallocate (void* p)
{
	RequireNotNil (p);
	::operator delete (p);
}






namespace	{
	const	unsigned int	kPreGUARD	=	0x39;
	const	unsigned int	kPost_GUARD	=	0x1f;
	struct	MemWithExtraStuff {
		union {
			struct	{
				unsigned int	fPreGuard;
				size_t			fBlockSize;
			};
			double	fOtherStuff;	// hack so we get right alignment
		};
	};
}





/*
 ********************************************************************************
 ************* Memory::SimpleSizeCountingGeneralPurposeAllocator ****************
 ********************************************************************************
 */
SimpleSizeCountingGeneralPurposeAllocator::SimpleSizeCountingGeneralPurposeAllocator ():
	fBaseAllocator (sDefaultAllocator),
	fNetAllocationCount (0),
	fNetAllocatedByteCount (0)
{
}

SimpleSizeCountingGeneralPurposeAllocator::SimpleSizeCountingGeneralPurposeAllocator (AbstractGeneralPurposeAllocator& baseAllocator):
	fBaseAllocator (baseAllocator),
	fNetAllocationCount (0),
	fNetAllocatedByteCount (0)
{
}

SimpleSizeCountingGeneralPurposeAllocator::~SimpleSizeCountingGeneralPurposeAllocator ()
{
	//TraceContextBumper trcCtx (_T ("SimpleSizeCountingGeneralPurposeAllocator::~SimpleSizeCountingGeneralPurposeAllocator"));
	//DbgTrace (L"fNetAllocationCount=%d, fNetAllocatedByteCount=%d", fNetAllocationCount, fNetAllocatedByteCount);
	// caller must free all entries before destroying allocator
	Require (fNetAllocationCount == 0);
	Require (fNetAllocatedByteCount == 0);
}

void*	SimpleSizeCountingGeneralPurposeAllocator::Allocate (size_t size)
{
	size_t				effectiveSize	=	size + sizeof (MemWithExtraStuff) + sizeof (unsigned int);
	MemWithExtraStuff*	p				=	reinterpret_cast<MemWithExtraStuff*> (fBaseAllocator.Allocate (effectiveSize));
	p->fPreGuard = kPreGUARD;
	p->fBlockSize = size;
	memcpy (reinterpret_cast<Byte*> (p) + size + sizeof (MemWithExtraStuff), &kPost_GUARD, sizeof (kPost_GUARD));
	Execution::AtomicIncrement (&fNetAllocationCount);
	Execution::AtomicAdd (&fNetAllocatedByteCount, static_cast<int32_t> (size));
	return (reinterpret_cast<Byte*> (p) + sizeof (MemWithExtraStuff));
}

void	SimpleSizeCountingGeneralPurposeAllocator::Deallocate (void* ptr)
{
	RequireNotNil (ptr);
	MemWithExtraStuff*	p	=	reinterpret_cast<MemWithExtraStuff*> (reinterpret_cast<Byte*> (ptr) - sizeof (MemWithExtraStuff));
	SUPER_ASSERT_ (p->fPreGuard == kPreGUARD);
	SUPER_ASSERT_ (::memcmp (reinterpret_cast<Byte*> (p) + p->fBlockSize + sizeof (MemWithExtraStuff), &kPost_GUARD, sizeof (kPost_GUARD)) == 0);
	Execution::AtomicDecrement (&fNetAllocationCount);
	Execution::AtomicSubtract (&fNetAllocatedByteCount, p->fBlockSize);
	fBaseAllocator.Deallocate (p);
}

size_t	SimpleSizeCountingGeneralPurposeAllocator::GetNetAllocationCount () const
{
	Require (fNetAllocationCount >= 0);	// bad use of this class - not a bug with the class - if this fails (probably)
	return static_cast<size_t> (fNetAllocationCount);
}

size_t	SimpleSizeCountingGeneralPurposeAllocator::GetNetAllocatedByteCount () const
{
	Require (fNetAllocatedByteCount >= 0);	// bad use of this class - not a bug with the class - if this fails (probably)
	return static_cast<size_t> (fNetAllocatedByteCount);
}














/*
 ********************************************************************************
 ********************* Memory::LeakTrackingGeneralPurposeAllocator **************
 ********************************************************************************
 */
namespace	{
	typedef	LeakTrackingGeneralPurposeAllocator::PTRMAP	PTRMAP;
	void	ExtractInfo_ (const PTRMAP& m, set<size_t>* sizeSet, size_t* totalAllocated)
		{
			RequireNotNil (sizeSet);
			RequireNotNil (totalAllocated);
			sizeSet->clear ();
			*totalAllocated = 0;
			for (PTRMAP::const_iterator i = m.begin (); i != m.end (); ++i) {
				sizeSet->insert (i->second);
				(*totalAllocated) += i->second;
			}
		}
	unsigned int	ExtractCountUsedForSize_ (const PTRMAP& m, size_t eltSize)
		{
			unsigned int	result	=	0;
			for (PTRMAP::const_iterator i = m.begin (); i != m.end (); ++i) {
				if (i->second == eltSize) {
					result++;
				}
			}
			return result;
		}
}
LeakTrackingGeneralPurposeAllocator::LeakTrackingGeneralPurposeAllocator ():
	fBaseAllocator (sDefaultAllocator),
	fCritSection (),
	fAllocations ()
{
}

LeakTrackingGeneralPurposeAllocator::LeakTrackingGeneralPurposeAllocator (AbstractGeneralPurposeAllocator& baseAllocator):
	fBaseAllocator (baseAllocator),
	fCritSection (),
	fAllocations ()
{
}

LeakTrackingGeneralPurposeAllocator::~LeakTrackingGeneralPurposeAllocator ()
{
	// Avoid DbgTrace (etc) calls here - since when we do 'global op new' debugging, this gets deleted VERY late - after
	// most other modules (including DbgTrace support) are shutdown)
	// But - Assert OK ;-)
	//		-LGP 2008-05-27
	SUPER_ASSERT_ (fAllocations.size () == 0);
}

void*	LeakTrackingGeneralPurposeAllocator::Allocate (size_t size)
{
	void*	memptr	=	fBaseAllocator.Allocate (size);
	AutoCriticalSection enterCriticalSection (fCritSection);
	try {
		fAllocations.insert (PTRMAP::value_type (memptr, size));
		return memptr;
	}
	catch (...) {
		delete memptr;
		Execution::DoReThrow ();
	}
}

void	LeakTrackingGeneralPurposeAllocator::Deallocate (void* p)
{
	RequireNotNil (p);
	AutoCriticalSection enterCriticalSection (fCritSection);
	PTRMAP::iterator	i	=	fAllocations.find (p);
	SUPER_ASSERT_ (i != fAllocations.end ());
	fAllocations.erase (i);
	fBaseAllocator.Deallocate (p);
}

size_t	LeakTrackingGeneralPurposeAllocator::GetNetAllocationCount () const
{
	AutoCriticalSection enterCriticalSection (fCritSection);
	return fAllocations.size ();
}

size_t	LeakTrackingGeneralPurposeAllocator::GetNetAllocatedByteCount () const
{
	AutoCriticalSection enterCriticalSection (fCritSection);
	size_t	total	=	0;
	for (PTRMAP::const_iterator i = fAllocations.begin (); i != fAllocations.end (); ++i) {
		total += i->second;
	}
	return total;
}

LeakTrackingGeneralPurposeAllocator::Snapshot	LeakTrackingGeneralPurposeAllocator::GetSnapshot () const
{
	AutoCriticalSection enterCriticalSection (fCritSection);
	return Snapshot (fAllocations);
}

namespace	{
	void	DUMPCurMemStats_ (const LeakTrackingGeneralPurposeAllocator::Snapshot& curSnapshot, const LeakTrackingGeneralPurposeAllocator::Snapshot& sinceSnapshot)
		{
			set<size_t>	sizes;
			set<size_t>	prevSizes;
			size_t	totalRemainingAlloced	=	0;
			size_t	prevTotalRemainingAlloced	=	0;
			ExtractInfo_ (curSnapshot.fAllocations, &sizes, &totalRemainingAlloced);
			ExtractInfo_ (sinceSnapshot.fAllocations, &prevSizes, &prevTotalRemainingAlloced);
			DbgTrace ("Net Allocation Count = %d (prev %d)", curSnapshot.fAllocations.size (), sinceSnapshot.fAllocations.size ());
			DbgTrace ("Net Allocation Byte Count = %d (prev %d)", totalRemainingAlloced, prevTotalRemainingAlloced);
			if (totalRemainingAlloced > prevTotalRemainingAlloced) {
				DbgTrace ("Leaked %d bytes", totalRemainingAlloced - prevTotalRemainingAlloced);
			}
			else if (prevTotalRemainingAlloced > totalRemainingAlloced) {
				DbgTrace ("Reverse-Leaked %d bytes", prevTotalRemainingAlloced - totalRemainingAlloced);
			}
			#if 0
				{
					//REALLY COULD USE STROIKA TALLY HERE
					for (set<size_t>::const_iterator si = sizes.begin (); si != sizes.end (); ++si) {
						unsigned int cnt	=	0;
						for (PTRMAP::const_iterator i = fAllocMap.begin (); i != fAllocMap.end (); ++i) {
							if (i->second == *si) {
								cnt++;
							}
						}
						DbgTrace ("items of size %d, count %d", *si, cnt);
					}
				}
			#endif
			{
				// See how the current values differ from the previous run
				set<size_t>::const_iterator	psi	=	prevSizes.begin ();
				for (set<size_t>::const_iterator  si = sizes.begin (); si != sizes.end ();) {
					/*
					 * try to iterate two lists at same time - bump the pointer which is further behind
					 */
					if (psi == prevSizes.end ()) {
						// then we have 'leak' - new sizes
						DbgTrace ("Leak: new size bucket %d", *si);
						++si;
					}
					else {
						if (*si < *psi) {
							DbgTrace ("Leak: new size bucket %d", *si);
							++si;
						}
						else if (*si == *psi) {
							// then here we just shoudl chekc if same number of entries - and output THAT
							unsigned int	oldCountThisSize	=	ExtractCountUsedForSize_ (sinceSnapshot.fAllocations, *si);
							unsigned int	newCountThisSize	=	ExtractCountUsedForSize_ (curSnapshot.fAllocations, *si);
							if (oldCountThisSize < newCountThisSize) {
								DbgTrace ("Leak: for bucket size %d, oldCount=%d, newCount=%d", *si, oldCountThisSize, newCountThisSize);
							}
							else if (oldCountThisSize > newCountThisSize) {
								DbgTrace ("Reverse-Leak: for bucket size %d, oldCount=%d, newCount=%d", *si, oldCountThisSize, newCountThisSize);
							}
							++si;
							++psi;
						}
						else {
							DbgTrace ("Reverse-Leak: old size bucket %d", *psi);
							++psi;
						}
					}
				}
				while (psi != prevSizes.end ()) {
					DbgTrace ("Reverse-Leak: old size bucket %d", *psi);
					psi++;
				}
			}
		}
}
void	LeakTrackingGeneralPurposeAllocator::DUMPCurMemStats (const Snapshot& sinceSnapshot)
{
	Snapshot	curSnapshot	=	GetSnapshot ();
	TraceContextBumper ctx (_T ("LeakTrackingGeneralPurposeAllocator::DUMPCurMemStats"));
	DUMPCurMemStats_ (curSnapshot, sinceSnapshot);
}






LeakTrackingGeneralPurposeAllocator::Snapshot::Snapshot ():
	fAllocations ()
{
}

LeakTrackingGeneralPurposeAllocator::Snapshot::Snapshot (const PTRMAP& m):
	fAllocations (m)
{
}
