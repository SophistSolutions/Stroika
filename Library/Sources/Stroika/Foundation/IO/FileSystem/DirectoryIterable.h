/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_DirectoryIterable_h_
#define _Stroika_Foundation_IO_FileSystem_DirectoryIterable_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <filesystem>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Traversal/Iterable.h"

#include "Stroika/Foundation/IO/FileSystem/DirectoryIterator.h"

/**
 * TODO:
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::IO::FileSystem {

    using Characters::String;

    /**
     *  This iterable returns the file names contained in the given argument directory.
     *  See @todo.
     *
     *  See @DirectoryIterator
     *
     *  \note   DirectoryIterable will NOT return the special values '.' and '..' which would be returned from readdir
     *
     *  \par Example Usage
     *      \code
     *          Iterable<String> textFilesInSlashTmp = DirectoryIterable ("/tmp").Where ([] (const String& i) { return i.EndsWith (".txt"); });
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     */
    class DirectoryIterable : public Traversal::Iterable<filesystem::path> {
    public:
        using IteratorReturnType = DirectoryIterator::IteratorReturnType;

    public:
        /**
         */
        DirectoryIterable (const filesystem::path& directory, IteratorReturnType iteratorReturns = IteratorReturnType::eDEFAULT);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DirectoryIterable.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_DirectoryIterable_h_*/
