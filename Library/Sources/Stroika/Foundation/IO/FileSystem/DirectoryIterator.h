/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_DirectoryIterator_h_
#define _Stroika_Foundation_IO_FileSystem_DirectoryIterator_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Configuration/Common.h"
#include "../../Configuration/Enumeration.h"
#include "../../Traversal/Iterator.h"

#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace FileSystem {

                using Characters::String;

                /**
                 *  This iterator returns the file names contained in the given argument directory.
                 *  See @todo.
                 *
                 *  See @DirectoryIterable
                 *
                 *  \note   DirectoryIterable will NOT return the special values '.' and '..' which would be returned from readdir
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 */
                class DirectoryIterator : public Traversal::Iterator<String> {
                public:
                    /**
                     */
                    enum class IteratorReturnType {
                        eFilenameOnly,
                        eDirPlusFilename, // argument directory + pathname (so can be relative)
                        eFullPathName,

                        eDEFAULT = eFilenameOnly,

                        Stroika_Define_Enum_Bounds (eFilenameOnly, eFullPathName)
                    };

                public:
                    /**
                     */
                    DirectoryIterator (const String& directory, IteratorReturnType iteratorReturns = IteratorReturnType::eDEFAULT);

                private:
                    class Rep_;
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
#include "DirectoryIterator.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_DirectoryIterator_h_*/
