/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
 ****************************** PluralizeNoun ***********************************
 ********************************************************************************
 */
String Linguistics::PluralizeNoun (const String& s, int count)
{
    // Implement VERY WEAK ENGLISH rules for now... (ignores y ->ies, and other cases too)
    if (count == 1) {
        return s;
    }
    else {
        String tmp = s;
        tmp.push_back ('s');
        return tmp;
    }
}

String Linguistics::PluralizeNoun (const String& s, const String& sPlural, int count)
{
    return count == 1 ? s : sPlural;
}

/*
 ********************************************************************************
 **************************** MungeStringSoSingular *****************************
 ********************************************************************************
 */
String Linguistics::MungeStringSoSingular (const String& s)
{
    String r = s;
    // take an ENGLISH string (assume English) - and munge it so its singular (if it happened to have been plural)

    // hande special case of 'wives' -> 'wife' as in 'midwives'
    if (r.length () >= 5) {
        size_t l = r.length ();
        if (s[l - 5] == 'w' and s[l - 4] == 'i' and s[l - 3] == 'v' and s[l - 2] == 'e' and s[l - 1] == 's') {
            r = r.substr (0, l - 3);
            r.push_back ('f');
            r.push_back ('e');
            return r;
        }
    }

    // trim trailing s from the name (if prev letter is a non-s consonant)
    // or change 'ies' to 'y' at end
    if (s.length () > 3) {
        if (s[s.length () - 3] == 'i' and s[s.length () - 2] == 'e' and s[s.length () - 1] == 's') {
            //r = s.substr (0, s.length () - 3) + "y";
            r = s.substr (0, s.length () - 3);
            r.push_back ('y');
        }
        else if (s.length () > 4 and s[s.length () - 4] == 's' and s[s.length () - 3] == 's' and s[s.length () - 2] == 'e' and s[s.length () - 1] == 's') {
            r = s.substr (0, s.length () - 2);
        }
        else if (s[s.length () - 3] == 's' and s[s.length () - 2] == 'e' and s[s.length () - 1] == 's') {
            r = s.substr (0, s.length () - 1);
        }
        // because of diabets mellitus - (and others???? - don't map trailing 'us' to 'u'
        else if (s[s.length () - 1] == 's' and s[s.length () - 2].IsASCII () and s[s.length () - 2].IsAlphabetic () and (s[s.length () - 2] != 's' and s[s.length () - 2] != 'u')) {
            r = s.substr (0, s.length () - 1);
        }
    }
    return r;
}

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
