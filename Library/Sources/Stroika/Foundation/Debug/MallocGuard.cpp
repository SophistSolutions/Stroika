/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <exception>
#include    <malloc.h>

#if     qPlatform_POSIX
#include    <unistd.h>
#endif

#include    "../Memory/Common.h"

#include    "MallocGuard.h"



using   namespace   Stroika::Foundation;
using   Memory::Byte;


#if     qStroika_Foundation_Debug_MallocGuard
namespace {
    using   GuradBytes_ =   Byte[16];
    constexpr   GuradBytes_ kMallocGuardHeader_ =   { 0xf3, 0xfa, 0x0b, 0x93, 0x48, 0x50, 0x46, 0xe6, 0x22, 0xf1, 0xfa, 0xc0, 0x9a, 0x0b, 0xeb, 0x23, };
    constexpr   GuradBytes_ kMallocGuardFooter_ =   { 0x07, 0x41, 0xa4, 0x2b, 0xba, 0x97, 0xcb, 0x38, 0x46, 0x1e, 0x3c, 0x42, 0x3c, 0x5f, 0x0c, 0x80, };
    constexpr   Byte        kDeadMansLand_[]    =   { 0x1d, 0xb6, 0x20, 0x27, 0x43, 0x7a, 0x3d, 0x1a, 0x13, 0x65, };

    // need header with size, so we can ....
    struct alignas(alignof(long double))  HeaderOrFooter_ {
        GuradBytes_ fGuard;
        size_t      fRequestedBlockSize;
    };


    void    OhShit_ ()
    {
        static  bool    sDone_      { false };      // doing terminate MIGHT allocate more memory ... just go with the flow if that happens - and dont re-barf (e.g. allow backtrace if possible)
        if (not sDone_) {
            sDone_ = true;
            const char  kMsg_[] =   "Fatal Error detected in Stroika Malloc Guard\n";
            ::write (2, kMsg_, NEltsOf (kMsg_));
            std::terminate ();
        }
    }


    void*   ExposedPtrToBackendPtr_ (void* p)
    {
        if (p == nullptr) {
            OhShit_ ();
        }
        return reinterpret_cast<HeaderOrFooter_*> (p) - 1;
    }
    void*   BackendPtrToExposedPtr_ (void* p)
    {
        if (p == nullptr) {
            OhShit_ ();
        }
        return reinterpret_cast<HeaderOrFooter_*> (p) + 1;
    }
    size_t  AdjustMallocSize_ (size_t s)
    {
        return s + 2 * sizeof (HeaderOrFooter_);
    }

    bool    IsDeadMansLand_ (const Byte* s, const Byte* e)
    {
        return false;
    }
    void    SetDeadMansLand_ (Byte* s, Byte* e)
    {
        const   Byte*   pBadFillStart   =   begin (kDeadMansLand_);
        const   Byte*   pBadFillEnd     =   end (kDeadMansLand_);
        const   Byte*   badFillI        =   pBadFillStart;
        for (Byte* oi = s; oi != e; ++oi) {
            *oi = *badFillI;
            badFillI++;
            if (badFillI == pBadFillEnd) {
                badFillI = pBadFillStart;
            }
        }
    }
    void    SetDeadMansLand_ (void* p)
    {
        const HeaderOrFooter_*  hp  =   reinterpret_cast<const HeaderOrFooter_*> (p);
        SetDeadMansLand_ (reinterpret_cast<Byte*> (p), reinterpret_cast<Byte*> (p) + AdjustMallocSize_ (hp->fRequestedBlockSize));
    }

    /*
     *  not 100% threadsafe, but OK.
     *
     *  Also FreeList alway uses BackendPtr - not ExternalPtr
     */
    void*   sFreeList_[100];
    void**  sFreeList_NextFreeI_    =   &sFreeList_[0];
    void    Add2FreeList_ (void* p)
    {
        *sFreeList_NextFreeI_ = p;
        void**  next = sFreeList_NextFreeI_ + 1;
        if (next >= end (sFreeList_)) {
            next = begin (sFreeList_);
        }
        sFreeList_NextFreeI_ = next;    // race in that we could SKIP recording a free element, but thats harmless - just a missed opportunity to detect an error
    }
    void    ClearFromFreeList_ (void* p)
    {
        // not a race because you cannot free and allocate the same pointer at the same time
        for (void** i = begin (sFreeList_); i != end (sFreeList_); ++i) {
            if (*i == p) {
                *i = nullptr;
            }
        }
    }
    bool    IsInFreeList_ (const void* p)
    {
        for (void** i = begin (sFreeList_); i != end (sFreeList_); ++i) {
            if (*i == p) {
                return true;
            }
        }
        return false;
    }

    void    Validate_ (const HeaderOrFooter_& header, const HeaderOrFooter_& footer)
    {
        if (::memcmp (&header.fGuard, &kMallocGuardHeader_, sizeof (kMallocGuardHeader_)) != 0) {
            OhShit_ ();
        }
        if (::memcmp (&footer.fGuard, &kMallocGuardFooter_, sizeof (kMallocGuardFooter_)) != 0) {
            OhShit_ ();
        }
        if (header.fRequestedBlockSize != footer.fRequestedBlockSize) {
            OhShit_ ();
        }
        // OK
    }
    void    ValidateBackendPtr_ (const void* p)
    {
        const HeaderOrFooter_*  hp  =   reinterpret_cast<const HeaderOrFooter_*> (p);
        const HeaderOrFooter_*  fp  =   reinterpret_cast<const HeaderOrFooter_*> (reinterpret_cast<const Byte*> (hp + 1) + hp->fRequestedBlockSize);
        HeaderOrFooter_ footer;
        (void)::memcpy (&footer, fp, sizeof (footer));  // align access
        Validate_ (*hp, footer);
        if (IsInFreeList_ (p)) {
            OhShit_ ();
        }
    }
    void   PatchNewPointer_ (void* p, size_t requestedSize)
    {
        HeaderOrFooter_*  hp   =   reinterpret_cast< HeaderOrFooter_*> (p);
        (void)::memcpy (begin (hp->fGuard), begin (kMallocGuardHeader_),  NEltsOf (kMallocGuardHeader_));
        hp->fRequestedBlockSize = requestedSize;
        HeaderOrFooter_*  fp  =    reinterpret_cast< HeaderOrFooter_*> (reinterpret_cast<Byte*> (hp + 1) + hp->fRequestedBlockSize);
        (void)::memcpy (begin (fp->fGuard), begin (kMallocGuardFooter_),  NEltsOf (kMallocGuardFooter_));
        fp->fRequestedBlockSize = requestedSize;
    }
}
#endif


#if     qStroika_Foundation_Debug_MallocGuard

extern "C"  void    __libc_free (void* __ptr);
extern "C"  void*   __libc_malloc (size_t __size);
extern "C"  void*   __libc_realloc (void* __ptr, size_t __size);
extern "C"  void*   __libc_calloc (size_t __nmemb, size_t __size);
extern "C"  void    __libc_free (void* __ptr);
extern "C"  void*   __libc_memalign (size_t __alignment, size_t __size);

#if 0
weak_alias (__malloc_info, malloc_info)
5256
5257
5258    strong_alias (__libc_calloc, __calloc) weak_alias (__libc_calloc, calloc)
5259    strong_alias (__libc_free, __cfree) weak_alias (__libc_free, cfree)
5260    strong_alias (__libc_free, __free) strong_alias (__libc_free, free)
5261    strong_alias (__libc_malloc, __malloc) strong_alias (__libc_malloc, malloc)
5262    strong_alias (__libc_memalign, __memalign)
5263    weak_alias (__libc_memalign, memalign)
5264    strong_alias (__libc_realloc, __realloc) strong_alias (__libc_realloc, realloc)
5265    strong_alias (__libc_valloc, __valloc) weak_alias (__libc_valloc, valloc)
5266    strong_alias (__libc_pvalloc, __pvalloc) weak_alias (__libc_pvalloc, pvalloc)
5267    strong_alias (__libc_mallinfo, __mallinfo)
5268    weak_alias (__libc_mallinfo, mallinfo)
5269    strong_alias (__libc_mallopt, __mallopt) weak_alias (__libc_mallopt, mallopt)
5270
5271    weak_alias (__malloc_stats, malloc_stats)
5272    weak_alias (__malloc_usable_size, malloc_usable_size)
5273    weak_alias (__malloc_trim, malloc_trim)
5274    weak_alias (__malloc_get_state, malloc_get_state)
5275    weak_alias (__malloc_set_state, malloc_set_state)
#endif

extern "C"  void*   calloc (size_t __nmemb, size_t __size)
{
    size_t  n   =   __nmemb * __size;
    void*   p   =   malloc (n);
    (void)::memset (p, 0, n);
    return p;
}

extern "C"  void    cfree (void* __ptr)
{
    free (__ptr);
}

extern "C"  void    free (void* __ptr)
{
    if (__ptr == nullptr) {
        // according to http://linux.die.net/man/3/free
        // "if ptr is NULL, no operation is performed." - and glibc does call this internally
        return;
    }
    void*   p = ExposedPtrToBackendPtr_ (__ptr);
    ValidateBackendPtr_ (p);
    SetDeadMansLand_ (p);
    Add2FreeList_ (p);
    __libc_free (p);
}

extern "C"  void*   malloc (size_t __size)
{
    void*   p   =   __libc_malloc (AdjustMallocSize_ (__size));
    PatchNewPointer_ (p, __size);
    ClearFromFreeList_ (p);
    ValidateBackendPtr_ (p);
    if (p != nullptr) {
        p = BackendPtrToExposedPtr_ (p);
    }
    return p;
}

extern "C"  void*    realloc (void* __ptr, size_t __size)
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
    void*   p   =   ExposedPtrToBackendPtr_ (__ptr);
    ValidateBackendPtr_ (p);
    size_t  n   =   AdjustMallocSize_ (__size);
    void*   newP = __libc_realloc (p, n);
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

extern "C"  void*   valloc (size_t __size)
{
    OhShit_ ();
    return nullptr;
}

extern "C"  void*   pvalloc (size_t __size)
{
    OhShit_ ();
    return nullptr;
}

extern "C"  void*   memalign (size_t __alignment, size_t __size)
{
    OhShit_ ();
    return nullptr;
}

extern "C"  size_t malloc_usable_size (void* ptr)
{
    if (ptr == nullptr) {
        return 0;
    }
    void*   p   =   ExposedPtrToBackendPtr_ (ptr);
    ValidateBackendPtr_ (p);
    const HeaderOrFooter_*  hp  =   reinterpret_cast<const HeaderOrFooter_*> (p);
    return hp->fRequestedBlockSize;
}
#endif
