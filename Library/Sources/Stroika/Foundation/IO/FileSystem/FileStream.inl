/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileStream_inl_
#define _Stroika_Foundation_IO_FileSystem_FileStream_inl_ 1

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
            struct DefaultNames<IO::FileSystem::FileStream::SeekableFlag> : EnumNames<IO::FileSystem::FileStream::SeekableFlag> {
                static constexpr EnumNames<IO::FileSystem::FileStream::SeekableFlag> k{
                    EnumNames<IO::FileSystem::FileStream::SeekableFlag>::BasicArrayInitializer{
                        {
                            {IO::FileSystem::FileStream::SeekableFlag::eSeekable, L"Seekable"},
                            {IO::FileSystem::FileStream::SeekableFlag::eNotSeekable, L"NotSeekable"},
                        }}};
                DefaultNames ()
                    : EnumNames<IO::FileSystem::FileStream::SeekableFlag> (k)
                {
                }
            };
            template <>
            struct DefaultNames<IO::FileSystem::FileStream::AdoptFDPolicy> : EnumNames<IO::FileSystem::FileStream::AdoptFDPolicy> {
                static constexpr EnumNames<IO::FileSystem::FileStream::AdoptFDPolicy> k{
                    EnumNames<IO::FileSystem::FileStream::AdoptFDPolicy>::BasicArrayInitializer{
                        {
                            {IO::FileSystem::FileStream::AdoptFDPolicy::eCloseOnDestruction, L"Close-On-Destruction"},
                            {IO::FileSystem::FileStream::AdoptFDPolicy::eDisconnectOnDestruction, L"Disconnect-On-Destruction"},
                        }}};
                DefaultNames ()
                    : EnumNames<IO::FileSystem::FileStream::AdoptFDPolicy> (k)
                {
                }
            };
        }
    }
}
#endif /*_Stroika_Foundation_IO_FileSystem_FileStream_inl_*/
