/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
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
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace Configuration {
            constexpr   EnumNames<IO::FileAccessMode>    DefaultNames<IO::FileAccessMode>::k;
        }
    }
}
