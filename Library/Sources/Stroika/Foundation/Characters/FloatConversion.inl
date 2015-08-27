/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_FloatConversion_inl_
#define _Stroika_Foundation_Characters_FloatConversion_inl_  1


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
            *********************** Float2StringOptions::Precision *************************
            ********************************************************************************
            */
            inline  Float2StringOptions::Precision::Precision (unsigned int p)
                : fPrecision (p)
            {
            }


            /*
            ********************************************************************************
            ****************************** Float2StringOptions *****************************
            ********************************************************************************
            */
            inline  Float2StringOptions::Float2StringOptions (UseCLocale)
            {
            }
            inline  Float2StringOptions::Float2StringOptions (UseCurrentLocale)
                : fUseLocale (locale ())
            {
            }
            inline  Float2StringOptions::Float2StringOptions (const std::locale& l)
                :  fUseLocale (l)
            {
            }
            inline  Float2StringOptions::Float2StringOptions (std::ios_base::fmtflags fmtFlags)
                : fFmtFlags (fmtFlags)
            {
            }
            inline  Float2StringOptions::Float2StringOptions (Precision precision)
                : fPrecision (precision.fPrecision)
            {
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Characters_FloatConversion_inl_*/
