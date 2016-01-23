/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/CString/Utilities.h"
#include    "../Characters/Format.h"
#include    "../Configuration/Enumeration.h"

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





/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace Configuration {
#if     qCompilerAndStdLib_constexpr_Buggy
            template<>
            const EnumNames<FileAccessMode>   DefaultNames<FileAccessMode>::k {
                EnumNames<FileAccessMode>::BasicArrayInitializer {
                    {
                        { FileAccessMode::eNoAccess, L"No-Access" },
                        { FileAccessMode::eRead, L"Read" },
                        { FileAccessMode::eWrite, L"Write" },
                        { FileAccessMode::eReadWrite, L"Read-Write" },
                    }
                }
            };
#else
            template<>
            constexpr   EnumNames<IO::FileAccessMode>    DefaultNames<IO::FileAccessMode>::k;
#endif
        }
    }
}
#if 0
namespace Stroika {
    namespace Foundation {
        namespace Configuration {
            template<>
            const EnumNames<FileAccessMode>   DefaultNames<FileAccessMode>::k
#if     qCompilerAndStdLib_const_Array_Init_wo_UserDefined_Buggy
                =
#endif
            {
                {
                    EnumNames<FileAccessMode>::BasicArrayInitializer {
                        {
                            { FileAccessMode::eNoAccess, L"No-Access" },
                            { FileAccessMode::eRead, L"Read" },
                            { FileAccessMode::eWrite, L"Write" },
                            { FileAccessMode::eReadWrite, L"Read-Write" },
                        }
                    }
                }
            };
        }
    }
}
#endif
