/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileSystem_inl_
#define _Stroika_Foundation_IO_FileSystem_FileSystem_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace FileSystem {
            }
        }
    }
}
namespace Stroika {
    namespace Foundation {
        namespace Configuration {
            template <>
            struct DefaultNames<IO::FileSystem::FileSystem::RemoveDirectoryPolicy> : EnumNames<IO::FileSystem::FileSystem::RemoveDirectoryPolicy> {
                static constexpr EnumNames<IO::FileSystem::FileSystem::RemoveDirectoryPolicy> k{
                    EnumNames<IO::FileSystem::FileSystem::RemoveDirectoryPolicy>::BasicArrayInitializer{
                        {
                            {IO::FileSystem::FileSystem::RemoveDirectoryPolicy::eFailIfNotEmpty, L"Fail-If-Not-Empty"},
                            {IO::FileSystem::FileSystem::RemoveDirectoryPolicy::eRemoveAnyContainedFiles, L"Remove-Any-Contained-Files"},
                        }}};
                DefaultNames ()
                    : EnumNames<IO::FileSystem::FileSystem::RemoveDirectoryPolicy> (k)
                {
                }
            };
        }
    }
}
#endif /*_Stroika_Foundation_IO_FileSystem_FileSystem_inl_*/
