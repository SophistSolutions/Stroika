/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <cstdarg>
#include    <cstdlib>
#include    <iomanip>
#include    <cctype>
#include    <sstream>

#include    "../Characters/CString/Utilities.h"
#include    "../Containers/Common.h"
#include    "../Debug/Assertions.h"
#include    "../Debug/Trace.h"

#include    "Words.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Linguistics;







namespace  {
    ////////// LEGACY CODE FROM OL STRINGUTILS MOVED HERE - PRBABLY SHOULD ALL BE REMOVED - BUT LEAVE FOR NOW ///////////////
    ///////// -- LGP 2013-10-17
    inline  char    TOLOWER (char c)
    {
#if     qUseASCIIRangeSpeedHack
        if ('A' <= c and c <= 'Z') {
            Ensure (std::tolower (c) == ((c - 'A') + 'a'));
            return (c - 'A') + 'a';
        }
        else if (c <= 127) {
            Ensure (std::tolower (c) == c);
            return c;
        }
        else {
            return std::tolower (c);
        }
#else
        return std::tolower (c);
#endif
    }
    inline  wchar_t TOLOWER (wchar_t c)
    {
#if     qUseASCIIRangeSpeedHack
        if ('A' <= c and c <= 'Z') {
            Ensure (towlower (c) == ((c - 'A') + 'a'));
            return (c - 'A') + 'a';
        }
        else if (c <= 127) {
            Ensure (wchar_t (towlower (c)) == c);
            return c;
        }
        else {
            return towlower (c);
        }
#else
        return towlower (c);
#endif
    }
    inline  bool    ISSPACE (char c)
    {
#if     qUseASCIIRangeSpeedHack
        if (0 < c and c <= 127) {
            //  space, tab, carriage return, newline, vertical tab or form feed
            bool    r   =   (c == ' ' or c == '\t' or c == '\r' or c == '\n' or c == '\v' or c == '\f');
            Ensure (!!isspace (c) == r);
            return r;
        }
        else {
            DISABLE_COMPILER_MSC_WARNING_START(4800)
            return isspace (c);
            DISABLE_COMPILER_MSC_WARNING_END(4800)
        }
#else
        return !!isspace (c);
#endif
    }
    inline  bool    ISSPACE (wchar_t c)
    {
#if     qUseASCIIRangeSpeedHack
        if (0 < c and c <= 127) {
            //  space, tab, carriage return, newline, vertical tab or form feed
            bool    r   =   (c == ' ' or c == '\t' or c == '\r' or c == '\n' or c == '\v' or c == '\f');
            Ensure (!!iswspace (c) == r);
            return r;
        }
        else {
            DISABLE_COMPILER_MSC_WARNING_START(4800)
            return iswspace (c);
            DISABLE_COMPILER_MSC_WARNING_END(4800)
        }
#else
        return !!iswspace (c);
#endif
    }
    inline  char    TOUPPER (char c)
    {
#if     qUseASCIIRangeSpeedHack
        if ('a' <= c and c <= 'z') {
            Ensure (std::toupper (c) == ((c - 'a') + 'A'));
            return (c - 'a') + 'A';
        }
        else if (c <= 127) {
            Ensure (std::toupper (c) == c);
            return c;
        }
        else {
            return std::toupper (c);
        }
#else
        return std::toupper (c);
#endif
    }
    inline  wchar_t TOUPPER (wchar_t c)
    {
#if     qUseASCIIRangeSpeedHack
        if ('a' <= c and c <= 'z') {
            Ensure (towupper (c) == ((c - 'a') + 'A'));
            return (c - 'a') + 'A';
        }
        else if (c <= 127) {
            Ensure (wchar_t (towupper (c)) == c);
            return c;
        }
        else {
            return towupper (c);
        }
#else
        return towupper (c);
#endif
    }
    void    toupper (wstring* s)
    {
        RequireNotNull (s);
        wstring::iterator   end =   s->end ();
        for (wstring::iterator i = s->begin (); i != end; ++i) {
            *i = TOUPPER (*i);
        }
    }

    void    toupper (string* s)
    {
        RequireNotNull (s);
        string::iterator    end =   s->end ();
        for (string::iterator i = s->begin (); i != end; ++i) {
            *i = TOUPPER (*i);
        }
    }
    inline  wstring toupper (const wstring& s)
    {
        wstring r   =   s;
        toupper (&r);
        return r;
    }
    inline  string  toupper (const string& s)
    {
        string  r   =   s;
        toupper (&r);
        return r;
    }
}




/*
 ********************************************************************************
 ****************************** PluralizeNoun ***********************************
 ********************************************************************************
 */
namespace   {
    template    <typename STRING>
    inline  STRING  PluralizeNoun_HLPR (const STRING& str, int count)
    {
        // Implement VERY WEAK ENGLISH rules for now... (ignores y ->ies, and other cases too)
        if (count == 1) {
            return str;
        }
        else {
            STRING  tmp =   str;
            tmp.push_back ('s');
            return tmp;
        }
    }
}
#if 1
String Linguistics::PluralizeNoun (const String& s, int count)
{
    return PluralizeNoun_HLPR (s, count);
}

String  Linguistics::PluralizeNoun (const String& s, const String& sPlural, int count)
{
    return count == 1 ? s : sPlural;
}

#else
string  Linguistics::PluralizeNoun (const string& s, int count)
{
    return PluralizeNoun_HLPR (s, count);
}

wstring Linguistics::PluralizeNoun (const wstring& s, int count)
{
    return PluralizeNoun_HLPR (s, count);
}

string  Linguistics::PluralizeNoun (const string& s, const string& sPlural, int count)
{
    return count == 1 ? s : sPlural;
}

wstring Linguistics::PluralizeNoun (const wstring& s, const wstring& sPlural, int count)
{
    return count == 1 ? s : sPlural;
}
#endif






/*
 ********************************************************************************
 **************************** MungeStringSoSingular *****************************
 ********************************************************************************
 */
namespace   {
    template    <typename STR>
    inline  STR MungeStringSoSingular_ (const STR& s)
    {
        STR r   =   s;
        // take an ENGLISH string (assume English) - and munge it so its singular (if it happened to have been plural)

        // hande special case of 'wives' -> 'wife' as in 'midwives'
        if (r.length () >= 5) {
            size_t  l   =   r.length ();
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
            else if (s[s.length () - 1] == 's' and s[s.length () - 2].IsAscii () and s[s.length () - 2].IsAlphabetic () and (s[s.length () - 2] != 's' and s[s.length () - 2] != 'u')) {
                r = s.substr (0, s.length () - 1);
            }
        }
        return r;
    }
}
#if 1
String  Linguistics::MungeStringSoSingular (const String& s)
{
    return MungeStringSoSingular_ (s);
}
#else
wstring Linguistics::MungeStringSoSingular (const wstring& s)
{
    return MungeStringSoSingular_ (s);
}

string  Linguistics::MungeStringSoSingular (const string& s)
{
    return MungeStringSoSingular_ (s);
}
#endif







/*
 ********************************************************************************
 ************************************ CapitalizeEachWord ************************
 ********************************************************************************
 */
#if 0
namespace   {
    template    <typename STR>
    inline  STR CapitalizeEachWord_ (const STR& s)
    {
        STR r;
        // take an ENGLISH string (assume English)

        // toupper each lower-case character preceeded by a space
        bool    prevCharSpace   =   true;   // so we upper first char
        for (Character i : r)
            if (prevCharSpace) {
                i = i.ToUpperCase ();
            }
        r += i;
        prevCharSpace = i.IsWhitespace ();
    }
    return r;
}
}
#endif
#if 1
String Linguistics::CapitalizeEachWord (const String& s)
{
    String r;
    // take an ENGLISH string (assume English)

    // toupper each lower-case character preceeded by a space
    bool    prevCharSpace   =   true;   // so we upper first char
    for (Character i : s)  {
        if (prevCharSpace) {
            i = i.ToUpperCase ();
        }
        r += i;
        prevCharSpace = i.IsWhitespace ();
    }
    return r;
}
#else
wstring Linguistics::CapitalizeEachWord (const wstring& s)
{
    return CapitalizeEachWord_ (s);
}

string  Linguistics::CapitalizeEachWord (const string& s)
{
    return CapitalizeEachWord_ (s);
}
#endif







/*
 ********************************************************************************
 ******************************* CapitalizeEachSentence *************************
 ********************************************************************************
 */
namespace   {
    template    <typename STR>
    inline  STR CapitalizeEachSentence_ (const STR& s)
    {
        // WAY too kludgly - but hopefully adequate for primitive message cleanups...
        //      -- LGP 2008-09-20
        STR r   =   s;
        // take an ENGLISH string (assume English)

        // toupper each lower-case character preceeded by a ENDOFSENTECE PUNCT
        bool    nextCharStartsSentence  =   true;   // so we upper first char
        for (auto i = r.begin (); i != r.end (); ++i) {
            if (nextCharStartsSentence and !ISSPACE (*i)) {
                *i = TOUPPER (*i);
                nextCharStartsSentence = false;
            }
            else {
                nextCharStartsSentence |= (*i == '.' or * i == '!' or * i == '?');
            }
        }
        return r;
    }
}

#if 1
String  Linguistics::CapitalizeEachSentence (const String& s)
{
    // WAY too kludgly - but hopefully adequate for primitive message cleanups...
    //      -- LGP 2008-09-20
    String r;
    // take an ENGLISH string (assume English)

    // toupper each lower-case character preceeded by a ENDOFSENTECE PUNCT
    bool    nextCharStartsSentence  =   true;   // so we upper first char
    for (Character i : s) {
        if (nextCharStartsSentence and i.IsWhitespace ()) {
            i = i.ToUpperCase ();
            nextCharStartsSentence = false;
        }
        else {
            nextCharStartsSentence |= (i == '.' or i == '!' or i == '?');
        }
        r += i;
    }
    return r;
}
#else
wstring Linguistics::CapitalizeEachSentence (const wstring& s)
{
    return CapitalizeEachSentence_ (s);
}

string  Linguistics::CapitalizeEachSentence (const string& s)
{
    return CapitalizeEachSentence_ (s);
}
#endif








/*
 ********************************************************************************
 ******************************** UnCapitalizeFirstWord *************************
 ********************************************************************************
 */
namespace   {
    template    <typename STR>
    inline  STR UnCapitalizeFirstWord_ (const STR& s)
    {
        // WAY too kludgly - but hopefully adequate for primitive message cleanups...
        //      -- LGP 2008-09-20
        STR r   =   s;
        if (r.length () > 2) {
            if (TOUPPER (r[0]) == r[0] and TOUPPER (r[1]) != r[1]) {
                r[0] = TOLOWER (r[0]);
            }
        }
        return r;
    }
}
#if 1
String Linguistics::UnCapitalizeFirstWord (const String& s)
{
    // WAY too kludgly - but hopefully adequate for primitive message cleanups...
    //      -- LGP 2008-09-20
    String r    =   s;
    if (r.length () > 2) {
        if (r[0].ToUpperCase () == r[0] and r[1].ToUpperCase () != r[1]) {
            r.SetCharAt (r[0].ToLowerCase (), 0);
        }
    }
    return r;
}
#else
wstring Linguistics::UnCapitalizeFirstWord (const wstring& s)
{
    return UnCapitalizeFirstWord_ (s);
}

string  Linguistics::UnCapitalizeFirstWord (const string& s)
{
    return UnCapitalizeFirstWord_ (s);
}
#endif






/*
 ********************************************************************************
 ********************************** IsAllCaps ***********************************
 ********************************************************************************
 */
#if 1
bool    Linguistics::IsAllCaps (const String& s)
{
    return not s.empty () and s == s.ToUpperCase ();
}
#else
bool    Linguistics::IsAllCaps (const string& s)
{
    return not s.empty () and s == toupper (s);
}

bool    Linguistics::IsAllCaps (const wstring& s)
{
    return not s.empty () and s == toupper (s);
}
#endif



