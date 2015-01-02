/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <limits>

#include    "../Containers/Common.h"
#include    "../Debug/Assertions.h"
#include    "../Debug/Trace.h"
#include    "../Math/Common.h"

#include    "String2Float.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;





/*
 ********************************************************************************
 ********************************* String2Float *********************************
 ********************************************************************************
 */
namespace {
    template    <typename RETURN_TYPE, typename FUNCTION>
    inline  RETURN_TYPE  String2Float_ (const String& s, FUNCTION F)
    {
        wchar_t*        e   = nullptr;
        const wchar_t*  cst = s.c_str ();
        RETURN_TYPE  d = F (cst, &e);
        if (d == 0) {
            if (cst == e) {
                return Math::nan<RETURN_TYPE> ();
            }
        }
        return d;
    }
}


namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {
            template    <>
            float  String2Float (const String& s)
            {
                return String2Float_<float> (s, wcstof);
            }

            template    <>
            double  String2Float (const String& s)
            {
                return String2Float_<double> (s, wcstod);
            }

            template    <>
            long double  String2Float (const String& s)
            {
                return String2Float_<long double> (s, wcstold);
            }
        }
    }
}
