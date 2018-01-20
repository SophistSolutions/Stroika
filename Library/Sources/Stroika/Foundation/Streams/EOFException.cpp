/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/String_Constant.h"

#include "EOFException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;

/*
 ********************************************************************************
 *************************** Streams::EOFException ******************************
 ********************************************************************************
 */
const EOFException EOFException::kThe;

EOFException::EOFException (bool partialReadCompleted)
    : StringException (partialReadCompleted ? Characters::String_Constant{L"Premature End-Of-Stream (partial read)"} : Characters::String_Constant{L"Premature End-Of-Stream"})
    , fPartialReadCompleted_ (partialReadCompleted)
{
}