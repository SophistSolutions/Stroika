/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cctype>
#include <cstdarg>
#include <cstdlib>
#include <iomanip>
#include <sstream>

#include "../Characters/CString/Utilities.h"
#include "../Containers/Common.h"
#include "../Debug/Assertions.h"
#include "../Debug/Trace.h"

#include "Words.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Linguistics;

/*
 ********************************************************************************
 ************************************ CapitalizeEachWord ************************
 ********************************************************************************
 */
String Linguistics::CapitalizeEachWord (const String& s)
{
    String r;
    // take an ENGLISH string (assume English)

    // toupper each lower-case character preceeded by a space
    bool prevCharSpace = true; // so we upper first char
    for (Character i : s) {
        if (prevCharSpace) {
            i = i.ToUpperCase ();
        }
        r += i;
        prevCharSpace = i.IsWhitespace ();
    }
    return r;
}

/*
 ********************************************************************************
 ******************************* CapitalizeEachSentence *************************
 ********************************************************************************
 */
String Linguistics::CapitalizeEachSentence (const String& s)
{
    // WAY too kludgly - but hopefully adequate for primitive message cleanups...
    //      -- LGP 2008-09-20
    String r;
    // take an ENGLISH string (assume English)

    // toupper each lower-case character preceeded by a ENDOFSENTECE PUNCT
    bool nextCharStartsSentence = true; // so we upper first char
    for (Character i : s) {
        if (nextCharStartsSentence and i.IsWhitespace ()) {
            i                      = i.ToUpperCase ();
            nextCharStartsSentence = false;
        }
        else {
            nextCharStartsSentence |= (i == '.' or i == '!' or i == '?');
        }
        r += i;
    }
    return r;
}

/*
 ********************************************************************************
 ******************************** UnCapitalizeFirstWord *************************
 ********************************************************************************
 */
String Linguistics::UnCapitalizeFirstWord (const String& s)
{
    // WAY too kludgly - but hopefully adequate for primitive message cleanups...
    //      -- LGP 2008-09-20
    String r = s;
    if (r.length () > 2) {
        if (r[0].ToUpperCase () == r[0] and r[1].ToUpperCase () != r[1]) {
            r.SetCharAt (r[0].ToLowerCase (), 0);
        }
    }
    return r;
}

/*
 ********************************************************************************
 ********************************** IsAllCaps ***********************************
 ********************************************************************************
 */
bool Linguistics::IsAllCaps (const String& s)
{
    return not s.empty () and s == s.ToUpperCase ();
}
