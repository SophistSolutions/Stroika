/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_TemporaryFile_inl_
#define _Stroika_Foundation_IO_FileSystem_TemporaryFile_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::IO::FileSystem {

    /*
     ********************************************************************************
     *********************** FileSystem::AppTempFileManager *************************
     ********************************************************************************
     */
    inline AppTempFileManager& AppTempFileManager::Get ()
    {
        static AppTempFileManager sAppTempFileManager_;
        return sAppTempFileManager_;
    }
    inline String AppTempFileManager::GetMasterTempDir () const
    {
        return fTmpDir;
    }

    /*
     ********************************************************************************
     ***************************** FileSystem::ScopedTmpDir *************************
     ********************************************************************************
     */
    inline ScopedTmpDir::operator Directory () const
    {
        return fTmpDir;
    }
    inline Directory ScopedTmpDir::GetDirectory () const
    {
        return fTmpDir;
    }

    /*
     ********************************************************************************
     **************************** FileSystem::ScopedTmpFile *************************
     ********************************************************************************
     */
    inline ScopedTmpFile::operator String () const
    {
        return fTmpFile;
    }

}

#endif /*_Stroika_Foundation_IO_FileSystem_TemporaryFile_inl_*/
