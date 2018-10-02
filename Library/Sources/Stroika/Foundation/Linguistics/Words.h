/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Linguistics_Words_h_
#define _Stroika_Foundation_Linguistics_Words_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"
#include "../Configuration/Common.h"

/**
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *      @todo   Very VERY primitive linguistic support, but this could easily evolve over time. Led has a bunch of
 *              lingusitic code I could move here, and I'm sure I could dig up more...
 *
 *      @todo   Perhaps these functions should take as arugment a locale() object. This is currently totally English-biased/based
 */

namespace Stroika::Foundation::Linguistics {

    using Characters::String;

    /**
     *  Implement current-ui-language-specific noun-pluralization logic for the given noun string (assuming the count of that noun
     *  is given (english rules - if count != 1 - append s, but we don't wnat that logic to proliferate through the app, so
     *  its easier to localize.
     *
     *  The variation with two strings - the second one is the explicit plural - just plugged in if the count is non-zero
     */
    String PluralizeNoun (const String& s, int count = 1000);
    String PluralizeNoun (const String& s, const String& sPlural, int count = 1000);

    /**
     */
    String MungeStringSoSingular (const String& s);

    /**
     * e.g. 'joe smith' becomes 'Joe Smith'
     */
    String CapitalizeEachWord (const String& s);

    /**
     * e.g. 'joe smith' becomes 'Joe smith'
     */
    String CapitalizeEachSentence (const String& s);

    /**
     * e.g.  'Joe Smith' becomes 'joe Smith', but 'IBM eats' stays 'IBM eats'
     */
    String UnCapitalizeFirstWord (const String& s);

    /**
     */
    bool IsAllCaps (const String& s);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Linguistics_Words_h_*/
