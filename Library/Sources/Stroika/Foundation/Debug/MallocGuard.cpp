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
    void    Validate_ (const HeaderOrFooter_& header, const HeaderOrFooter_& footer)
    {
        if (::memcmp (&header.fGuard, &kMallocGuardHeader_, sizeof (kMallocGuardHeader_)) != 0) {
            AssertNotReached ();
            std::terminate ();
        }
        if (::memcmp (&footer.fGuard, &kMallocGuardFooter_, sizeof (kMallocGuardFooter_)) != 0) {
            AssertNotReached ();
            std::terminate ();
        }
        if (header.fRequestedBlockSize != footer.fRequestedBlockSize) {
            AssertNotReached ();
            std::terminate ();
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
    size_t  AdjustMallocSize_ (size_t s)
    {
        return s + 2 * sizeof (HeaderOrFooter_);
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

extern "C" void __libc_free (void* __ptr);
extern "C" void* __libc_malloc (size_t __size);
extern "C" void* __libc_realloc (void* __ptr, size_t __size);


extern "C" void free (void* __ptr)
{
    __libc_free (__ptr);
    // drop on the floor
}

extern "C" void* malloc (size_t __size)
{
    return __libc_malloc (__size);
}

extern "C" void* realloc (void* __ptr, size_t __size)
{
    return __libc_realloc (__ptr, __size);
}
#endif