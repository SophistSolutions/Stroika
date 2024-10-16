/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"

#include "Utils.h"

using namespace Stroika::Foundation;

using namespace Characters;
using namespace Database;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 *********************** SQL::Utils::QuoteStringForDB *****************************
 ********************************************************************************
 */
String SQL::Utils::QuoteStringForDB (const String& s)
{
    if (s.Contains ('\'')) {
        StringBuilder sb;
        for (Character c : s) {
            if (c == '\'') {
                sb << '\'';
            }
            sb << c;
        }
        return sb;
    }
    else {
        return s;
    }
}
