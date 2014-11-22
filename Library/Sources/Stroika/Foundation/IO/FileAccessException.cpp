/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/CString/Utilities.h"
#include    "../Characters/Format.h"

#include    "FileAccessException.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::IO;





/// TMPHACK - PUT HERE TIL WE HAVE CONSTEXPR WORKING
const Configuration::EnumNames<FileAccessMode>   IO::Stroika_Enum_Names(FileAccessMode) = {
    { FileAccessMode::eNoAccess, L"No-Access" },
    { FileAccessMode::eRead, L"Read" },
    { FileAccessMode::eWrite, L"Write" },
    { FileAccessMode::eReadWrite, L"Read-Write" },
};



/*
 ********************************************************************************
 **************************** IO::FileAccessException ***************************
 ********************************************************************************
 */
namespace   {
    wstring mkMessage_ (const String& fileName, FileAccessMode fileAccessMode)
    {
        DISABLE_COMPILER_MSC_WARNING_START(4800)
        wstring message;
        if (static_cast<bool> (fileAccessMode & FileAccessMode::eRead) and static_cast<bool> (fileAccessMode & FileAccessMode::eWrite)) {
            message = L"Cannot read from or write to file";
        }
        else if (static_cast<bool> (fileAccessMode & FileAccessMode::eRead)) {
            message = L"Cannot read from file";
        }
        else if (static_cast<bool> (fileAccessMode & FileAccessMode::eWrite)) {
            message = L"Cannot write to file";
        }
        else {
            message = L"Access failure for file";
        }
        DISABLE_COMPILER_MSC_WARNING_END(4800)
        if (not fileName.empty ()) {
            message = Characters::CString::Format (L"%s: '%.200s'", message.c_str (), fileName.LimitLength (100, false).c_str ());
        }
        return message;
    }
}
FileAccessException::FileAccessException (const String& fileName, FileAccessMode fileAccessMode)
    : StringException (mkMessage_ (fileName, fileAccessMode))
    , fFileName_ (fileName)
    , fFileAccessMode_ (fileAccessMode)
{
}

