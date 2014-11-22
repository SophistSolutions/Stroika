/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileAccessMode_h_
#define _Stroika_Foundation_IO_FileAccessMode_h_    1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Enumeration.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   IO {


            // First draft of access-mode support. Much better stuff in PHRDB permissions security logic.
            // But this will do for now...
            //      -- LGP 2009-08-15
            enum    class FileAccessMode : uint8_t {
                eNoAccess,
                eRead   = 0x1,
                eWrite  = 0x2,

                // composite values
                eReadWrite  = eRead | eWrite,

                Stroika_Define_Enum_Bounds(eNoAccess, eReadWrite)
            };

            // TEMPORARILY STORED IN FileAccessException.cpp - because we hope to soon have
            // constexr for EnumNames<>
            // search IO::Stroika_Enum_Names(FileAccessMode)
            extern  const Configuration::EnumNames<FileAccessMode>   Stroika_Enum_Names(FileAccessMode);

            FileAccessMode  operator& (FileAccessMode l, FileAccessMode r);
            FileAccessMode  operator| (FileAccessMode l, FileAccessMode r);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

/**
* @todo add .inl file
*/
namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
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



#endif  /*_Stroika_Foundation_IO_FileAccessMode_h_*/
