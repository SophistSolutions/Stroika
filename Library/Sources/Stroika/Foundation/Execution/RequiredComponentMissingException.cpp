/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"

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
        return Characters::Format (L"A required component - %s - is missing, or is out of date", component.As<wstring> ().c_str ());
    }
}
RequiredComponentMissingException::RequiredComponentMissingException (const String& component)
    : Execution::RuntimeErrorException<>{mkMsg_ (component)}
    , fComponent{component}
{
}
