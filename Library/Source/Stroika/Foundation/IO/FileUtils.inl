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

namespace	FileUtils {
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
	Stroika::Foundation::ModuleInit::ModuleInitializer<Stroika::Foundation::FileUtils::Private::UsingModuleHelper>	_FileUtils_MOULULE_INIT_;	// this object constructed for the CTOR/DTOR per-module side-effects
}


namespace	Stroika {	
	namespace	Foundation {

//	class	FileUtils::ThroughTmpFileWriter
inline	FileUtils::ThroughTmpFileWriter::operator tstring () const
		{
			Require (not fTmpFilePath.empty ());	// cannot access after Commit ()
			return fTmpFilePath;
		}


//	class	FileReader
	inline	const Byte*	FileUtils::FileReader::GetFileStart () const
		{
			return fFileDataStart;
		}
	inline	const Byte* FileUtils::FileReader::GetFileEnd () const
		{
			return fFileDataEnd;
		}
	inline	size_t	FileUtils::FileReader::GetFileSize () const
		{
			return fFileDataEnd - fFileDataStart;
		}



//	class	MemoryMappedFileReader
	inline	const Byte*	FileUtils::MemoryMappedFileReader::GetFileStart () const
		{
			return fFileDataStart;
		}
	inline	const Byte* FileUtils::MemoryMappedFileReader::GetFileEnd () const
		{
			return fFileDataEnd;
		}
	inline	size_t	FileUtils::MemoryMappedFileReader::GetFileSize () const
		{
			return fFileDataEnd - fFileDataStart;
		}



	//	class	FileUtils::AppTempFileManager
	inline	FileUtils::AppTempFileManager&	FileUtils::AppTempFileManager::Get ()
		{
			return _FileUtils_MOULULE_INIT_.Actual ().fAppTempFileManager;
		}
	inline	tstring	FileUtils::AppTempFileManager::GetMasterTempDir () const
		{
			return fTmpDir;
		}


	//	class	FileUtils::ScopedTmpDir {
	inline	FileUtils::ScopedTmpDir::operator tstring () const
		{
			return fTmpDir;
		}

	//	class	FileUtils::ScopedTmpFile {
	inline	FileUtils::ScopedTmpFile::operator tstring () const
		{
			return fTmpFile;
		}

	template	<>
		inline	__declspec(noreturn)	void	Exceptions::DoThrow (const FileUtils::FileAccessException& e2Throw)
			{
				DbgTrace (_T ("Throwing FileAccessException: fFileName = '%s'; FileAccessMode=%d"), e2Throw.fFileName.c_str (), e2Throw.fFileAccessMode);
				throw e2Throw;
			}
	}
}

#endif	/*_FileUtils_inl*/
