/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String2Float_h_
#define _Stroika_Foundation_Characters_String2Float_h_    1

#include    "../StroikaPreComp.h"

#include    "String.h"



/**
 * TODO:
 *
 *      @todo   Consdier if we should have variants of these funtions taking a locale, or
 *              always using C/currnet locale. For the most part - I find it best to use the C locale.
 *              But DOCUMENT in all cases!!! And maybe implement variants...
 */




namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /**
             *  Convert the given decimal-format floating point string to an float,
             *  double, or long double.
             *
             *  String2Float will return nan () if no valid parse.
             *
             *  @see strtod(), or @see wcstod (). This is a simple wrapper on strtod() / wcstod () /
             *  strtold, etc... except that it returns nan() on invalid data, instead of zero.
             *
             *  strtod() /etc are more flexible. This is merely meant to be an often convenient wrapper.
             *  Use strtod etc directly to see if the string parsed properly.
             */
            template    <typename T>
            T  String2Float (const String& s);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "String2Float.inl"

#endif  /*_Stroika_Foundation_Characters_String2Float_h_*/
