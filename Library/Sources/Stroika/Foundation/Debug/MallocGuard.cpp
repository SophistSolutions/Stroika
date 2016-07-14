/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <exception>
#include    <malloc.h>

#include    "../Memory/Common.h"

#include    "MallocGuard.h"



using   namespace   Stroika::Foundation;
using   Memory::Byte;


#if     qStroika_Foundation_Debug_MallogGuard
namespace {
    using   GuradBytes_ =   Byte[16];
    constexpr   GuradBytes_ kMallocGuardHeader_ = { 0xf3, 0xfa, 0x0b, 0x93, 0x48, 0x50, 0x46, 0xe6, 0x22, 0xf1, 0xfa, 0xc0, 0x9a, 0x0b, 0xeb, 0x23, };
    constexpr   GuradBytes_ kMallocGuardFooter_ = { 0x07, 0x41, 0xa4, 0x2b, 0xba, 0x97, 0xcb, 0x38, 0x46, 0x1e, 0x3c, 0x42, 0x3c, 0x5f, 0x0c, 0x80, };

    // need header with size, so we can ....
    struct alignas(alignof(long double))  HeaderOrFooter_ {
        GuradBytes_ fGuard;
        size_t      fRequestedBlockSize;
    };


    void    OhShit_ ()
    {
        AssertNotReached ();
        std::terminate ();
    }


    void*   ExposedPtrToBackendPtr_ (void* p)
    {
        if (p == nullptr) {
            OhShit_ ();
        }
        return reinterpret_cast<const HeaderOrFooter_*> (p) - 1;
    }
    void*   BackendPtrToExposedPtr_ (void* p)
    {
        if (p == nullptr) {
            OhShit_ ();
        }
        return reinterpret_cast<const HeaderOrFooter_*> (p) + 1;
    }
    size_t  AdjustMallocSize_ (size_t s)
    {
        return s + 2 * sizeof (HeaderOrFooter_);
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
    void    Validate_ (const void* p)
    {
        const HeaderOrFooter_*  h = reinterpret_cast<const HeaderOrFooter_*> (p) - 1;
        const HeaderOrFooter_*  fp  = reinterpret_cast<const HeaderOrFooter_*> (reinterpret_cast<const Byte*> (p) + h->fRequestedBlockSize);
        HeaderOrFooter_ footer;
        (void)::memcpy (&footer, fp, sizeof (footer));  // align access
        Validate_ (*h, footer);
    }
    // returns the pointer to use externally
    void*   PatchNewPointer_ (void* p, size_t requestedSize)
    {
        return p;
    }

    // todo variant that does pointer arithmatic
    // defines that contril how we handle failure (debug failure proc)?


}
#endif


#if     qStroika_Foundation_Debug_MallogGuard

extern "C"  void    __libc_free (void* __ptr);
extern "C"  void*   __libc_malloc (size_t __size);
extern "C"  void*   __libc_realloc (void* __ptr, size_t __size);
extern "C"  void*   __libc_calloc (size_t __nmemb, size_t __size);
extern "C"  void    __libc_free (void* __ptr);
extern "C"  size_t  __malloc_usable_size (void* __ptr);
/* Allocate SIZE bytes allocated to ALIGNMENT bytes.  */
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
    void*   p = ExposedPtrToBackendPtr_ (__ptr);
    __libc_free (p);
}

extern "C"  void*   malloc (size_t __size)
{
    void*   p   =   __libc_malloc (AdjustMallocSize_ (__size));
    if (p != nullptr) {
        p = BackendPtrToExposedPtr_ (p);
    }
    return p;
}

extern "C"  void*    realloc (void* __ptr, size_t __size)
{
    void*   p   =   ExposedPtrToBackendPtr_ (__ptr);
    size_t  n   =   AdjustMallocSize_ (__size);
    p = __libc_realloc (p, n);
    if (p != nullptr) {
        p = BackendPtrToExposedPtr_ (p);
    }
    return p;
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
#endif
