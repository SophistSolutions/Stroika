/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/StringUtils.h"
#include    "../Characters/Format.h"

#include    "FileAccessException.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::IO;



/*
 ********************************************************************************
 **************************** IO::FileAccessException ***************************
 ********************************************************************************
 */
namespace   {
    wstring mkMessage_ (const TString& fileName, FileAccessMode fileAccessMode)
    {
#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning (push)
#pragma warning (4 : 4800)
#endif
        wstring message;
        if (static_cast<bool> (fileAccessMode & FileAccessMode::eRead_FAM) and static_cast<bool> (fileAccessMode & FileAccessMode::eWrite_FAM)) {
            message = L"Cannot read from or write to file";
        }
        else if (static_cast<bool> (fileAccessMode & FileAccessMode::eRead_FAM)) {
            message = L"Cannot read from file";
        }
        else if (static_cast<bool> (fileAccessMode & FileAccessMode::eWrite_FAM)) {
            message = L"Cannot write to file";
        }
        else {
            message = L"Access failure for file";
        }
#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning (pop)
#endif
        if (not fileName.empty ()) {
            message = Characters::Format (L"%s: '%.200s'", message.c_str (), Characters::LimitLength (Characters::TString2Wide (fileName), 100, false).c_str ());
        }
        return message;
    }
}
FileAccessException::FileAccessException (const TString& fileName, FileAccessMode fileAccessMode)
    : StringException (mkMessage_ (fileName, fileAccessMode))
    , fFileName_ (fileName)
    , fFileAccessMode_ (fileAccessMode)
{
}

