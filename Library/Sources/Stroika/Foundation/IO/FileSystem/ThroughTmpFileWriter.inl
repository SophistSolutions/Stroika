/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::IO::FileSystem {

    /*
     ********************************************************************************
     ********************* FileSystem::ThroughTmpFileWriter *************************
     ********************************************************************************
     */
    inline filesystem::path ThroughTmpFileWriter::GetTmpFilePath () const
    {
        Require (not fTmpFilePath_.empty ()); // cannot access after Commit ()
        return fTmpFilePath_;
    }
    inline filesystem::path ThroughTmpFileWriter::GetRealFilePath () const
    {
        Require (not fRealFilePath_.empty ());
        return fRealFilePath_;
    }
    /**
     *  Before commit this returns the tmpfile name. After commit returns the eventual file name.
     */
    inline filesystem::path ThroughTmpFileWriter::GetFilePath () const
    {
        Require (not fRealFilePath_.empty ());
        if (not fTmpFilePath_.empty ()) {
            return fTmpFilePath_;
        }
        return fRealFilePath_;
    }

}
