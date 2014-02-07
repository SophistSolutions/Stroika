/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Float2String_inl_
#define _Stroika_Foundation_Characters_Float2String_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Containers/Common.h"
#include    "../Memory/SmallStackBuffer.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /*
            ********************************************************************************
            ****************************** Float2StringOptions *****************************
            ********************************************************************************
            */
            inline  Float2StringOptions::Precision::Precision (unsigned int p) :
                fPrecision (p)
            {
            }


            /*
            ********************************************************************************
            ****************************** Float2StringOptions *****************************
            ********************************************************************************
            */
            inline  Float2StringOptions::Float2StringOptions ()
                : fPrecision ()
                , fFmtFlags ()
                , fUseLocale ()
                , fTrimTrailingZeros (true)
            {
            }
            inline  Float2StringOptions::Float2StringOptions (UseCLocale)
                : fPrecision ()
                , fFmtFlags ()
                , fUseLocale ()
                , fTrimTrailingZeros (true)
            {
            }
            inline  Float2StringOptions::Float2StringOptions (UseCurrentLocale)
                : fPrecision ()
                , fFmtFlags ()
                , fUseLocale (locale ())
                , fTrimTrailingZeros (true)
            {
            }
            inline  Float2StringOptions::Float2StringOptions (const std::locale& l)
                : fPrecision ()
                , fFmtFlags ()
                , fUseLocale (l)
                , fTrimTrailingZeros (true)
            {
            }
            inline  Float2StringOptions::Float2StringOptions (std::ios_base::fmtflags fmtFlags)
                : fPrecision ()
                , fFmtFlags (fmtFlags)
                , fUseLocale ()
                , fTrimTrailingZeros (true)
            {
            }
            inline  Float2StringOptions::Float2StringOptions (Precision precision)
                : fPrecision (precision.fPrecision)
                , fFmtFlags ()
                , fUseLocale ()
                , fTrimTrailingZeros (true)
            {
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Characters_Float2String_inl_*/
