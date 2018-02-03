/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/CString/Utilities.h"
#include "../Characters/Format.h"

#include "FileBusyException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO;

/*
 ********************************************************************************
 **************************** IO::FileBusyException *****************************
 ********************************************************************************
 */
namespace {
    wstring mkMessage_ (const String& fileName)
    {
        if (fileName.empty ()) {
            return L"File busy";
        }
        else {
            return Characters::CString::Format (L"File busy: '%s'", fileName.LimitLength (100, false).c_str ());
        }
    }
}

FileBusyException::FileBusyException (const String& fileName)
    : StringException (mkMessage_ (fileName))
    , fFileName_ (fileName)
{
}
