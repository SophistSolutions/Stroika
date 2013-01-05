/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <cstdarg>
#include    <cstdlib>
#include    <iomanip>
#include    <sstream>

#include    "../Containers/Common.h"
#include    "../Debug/Assertions.h"
#include    "../Debug/Trace.h"
#include    "../Math/Common.h"
#include    "../Memory/SmallStackBuffer.h"
#include    "CodePage.h"

#include    "StringUtils.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Memory;






/*
 ********************************************************************************
 *********************************** tolower ************************************
 ********************************************************************************
 */
void    Characters::tolower (wstring* s)
{
    RequireNotNull (s);
    wstring::iterator   end =   s->end ();
    for (wstring::iterator i = s->begin (); i != end; ++i) {
        *i = TOLOWER (*i);
    }
}

void    Characters::tolower (string* s)
{
    RequireNotNull (s);
    string::iterator    end =   s->end ();
    for (string::iterator i = s->begin (); i != end; ++i) {
        *i = TOLOWER (*i);
    }
}





/*
 ********************************************************************************
 *********************************** toupper ************************************
 ********************************************************************************
 */
void    Characters::toupper (wstring* s)
{
    RequireNotNull (s);
    wstring::iterator   end =   s->end ();
    for (wstring::iterator i = s->begin (); i != end; ++i) {
        *i = TOUPPER (*i);
    }
}

void    Characters::toupper (string* s)
{
    RequireNotNull (s);
    string::iterator    end =   s->end ();
    for (string::iterator i = s->begin (); i != end; ++i) {
        *i = TOUPPER (*i);
    }
}











/*
 ********************************************************************************
 ********************************* Contains *************************************
 ********************************************************************************
 */
bool    Characters::Contains (const wstring& string2Search, const wstring& substr, CompareOptions co)
{
    if (co == CompareOptions::eWithCase) {
        return string2Search.find (substr) != wstring::npos;
    }
    else {
        if (string2Search.length () < substr.length ()) {
            return false;
        }
        // sloppy but workable implementation
        size_t  maxL    =   string2Search.length () - substr.length ();
        for (size_t i = 0; i <= maxL; ++i) {
            if (::wcsncasecmp (string2Search.c_str () + i, substr.c_str (), substr.length ()) == 0) {
                return true;
            }
        }
        return false;
    }
}





