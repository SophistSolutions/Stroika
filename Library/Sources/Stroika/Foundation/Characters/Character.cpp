/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Execution/Exceptions.h"

#include "Character.h"

using namespace Stroika;
using namespace Stroika::Foundation;

/*
 ********************************************************************************
 **************** Private_::ThrowSurrogatesOutOfRange_ **************************
 ********************************************************************************
 */
void Characters::Private_::ThrowSurrogatesOutOfRange_ ()
{
    static const auto kException_ = out_of_range{"UNICODE char16_t surrogates out of range"};
    Execution::Throw (kException_);
}
