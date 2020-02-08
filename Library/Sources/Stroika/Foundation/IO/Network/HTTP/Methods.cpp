/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "Methods.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;

/*
 ********************************************************************************
 ****************** HTTP::MethodsRegularExpressions *****************************
 ********************************************************************************
 */
const RegularExpression MethodsRegularExpressions::kGet       = L"GET"_RegEx;
const RegularExpression MethodsRegularExpressions::kPut       = L"PUT"_RegEx;
const RegularExpression MethodsRegularExpressions::kPatch     = L"PATCH"_RegEx;
const RegularExpression MethodsRegularExpressions::kPost      = L"POST"_RegEx;
const RegularExpression MethodsRegularExpressions::kPostOrPut = L"PUT|POST"_RegEx;
const RegularExpression MethodsRegularExpressions::kDelete    = L"DELETE"_RegEx;
const RegularExpression MethodsRegularExpressions::kOptions   = L"OPTIONS"_RegEx;
