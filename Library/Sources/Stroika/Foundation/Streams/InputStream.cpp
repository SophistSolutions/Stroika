/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Character.h"
#include    "../Characters/String.h"
#include    "../Characters/StringBuilder.h"
#include    "../Containers/Common.h"
#include    "../Debug/Trace.h"

#include    "InputStream.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;


using   Characters::Character;
using   Characters::String;
using   Characters::StringBuilder;
using   Memory::BLOB;
using   Memory::Byte;



// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1






/*
 ********************************************************************************
 ************************* Streams::InputStream<ELEMENT_TYPE> *******************
 ********************************************************************************
 */
template    <>
template    <>
String InputStream<Character>::ReadLine () const
{
    Require (IsSeekable ());
    StringBuilder   result;
    while (true) {
        Character   c   =   ReadCharacter ();
        if (c.GetCharacterCode () == '\0') {
            // EOF
            return result.str ();
        }
        result.push_back (c);
        if (c == '\n') {
            return result.str ();
        }
        else if (c == '\r') {
            Character   c   =   ReadCharacter ();
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


template    <>
template    <>
Characters::Character   InputStream<Characters::Character>::ReadCharacter () const
{
    Characters::Character   c;
    if (Read (&c, &c + 1) == 1) {
        return c;
    }
    return '\0';
}


template    <>
template    <>
Traversal::Iterable<String> InputStream<Character>::ReadLines () const
{
    InputStream<Character> copyOfStream =  *this;
    return Traversal::CreateGenerator<String> ([copyOfStream] () -> Memory::Optional<String> {
        String  line = copyOfStream.ReadLine ();
        if (line.empty ())
        {
            return Memory::Optional<String> ();
        }
        else {
            return line;
        }
    });
}


template    <>
template    <>
String InputStream<Character>::ReadAll (size_t upTo) const
{
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"InputStream<Character>::ReadAll");
    DbgTrace (L"(upTo: %u)", upTo);
#endif
    Characters::StringBuilder result;
    size_t  nEltsLeft = upTo;
    while (nEltsLeft > 0) {
        Character buf[16 * 1024];
        Character*  s   =   std::begin (buf);
        Character*  e   =   std::end (buf);
        if (nEltsLeft < NEltsOf (buf)) {
            e = s + nEltsLeft;
        }
        size_t n = Read (s, e);
        Assert (0 <= n and n <= nEltsLeft);
        Assert (0 <= n and n < NEltsOf (buf));
        if (n == 0) {
            break;
        }
        else {
            Assert (n <= nEltsLeft);
            nEltsLeft -= n;
            result.Append (s, s + n);
        }
    }
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"Returning %d characters", result.GetLength ());
#endif
    return result.str ();
}


template    <>
template    <>
Memory::BLOB InputStream<Byte>::ReadAll (size_t upTo) const
{
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"InputStream<Byte>::ReadAll");
    DbgTrace (L"(upTo: %u)", upTo);
#endif
    if (IsSeekable ()) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("Seekable case");
#endif
        SeekOffsetType  size = GetOffsetToEndOfStream ();
        if (size >= numeric_limits<size_t>::max ()) {
            Execution::Throw (bad_alloc ());
        }
        size_t sb = static_cast<size_t> (size);
        sb = min (sb, upTo);
        if (sb == 0) {
            return BLOB ();
        }
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("(should be able to read)sb=%u", sb);
#endif
        // @todo this isn't crazy worse than SmallStackBuffer, because if sb is the size read and wouldn't
        // fit in a small stack buffer (stack part) - we avoid a second allocation.
        // But - on balance - thats a lot of iffs. And we probably should use SmallStackBuffer and just
        // do another alloc if needed. Maybe use an artificially large stack allocaiton here so we are likely to
        // actually use the stack, or new stl 'get_temporeary_buffer' or wahtever the new routine is for stack allocs
        //
        Byte* b = new Byte[sb];   // if this fails, we had no way to create the BLOB
        try {

            // Even though we've allocated a large enough buffer, the guarnatee of Read () is that it will return
            // > 0, or less or equal than args buf size, but only 0 means EOF, so must re-read if more out there.
#if 1
            size_t  bytesLeft =  sb;
            Byte*   pi = b;
            while (bytesLeft > 0) {
                size_t n = Read (pi, pi + bytesLeft);
                Assert (n <= bytesLeft);
                bytesLeft -= n;
                pi += n;
                if (n == 0) {
                    AssertNotReached ();        // @todo decide how to handle this - means stream changed size - doesnt work with this
                    // algorithm, but not necesarily SB disallowed.
                    // for now - hack
                    sb -= bytesLeft;
                }
            }
#else
            // WRONG
            size_t n = Read (b, b + sb);
            Assert (n <= sb);
#endif
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("returning %u bytes", sb);
#endif
            return BLOB::Attach (b, b + sb);
        }
        catch (...) {
            delete[] (b);
            Execution::ReThrow ();
        }
    }
    else {
        // Less efficient implementation
        vector<Byte>    r;
        for (size_t nEltsLeft = upTo; nEltsLeft != 0; ) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("Unseekable case: nEltsLeft=%u", nEltsLeft);
#endif
            Byte            buf[64 * 1024];
            Byte*           s           =   std::begin (buf);
            Byte*           e           =   std::end (buf);
            if (nEltsLeft < NEltsOf (buf)) {
                e = s + nEltsLeft;
            }
            Assert (s < e);
            size_t          n           =   Read (s, e);
            Assert (0 <= n and n <= nEltsLeft);
            Assert (0 <= n and n < NEltsOf (buf));
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
        return BLOB (r);
    }
}
