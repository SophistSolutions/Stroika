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
    static const auto kException_ = Execution::RuntimeErrorException<out_of_range>{"UNICODE char16_t surrogates out of range"};
    Execution::Throw (kException_);
}

/*
 ********************************************************************************
 ************************** Private_::ThrowNotIsASCII_ **************************
 ********************************************************************************
 */
void Characters::Private_::ThrowNotIsASCII_ ()
{
    static const auto kException_ = Execution::RuntimeErrorException{"Argument not valid ASCII"};
    Execution::Throw (kException_);
}

/*
 ********************************************************************************
 ************************** Private_::ThrowNotIsLatin1_ *************************
 ********************************************************************************
 */
void Characters::Private_::ThrowNotIsLatin1_ ()
{
    static const auto kException_ = Execution::RuntimeErrorException{"Argument not valid Character_ISOLatin1 (UNICODE code point > U+00ff)"};
    Execution::Throw (kException_);
}
