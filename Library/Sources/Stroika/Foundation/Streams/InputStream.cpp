/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Character.h"
#include    "../Characters/String.h"
#include    "../Characters/StringBuilder.h"

#include    "InputStream.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;


using   Characters::Character;
using   Characters::String;
using   Characters::StringBuilder;
using   Memory::BLOB;
using   Memory::Byte;





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
String InputStream<Character>::ReadAll () const
{
    Characters::StringBuilder result;
    while (true) {
        Character buf[16 * 1024];
        size_t n = Read (std::begin (buf), std::end (buf));
        Assert (0 <= n and n <= NEltsOf (buf));
        if (n == 0) {
            break;
        }
        else {
            result.Append (std::begin (buf), std::begin (buf) + n);
        }
    }
    return result.str ();
}


template    <>
template    <>
Memory::BLOB InputStream<Byte>::ReadAll () const
{
    if (this->IsSeekable ()) {
        SeekOffsetType  size = this->GetOffsetToEndOfStream ();
        if (size >= numeric_limits<size_t>::max ()) {
            Execution::DoThrow<bad_alloc> (bad_alloc ());
        }
        size_t sb = static_cast<size_t> (size);
        if (sb == 0) {
            return BLOB ();
        }
        Byte* b = new Byte[sb];   // if this fails, we had no way to create the BLOB
        size_t n = this->Read (b, b + sb);
        Assert (n <= sb);
        return BLOB::Attach (b, b + n);
    }
    else {
        // Less efficient implementation
        vector<Byte>    r;
        size_t          n;
        Byte            buf[32 * 1024];
        while ( (n = this->Read (std::begin (buf), std::end (buf))) != 0) {
            r.insert (r.end (), std::begin (buf), std::begin (buf) + n);
        }
        return BLOB (r);
    }
}
