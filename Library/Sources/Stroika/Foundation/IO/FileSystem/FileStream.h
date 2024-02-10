/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileStream_h_
#define _Stroika_Foundation_IO_FileSystem_FileStream_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Configuration/Enumeration.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::IO::FileSystem::FileStream {

    /**
     *  This only applies to File Streams constructed with an argument FileDescriptor. This controls whether the adopted
     *  file descriptor will be automatically closed when the last 'shared' reference to the stream goes out of scope.
     *
     *  By far the most common answer will be eCloseOnDestruction - the default, but eDisconnectOnDestruction can be helpful
     *  when multiple streams are associated with a given file descriptor.
     */
    enum class AdoptFDPolicy {
        eCloseOnDestruction,
        eDisconnectOnDestruction,

        eDEFAULT = eCloseOnDestruction,

        Stroika_Define_Enum_Bounds (eCloseOnDestruction, eDisconnectOnDestruction)
    };
    using AdoptFDPolicy::eCloseOnDestruction;
    using AdoptFDPolicy::eDisconnectOnDestruction;

    /**
     */
    using FileDescriptorType = int;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FileStream.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_FileStream_h_*/
