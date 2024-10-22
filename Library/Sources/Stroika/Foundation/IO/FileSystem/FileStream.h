/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileStream_h_
#define _Stroika_Foundation_IO_FileSystem_FileStream_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_Foundation_Common_Platform_POSIX
#include <unistd.h>
#endif

#include "Stroika/Foundation/Common/Enumeration.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::IO::FileSystem::FileStream {

    /**
     *  This only applies to File Streams constructed with an argument FileDescriptor. This controls whether the adopted
     *  file descriptor will be automatically closed when the last 'shared' reference to the stream goes out of scope.
     *
     *  By far the most common answer will be eCloseOnDestruction, but eDisconnectOnDestruction can be helpful
     *  when multiple streams are associated with a given file descriptor (or for predefined descriptors like stdin).
     * 
     *  Intentionally provide no eDEFAULT, since rarely used by file descriptor, and best to be clear when doing so about the
     *  treatment on close.
     */
    enum class AdoptFDPolicy {
        eCloseOnDestruction,
        eDisconnectOnDestruction,

        Stroika_Define_Enum_Bounds (eCloseOnDestruction, eDisconnectOnDestruction)
    };
    using AdoptFDPolicy::eCloseOnDestruction;
    using AdoptFDPolicy::eDisconnectOnDestruction;

    /**
     *      https://en.wikipedia.org/wiki/File_descriptor
     * 
     *      Integer value   Name            <unistd.h> symbolic constant[1] <stdio.h> file stream[2]
     *      0               Standard input  STDIN_FILENO                    stdin
     *      1               Standard output STDOUT_FILENO                   stdout
     *      2               Standard error  STDERR_FILENO                   stderr
     */
    using FileDescriptorType = int;

#if !qStroika_Foundation_Common_Platform_POSIX and !defined(STDIN_FILENO)
#define STDIN_FILENO 0
#endif
#if !qStroika_Foundation_Common_Platform_POSIX and !defined(STDOUT_FILENO)
#define STDOUT_FILENO 1
#endif
#if !qStroika_Foundation_Common_Platform_POSIX and !defined(STDERR_FILENO)
#define STDERR_FILENO 2
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FileStream.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_FileStream_h_*/
