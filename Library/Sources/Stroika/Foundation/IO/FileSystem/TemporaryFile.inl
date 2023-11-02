/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
    inline filesystem::path AppTempFileManager::GetRootTempDir () const
    {
        return fTmpDir_;
    }

    /*
     ********************************************************************************
     ***************************** FileSystem::ScopedTmpDir *************************
     ********************************************************************************
     */
    inline ScopedTmpDir::operator filesystem::path () const
    {
        return fTmpDir_;
    }

    /*
     ********************************************************************************
     **************************** FileSystem::ScopedTmpFile *************************
     ********************************************************************************
     */
    inline ScopedTmpFile::operator filesystem::path () const
    {
        return fTmpFile_;
    }

}

#endif /*_Stroika_Foundation_IO_FileSystem_TemporaryFile_inl_*/
