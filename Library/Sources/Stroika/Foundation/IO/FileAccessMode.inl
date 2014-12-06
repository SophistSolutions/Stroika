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
            namespace Private_ {
                struct  FileAccessModeData {
                    FileAccessModeData ();
                    Configuration::EnumNames<FileAccessMode>    fFileAccessModeConfigNames;
                };
            }
#endif
            constexpr   Configuration::EnumNames<FileAccessMode>    Stroika_Enum_Names(FileAccessMode)
            {
                Configuration::EnumNames<FileAccessMode>::BasicArrayInitializer {
                    {
                        { FileAccessMode::eNoAccess, L"No-Access" },
                        { FileAccessMode::eRead, L"Read" },
                        { FileAccessMode::eWrite, L"Write" },
                        { FileAccessMode::eReadWrite, L"Read-Write" },
                    }
                }
            };


        }
    }
}



#if     qCompilerAndStdLib_constexpr_Buggy
namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::IO::Private_::FileAccessModeData> _Stroika_Foundation_IO_FileAccessModeData_ModuleData_; // this object constructed for the CTOR/DTOR per-module side-effects
}
namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            inline  constexpr   Configuration::EnumNames<FileAccessMode>    Get_FileAccessMode_BWA ()
            {
                return Execution::ModuleInitializer<Private_::FileAccessModeData>::Actual ().fFileAccessModeConfigNames;
            }
        }
    }
}
#endif
#endif  /*_Stroika_Foundation_IO_FileAccessMode_inl_*/
