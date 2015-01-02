/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_Common_h_
#define _Stroika_Foundation_IO_FileSystem_Common_h_  1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"
#include    "../../Time/DateTime.h"

#include    "../FileAccessMode.h"
#include    "Directory.h"



/**
 * TODO:
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {


                using   Characters::String;
                using   Memory::Byte;
                using   Time::DateTime;


                /**
                 *  @todo MAYBNE MAKE UNSINGED???
                 */
                using   FileOffset_t    =   int64_t;


            }
        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Common.inl"

#endif  /*_Stroika_Foundation_IO_FileSystem_Common_h_*/
