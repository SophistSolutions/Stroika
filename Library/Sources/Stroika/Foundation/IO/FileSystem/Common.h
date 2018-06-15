/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_Common_h_
#define _Stroika_Foundation_IO_FileSystem_Common_h_ 1

#include "../../StroikaPreComp.h"

// clang-format off
#if __has_include(<filesystem>)
#include <filesystem>
#elif __has_include(<experimental/filesystem>) && !qCompilerAndStdLib_stdfilesystemAppearsPresentButDoesntWork_Buggy
#include <experimental/filesystem>
#elif qHasFeature_boost
#include <boost/filesystem.hpp>
#endif
// clang-format on

#include "../../Characters/String.h"
#include "../../Configuration/Common.h"
#include "../../Time/DateTime.h"

#include "../FileAccessMode.h"
#include "Directory.h"

/**
 * TODO:
 *
 */

// clang-format off
#if !(__cpp_lib_filesystem >= 201603) 
#if (__cpp_lib_experimental_filesystem >= 201406 || __has_include(<experimental/filesystem>)) && !qCompilerAndStdLib_stdfilesystemAppearsPresentButDoesntWork_Buggy
namespace std {
    namespace filesystem {
        using namespace std::experimental::filesystem;
    }
}
#elif qHasFeature_boost
namespace std {
    namespace filesystem {
        using namespace boost::filesystem;
    }
}
#endif
#endif
// clang-format on

namespace Stroika::Foundation {
    namespace IO {
        namespace FileSystem {

// clang-format off
#if __has_include(<filesystem>) || __has_include(<experimental/filesystem>)
                using namespace std::filesystem;
#endif
            // clang-format on

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

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_Common_h_*/
