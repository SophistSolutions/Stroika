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
                    class   UsingModuleHelper {
                    public:
                        UsingModuleHelper ();
                        ~UsingModuleHelper ();

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
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::IO::FileSystem::Private::UsingModuleHelper>  _FileUtils_MOULULE_INIT_;   // this object constructed for the CTOR/DTOR per-module side-effects
}


namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {

                //  class   IO::ThroughTmpFileWriter
                inline  ThroughTmpFileWriter::operator TString () const
                {
                    Require (not fTmpFilePath.empty ());    // cannot access after Commit ()
                    return fTmpFilePath;
                }


                //  class   FileReader
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



                //  class   MemoryMappedFileReader
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



                //  class   AppTempFileManager
                inline  AppTempFileManager& AppTempFileManager::Get ()
                {
                    return _FileUtils_MOULULE_INIT_.Actual ().fAppTempFileManager;
                }
                inline  TString AppTempFileManager::GetMasterTempDir () const
                {
                    return fTmpDir;
                }


                //  class   ScopedTmpDir {
                inline  ScopedTmpDir::operator TString () const
                {
                    return fTmpDir;
                }

                //  class   ScopedTmpFile {
                inline  ScopedTmpFile::operator TString () const
                {
                    return fTmpFile;
                }

            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_FileSystem_FileUtils_inl_*/
