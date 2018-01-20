/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/CString/Utilities.h"
#include "../Characters/Format.h"

#include "FileAccessException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO;

/*
 ********************************************************************************
 **************************** IO::FileAccessException ***************************
 ********************************************************************************
 */
namespace {
    wstring mkMessage_ (const Memory::Optional<String>& fileName, const Memory::Optional<FileAccessMode>& fileAccessMode)
    {
        DISABLE_COMPILER_MSC_WARNING_START (4800)
        wstring message;
        if (fileAccessMode and static_cast<bool> (*fileAccessMode & FileAccessMode::eRead) and static_cast<bool> (*fileAccessMode & FileAccessMode::eWrite)) {
            message = L"Cannot read from or write to file";
        }
        else if (fileAccessMode and static_cast<bool> (*fileAccessMode & FileAccessMode::eRead)) {
            message = L"Cannot read from file";
        }
        else if (fileAccessMode and static_cast<bool> (*fileAccessMode & FileAccessMode::eWrite)) {
            message = L"Cannot write to file";
        }
        else {
            message = L"Access failure for file";
        }
        DISABLE_COMPILER_MSC_WARNING_END (4800)
        if (fileName) {
            message = Characters::CString::Format (L"%s: '%.200s'", message.c_str (), fileName->LimitLength (100, false).c_str ());
        }
        return message;
    }
}
FileAccessException::FileAccessException (const Memory::Optional<String>& fileName, const Memory::Optional<FileAccessMode>& fileAccessMode)
    : StringException (mkMessage_ (fileName, fileAccessMode))
    , fFileName_ (fileName)
    , fFileAccessMode_ (fileAccessMode)
{
}
