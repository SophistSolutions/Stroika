/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_DirectoryIterator_h_
#define _Stroika_Foundation_IO_FileSystem_DirectoryIterator_h_  1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"
#include    "../../Traversal/Iterator.h"




/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *
 *      @todo   Add CTOR argument with RegExp() - to filter names. Do this instead
 *              of globbing, as its more standardized (the matching strings).
 *
 *      @todo   Add CTOR argument to return full paths verus filenames.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {


                using   Characters::String;


                /**
                 *  This iterator returns the file names contained in the given argument directory.
                 *  See @todo.
                 *
                 *  See @DirectoryIterable
                 *
                 *  \note   DirectoryIterable will NOT return the special values '.' and '..' which would be returned from readdir
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Must-Externally-Synchronize-Letter-Thread-Safety">Must-Externally-Synchronize-Letter-Thread-Safety</a>
                 */
                class   DirectoryIterator : public Traversal::Iterator<String> {
                public:
                    /**
                     */
                    DirectoryIterator (const String& directory);

                private:
                    class   Rep_;
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
#include    "DirectoryIterator.inl"

#endif  /*_Stroika_Foundation_IO_FileSystem_DirectoryIterator_h_*/
