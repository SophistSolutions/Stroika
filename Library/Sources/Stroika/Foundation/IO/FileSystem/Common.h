/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_Common_h_
#define _Stroika_Foundation_IO_FileSystem_Common_h_ 1

#include "../../StroikaPreComp.h"

#if __has_include(<filesystem>)
#include <filesystem>
#elif __has_include(<experimental / filesystem>)
#include <experimental/filesystem>
#endif

#include "../../Characters/String.h"
#include "../../Configuration/Common.h"
#include "../../Time/DateTime.h"

#include "../FileAccessMode.h"
#include "Directory.h"

/**
 * TODO:
 *
 */

#if !(__cpp_lib_filesystem >= 201603) && (__cpp_lib_experimental_filesystem >= 201406 || __has_include(<experimental / filesystem>))
namespace std {
    namespace filesystem {
        using namespace std::experimental::filesystem;
    }
}
#endif

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace FileSystem {

#if __has_include(<filesystem>) || __has_include(<experimental / filesystem>)
                using namespace std::filesystem;
#endif

                using Characters::String;
                using Memory::Byte;
                using Time::DateTime;

                /**
                 *  @todo MAYBNE MAKE UNSINGED???
                 */
                using FileOffset_t = int64_t;
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_Common_h_*/
