/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_Directory_h_
#define _Stroika_Foundation_IO_FileSystem_Directory_h_  1

#include    "../../StroikaPreComp.h"

#if         qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../../Characters/TChar.h"
#include    "../../Configuration/Common.h"
#include    "../../Debug/Assertions.h"
#include    "../../Execution/Exceptions.h"
#include    "../../Execution/Thread.h"
#include    "../../Time/DateTime.h"

#include    "../FileAccessMode.h"



/**
 * TODO:
 *
 *      (o)     Add access /creation time etc calls
 *      (o)     almost TOTALLY un-implemtend - miggrade code from FileUtils to here...
 */




namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {

                using   Characters::TChar;
                using   Characters::TString;
                using   Time::DateTime;




                /*
                 * A 'Directory' here refers to a "Directory Name". The methods of Directory operate on the actual filesystem (like Create), but
                 * this objects lifetime is not connected to the lifetime of any objects with similar names on the filesystem.
                 */
                class   Directory {
                public:

                    // If the argument name is not already '/'-terminated, this CTOR will adjust it
                    Directory (const TString& fileFullPath);

                    void    Create () const;    // fails if exists
                    void    AssureExists (bool createParentComponentsIfNeeded = true) const;    // fails if doesnt exist at the end, or is not a directory (unclear if its a directry but not accessible)?
                    bool    Exists () const;    // returns true iff exsits AND is directory (what about slink?)

                    void    Delete () const;    // fails if exists after teh operaotion - Fails if doest exist
                    void    AssureDeleted (bool autoDeleteContentsAsNeeded = true) const;



                    // only works (For now) with type wstring
                    template    <typename T>
                    wstring As () const;

                    TString AsTString () const;

                private:
                    TString fFileFullPath_;
                };

            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_FileSystem_Directory_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Directory.inl"
