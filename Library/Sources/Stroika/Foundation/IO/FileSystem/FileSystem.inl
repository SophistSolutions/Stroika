/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileSystem_inl_
#define _Stroika_Foundation_IO_FileSystem_FileSystem_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::FileSystem {
}

namespace Stroika::Foundation::Configuration {
    template <>
    struct DefaultNames<IO::FileSystem::RemoveDirectoryPolicy> : EnumNames<IO::FileSystem::RemoveDirectoryPolicy> {
        static constexpr EnumNames<IO::FileSystem::RemoveDirectoryPolicy> k{
            EnumNames<IO::FileSystem::RemoveDirectoryPolicy>::BasicArrayInitializer{
                {
                    {IO::FileSystem::RemoveDirectoryPolicy::eFailIfNotEmpty, L"Fail-If-Not-Empty"},
                    {IO::FileSystem::RemoveDirectoryPolicy::eRemoveAnyContainedFiles, L"Remove-Any-Contained-Files"},
                }}};
        DefaultNames ()
            : EnumNames<IO::FileSystem::RemoveDirectoryPolicy> (k)
        {
        }
    };
}

#endif /*_Stroika_Foundation_IO_FileSystem_FileSystem_inl_*/
