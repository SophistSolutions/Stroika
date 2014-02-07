/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Float2String_h_
#define _Stroika_Foundation_Characters_Float2String_h_    1

#include    "../StroikaPreComp.h"

#include    <ios>
#include    <locale>

#include    "../Configuration/Common.h"
#include    "../Memory/Optional.h"

#include    "String.h"



/**
 * TODO:
 *
 *  @todo   Consider moving notion of Precision into Math module. And if so - and maybe otherwise - make
 *          correct.
 *
 *  @todo   Notion of fPrecision in this module should probably be called something else (precisionAfterDecimalPoint?)
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /**
             *  Note - Float2String uses the locale specified by Float2StringOptions, but defaults to
             *  the "C" locale.
             *
             *  Precision (here) is defined to be the number of digits after the decimal point.
             *
             *  This prints and trims any trailing zeros (after the decimal point - fTrimTrailingZeros -
             *  by deafult.
             *
             *  Float2String maps nan to empty string;
             */
            struct  Float2StringOptions {
                enum UseCLocale { eUseCLocale };
                enum UseCurrentLocale { eUseCurrentLocale };
                struct Precision {
                    Precision (unsigned int p);
                    unsigned int fPrecision;
                };
                Float2StringOptions ();
                Float2StringOptions (UseCLocale);   // same as default
                Float2StringOptions (UseCurrentLocale);
                Float2StringOptions (const std::locale& l);
                Float2StringOptions (std::ios_base::fmtflags fmtFlags);
                Float2StringOptions (Precision precision);

                Memory::Optional<unsigned int>              fPrecision;
                Memory::Optional<std::ios_base::fmtflags>   fFmtFlags;
                Memory::Optional<std::locale>               fUseLocale; // if empty, use C-locale
                bool                                        fTrimTrailingZeros;
            };
            String Float2String (float f, const Float2StringOptions& options = Float2StringOptions ());
            String Float2String (double f, const Float2StringOptions& options = Float2StringOptions ());
            String Float2String (long double f, const Float2StringOptions& options = Float2StringOptions ());


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Float2String.inl"

#endif  /*_Stroika_Foundation_Characters_Float2String_h_*/
