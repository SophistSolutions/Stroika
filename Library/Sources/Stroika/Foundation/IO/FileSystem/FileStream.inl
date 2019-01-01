/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileStream_inl_
#define _Stroika_Foundation_IO_FileSystem_FileStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::FileSystem {
}

namespace Stroika::Foundation::Configuration {
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

#endif /*_Stroika_Foundation_IO_FileSystem_FileStream_inl_*/
