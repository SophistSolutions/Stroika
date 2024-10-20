/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_Common_h_
#define _Stroika_Foundation_IO_FileSystem_Common_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <filesystem>

namespace Stroika::Foundation::Characters {
    class String;
}
namespace Stroika::Foundation::Time {
    class DateTime;
}

namespace Stroika::Foundation::IO::FileSystem {

    using namespace std::filesystem;

    using Characters::String;
    using Time::DateTime;

    /**
     *  @todo MAYBE MAKE UNSIGNED???
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
