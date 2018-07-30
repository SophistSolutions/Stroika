/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_TemporaryFile_inl_
#define _Stroika_Foundation_IO_FileSystem_TemporaryFile_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::FileSystem::Private {
    class FileUtilsModuleData_ {
    public:
        FileUtilsModuleData_ ();
        ~FileUtilsModuleData_ ();

    private:
        AppTempFileManager fAppTempFileManager;

    private:
        friend class IO::FileSystem::AppTempFileManager; // so AppTempFileManager::Get () can reference fAppTempFileManager
    };
}
namespace {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::IO::FileSystem::Private::FileUtilsModuleData_> _Stroika_Foundation_IO_FileSystem_TemporaryFile_ModuleInit_; // this object constructed for the CTOR/DTOR per-module side-effects
}

namespace Stroika::Foundation::IO::FileSystem {

    /*
     ********************************************************************************
     *********************** FileSystem::AppTempFileManager *************************
     ********************************************************************************
     */
    inline AppTempFileManager& AppTempFileManager::Get ()
    {
        return Execution::ModuleInitializer<Private::FileUtilsModuleData_>::Actual ().fAppTempFileManager;
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
