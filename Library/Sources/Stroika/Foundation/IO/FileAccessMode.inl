/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileAccessMode_inl_
#define _Stroika_Foundation_IO_FileAccessMode_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   IO {


            /*
             ********************************************************************************
             ***************************** IO::FileAccessMode *******************************
             ********************************************************************************
             */
            inline FileAccessMode operator& (FileAccessMode l, FileAccessMode r)
            {
                return FileAccessMode (static_cast<int> (l) & static_cast<int> (r));
            }
            inline FileAccessMode operator| (FileAccessMode l, FileAccessMode r)
            {
                return FileAccessMode (static_cast<int> (l) | static_cast<int> (r));
            }
#if     qCompilerAndStdLib_constexpr_Buggy
            // NB: we put this here instead of CPP file since sometimes accessed before main and to avoid moduleinit complexity (beacuse we soon will
            // support constexpr on MSVC)
            static  const
#else
            static  const
            // DAMN! NOT WORKING YET!!!
            //constexpr
#endif
            Configuration::EnumNames<FileAccessMode>   Stroika_Enum_Names(FileAccessMode)
            {
                { FileAccessMode::eNoAccess, L"No-Access" },
                { FileAccessMode::eRead, L"Read" },
                { FileAccessMode::eWrite, L"Write" },
                { FileAccessMode::eReadWrite, L"Read-Write" },
            };


        }
    }
}
#endif  /*_Stroika_Foundation_IO_FileAccessMode_inl_*/
