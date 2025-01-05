/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

namespace Stroika::Foundation::IO::FileSystem {

    /*
     ********************************************************************************
     ************************ FileSystem::AppTmpFileManager *************************
     ********************************************************************************
     */
    inline filesystem::path AppTmpFileManager::GetRootTmpDir () const
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
