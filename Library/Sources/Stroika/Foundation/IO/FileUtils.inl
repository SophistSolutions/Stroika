/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_FileUtils_inl_
#define	_Stroika_Foundation_IO_FileUtils_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {
			namespace	Private {
				class	UsingModuleHelper {
					public:
						UsingModuleHelper ();
						~UsingModuleHelper ();
				
					private:
						AppTempFileManager	fAppTempFileManager;

					private:
						friend	AppTempFileManager;
				};
			}
		}
	}
}
namespace	{
	Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::IO::Private::UsingModuleHelper>	_FileUtils_MOULULE_INIT_;	// this object constructed for the CTOR/DTOR per-module side-effects
}


namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {

		//	class	IO::ThroughTmpFileWriter
			inline	ThroughTmpFileWriter::operator tstring () const
				{
					Require (not fTmpFilePath.empty ());	// cannot access after Commit ()
					return fTmpFilePath;
				}


		//	class	FileReader
			inline	const Byte*	FileReader::GetFileStart () const
				{
					return fFileDataStart;
				}
			inline	const Byte* FileReader::GetFileEnd () const
				{
					return fFileDataEnd;
				}
			inline	size_t	FileReader::GetFileSize () const
				{
					return fFileDataEnd - fFileDataStart;
				}



		//	class	MemoryMappedFileReader
			inline	const Byte*	MemoryMappedFileReader::GetFileStart () const
				{
					return fFileDataStart;
				}
			inline	const Byte* MemoryMappedFileReader::GetFileEnd () const
				{
					return fFileDataEnd;
				}
			inline	size_t	MemoryMappedFileReader::GetFileSize () const
				{
					return fFileDataEnd - fFileDataStart;
				}



			//	class	AppTempFileManager
			inline	AppTempFileManager&	AppTempFileManager::Get ()
				{
					return _FileUtils_MOULULE_INIT_.Actual ().fAppTempFileManager;
				}
			inline	tstring	AppTempFileManager::GetMasterTempDir () const
				{
					return fTmpDir;
				}


			//	class	ScopedTmpDir {
			inline	ScopedTmpDir::operator tstring () const
				{
					return fTmpDir;
				}

			//	class	ScopedTmpFile {
			inline	ScopedTmpFile::operator tstring () const
				{
					return fTmpFile;
				}
		}



		template	<>
			inline	__declspec(noreturn)	void	Execution::DoThrow (const IO::FileAccessException& e2Throw)
				{
					DbgTrace (_T ("Throwing FileAccessException: fFileName = '%s'; FileAccessMode=%d"), e2Throw.fFileName.c_str (), e2Throw.fFileAccessMode);
					throw e2Throw;
				}
	}
}
#endif	/*_Stroika_Foundation_IO_FileUtils_inl_*/
