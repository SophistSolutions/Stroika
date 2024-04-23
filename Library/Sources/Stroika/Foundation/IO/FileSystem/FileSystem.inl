/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::IO::FileSystem {
}

namespace Stroika::Foundation::Configuration {
    template <>
    constexpr EnumNames<IO::FileSystem::RemoveDirectoryPolicy> DefaultNames<IO::FileSystem::RemoveDirectoryPolicy>::k{{{
        {IO::FileSystem::RemoveDirectoryPolicy::eFailIfNotEmpty, L"Fail-If-Not-Empty"},
        {IO::FileSystem::RemoveDirectoryPolicy::eRemoveAnyContainedFiles, L"Remove-Any-Contained-Files"},
    }}};
}
