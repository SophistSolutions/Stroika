/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/CString/Utilities.h"
#include    "../Characters/Format.h"

#include    "FileAccessMode.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::IO;


namespace {
    Configuration::EnumNames<FileAccessMode>    mkNames_ ()
    {
        static const   Configuration::EnumNames<FileAccessMode>    xFileAccessMode {
            Configuration::EnumNames<FileAccessMode>::BasicArrayInitializer {
                {
                    { FileAccessMode::eNoAccess, L"No-Access" },
                    { FileAccessMode::eRead, L"Read" },
                    { FileAccessMode::eWrite, L"Write" },
                    { FileAccessMode::eReadWrite, L"Read-Write" },
                }
            }
        };
        return xFileAccessMode;
    }
}



/*
 ********************************************************************************
 ********************************* FileAccessMode *******************************
 ********************************************************************************
 */
#if     qCompilerAndStdLib_constexpr_Buggy
Stroika::Foundation::IO::Private_::FileAccessModeData::FileAccessModeData ()
    : fFileAccessModeConfigNames  (mkNames_ ())
{
}
#endif
