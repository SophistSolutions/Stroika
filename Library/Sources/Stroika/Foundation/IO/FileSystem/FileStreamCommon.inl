/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileStreamCommon_inl_
#define _Stroika_Foundation_IO_FileSystem_FileStreamCommon_inl_ 1

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
            struct DefaultNames<IO::FileSystem::FileStreamCommon::SeekableFlag> : EnumNames<IO::FileSystem::FileStreamCommon::SeekableFlag> {
                static constexpr EnumNames<IO::FileSystem::FileStreamCommon::SeekableFlag> k{
                    EnumNames<IO::FileSystem::FileStreamCommon::SeekableFlag>::BasicArrayInitializer{
                        {
                            {IO::FileSystem::FileStreamCommon::SeekableFlag::eSeekable, L"Seekable"},
                            {IO::FileSystem::FileStreamCommon::SeekableFlag::eNotSeekable, L"NotSeekable"},
                        }}};
                DefaultNames ()
                    : EnumNames<IO::FileSystem::FileStreamCommon::SeekableFlag> (k)
                {
                }
            };
            template <>
            struct DefaultNames<IO::FileSystem::FileStreamCommon::AdoptFDPolicy> : EnumNames<IO::FileSystem::FileStreamCommon::AdoptFDPolicy> {
                static constexpr EnumNames<IO::FileSystem::FileStreamCommon::AdoptFDPolicy> k{
                    EnumNames<IO::FileSystem::FileStreamCommon::AdoptFDPolicy>::BasicArrayInitializer{
                        {
                            {IO::FileSystem::FileStreamCommon::AdoptFDPolicy::eCloseOnDestruction, L"Close-On-Destruction"},
                            {IO::FileSystem::FileStreamCommon::AdoptFDPolicy::eDisconnectOnDestruction, L"Disconnect-On-Destruction"},
                        }}};
                DefaultNames ()
                    : EnumNames<IO::FileSystem::FileStreamCommon::AdoptFDPolicy> (k)
                {
                }
            };
        }
    }
}
#endif /*_Stroika_Foundation_IO_FileSystem_FileStreamCommon_inl_*/
