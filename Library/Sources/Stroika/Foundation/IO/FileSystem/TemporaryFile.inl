/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
