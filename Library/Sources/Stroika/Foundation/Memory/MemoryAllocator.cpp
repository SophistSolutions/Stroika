/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <atomic>
#include <cstdlib>
#include <new>
#include <set>

#include "../Debug/Debugger.h"
#include "../Execution/Common.h"
#include "../Execution/ModuleInit.h"
#include "../Execution/Throw.h"

#include "MemoryAllocator.h"

using std::byte;

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Memory;

using Debug::TraceContextBumper;

namespace {
    SimpleAllocator_CallLIBCMallocFree sDefaultAllocator_;
}

// Since this code frequently gets used with 'DEBUG' turned off - and so no assert checking, we may
// sometimes want to 'force asserts on' at least for this modules checking.
//#define   qSuperAssertChecks_MemAllocator 0
//#define   qSuperAssertChecks_MemAllocator 1
#ifndef qSuperAssertChecks_MemAllocator
#define qSuperAssertChecks_MemAllocator qDebug
#endif

namespace {
#if qSuperAssertChecks_MemAllocator
    inline void SUPER_ASSERT_ (bool t)
    {
        if constexpr (qDebug) {
            Assert (t);
        }
        else {
            if (not t) {
                Debug::DropIntoDebuggerIfPresent ();
            }
        }
    }
#else
#define SUPER_ASSERT_(x)
#endif
}

/*
 ********************************************************************************
 ********************* Memory::SimpleAllocator_CallLIBCMallocFree ***************
 ********************************************************************************
 */
void* SimpleAllocator_CallLIBCMallocFree::Allocate (size_t size)
{
    void* p = malloc (size);
    if (p == nullptr)
        [[UNLIKELY_ATTR]]
        {
            Execution::Throw (bad_alloc ());
        }
    return p;
}

void SimpleAllocator_CallLIBCMallocFree::Deallocate (void* p)
{
    RequireNotNull (p);
    free (p);
}

/*
 ********************************************************************************
 ******************* Memory::SimpleAllocator_CallLIBCNewDelete ******************
 ********************************************************************************
 */
void* SimpleAllocator_CallLIBCNewDelete::Allocate (size_t size)
{
    return ::operator new (size);
}

void SimpleAllocator_CallLIBCNewDelete::Deallocate (void* p)
{
    RequireNotNull (p);
    ::operator delete (p);
}

namespace {
    const unsigned int kPreGUARD   = 0x39;
    const unsigned int kPost_GUARD = 0x1f;
    // force alignment to worst we can be required to support, so memory allocated by our object has good alignment
    struct alignas (double) MemWithExtraStuff {
        unsigned int fPreGuard;
        size_t       fBlockSize;
    };
}

/*
 ********************************************************************************
 ************* Memory::SimpleSizeCountingGeneralPurposeAllocator ****************
 ********************************************************************************
 */
SimpleSizeCountingGeneralPurposeAllocator::SimpleSizeCountingGeneralPurposeAllocator ()
    : fBaseAllocator_ (sDefaultAllocator_)
    , fNetAllocationCount_ (0)
    , fNetAllocatedByteCount_ (0)
{
}

SimpleSizeCountingGeneralPurposeAllocator::SimpleSizeCountingGeneralPurposeAllocator (AbstractGeneralPurposeAllocator& baseAllocator)
    : fBaseAllocator_ (baseAllocator)
    , fNetAllocationCount_ (0)
    , fNetAllocatedByteCount_ (0)
{
}

SimpleSizeCountingGeneralPurposeAllocator::~SimpleSizeCountingGeneralPurposeAllocator ()
{
    //TraceContextBumper trcCtx ("SimpleSizeCountingGeneralPurposeAllocator::~SimpleSizeCountingGeneralPurposeAllocator");
    //DbgTrace (L"fNetAllocationCount=%d, fNetAllocatedByteCount=%d", fNetAllocationCount, fNetAllocatedByteCount);
    // caller must free all entries before destroying allocator
    Require (fNetAllocationCount_ == 0);
    Require (fNetAllocatedByteCount_ == 0);
}

void* SimpleSizeCountingGeneralPurposeAllocator::Allocate (size_t size)
{
    size_t             effectiveSize = size + sizeof (MemWithExtraStuff) + sizeof (unsigned int);
    MemWithExtraStuff* p             = reinterpret_cast<MemWithExtraStuff*> (fBaseAllocator_.Allocate (effectiveSize));
    p->fPreGuard                     = kPreGUARD;
    p->fBlockSize                    = size;
    memcpy (reinterpret_cast<byte*> (p) + size + sizeof (MemWithExtraStuff), &kPost_GUARD, sizeof (kPost_GUARD));
    fNetAllocationCount_++;
    fNetAllocatedByteCount_ += static_cast<int32_t> (size);
    return (reinterpret_cast<byte*> (p) + sizeof (MemWithExtraStuff));
}

void SimpleSizeCountingGeneralPurposeAllocator::Deallocate (void* ptr)
{
    RequireNotNull (ptr);
    MemWithExtraStuff* p = reinterpret_cast<MemWithExtraStuff*> (reinterpret_cast<byte*> (ptr) - sizeof (MemWithExtraStuff));
    SUPER_ASSERT_ (p->fPreGuard == kPreGUARD);
    SUPER_ASSERT_ (::memcmp (reinterpret_cast<byte*> (p) + p->fBlockSize + sizeof (MemWithExtraStuff), &kPost_GUARD, sizeof (kPost_GUARD)) == 0);
    --fNetAllocationCount_;
    fNetAllocatedByteCount_ -= p->fBlockSize;
    fBaseAllocator_.Deallocate (p);
}

size_t SimpleSizeCountingGeneralPurposeAllocator::GetNetAllocationCount () const
{
    Require (fNetAllocationCount_ >= 0); // bad use of this class - not a bug with the class - if this fails (probably)
    return static_cast<size_t> (fNetAllocationCount_);
}

size_t SimpleSizeCountingGeneralPurposeAllocator::GetNetAllocatedByteCount () const
{
    Require (fNetAllocatedByteCount_ >= 0); // bad use of this class - not a bug with the class - if this fails (probably)
    return static_cast<size_t> (fNetAllocatedByteCount_);
}

/*
 ********************************************************************************
 ********************* Memory::LeakTrackingGeneralPurposeAllocator **************
 ********************************************************************************
 */
namespace {
    using PTRMAP = LeakTrackingGeneralPurposeAllocator::PTRMAP;
    void ExtractInfo_ (const PTRMAP& m, set<size_t>* sizeSet, size_t* totalAllocated)
    {
        RequireNotNull (sizeSet);
        RequireNotNull (totalAllocated);
        sizeSet->clear ();
        *totalAllocated = 0;
        for (auto i = m.begin (); i != m.end (); ++i) {
            sizeSet->insert (i->second);
            (*totalAllocated) += i->second;
        }
    }
    unsigned int ExtractCountUsedForSize_ (const PTRMAP& m, size_t eltSize)
    {
        unsigned int result = 0;
        for (auto i = m.begin (); i != m.end (); ++i) {
            if (i->second == eltSize) {
                result++;
            }
        }
        return result;
    }
}
LeakTrackingGeneralPurposeAllocator::LeakTrackingGeneralPurposeAllocator ()
    : fCritSection_ ()
    , fBaseAllocator_ (sDefaultAllocator_)
    , fAllocations_ ()
{
}

LeakTrackingGeneralPurposeAllocator::LeakTrackingGeneralPurposeAllocator (AbstractGeneralPurposeAllocator& baseAllocator)
    : fCritSection_ ()
    , fBaseAllocator_ (baseAllocator)
    , fAllocations_ ()
{
}

LeakTrackingGeneralPurposeAllocator::~LeakTrackingGeneralPurposeAllocator ()
{
    // Avoid DbgTrace (etc) calls here - since when we do 'global op new' debugging, this gets deleted VERY late - after
    // most other modules (including DbgTrace support) are shutdown)
    // But - Assert OK ;-)
    //      -LGP 2008-05-27
    SUPER_ASSERT_ (fAllocations_.size () == 0);
}

void* LeakTrackingGeneralPurposeAllocator::Allocate (size_t size)
{
    void* memptr = fBaseAllocator_.Allocate (size);
    AssertNotNull (memptr);
    [[maybe_unused]] auto&& critSec = lock_guard{fCritSection_};
    try {
        fAllocations_.insert (PTRMAP::value_type (memptr, size));
        return memptr;
    }
    catch (...) {
        fBaseAllocator_.Deallocate (memptr);
        Execution::ReThrow ();
    }
}

void LeakTrackingGeneralPurposeAllocator::Deallocate (void* p)
{
    RequireNotNull (p);
    [[maybe_unused]] auto&& critSec = lock_guard{fCritSection_};
    PTRMAP::iterator        i       = fAllocations_.find (p);
    SUPER_ASSERT_ (i != fAllocations_.end ());
    fAllocations_.erase (i);
    fBaseAllocator_.Deallocate (p);
}

size_t LeakTrackingGeneralPurposeAllocator::GetNetAllocationCount () const
{
    [[maybe_unused]] auto&& critSec = lock_guard{fCritSection_};
    return fAllocations_.size ();
}

size_t LeakTrackingGeneralPurposeAllocator::GetNetAllocatedByteCount () const
{
    [[maybe_unused]] auto&& critSec = lock_guard{fCritSection_};
    size_t                  total   = 0;
    for (auto i = fAllocations_.begin (); i != fAllocations_.end (); ++i) {
        total += i->second;
    }
    return total;
}

LeakTrackingGeneralPurposeAllocator::Snapshot LeakTrackingGeneralPurposeAllocator::GetSnapshot () const
{
    [[maybe_unused]] auto&& critSec = lock_guard{fCritSection_};
    return Snapshot (fAllocations_);
}

namespace {
    void DUMPCurMemStats_ (const LeakTrackingGeneralPurposeAllocator::Snapshot& curSnapshot, const LeakTrackingGeneralPurposeAllocator::Snapshot& sinceSnapshot)
    {
        set<size_t> sizes;
        set<size_t> prevSizes;
        size_t      totalRemainingAlloced     = 0;
        size_t      prevTotalRemainingAlloced = 0;
        ExtractInfo_ (curSnapshot.fAllocations, &sizes, &totalRemainingAlloced);
        ExtractInfo_ (sinceSnapshot.fAllocations, &prevSizes, &prevTotalRemainingAlloced);
        DbgTrace ("Net Allocation Count = %d (prev %d)", curSnapshot.fAllocations.size (), sinceSnapshot.fAllocations.size ());
        DbgTrace ("Net Allocation byte Count = %d (prev %d)", totalRemainingAlloced, prevTotalRemainingAlloced);
        if (totalRemainingAlloced > prevTotalRemainingAlloced) {
            DbgTrace ("Leaked %d bytes", totalRemainingAlloced - prevTotalRemainingAlloced);
        }
        else if (prevTotalRemainingAlloced > totalRemainingAlloced) {
            DbgTrace ("Reverse-Leaked %d bytes", prevTotalRemainingAlloced - totalRemainingAlloced);
        }
#if 0
        {
            //REALLY COULD USE STROIKA TALLY HERE
            for (auto si = sizes.begin (); si != sizes.end (); ++si) {
                unsigned int cnt    =   0;
                for (auto i = fAllocMap.begin (); i != fAllocMap.end (); ++i) {
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
            set<size_t>::const_iterator psi = prevSizes.begin ();
            for (auto si = sizes.begin (); si != sizes.end ();) {
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
                        unsigned int oldCountThisSize = ExtractCountUsedForSize_ (sinceSnapshot.fAllocations, *si);
                        unsigned int newCountThisSize = ExtractCountUsedForSize_ (curSnapshot.fAllocations, *si);
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
void LeakTrackingGeneralPurposeAllocator::DUMPCurMemStats (const Snapshot& sinceSnapshot)
{
    Snapshot           curSnapshot = GetSnapshot ();
    TraceContextBumper ctx ("LeakTrackingGeneralPurposeAllocator::DUMPCurMemStats");
    DUMPCurMemStats_ (curSnapshot, sinceSnapshot);
}

LeakTrackingGeneralPurposeAllocator::Snapshot::Snapshot ()
    : fAllocations ()
{
}

LeakTrackingGeneralPurposeAllocator::Snapshot::Snapshot (const PTRMAP& m)
    : fAllocations (m)
{
}
