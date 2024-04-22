/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"

#include "RequiredComponentMissingException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ********************** RequiredComponentMissingException ***********************
 ********************************************************************************
 */
namespace {
    String mkMsg_ (const String& component)
    {
        String cName = component;
        if (cName == RequiredComponentMissingException::kPDFViewer) {
            cName = "PDF Viewer"sv;
        }
        else if (cName == RequiredComponentMissingException::kPDFOCXViewer) {
            cName = "PDF Viewer (OCX)"sv;
        }
        return "A required component - {} - is missing, or is out of date"_f(component);
    }
}
RequiredComponentMissingException::RequiredComponentMissingException (const String& component)
    : Execution::RuntimeErrorException<>{mkMsg_ (component)}
    , fComponent{component}
{
}
