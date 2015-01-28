/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <cstdarg>
#include    <cstdlib>
#include    <iomanip>
#include    <limits>
#include    <sstream>

#include    "../Containers/Common.h"
#include    "../Debug/Assertions.h"
#include    "../Debug/Trace.h"
#include    "../Math/Common.h"
#include    "../Memory/SmallStackBuffer.h"
#include    "CodePage.h"

#include    "Float2String.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Memory;





/*
 ********************************************************************************
 ********************************* Float2String *********************************
 ********************************************************************************
 */
namespace {
    template    <typename FLOAT_TYPE>
    inline  String Float2String_ (FLOAT_TYPE f, const Float2StringOptions& options)
    {
        if (std::isnan (f)) {
            return String ();
        }
        stringstream s;
        if (options.fUseLocale.IsPresent ()) {
            s.imbue (*options.fUseLocale);
        }
        else {
            static  const   locale  kCLocale_ = locale::classic ();
            s.imbue (kCLocale_);
        }
        if (options.fPrecision.IsPresent ()) {
            s << setprecision (*options.fPrecision);
        }
        if (options.fFmtFlags.IsPresent ()) {
            s << setiosflags (*options.fFmtFlags);
        }
        s << f;
        String tmp = String::FromAscii (s.str ());
        if (options.fTrimTrailingZeros) {
            // strip trailing zeros - except for the last first one after the decimal point
            size_t pastDot = tmp.find ('.');
            if (pastDot != String::npos) {
                pastDot++;
                size_t pPastLastZero = tmp.length ();
                for (; (pPastLastZero - 1) > pastDot; --pPastLastZero) {
                    if (tmp[pPastLastZero - 1] != '0') {
                        break;
                    }
                }
                tmp = tmp.SubString (0, pPastLastZero);
            }
        }
        return tmp;
    }

}
String Characters::Float2String (float f, const Float2StringOptions& options)
{
    return Float2String_<long double> (f, options);
}

String Characters::Float2String (double f, const Float2StringOptions& options)
{
    return Float2String_<long double> (f, options);
}

String Characters::Float2String (long double f, const Float2StringOptions& options)
{
    return Float2String_<long double> (f, options);
}


