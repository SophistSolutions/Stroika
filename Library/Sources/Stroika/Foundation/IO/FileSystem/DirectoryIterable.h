/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_DirectoryIterable_h_
#define _Stroika_Foundation_IO_FileSystem_DirectoryIterable_h_  1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"
#include    "../../Traversal/Iterable.h"




/**
 * TODO:
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *      @todo   Add CTOR argument with RegExp() - to filter names. Do this instead
 *              of globbing, as its more standardized (the matching strings).
 *
 *      @todo   Add CTOR argument to retunr full paths versus filenames.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {


                using   Characters::String;


                /**
                 *  This iterable returns the file names contained in the given argument directory.
                 *  See @todo.
                 *
                 *  See @DirectoryIterator
                 *
                 *  Does return the special value ..
                 */
                class   DirectoryIterable : public Traversal::Iterable<String> {
                public:
                    /**
                     */
                    DirectoryIterable (const String& directory);
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
#include    "DirectoryIterable.inl"

#endif  /*_Stroika_Foundation_IO_FileSystem_DirectoryIterable_h_*/
