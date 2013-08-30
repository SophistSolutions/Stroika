/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileUtils_inl_
#define _Stroika_Foundation_IO_FileSystem_FileUtils_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {
                namespace   Private {
                    class   FileUtilsModuleData_ {
                    public:
                        FileUtilsModuleData_ ();
                        ~FileUtilsModuleData_ ();

                    private:
                        AppTempFileManager  fAppTempFileManager;

                    private:
                        friend  class   FileSystem::AppTempFileManager;     // so AppTempFileManager::Get () can reference fAppTempFileManager
                    };
                }
            }
        }
    }
}
namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::IO::FileSystem::Private::FileUtilsModuleData_>  _Stroika_Foundation_IO_FileSystem_FileUtils_ModuleInit_;   // this object constructed for the CTOR/DTOR per-module side-effects
}


namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {


                /*
                 ********************************************************************************
                 ********************* FileSystem::ThroughTmpFileWriter *************************
                 ********************************************************************************
                 */
                inline  ThroughTmpFileWriter::operator String () const
                {
                    Require (not fTmpFilePath.empty ());    // cannot access after Commit ()
                    return fTmpFilePath;
                }


                /*
                 ********************************************************************************
                 ******************************* FileSystem::FileReader *************************
                 ********************************************************************************
                 */
                inline  const Byte* FileReader::GetFileStart () const
                {
                    return fFileDataStart;
                }
                inline  const Byte* FileReader::GetFileEnd () const
                {
                    return fFileDataEnd;
                }
                inline  size_t  FileReader::GetFileSize () const
                {
                    return fFileDataEnd - fFileDataStart;
                }



                /*
                 ********************************************************************************
                 ******************* FileSystem::MemoryMappedFileReader *************************
                 ********************************************************************************
                 */
                inline  const Byte* MemoryMappedFileReader::GetFileStart () const
                {
                    return fFileDataStart;
                }
                inline  const Byte* MemoryMappedFileReader::GetFileEnd () const
                {
                    return fFileDataEnd;
                }
                inline  size_t  MemoryMappedFileReader::GetFileSize () const
                {
                    return fFileDataEnd - fFileDataStart;
                }


                /*
                 ********************************************************************************
                 *********************** FileSystem::AppTempFileManager *************************
                 ********************************************************************************
                 */
                inline  AppTempFileManager& AppTempFileManager::Get ()
                {
                    return Execution::ModuleInitializer<Private::FileUtilsModuleData_>::Actual ().fAppTempFileManager;
                }
                inline  String AppTempFileManager::GetMasterTempDir () const
                {
                    return fTmpDir;
                }


                /*
                 ********************************************************************************
                 ***************************** FileSystem::ScopedTmpDir *************************
                 ********************************************************************************
                 */
                inline  ScopedTmpDir::operator Directory () const
                {
                    return fTmpDir;
                }
                inline  Directory ScopedTmpDir::GetDirectory () const
                {
                    return fTmpDir;
                }


                /*
                 ********************************************************************************
                 **************************** FileSystem::ScopedTmpFile *************************
                 ********************************************************************************
                 */
                inline  ScopedTmpFile::operator String () const
                {
                    return fTmpFile;
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_FileSystem_FileUtils_inl_*/
