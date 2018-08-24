/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Character.h"
#include "../Characters/String.h"
#include "../Characters/StringBuilder.h"
#include "../Containers/Common.h"
#include "../Debug/Trace.h"

#include "InputStream.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;

using Characters::Character;
using Characters::String;
using Characters::StringBuilder;
using Memory::BLOB;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ******************** Streams::InputStream<ELEMENT_TYPE>::Ptr *******************
 ********************************************************************************
 */
template <>
template <>
String InputStream<Character>::Ptr::ReadLine () const
{
    Require (IsSeekable ());
    StringBuilder result;
    while (true) {
        Character c = ReadCharacter ();
        if (c.GetCharacterCode () == '\0') {
            // EOF
            return result.str ();
        }
        result.push_back (c);
        if (c == '\n') {
            return result.str ();
        }
        else if (c == '\r') {
            c = ReadCharacter ();
            // if CR is follwed by LF, append that to result too before returning. Otherwise, put the character back
            if (c == '\n') {
                result.push_back (c);
                return result.str ();
            }
            else {
                Seek (Whence::eFromCurrent, -1);
            }
            return result.str ();
        }
    }
}

template <>
template <>
Characters::Character InputStream<Characters::Character>::Ptr::ReadCharacter () const
{
    Characters::Character c;
    if (Read (&c, &c + 1) == 1) {
        return c;
    }
    return '\0';
}

template <>
template <>
Traversal::Iterable<String> InputStream<Character>::Ptr::ReadLines () const
{
    InputStream<Character>::Ptr copyOfStream = *this;
    return Traversal::CreateGenerator<String> ([copyOfStream]() -> optional<String> {
        String line = copyOfStream.ReadLine ();
        if (line.empty ()) {
            return nullopt;
        }
        else {
            return line;
        }
    });
}

DISABLE_COMPILER_MSC_WARNING_START (6262) // stack usage OK
template <>
template <>
String InputStream<Character>::Ptr::ReadAll (size_t upTo) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"InputStream<Character>::Ptr::ReadAll", L"upTo: %llu", static_cast<unsigned long long> (upTo));
#endif
    Require (upTo >= 1);
    Characters::StringBuilder result;
    size_t                    nEltsLeft = upTo;
    while (nEltsLeft > 0) {
        Character  buf[16 * 1024];
        Character* s = std::begin (buf);
        Character* e = std::end (buf);
        if (nEltsLeft < NEltsOf (buf)) {
            e = s + nEltsLeft;
        }
        size_t n = Read (s, e);
        Assert (0 <= n and n <= nEltsLeft);
        Assert (0 <= n and n <= NEltsOf (buf));
        if (n == 0) {
            break;
        }
        else {
            Assert (n <= nEltsLeft);
            nEltsLeft -= n;
            result.Append (s, s + n);
        }
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"Returning %llu characters", static_cast<unsigned long long> (result.GetLength ()));
#endif
    return result.str ();
}
DISABLE_COMPILER_MSC_WARNING_END (6262)

DISABLE_COMPILER_MSC_WARNING_START (6262) // stack usage OK
template <>
template <>
Memory::BLOB InputStream<byte>::Ptr::ReadAll (size_t upTo) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"InputStream<byte>::Ptr::ReadAll", L"upTo: %llu", static_cast<unsigned long long> (upTo));
#endif
    Require (upTo >= 1);
    vector<byte> r; // @todo Consider using SmallStackBuffer<>
    if (IsSeekable ()) {
        /*
         * Avoid realloc's if not hard.
         */
        SeekOffsetType size = GetOffsetToEndOfStream ();
        if (size >= numeric_limits<size_t>::max ()) {
            Execution::Throw (bad_alloc ());
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
        if (nEltsLeft < NEltsOf (buf)) {
            e = s + nEltsLeft;
        }
        Assert (s < e);
        size_t n = Read (s, e);
        Assert (0 <= n and n <= nEltsLeft);
        Assert (0 <= n and n <= NEltsOf (buf));
        if (n == 0) {
            break;
        }
        else {
            // @todo???
            //      could also maintain linked list - std::list<> - of BLOBs, and then construct BLOB from
            //      list of BLOBs - that would be quite efficeint too - maybe more
            Containers::ReserveSpeedTweekAddNCapacity (r, n, 32 * 1024); // grow exponentially, so not too many reallocs
            Assert (n <= nEltsLeft);
            nEltsLeft -= n;
            r.insert (r.end (), s, s + n);
        }
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("returning %llu bytes", static_cast<unsigned long long> (r.size ()));
#endif
    return BLOB (r);
}
DISABLE_COMPILER_MSC_WARNING_END (6262)
