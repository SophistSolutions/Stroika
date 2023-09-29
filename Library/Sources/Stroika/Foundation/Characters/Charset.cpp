/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "String.h"

#include "Charset.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

struct Charset::Rep_ {
    String fValue;
};

/*
 ********************************************************************************
 ****************************** Characters::Charset *****************************
 ********************************************************************************
 */
Charset::Charset (const std::string& charsetName)
    : fRep_{make_shared<Rep_> (String{charsetName})}
{
}
Charset::Charset (const std::string_view& charsetName)
    : fRep_{make_shared<Rep_> (String{charsetName})}
{
}
Charset::Charset (const String& charsetName)
    : fRep_{make_shared<Rep_> (charsetName)}
{
}

Charset::operator String () const
{
    return fRep_->fValue;
}
