/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <atomic>
#include <exception>
#if qPlatform_Linux
#include <malloc.h>
#endif

#if qPlatform_POSIX
#include <unistd.h>
#endif

#include "../Memory/Common.h"

#include "Trace.h"

#include "MallocGuard.h"

using namespace Stroika::Foundation;
using Memory::Byte;

#if qStroika_Foundation_Debug_MallocGuard
namespace {
    constexpr array<Byte, 16> kMallocGuardHeader_BASE_{
        0xf3,
        0xfa,
        0x0b,
        0x93,
        0x48,
        0x50,
        0x46,
        0xe6,
        0x22,
        0xf1,
        0xfa,
        0xc0,
        0x9a,
        0x0b,
        0xeb,
        0x23,
    };
    constexpr array<Byte, 16> kMallocGuardFooter_BASE_{
        0x07,
        0x41,
        0xa4,
        0x2b,
        0xba,
        0x97,
        0xcb,
        0x38,
        0x46,
        0x1e,
        0x3c,
        0x42,
        0x3c,
        0x5f,
        0x0c,
        0x80,
    };

    using GuradBytes_ = array<Byte, qStroika_Foundation_Debug_MallocGuard_GuardSize>;
#if qStroika_Foundation_Debug_MallocGuard_GuardSize == 16
    constexpr GuradBytes_ kMallocGuardHeader_{kMallocGuardHeader_BASE_};
    constexpr GuradBytes_ kMallocGuardFooter_{kMallocGuardFooter_BASE_};
#else
    const GuradBytes_ kMallocGuardHeader_;
    const GuradBytes_ kMallocGuardFooter_;
    struct DoInit_ ()
    {
        DoInit_ ()
        {
            size_t fromI = 0;
            for (size_t i = 0; i < NEltsOf (kMallocGuardHeader_); ++i) {
                kMallocGuardHeader_[i] = kMallocGuardHeader_BASE_[fromI];
                kMallocGuardFooter_[i] = kMallocGuardFooter_[fromI];
                fromI++;
                if (fromI >= NEltsOf (kMallocGuardHeader_BASE_)) {
                    fromI = 0;
                }
            }
        }
    }
}
sDoInit_x_;
#endif
    constexpr Byte kDeadMansLand_[] = {
        0x1d,
        0xb6,
        0x20,
        0x27,
        0x43,
        0x7a,
        0x3d,
        0x1a,
        0x13,
        0x65,
    };

    struct alignas (alignof (long double)) Header_ {
        size_t      fRequestedBlockSize;
        GuradBytes_ fGuard;
    };
    struct alignas (alignof (long double)) Footer_ {
        GuradBytes_ fGuard;
        size_t      fRequestedBlockSize;
    };

    void OhShit_ (const char* why)
    {
        static bool sDone_{false}; // doing terminate MIGHT allocate more memory ... just go with the flow if that happens - and dont re-barf (e.g. allow backtrace if possible)
        if (not sDone_) {
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wunused-result\"")
            sDone_             = true;
            const char kMsg_[] = "Fatal Error detected in Stroika Malloc Guard\n";
            ::write (2, kMsg_, NEltsOf (kMsg_));
            DbgTrace ("%s", kMsg_);
            {
                ::write (2, why, ::strlen (why));
                ::write (2, "\n", 1);
                DbgTrace ("%s", why);
            }
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wunused-result\"")
            terminate ();
        }
    }

    void* ExposedPtrToBackendPtr_ (void* p)
    {
        if (p == nullptr) {
            OhShit_ ("unexpected nullptr in ExposedPtrToBackendPtr_");
        }
        return reinterpret_cast<Header_*> (p) - 1;
    }
    void* BackendPtrToExposedPtr_ (void* p)
    {
        if (p == nullptr) {
            OhShit_ ("unexpected nullptr in BackendPtrToExposedPtr_");
        }
        return reinterpret_cast<Header_*> (p) + 1;
    }
    size_t AdjustMallocSize_ (size_t s)
    {
        // Header_ before 's' and after 's'
        return sizeof (Header_) + s + sizeof (Footer_);
    }

    bool IsDeadMansLand_ (const Byte* s, const Byte* e)
    {
        // NYI cuz not clear if/how/where to use...
        return false;
    }
    void SetDeadMansLand_ (Byte* s, Byte* e)
    {
        const Byte* pBadFillStart = begin (kDeadMansLand_);
        const Byte* pBadFillEnd   = end (kDeadMansLand_);
        const Byte* badFillI      = pBadFillStart;
        for (Byte* oi = s; oi != e; ++oi) {
            *oi = *badFillI;
            badFillI++;
            if (badFillI == pBadFillEnd) {
                badFillI = pBadFillStart;
            }
        }
    }
    void SetDeadMansLand_ (void* p)
    {
        const Header_* hp = reinterpret_cast<const Header_*> (p);
        SetDeadMansLand_ (reinterpret_cast<Byte*> (p), reinterpret_cast<Byte*> (p) + AdjustMallocSize_ (hp->fRequestedBlockSize));
    }

    /*
     *  Not 100% threadsafe, but probably OK.
     *
     *  If we add something and it doesn't really get added, we just miss the opporuntity to detect a bug, but we don't create one.
     *
     *  Assure we flush any entries CLEARED with ClearFromFreeList_ (why we use atomic_store_explicit)
     *
     *  Also FreeList alway uses BackendPtr - not ExternalPtr
     */
    volatile void*  sFreeList_[100];
    volatile void** sFreeList_NextFreeI_ = &sFreeList_[0];
    void            Add2FreeList_ (void* p)
    {
        *sFreeList_NextFreeI_ = p;
        volatile void** next  = sFreeList_NextFreeI_ + 1;
        if (next >= end (sFreeList_)) {
            next = begin (sFreeList_);
        }
        sFreeList_NextFreeI_ = next; // race in that we could SKIP recording a free element, but thats harmless - just a missed opportunity to detect an error
    }
    void* MyAtomicLoad_ (volatile void** p)
    {
        //unclear why this doesn't work....return std::atomic_load_explicit (p, memory_order_acquire);
        // unclear that this is safe (but we do in Thread.cpp code too)
        return atomic_load_explicit (reinterpret_cast<volatile atomic<void*>*> (p), memory_order_acquire);
    }
    void MyAtomicStore_ (volatile void** p, void* value)
    {
        //unclear why this doesn't work....std::atomic_store_explicit (p, nullptr, memory_order_release);
        // unclear that this is safe (but we do in Thread.cpp code too)
        atomic_store_explicit (reinterpret_cast<volatile atomic<void*>*> (p), value, memory_order_release);
    }
    void ClearFromFreeList_ (void* p)
    {
        // not a race because you cannot free and allocate the same pointer at the same time
        for (volatile void** i = begin (sFreeList_); i != end (sFreeList_); ++i) {
            if (MyAtomicLoad_ (i) == p) {
                MyAtomicStore_ (i, nullptr);
            }
        }
    }
    bool IsInFreeList_ (const void* p)
    {
        for (volatile void** i = begin (sFreeList_); i != end (sFreeList_); ++i) {
            if (MyAtomicLoad_ (i) == p) {
                return true;
            }
        }
        return false;
    }

    void Validate_ (const Header_& header, const Footer_& footer)
    {
        if (::memcmp (&header.fGuard, &kMallocGuardHeader_, sizeof (kMallocGuardHeader_)) != 0) {
            OhShit_ ("Invalid leading header guard");
        }
        if (::memcmp (&footer.fGuard, &kMallocGuardFooter_, sizeof (kMallocGuardFooter_)) != 0) {
            OhShit_ ("Invalid trailing footer guard");
        }
        if (header.fRequestedBlockSize != footer.fRequestedBlockSize) {
            OhShit_ ("Mismatch between header/trailer block sizes");
        }
        // OK
    }
    void ValidateBackendPtr_ (const void* p)
    {
        if (IsInFreeList_ (p)) {
            // check FIRST because if freed, the header will be all corrupted
            OhShit_ ("Pointer already freed (recently)");
        }
        const Header_* hp = reinterpret_cast<const Header_*> (p);
        const Footer_* fp = reinterpret_cast<const Footer_*> (reinterpret_cast<const Byte*> (hp + 1) + hp->fRequestedBlockSize);
        Footer_        footer;                         //tmporary so aligned
        (void)::memcpy (&footer, fp, sizeof (footer)); // align access
        Validate_ (*hp, footer);
    }

    void PatchNewPointer_ (void* p, size_t requestedSize)
    {
        Header_* hp = reinterpret_cast<Header_*> (p);
        (void)::memcpy (begin (hp->fGuard), begin (kMallocGuardHeader_), kMallocGuardHeader_.size ());
        hp->fRequestedBlockSize = requestedSize;
        Footer_* fp             = reinterpret_cast<Footer_*> (reinterpret_cast<Byte*> (hp + 1) + hp->fRequestedBlockSize);
        (void)::memcpy (begin (fp->fGuard), begin (kMallocGuardFooter_), kMallocGuardFooter_.size ());
        fp->fRequestedBlockSize = requestedSize;
    }
}
#endif

#if qStroika_Foundation_Debug_MallocGuard

extern "C" void  __libc_free (void* __ptr);
extern "C" void* __libc_malloc (size_t __size);
extern "C" void* __libc_realloc (void* __ptr, size_t __size);
extern "C" void* __libc_calloc (size_t __nmemb, size_t __size);
extern "C" void  __libc_free (void* __ptr);

extern "C" void* calloc (size_t __nmemb, size_t __size)
{
    size_t n = __nmemb * __size;
    void*  p = malloc (n);
    (void)::memset (p, 0, n);
    return p;
}

extern "C" void cfree (void* __ptr)
{
    free (__ptr);
}

extern "C" void free (void* __ptr)
{
    if (__ptr == nullptr) {
        // according to http://linux.die.net/man/3/free
        // "if ptr is NULL, no operation is performed." - and glibc does call this internally
        return;
    }
    void* p = ExposedPtrToBackendPtr_ (__ptr);
    ValidateBackendPtr_ (p);
    SetDeadMansLand_ (p);
    Add2FreeList_ (p);
    __libc_free (p);
}

extern "C" void* malloc (size_t __size)
{
    void* p = __libc_malloc (AdjustMallocSize_ (__size));
    PatchNewPointer_ (p, __size);
    ClearFromFreeList_ (p);
    ValidateBackendPtr_ (p);
    if (p != nullptr) {
        p = BackendPtrToExposedPtr_ (p);
    }
    return p;
}

extern "C" void* realloc (void* __ptr, size_t __size)
{
    if (__ptr == nullptr) {
        // from http://linux.die.net/man/3/realloc
        // If ptr is NULL, then the call is equivalent to malloc(size),
        return malloc (__size);
    }
    if (__ptr != nullptr and __size == 0) {
        // from http://linux.die.net/man/3/realloc
        // if size is equal to zero, and ptr is not NULL, then the call is equivalent to free(ptr).
        free (__ptr);
        return nullptr;
    }
    void* p = ExposedPtrToBackendPtr_ (__ptr);
    ValidateBackendPtr_ (p);
    size_t n    = AdjustMallocSize_ (__size);
    void*  newP = __libc_realloc (p, n);
    if (newP != nullptr) {
        PatchNewPointer_ (newP, __size);
        if (newP != p) {
            //Already been freed, so not safe to set at this point!!! - SetDeadMansLand_ (p);
            Add2FreeList_ (p);
            ClearFromFreeList_ (newP);
        }
        ValidateBackendPtr_ (newP);
        newP = BackendPtrToExposedPtr_ (newP);
    }
    return newP;
}

extern "C" void* valloc (size_t __size)
{
    // http://linux.die.net/man/3/valloc "OBSOLETE"
    OhShit_ ("valloc not supported in qStroika_Foundation_Debug_MallocGuard (valloc is OBSOLETE)");
    return nullptr;
}

extern "C" void* pvalloc (size_t __size)
{
    // http://linux.die.net/man/3/valloc "OBSOLETE"
    OhShit_ ("pvalloc not supported in qStroika_Foundation_Debug_MallocGuard (pvalloc is OBSOLETE)");
    return nullptr;
}

extern "C" void* memalign (size_t __alignment, size_t __size)
{
    // http://linux.die.net/man/3/valloc "OBSOLETE"
    OhShit_ ("memalign not supported in qStroika_Foundation_Debug_MallocGuard (memalign is OBSOLETE)");
    return nullptr;
}

extern "C" size_t malloc_usable_size (void* ptr)
{
    if (ptr == nullptr) {
        return 0;
    }
    void* p = ExposedPtrToBackendPtr_ (ptr);
    ValidateBackendPtr_ (p);
    const Header_* hp = reinterpret_cast<const Header_*> (p);
    return hp->fRequestedBlockSize;
}

extern "C" int posix_memalign (void** memptr, size_t alignment, size_t size)
{
    // Probably SHOULD implement ... but so far not running into trouble cuz anything I link to calling this...
    OhShit_ ("posix_memalign () not supported in qStroika_Foundation_Debug_MallocGuard (it should be)");
    return 0;
}
#endif
