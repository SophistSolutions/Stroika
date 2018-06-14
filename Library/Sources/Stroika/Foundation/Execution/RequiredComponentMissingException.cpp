/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/CString/Utilities.h"
#include "../Characters/Format.h"

#include "RequiredComponentMissingException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ********************** RequiredComponentMissingException ***********************
 ********************************************************************************
 */
namespace {
    wstring mkMsg (const wstring& component)
    {
        wstring cName = component;
        if (cName == RequiredComponentMissingException::kPDFViewer) {
            cName = L"PDF Viewer";
        }
        else if (cName == RequiredComponentMissingException::kPDFOCXViewer) {
            cName = L"PDF Viewer (OCX)";
        }
        wstring x = Characters::CString::Format (L"A required component - %s - is missing, or is out of date", component.c_str ());
        return x;
    }
}
RequiredComponentMissingException::RequiredComponentMissingException (const wstring& component)
    : StringException (mkMsg (component))
    , fComponent (component)
{
}
