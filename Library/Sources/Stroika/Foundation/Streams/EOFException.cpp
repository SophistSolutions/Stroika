/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
    : StringException (partialReadCompleted ? L"Premature End-Of-Stream (partial read)"_k : L"Premature End-Of-Stream"_k)
    , fPartialReadCompleted_ (partialReadCompleted)
{
}