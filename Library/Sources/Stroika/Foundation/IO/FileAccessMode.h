/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileAccessMode_h_
#define _Stroika_Foundation_IO_FileAccessMode_h_    1

#include    "../StroikaPreComp.h"

namespace   Stroika {
    namespace   Foundation {

        namespace   IO {

            // First draft of access-mode support. Much better stuff in PHRDB permissions security logic.
            // But this will do for now...
            //      -- LGP 2009-08-15
            enum FileAccessMode {
                eRead_FAM   = 0x1,
                eWrite_FAM  = 0x2,

                // composite values
                eReadWrite_FAM  = eRead_FAM | eWrite_FAM,
            };

        }
    }
}
#endif  /*_Stroika_Foundation_IO_FileAccessMode_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
