/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileAccessMode_h_
#define _Stroika_Foundation_IO_FileAccessMode_h_    1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Enumeration.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   IO {


            /**
             * First draft of access-mode support. Much better stuff in PHRDB permissions security logic.
             * But this will do for now...
             *      -- LGP 2009-08-15
             *
             *  \note   Configuration::DefaultNames<> supported
             */
            enum    class   FileAccessMode : uint8_t {
                eNoAccess,
                eRead   = 0x1,
                eWrite  = 0x2,

                // composite values
                eReadWrite  = eRead | eWrite,

                Stroika_Define_Enum_Bounds(eNoAccess, eReadWrite)
            };

#if     qSUPPORT_LEGACY_Stroika_Enum_Names
#if 0
#if     !qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy
            const Configuration::EnumNames<FileAccessMode>   Stroika_Enum_Names(FileAccessMode);
#endif
#endif
#endif

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

#include    "FileAccessMode.inl"

#endif  /*_Stroika_Foundation_IO_FileAccessMode_h_*/
