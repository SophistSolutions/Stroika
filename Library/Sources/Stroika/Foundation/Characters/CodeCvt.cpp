/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Execution/Exceptions.h"

#include "CodeCvt.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Memory;

void Characters::Private_::ThrowErrorConvertingBytes2Characters_ ()
{
    static const auto kException_ = Execution::RuntimeErrorException{"Error converting bytes to characters"sv};
    Execution::Throw (kException_);
}

void Characters::Private_::ThrowErrorConvertingCharacters2Bytes_ ()
{
    static const auto kException_ = Execution::RuntimeErrorException{"Error converting characters to output format"sv};
    Execution::Throw (kException_);
}

string Characters::Private_::AsNarrowSDKString_ (const String& s) { return s.AsNarrowSDKString (); }
