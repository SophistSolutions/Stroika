/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_Directory_h_
#define _Stroika_Foundation_IO_FileSystem_Directory_h_  1

#include    "../../StroikaPreComp.h"

#if         qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"
#include    "../../Debug/Assertions.h"
#include    "../../Execution/Exceptions.h"
#include    "../../Execution/Thread.h"
#include    "../../Time/DateTime.h"

#include    "../FileAccessMode.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *
 *      @todo   Probably get rid of most 'filesystem' access methods. Instead, make overloaded methods in the FILESYSTEM
 *              class so these directories can work with otehr filesystems.
 *
 *      (o)     Add access /creation time etc calls
 *      (o)     almost TOTALLY un-implemtend - miggrade code from FileUtils to here...
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {


                using   Characters::String;
                using   Characters::SDKString;
                using   Time::DateTime;


                /**
                 * A 'Directory' here refers to a "Directory Name". The methods of Directory operate on the actual filesystem (like Create), but
                 * this objects lifetime is not connected to the lifetime of any objects with similar names on the filesystem.
                 */
                class   Directory {
                public:
                    // If the argument name is not already '/'-terminated, this CTOR will adjust it
                    // CTOR does not create or check for the existence of the given directory (no IO)
                    Directory (const String& fileFullPath);

                public:
                    nonvirtual  void    Create () const;    // fails if exists

                public:
                    nonvirtual  void    AssureExists (bool createParentComponentsIfNeeded = true) const;    // fails if doesnt exist at the end, or is not a directory (unclear if its a directry but not accessible)?

                public:
                    // @todo cleanup / clarify docs (first is from old code second is my guess at new code at one point)
                    // returns true iff given path exists, is accessible, and is a directory
                    // returns true iff exsits AND is directory (what about slink?)
                    nonvirtual  bool    Exists () const;

                public:
                    /**
                     *   fails if exists after the operaotion - Fails if doest exist
                     */
                    nonvirtual  void    Delete () const;

                public:
                    nonvirtual  void    AssureDeleted (bool autoDeleteContentsAsNeeded = true) const;

                public:
                    // only works with type
                    //      wstring
                    //      String
                    template    <typename T>
                    nonvirtual  T As () const;

                public:
                    nonvirtual  SDKString AsSDKString () const;

                private:
                    String fFileFullPath_;
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
#include    "Directory.inl"

#endif  /*_Stroika_Foundation_IO_FileSystem_Directory_h_*/
