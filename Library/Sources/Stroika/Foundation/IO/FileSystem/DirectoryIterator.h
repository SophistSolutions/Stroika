/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_DirectoryIterator_h_
#define _Stroika_Foundation_IO_FileSystem_DirectoryIterator_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Configuration/Enumeration.h"
#include "Stroika/Foundation/Traversal/Iterator.h"

#include "Common.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::IO::FileSystem {

    using Characters::String;

    /**
     *  This iterator returns the file names contained in the given argument directory.
     *  See @todo.
     *
     *  See @DirectoryIterable
     *
     *  \note   DirectoryIterable will NOT return the special values '.' and '..' which would be returned from readdir
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note PROBBALY GOING TO DEPRECATE IN FAVOR OF https://en.cppreference.com/w/cpp/filesystem/directory_iterator -- LGP 2020-06-29
     */
    class DirectoryIterator : public Traversal::Iterator<filesystem::path> {
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
        DirectoryIterator (const filesystem::path& directory, IteratorReturnType iteratorReturns = IteratorReturnType::eDEFAULT);

    private:
        class Rep_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DirectoryIterator.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_DirectoryIterator_h_*/
