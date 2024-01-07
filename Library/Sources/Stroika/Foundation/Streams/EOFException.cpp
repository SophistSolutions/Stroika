/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "EOFException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Streams;

/*
 ********************************************************************************
 *************************** Streams::EOFException ******************************
 ********************************************************************************
 */
EOFException::EOFException (bool partialReadCompleted)
    : inherited{partialReadCompleted ? "End-Of-Stream (partial read)"sv : "End-Of-Stream"sv}
    , fPartialReadCompleted_{partialReadCompleted}
{
}
