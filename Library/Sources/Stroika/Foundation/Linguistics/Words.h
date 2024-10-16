/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Linguistics_Words_h_
#define _Stroika_Foundation_Linguistics_Words_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"

/**
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
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
