/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Execution/Exceptions.h"

#include "UTFConvert.h"

#include "Character.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

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
    static const auto kException_ = Execution::RuntimeErrorException{"Argument not valid Latin1 (UNICODE code point > U+00ff)"};
    Execution::Throw (kException_);
}



/*
 ********************************************************************************
 ************************************ Character *********************************
 ********************************************************************************
 */
template <>
void Character::AsHelper_ (Memory::StackBuffer<char8_t>* buf) const
{
    RequireNotNull (buf);
    buf->resize_uninitialized (4);  // enuf
    char32_t thisC = GetCharacterCode ();
    auto s = UTFConvert::kThe.ConvertSpan (span{&thisC, 1}, span{*buf});
    buf->ShrinkTo (s.size ());
}

template <>
void Character::AsHelper_ (Memory::StackBuffer<char16_t>* buf) const
{
    RequireNotNull (buf);
    buf->resize_uninitialized (4); // enuf
    char32_t thisC = GetCharacterCode ();
    auto     s     = UTFConvert::kThe.ConvertSpan (span{&thisC, 1}, span{*buf});
    buf->ShrinkTo (s.size ());
}
