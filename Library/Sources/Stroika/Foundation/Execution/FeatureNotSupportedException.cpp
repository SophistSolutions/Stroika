/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Format.h"

#include "FeatureNotSupportedException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ************************ FeatureNotSupportedException **************************
 ********************************************************************************
 */
FeatureNotSupportedException::FeatureNotSupportedException ()
    : Execution::RuntimeErrorException<>{"Feature Not Supported"sv}
{
}

FeatureNotSupportedException::FeatureNotSupportedException (const String& featureName)
    : Execution::RuntimeErrorException<>{"Feature '"sv + featureName + "' Not Supported"sv}
    , fFeature_{featureName}
{
}

optional<String> FeatureNotSupportedException::GetOperationName () const
{
    return fFeature_;
}
