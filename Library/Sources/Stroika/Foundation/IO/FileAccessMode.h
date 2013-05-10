/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
            enum class FileAccessMode : uint8_t {
                eRead_FAM   = 0x1,
                eWrite_FAM  = 0x2,

                // composite values
                eReadWrite_FAM  = eRead_FAM | eWrite_FAM,
            };
            FileAccessMode operator& (FileAccessMode l, FileAccessMode r);
            FileAccessMode operator| (FileAccessMode l, FileAccessMode r);




            /**
             * @todo add .inl file
             */
            inline FileAccessMode operator& (FileAccessMode l, FileAccessMode r)
            {
                return FileAccessMode (static_cast<int> (l) & static_cast<int> (r));
            }
            inline FileAccessMode operator| (FileAccessMode l, FileAccessMode r)
            {
                return FileAccessMode (static_cast<int> (l) | static_cast<int> (r));
            }
        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_IO_FileAccessMode_h_*/
