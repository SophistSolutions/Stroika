/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Containers/Support/ReserveTweaks.h"
#include "../Debug/Trace.h"
#include "../Memory/BLOB.h"

#include "InputStream.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;

using Characters::Character;
using Characters::String;
using Memory::BLOB;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ******************** Streams::InputStream<ELEMENT_TYPE>::Ptr *******************
 ********************************************************************************
 */
DISABLE_COMPILER_MSC_WARNING_START (6262) // stack usage OK
template <>
template <>
Memory::BLOB InputStream<byte>::Ptr::ReadAll (size_t upTo) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{L"InputStream<byte>::Ptr::ReadAll", L"upTo: %llu", static_cast<unsigned long long> (upTo)};
#endif
    Require (upTo >= 1);
    vector<byte> r; // @todo Consider using StackBuffer<>
    if (IsSeekable ()) {
        /*
         * Avoid realloc's if not hard.
         */
        SeekOffsetType size = GetOffsetToEndOfStream ();
        if (size >= numeric_limits<size_t>::max ()) [[unlikely]] {
            Execution::Throw (bad_alloc{});
        }
        size_t sb = static_cast<size_t> (size);
        sb        = min (sb, upTo);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("Seekable case: expectedSize = %llu, reserving %llu", static_cast<unsigned long long> (size), static_cast<unsigned long long> (sb));
#endif
        r.reserve (sb);
    }
    for (size_t nEltsLeft = upTo; nEltsLeft != 0;) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("nEltsLeft=%llu", static_cast<unsigned long long> (nEltsLeft));
#endif
        byte  buf[64 * 1024];
        byte* s = std::begin (buf);
        byte* e = std::end (buf);
        if (nEltsLeft < Memory::NEltsOf (buf)) {
            e = s + nEltsLeft;
        }
        Assert (s < e);
        size_t n = Read (s, e);
        Assert (0 <= n and n <= nEltsLeft);
        Assert (0 <= n and n <= Memory::NEltsOf (buf));
        if (n == 0) {
            break;
        }
        else {
            // @todo???
            //      could also maintain linked list - std::list<> - of BLOBs, and then construct BLOB from
            //      list of BLOBs - that would be quite efficient too - maybe more
            Containers::Support::ReserveTweaks::GetScaledUpCapacity4AddN (r, n, 32 * 1024); // grow exponentially, so not too many reallocs
            Assert (n <= nEltsLeft);
            nEltsLeft -= n;
            r.insert (r.end (), s, s + n);
        }
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("returning %llu bytes", static_cast<unsigned long long> (r.size ()));
#endif
    return BLOB{r};
}
DISABLE_COMPILER_MSC_WARNING_END (6262)
