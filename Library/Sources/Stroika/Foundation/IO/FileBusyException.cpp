/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/StringUtils.h"
#include    "../Characters/Format.h"

#include    "FileBusyException.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::IO;





/*
 ********************************************************************************
 **************************** IO::FileBusyException *****************************
 ********************************************************************************
 */
namespace   {
    wstring mkMessage_ (const TString& fileName)
    {
        if (fileName.empty ()) {
            return Characters::Format (L"File busy");
        }
        else {
            return Characters::Format (L"File busy: '%.200s'", Characters::LimitLength (Characters::TString2Wide (fileName), 100, false).c_str ());
        }
    }
}

FileBusyException::FileBusyException (const TString& fileName)
    : StringException (mkMessage_ (fileName))
    , fFileName_ (fileName)
{
}


