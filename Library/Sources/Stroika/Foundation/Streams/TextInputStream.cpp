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
TextInputStream::TextInputStream ()
    : fPutBackCharValid_ (false)
    , fPutBackCharacter_ ()
{
}

String TextInputStream::ReadLine ()
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
            Assert (not fPutBackCharValid_);
            fPutBackCharValid_ = true;
            fPutBackCharacter_ = c;
            return result;
        }
    }
}

String TextInputStream::ReadAll ()
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
