/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/CString/Utilities.h"
#include "../Characters/Format.h"

#include "FileFormatException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO;

/*
 ********************************************************************************
 **************************** IO::FileFormatException ***************************
 ********************************************************************************
 */
namespace {
    wstring mkMessage_ (const String& fileName)
    {
        if (fileName.empty ()) {
            return L"Invalid file format";
        }
        else {
            return Characters::CString::Format (L"Invalid file format: '%.200s'", fileName.LimitLength (100, false).c_str ());
        }
    }
}

FileFormatException::FileFormatException (const String& fileName)
    : StringException (mkMessage_ (fileName))
    , fFileName_ (fileName)
{
}
