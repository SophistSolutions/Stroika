/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/String_Constant.h"

#include "EOFException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Streams;

/*
 ********************************************************************************
 *************************** Streams::EOFException ******************************
 ********************************************************************************
 */
const EOFException EOFException::kThe;

EOFException::EOFException (bool partialReadCompleted)
    : inherited (partialReadCompleted ? L"Premature End-Of-Stream (partial read)"sv : L"Premature End-Of-Stream"sv)
    , fPartialReadCompleted_ (partialReadCompleted)
{
}