/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"

#include    "RequiredComponentMissingException.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;






/*
 ********************************************************************************
 ********************** RequiredComponentMissingException ***********************
 ********************************************************************************
 */
const   wchar_t RequiredComponentMissingException::kJava[]          =   L"Java";
const   wchar_t RequiredComponentMissingException::kPDFViewer[]     =   L"PDFViewer";
const   wchar_t RequiredComponentMissingException::kPDFOCXViewer[]  =   L"PDFOCXViewer";
namespace   {
    wstring mkMsg (const wstring& component)
    {
        wstring cName   =   component;
        if (cName == RequiredComponentMissingException::kPDFViewer)         { cName = L"PDF Viewer";        }
        else if (cName == RequiredComponentMissingException::kPDFOCXViewer) { cName = L"PDF Viewer (OCX)";  }
        wstring x   =   Characters::Format (L"A required component - %s - is missing, or is out of date", component.c_str ());
        return x;
    }
}
RequiredComponentMissingException::RequiredComponentMissingException (const wstring& component):
    StringException (mkMsg (component)),
    fComponent (component)
{
}





