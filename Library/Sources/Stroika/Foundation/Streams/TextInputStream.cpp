/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Containers/Common.h"
#include    "../Debug/Assertions.h"

#include    "TextInputStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;






/*
 ********************************************************************************
 ************************** Streams::TextInputStream ****************************
 ********************************************************************************
 */
String TextInputStream::ReadLine () const
{
    Require (IsSeekable ());
    String      result;
    while (true) {
        Character   c   =   Read ();
        if (c.GetCharacterCode () == '\0') {
            // EOF
            return result;
        }
        result.push_back (c);
        if (c == '\n') {
            return result;
        }
        else if (c == '\r') {
            Character   c   =   Read ();
            // if CR is follwed by LF, append that to result too before returning. Otherwise, put the character back
            if (c == '\n') {
                result.push_back (c);
                return result;
            }
            else {
                Seek (Whence::eFromCurrent, -1);
            }
            return result;
        }
    }
}

Traversal::Iterable<String> TextInputStream::ReadLines () const
{
    AssertNotImplemented ();
}

String TextInputStream::ReadAll () const
{
    String      result;
    while (true) {
        Character buf[1024];
        size_t n = Read (std::begin (buf), std::end (buf));
        Assert (0 <= n and n <= NEltsOf (buf));
        if (n == 0) {
            break;
        }
        else {
            result.Append (std::begin (buf), std::begin (buf) + n);
        }
    }
    return result;
}
