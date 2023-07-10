/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_ThroughTmpFileWriter_inl_
#define _Stroika_Foundation_IO_FileSystem_ThroughTmpFileWriter_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
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

#endif /*_Stroika_Foundation_IO_FileSystem_ThroughTmpFileWriter_inl_*/
