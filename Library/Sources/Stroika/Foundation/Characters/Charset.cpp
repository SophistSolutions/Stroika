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
    Rep_ (const String& v)
        : fValue{v}
    {
    }
    String fValue;
};

/*
 ********************************************************************************
 ****************************** Characters::Charset *****************************
 ********************************************************************************
 */
Charset::Charset (const std::string& charsetName)
    : fRep_{make_shared<Rep_> (String{charsetName})} // conversion throws if not valid ascii
{
}
Charset::Charset (const std::string_view& charsetName)
    : fRep_{make_shared<Rep_> (String{charsetName})} // conversion asserts valid ASCII
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

string Charset::AsNarrowSDKString () const
{
    return fRep_->fValue.AsNarrowSDKString ();
}

strong_ordering Charset::operator<=> (const Charset& rhs) const
{
    return String ::ThreeWayComparer{CompareOptions::eCaseInsensitive}(fRep_->fValue, rhs.fRep_->fValue);
}

bool Charset::operator== (const Charset& rhs) const
{
    return String::EqualsComparer{CompareOptions::eCaseInsensitive}(fRep_->fValue, rhs.fRep_->fValue);
}
