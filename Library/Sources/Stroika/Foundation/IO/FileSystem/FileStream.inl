/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileStream_inl_
#define _Stroika_Foundation_IO_FileSystem_FileStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::FileSystem::FileStream {
}

namespace Stroika::Foundation::Configuration {
    template <>
    constexpr EnumNames<IO::FileSystem::FileStream::AdoptFDPolicy> DefaultNames<IO::FileSystem::FileStream::AdoptFDPolicy>::k{{{
        {IO::FileSystem::FileStream::AdoptFDPolicy::eCloseOnDestruction, L"Close-On-Destruction"},
        {IO::FileSystem::FileStream::AdoptFDPolicy::eDisconnectOnDestruction, L"Disconnect-On-Destruction"},
    }}};
}

#endif /*_Stroika_Foundation_IO_FileSystem_FileStream_inl_*/
