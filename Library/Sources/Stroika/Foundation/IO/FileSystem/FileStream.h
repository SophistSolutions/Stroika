/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileStream_h_
#define _Stroika_Foundation_IO_FileSystem_FileStream_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 */

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace FileSystem {

                /**
                 *  'FileStream' is a quasi-namespace:   not usable in and of itself - but rather a collection of defines to share common things among the FileStream subclasses.
                 */
                struct FileStream {
                protected:
                    FileStream () = default;

                public:
                    /**
                     */
                    enum class SeekableFlag {
                        eSeekable,
                        eNotSeekable,

                        eDEFAULT = eSeekable,

                        Stroika_Define_Enum_Bounds (eSeekable, eNotSeekable)
                    };
                    static constexpr SeekableFlag eSeekable    = SeekableFlag::eSeekable;
                    static constexpr SeekableFlag eNotSeekable = SeekableFlag::eNotSeekable;

                public:
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
                    static constexpr AdoptFDPolicy eCloseOnDestruction      = AdoptFDPolicy::eCloseOnDestruction;
                    static constexpr AdoptFDPolicy eDisconnectOnDestruction = AdoptFDPolicy::eDisconnectOnDestruction;

                public:
                    /**
                     */
                    using FileDescriptorType = int;
                };
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FileStream.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_FileStream_h_*/
