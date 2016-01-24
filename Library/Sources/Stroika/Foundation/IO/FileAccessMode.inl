/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
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
#if     qSUPPORT_LEGACY_Stroika_Enum_Names
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
#endif


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
#if     !qCompilerAndStdLib_constexpr_Buggy
namespace Stroika {
    namespace Foundation {
        namespace Configuration {
            template<>
            struct   DefaultNames<IO::FileAccessMode> : EnumNames<IO::FileAccessMode> {
                static  constexpr   EnumNames<IO::FileAccessMode>    k {
                    EnumNames<IO::FileAccessMode>::BasicArrayInitializer {
                        {
                            { IO::FileAccessMode::eNoAccess, L"No-Access" },
                            { IO::FileAccessMode::eRead, L"Read" },
                            { IO::FileAccessMode::eWrite, L"Write" },
                            { IO::FileAccessMode::eReadWrite, L"Read-Write" },
                        }
                    }
                };
                DefaultNames () : EnumNames<IO::FileAccessMode> (k) {}
            };
        }
    }
}
#endif
#endif  /*_Stroika_Foundation_IO_FileAccessMode_inl_*/
