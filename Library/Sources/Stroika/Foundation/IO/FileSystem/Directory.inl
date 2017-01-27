/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_Directory_inl_
#define _Stroika_Foundation_IO_FileSystem_Directory_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {


                template<>
                inline  String Directory::As () const
                {
                    return fFileFullPath_;
                }
                template<>
                inline  wstring Directory::As () const
                {
                    return fFileFullPath_.As<wstring> ();
                }
                inline  SDKString Directory::AsSDKString () const
                {
                    return fFileFullPath_.AsSDKString ();
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_FileSystem_Directory_inl_*/
