/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
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
namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {
#if     qCompilerAndStdLib_const_Array_Init_wo_UserDefined_Buggy
            template    <>
            const   EnumNames<IO::FileAccessMode>   DefaultNames<IO::FileAccessMode>::k = EnumNames<IO::FileAccessMode>::BasicArrayInitializer {
                {
                    { IO::FileAccessMode::eNoAccess, L"No-Access" },
                    { IO::FileAccessMode::eRead, L"Read" },
                    { IO::FileAccessMode::eWrite, L"Write" },
                    { IO::FileAccessMode::eReadWrite, L"Read-Write" },
                }
            };
#else
            template    <>
            const   EnumNames<IO::FileAccessMode>   DefaultNames<IO::FileAccessMode>::k {
                EnumNames<IO::FileAccessMode>::BasicArrayInitializer {
                    {
                        { IO::FileAccessMode::eNoAccess, L"No-Access" },
                        { IO::FileAccessMode::eRead, L"Read" },
                        { IO::FileAccessMode::eWrite, L"Write" },
                        { IO::FileAccessMode::eReadWrite, L"Read-Write" },
                    }
                }
            };
#endif
        }
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
