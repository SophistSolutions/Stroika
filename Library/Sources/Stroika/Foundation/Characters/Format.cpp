/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <sstream>

#include "../Containers/Common.h"
#include "../Debug/Assertions.h"
#include "../Debug/Trace.h"
#include "../Math/Common.h"
#include "../Memory/SmallStackBuffer.h"
#include "CodePage.h"

#include "Format.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Memory;

/*
 ********************************************************************************
 ************************************* Format ***********************************
 ********************************************************************************
 */
DISABLE_COMPILER_MSC_WARNING_START (6262)
String Characters::FormatV (const wchar_t* format, va_list argsList)
{
    RequireNotNull (format);
    Memory::SmallStackBuffer<wchar_t, 10 * 1024> msgBuf (10 * 1024);
    const wchar_t*                               useFormat = format;
    wchar_t                                      newFormat[5 * 1024];
    {
        size_t origFormatLen = wcslen (format);
        Require (origFormatLen < NEltsOf (newFormat) / 2); // just to be sure safe - this is already crazy-big for format string...
        // Could use Memory::SmallStackBuffer<> but I doubt this will ever get triggered
        bool   lookingAtFmtCvt = false;
        size_t newFormatIdx    = 0;
        for (size_t i = 0; i < origFormatLen; ++i) {
            if (lookingAtFmtCvt) {
                switch (format[i]) {
                    case '%': {
                        lookingAtFmtCvt = false;
                    } break;
                    case 's': {
                        newFormat[newFormatIdx] = 'l';
                        newFormatIdx++;
                        lookingAtFmtCvt = false; // DONE
                    } break;
                    case '.': {
                        // could still be part for format string
                    } break;
                    default: {
                        if (isdigit (format[i])) {
                            // could still be part for format string
                        }
                        else {
                            lookingAtFmtCvt = false; // DONE
                        }
                    } break;
                }
            }
            else {
                if (format[i] == '%') {
                    lookingAtFmtCvt = true;
                }
            }
            newFormat[newFormatIdx] = format[i];
            newFormatIdx++;
        }
        Assert (newFormatIdx >= origFormatLen);
        if (newFormatIdx > origFormatLen) {
            newFormat[newFormatIdx] = '\0';
            useFormat               = newFormat;
        }
    }

    // SUBTLE: va_list looks like it is passed by value, but its not really,
    // and vswprintf, at least on GCC munges it. So we must use va_copy() to do this safely
    // @see http://en.cppreference.com/w/cpp/utility/variadic/va_copy
    va_list argListCopy;
    va_copy (argListCopy, argsList);
    // Assume only reason for failure is not enuf bytes, so allocate more.
    // If I'm wrong, we'll just runout of memory and throw out...
    while (::vswprintf (msgBuf, msgBuf.GetSize (), useFormat, argListCopy) < 0) {
        msgBuf.GrowToSize (msgBuf.GetSize () * 2);
        va_end (argListCopy);
        va_copy (argListCopy, argsList);
    }
    va_end (argListCopy);
    Assert (::wcslen (msgBuf) < msgBuf.GetSize ());
    return String (msgBuf);
}
DISABLE_COMPILER_MSC_WARNING_END (6262)

/*
 ********************************************************************************
 ************************************* Format ***********************************
 ********************************************************************************
 */
String Characters::Format (const wchar_t* format, ...)
{
    va_list argsList;
    va_start (argsList, format);
    String tmp = FormatV (format, argsList);
    va_end (argsList);
    return tmp;
}
