/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Support/ReserveTweaks.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

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
 ******************** Streams::InputStream::Ptr<ELEMENT_TYPE> *******************
 ********************************************************************************
 */
DISABLE_COMPILER_MSC_WARNING_START (6262) // stack usage OK
template <>
Memory::BLOB InputStream::Ptr<byte>::ReadAll (size_t upTo) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{L"InputStream::Ptr<byte>::ReadAll", L"upTo: %llu", static_cast<unsigned long long> (upTo)};
#endif
    Require (upTo >= 1);
    Memory::StackBuffer<byte> r;
    for (size_t nEltsLeft = upTo; nEltsLeft != 0;) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("nEltsLeft=%llu", static_cast<unsigned long long> (nEltsLeft));
#endif
        byte  buf[64 * 1024]; // intentionally uninitialized
        byte* s = std::begin (buf);
        byte* e = std::end (buf);
        if (nEltsLeft < Memory::NEltsOf (buf)) {
            e = s + nEltsLeft;
        }
        Assert (s < e);
        size_t n = Read (span{s, e}).size ();
        Assert (0 <= n and n <= nEltsLeft);
        Assert (0 <= n and n <= Memory::NEltsOf (buf));
        if (n == 0) {
            break;
        }
        else {
            // @todo???
            //      could also maintain linked list - std::list<> - of BLOBs, and then construct BLOB from
            //      list of BLOBs - that would be quite efficient too - maybe more
            Assert (n <= nEltsLeft);
            nEltsLeft -= n;
            r.push_back (span{s, n});
        }
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("returning %llu bytes", static_cast<unsigned long long> (r.size ()));
#endif
    return BLOB{r};
}
DISABLE_COMPILER_MSC_WARNING_END (6262)
