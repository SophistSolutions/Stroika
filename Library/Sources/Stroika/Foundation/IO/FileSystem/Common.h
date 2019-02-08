/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_Common_h_
#define _Stroika_Foundation_IO_FileSystem_Common_h_ 1

#include "../../StroikaPreComp.h"

#if __has_include(<filesystem>)
#include <filesystem>
#elif __has_include(<experimental/filesystem>) && !qCompilerAndStdLib_stdfilesystemAppearsPresentButDoesntWork_Buggy
#include <experimental/filesystem>
#elif qHasFeature_boost
#include <boost/filesystem.hpp>
#endif

namespace Stroika::Foundation::Characters {
    class String;
}
namespace Stroika::Foundation::Time {
    class DateTime;
}

/**
 * TODO:
 */

/*
 *  If forced to use boost filesystem or experimental filesystem, make it look like std::filesystem.
 */
#if !(__cpp_lib_filesystem >= 201603)
#if (__cpp_lib_experimental_filesystem >= 201406 || __has_include(<experimental/filesystem>)) && !qCompilerAndStdLib_stdfilesystemAppearsPresentButDoesntWork_Buggy
namespace std::filesystem {
    using namespace std::experimental::filesystem;
}
#elif qHasFeature_boost
namespace std::filesystem {
    using namespace boost::filesystem;
}
#endif
#endif

namespace Stroika::Foundation::IO::FileSystem {

#if !__has_include(<filesystem>) && !__has_include(<experimental/filesystem>) && !qHasFeature_boost
    static_assert (false, "some version of std::filesystem is required by Stroika v2.1 or later");
#endif
    using namespace std::filesystem;

    using Characters::String;
    using Time::DateTime;

    /**
     *  @todo MAYBE MAKE UNSINGED???
     */
    using FileOffset_t = int64_t;
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_Common_h_*/
