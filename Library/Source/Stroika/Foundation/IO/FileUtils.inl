/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_FileUtils_inl
#define	_FileUtils_inl	1


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

		//	class	IO::ThroughTmpFileWriter
		inline	IO::ThroughTmpFileWriter::operator tstring () const
				{
					Require (not fTmpFilePath.empty ());	// cannot access after Commit ()
					return fTmpFilePath;
				}


		//	class	FileReader
			inline	const Byte*	IO::FileReader::GetFileStart () const
				{
					return fFileDataStart;
				}
			inline	const Byte* IO::FileReader::GetFileEnd () const
				{
					return fFileDataEnd;
				}
			inline	size_t	IO::FileReader::GetFileSize () const
				{
					return fFileDataEnd - fFileDataStart;
				}



		//	class	MemoryMappedFileReader
			inline	const Byte*	IO::MemoryMappedFileReader::GetFileStart () const
				{
					return fFileDataStart;
				}
			inline	const Byte* IO::MemoryMappedFileReader::GetFileEnd () const
				{
					return fFileDataEnd;
				}
			inline	size_t	IO::MemoryMappedFileReader::GetFileSize () const
				{
					return fFileDataEnd - fFileDataStart;
				}



			//	class	IO::AppTempFileManager
			inline	IO::AppTempFileManager&	IO::AppTempFileManager::Get ()
				{
					return _FileUtils_MOULULE_INIT_.Actual ().fAppTempFileManager;
				}
			inline	tstring	IO::AppTempFileManager::GetMasterTempDir () const
				{
					return fTmpDir;
				}


			//	class	IO::ScopedTmpDir {
			inline	IO::ScopedTmpDir::operator tstring () const
				{
					return fTmpDir;
				}

			//	class	IO::ScopedTmpFile {
			inline	IO::ScopedTmpFile::operator tstring () const
				{
					return fTmpFile;
				}

			template	<>
				inline	__declspec(noreturn)	void	Execution::DoThrow (const IO::FileAccessException& e2Throw)
					{
						DbgTrace (_T ("Throwing FileAccessException: fFileName = '%s'; FileAccessMode=%d"), e2Throw.fFileName.c_str (), e2Throw.fFileAccessMode);
						throw e2Throw;
					}
	}
}

#endif	/*_FileUtils_inl*/
