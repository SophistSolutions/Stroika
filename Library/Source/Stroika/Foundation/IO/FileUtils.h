/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__FileUtils_h__
#define	__FileUtils_h__	1

#include	"../StroikaPreComp.h"

#include	<fstream>
#include	<sstream>
#include	<set>
#include	<vector>

#include	<Windows.h>
#include	<tchar.h>

#include	"../Debug/Assertions.h"
#include	"../Time/DateUtils.h"
#include	"../Execution/Exceptions.h"
#include	"../Characters/StringUtils.h"
#include	"../Configuration/Basics.h"
#include	"../Execution/ThreadUtils.h"


namespace	Stroika {	
	namespace	Foundation {


namespace	FileUtils {

	typedef	__int64	FileOffset_t;


	// These GetSpecialDir_XXX routines always return a valid directory (if createIfNotPresent) - and
	// it always ends in a '\\'
	//
	tstring	GetSpecialDir_MyDocuments (bool createIfNotPresent = true);
	tstring	GetSpecialDir_AppData (bool createIfNotPresent = true);
	tstring	GetSpecialDir_WinSxS ();	// empty string if doesn't exist

	tstring	GetSpecialDir_GetTempDir ();



	// First draft of access-mode support. Much better stuff in PHRDB permissions security logic.
	// But this will do for now...
	//		-- LGP 2009-08-15
	enum FileAccessMode {
		eRead_FAM	= 0x1,
		eWrite_FAM	= 0x2,

		// composite values
		eReadWrite_FAM	= eRead_FAM | eWrite_FAM,
	};

	// This exception is thrown when a given file is opened, or creation attempted, etc. It is a failure due to
	// file (or directory) access permissions. It nearly always is the result of an operation (attempted and failed)
	// on a given file (which is usually given in the object). It also is the result of a perticular operation/access
	// failure (like read, write, or list).
	//
	// For now inherits from StringException so places that currently don't refer to this will still be caught in a 
	// list of exceptions. Probably should do separate handler so can customize messages...
	class	FileAccessException : public Exceptions::StringException {
		public:
			FileAccessException (const tstring& fileName = tstring (), FileAccessMode fileAccessMode = eReadWrite_FAM);

		public:
			tstring			fFileName;
			FileAccessMode	fFileAccessMode;
	};


	// Use can use this utility macro to 'fill in' the filename for a block of code that could throw a FileAccessException
	// on a given filename (but where that code may not know the filename)
	#define	FileAccessException_FILENAME_UPDATE_HELPER(_FILENAME_,_CODE_)\
		try {\
			_CODE_;\
		}\
		catch (const FileAccessException& prevFAE) {\
			if (prevFAE.fFileName.empty ()) {\
				Exceptions::DoThrow (FileAccessException (_FILENAME_, prevFAE.fFileAccessMode));\
			}\
			Exceptions::DoReThrow ();\
		}\
		


	// doesn't actually open the file. It's purely advisory. But its helpful to assure
	// a consistent set of error reporting across differnt styles of opens. Just call this first,
	// and it will throw exceptions if the file doesn't exist, or has access privileges issues.
	void	CheckFileAccess (const tstring& fileFullPath, bool checkCanRead = true, bool checkCanWrite = false);



	tstring	AssureDirectoryPathSlashTerminated (const tstring& dirPath);

	// map ALL characters in the string to something safe to use for a filename (that is - get rid of slashes etc - if present)
	tstring	SafeFilenameChars (const tstring& s);

	tstring	ResolveShortcut (const tstring& path2FileOrShortcut);

	wstring	FileSizeToDisplayString (FileOffset_t bytes);

	FileOffset_t	GetFileSize (const tstring& fileName);
	DateTime		GetFileLastModificationDate (const tstring& fileName);
	DateTime		GetFileLastAccessDate (const tstring& fileName);

	void	SetFileAccessWideOpened (const tstring& filePathName);

	void	CreateDirectory (const tstring& directoryPath, bool createParentComponentsIfNeeded = true);

	void	CreateDirectoryForFile (const tstring& filePath);

	tstring	GetVolumeName (const tstring& driveLetterAbsPath);

	tstring	AssureLongFileName (const tstring& fileName);		// if Win32 'short-file-name' - 8.3 - extend and return associated longfilename

	tstring	GetFileSuffix (const tstring& fileName);
	tstring	GetFileBaseName (const tstring& pathName);		// get the base name (strippping path and suffix)
	tstring	StripFileSuffix (const tstring& pathName);		// get the full path WITHOUT the file suffix at the end
	tstring	GetFileDirectory (const tstring& pathName);		// get the directory part of the given pathname (if the path refers to a directory - ends in / - then return THAT name)

	bool	FileExists (const TCHAR* filePath);
	bool	FileExists (const tstring& filePath);

	// returns true iff given path exists, is accessible, and is a directory
	bool	DirectoryExists (const TCHAR* filePath);
	bool	DirectoryExists (const tstring& filePath);

	vector<tstring>	FindFiles (const tstring& path, const tstring& fileNameToMatch = _T ("*.*"));
	vector<tstring>	FindFilesOneDirUnder (const tstring& path, const tstring& fileNameToMatch = _T ("*.*"));

	void	DeleteAllFilesInDirectory (const tstring& path, bool ignoreErrors = true);

	void	CopyFile (const tstring& srcFile, const tstring& destPath);

	void	WriteString (ostream& out, const wstring& s);
	wstring	ReadString (istream& in);

	vector<Byte>	ReadBytes (istream& in);
	void			WriteBytes (ostream& out, const vector<Byte>& s);



	class	DirectoryChangeWatcher {
		private:
			DirectoryChangeWatcher (const DirectoryChangeWatcher&);		// declare but not defined, to prevent copies
			void operator= (const DirectoryChangeWatcher&);				// ''
		public:
			DirectoryChangeWatcher (const tstring& directoryName, bool watchSubTree = false, DWORD notifyFilter = FILE_NOTIFY_CHANGE_LAST_WRITE);
			virtual ~DirectoryChangeWatcher ();

		protected:
			virtual	void	ValueChanged ();

		private:
			static	void	ThreadProc (void* lpParameter);

		private:
			tstring						fDirectory;
			bool						fWatchSubTree;
			ThreadUtils::SimpleThread	fThread;
			HANDLE						fDoneEvent;
			HANDLE						fWatchEvent;
			bool						fQuitting;
	};



	namespace	Private {
		class	UsingModuleHelper;
	}
	class	AppTempFileManager {
		private:
			AppTempFileManager ();
			~AppTempFileManager ();

		public:
			static	AppTempFileManager&	Get ();
		public:
			nonvirtual	tstring	GetMasterTempDir () const;

		public:
			nonvirtual	tstring	GetTempFile (const tstring& fileNameBase);
			nonvirtual	tstring	GetTempDir (const tstring& fileNameBase);

		private:
			tstring	fTmpDir;

		private:
			friend	class	Private::UsingModuleHelper;
	};



	class	TempFileLibrarian {
		public:
			TempFileLibrarian (const tstring& privateDirectory, bool purgeDirectory, bool makeTMPDIRRel = true, bool deleteFilesOnDescruction = true);
			~TempFileLibrarian ();

		public:
			nonvirtual	tstring	GetTempFile (const tstring& fileNameBase);
			nonvirtual	tstring	GetTempDir (const tstring& fileNameBase);

		private:
			set<tstring>					fFiles;
			tstring							fPrivateDirectory;
			bool							fMakeTMPDIRRel;
			bool							fDeleteFilesOnDescruction;
			ThreadUtils::CriticalSection	fCriticalSection;
	};


	class	ScopedTmpDir {
		private:
			ScopedTmpDir (const ScopedTmpDir&);
			const ScopedTmpDir& operator= (const ScopedTmpDir&);
		public:
			ScopedTmpDir (const tstring& fileNameBase);
			ScopedTmpDir (TempFileLibrarian& tfl, const tstring& fileNameBase);
			~ScopedTmpDir ();
		public:
			operator tstring () const;
		private:
			tstring		fTmpDir;
	};

	class	ScopedTmpFile {
		private:
			ScopedTmpFile (const ScopedTmpFile&);
			const ScopedTmpFile& operator= (const ScopedTmpFile&);
		public:
			ScopedTmpFile (const tstring& fileNameBase);
			ScopedTmpFile (TempFileLibrarian& tfl, const tstring& fileNameBase);
			~ScopedTmpFile ();
		public:
			operator tstring () const;
		private:
			tstring	fTmpFile;
	};




	/*
	 * Specify the name of a file to write, and an optional file suffix for a tempfile, and
	 * support writing through the tempfile, and then atomicly renaming the file when done.
	 *
	 * Even in case of failure, assure the tmpfile is removed.
	 *
	 * The point of this is to allow writing a file in such a way that the entire file write is
	 * atomic. We don't want to partially  update a file and upon failure, leave it corrupted.
	 *
	 * Using this class, you create a tempfile, write to it, and the Commit () the change. NOTE,
	 * it is REQUIRED you call Commit () after all writing to tmpfile is done (and closed),
	 * otehrwise the changes are abandoned.
	 */
	class	ThroughTmpFileWriter {
		public:
			ThroughTmpFileWriter (const tstring& realFileName, const tstring& tmpSuffix = _T (".tmp"));
			~ThroughTmpFileWriter ();

		private:	// NOT IMPLEMENTED
			ThroughTmpFileWriter (const ThroughTmpFileWriter&);
			const ThroughTmpFileWriter& operator= (const ThroughTmpFileWriter&);

		public:
			nonvirtual operator tstring () const;

		public:
			// tmpfile must have been closed the time we call Commit, and it atomicly renames the file
			// to the target name. This CAN fail (in which case cleanup is handled automatically)
			nonvirtual	void	Commit ();

		private:
			tstring	fRealFilePath;
			tstring	fTmpFilePath;
	};




	tstring	GetEXEDir ();
	tstring	GetEXEPath ();


	class	FileReader {
		public:
			FileReader (const TCHAR* fileName);
			~FileReader ();

		public:
			const Byte*	GetFileStart () const;
			const Byte* GetFileEnd () const;
			size_t		GetFileSize () const;

		private:
			const Byte*	fFileDataStart;
			const Byte*	fFileDataEnd;
	};
	class	FileWriter {
		public:
			FileWriter (const TCHAR* fileName);
			~FileWriter ();

		public:
			void	Append (const Byte* data, size_t count);

		private:
			int		fFD;
	};


	class	MemoryMappedFileReader {
		public:
			MemoryMappedFileReader (const TCHAR* fileName);
			~MemoryMappedFileReader ();

		public:
			const Byte*	GetFileStart () const;
			const Byte* GetFileEnd () const;
			size_t		GetFileSize () const;

		private:
			const Byte*	fFileDataStart;
			const Byte*	fFileDataEnd;
			HANDLE		fFileHandle;
			HANDLE		fFileMapping;
	};

	class	DirectoryContentsIterator {
		public:
			DirectoryContentsIterator (const tstring& pathExpr);		// can include wildcards - see ::FindFirstFile docs
			~DirectoryContentsIterator ();
		private:
			DirectoryContentsIterator (const DirectoryContentsIterator&);
			nonvirtual	void	operator= (const DirectoryContentsIterator&);

		public:
			nonvirtual	bool	NotAtEnd () const;
			nonvirtual	tstring	operator *() const;
			nonvirtual	void	operator++ ();

		private:
			HANDLE			fHandle;
			WIN32_FIND_DATA	fFindFileData;
			tstring			fDirectory;
	};


	struct	AdjustSysErrorMode {
		static	UINT	GetErrorMode ();
		AdjustSysErrorMode (UINT newErrorMode);
		~AdjustSysErrorMode ();
		UINT	fSavedErrorMode;
	};
}

	}
}



#endif	/*__FileUtils_h__*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"FileUtils.inl"
