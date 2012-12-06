/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Containers/Common.h"

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
    String      result;
    while (true) {
        Character   c   =   Read ();
        if (c.GetCharacterCode () == '\0') {
            return result;
        }
        result.push_back (c.GetCharacterCode ());
        if (c == '\n') {
            return result;
        }
        else if (c == '\r') {
            Character   c   =   Read ();
            if (c == '\n') {
                result.push_back (c.GetCharacterCode ());
                return result;
            }
            _GetRep ()->_PutBack (c);
            return result;
        }
    }
}

String TextInputStream::ReadAll () const
{
    String      result;
    while (true) {
        Character   c   =   Read ();
        if (c.GetCharacterCode () == '\0') {
            return result;
        }
        result.push_back (c.GetCharacterCode ());
    }
    return result;
}
