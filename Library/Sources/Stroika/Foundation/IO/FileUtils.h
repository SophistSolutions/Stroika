/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_FileUtils_h_
#define	_Stroika_Foundation_IO_FileUtils_h_	1

#include	"../StroikaPreComp.h"

#include	<fstream>
#include	<sstream>
#include	<set>
#include	<vector>


#if			qPlatform_Windows
	#include	<Windows.h>
	#include	<tchar.h>
#endif

#include	"../Characters/StringUtils.h"
#include	"../Configuration/Common.h"
#include	"../Debug/Assertions.h"
#include	"../Execution/Exceptions.h"
#include	"../Execution/SimpleThread.h"
#include	"../Time/DateUtils.h"

#include	"FileAccessMode.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {

			using	Characters::TChar;
			using	Characters::TString;
			using	Time::DateTime;

			typedef	__int64	FileOffset_t;


			// These GetSpecialDir_XXX routines always return a valid directory (if createIfNotPresent) - and
			// it always ends in a '\\'
			//
			TString	GetSpecialDir_MyDocuments (bool createIfNotPresent = true);
			TString	GetSpecialDir_AppData (bool createIfNotPresent = true);
#if			qPlatform_Windows
			TString	GetSpecialDir_WinSxS ();	// empty string if doesn't exist
#endif

			TString	GetSpecialDir_GetTempDir ();





			// doesn't actually open the file. It's purely advisory. But its helpful to assure
			// a consistent set of error reporting across differnt styles of opens. Just call this first,
			// and it will throw exceptions if the file doesn't exist, or has access privileges issues.
			void	CheckFileAccess (const TString& fileFullPath, bool checkCanRead = true, bool checkCanWrite = false);



			TString	AssureDirectoryPathSlashTerminated (const TString& dirPath);

			// map ALL characters in the string to something safe to use for a filename (that is - get rid of slashes etc - if present)
			TString	SafeFilenameChars (const TString& s);

			TString	ResolveShortcut (const TString& path2FileOrShortcut);

			wstring	FileSizeToDisplayString (FileOffset_t bytes);

			FileOffset_t	GetFileSize (const TString& fileName);
			DateTime		GetFileLastModificationDate (const TString& fileName);
			DateTime		GetFileLastAccessDate (const TString& fileName);

			void	SetFileAccessWideOpened (const TString& filePathName);

			void	CreateDirectory (const TString& directoryPath, bool createParentComponentsIfNeeded = true);

			void	CreateDirectoryForFile (const TString& filePath);

			TString	GetVolumeName (const TString& driveLetterAbsPath);

			TString	AssureLongFileName (const TString& fileName);		// if Win32 'short-file-name' - 8.3 - extend and return associated longfilename

			TString	GetFileSuffix (const TString& fileName);
			TString	GetFileBaseName (const TString& pathName);		// get the base name (strippping path and suffix)
			TString	StripFileSuffix (const TString& pathName);		// get the full path WITHOUT the file suffix at the end
			TString	GetFileDirectory (const TString& pathName);		// get the directory part of the given pathname (if the path refers to a directory - ends in / - then return THAT name)

			bool	FileExists (const TChar* filePath);
			bool	FileExists (const TString& filePath);

			// returns true iff given path exists, is accessible, and is a directory
			bool	DirectoryExists (const TChar* filePath);
			bool	DirectoryExists (const TString& filePath);

			vector<TString>	FindFiles (const TString& path, const TString& fileNameToMatch = _T ("*.*"));
			vector<TString>	FindFilesOneDirUnder (const TString& path, const TString& fileNameToMatch = _T ("*.*"));

			void	DeleteAllFilesInDirectory (const TString& path, bool ignoreErrors = true);

			void	CopyFile (const TString& srcFile, const TString& destPath);

			void	WriteString (ostream& out, const wstring& s);
			wstring	ReadString (istream& in);

			vector<Byte>	ReadBytes (istream& in);
			void			WriteBytes (ostream& out, const vector<Byte>& s);



			class	DirectoryChangeWatcher {
				private:
					DirectoryChangeWatcher (const DirectoryChangeWatcher&);		// declare but not defined, to prevent copies
					void operator= (const DirectoryChangeWatcher&);				// ''
				public:
					DirectoryChangeWatcher (const TString& directoryName, bool watchSubTree = false, DWORD notifyFilter = FILE_NOTIFY_CHANGE_LAST_WRITE);
					virtual ~DirectoryChangeWatcher ();

				protected:
					virtual	void	ValueChanged ();

				private:
					static	void	ThreadProc (void* lpParameter);

				private:
					TString						fDirectory;
					bool						fWatchSubTree;
					Execution::SimpleThread	fThread;
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
					nonvirtual	TString	GetMasterTempDir () const;

				public:
					nonvirtual	TString	GetTempFile (const TString& fileNameBase);
					nonvirtual	TString	GetTempDir (const TString& fileNameBase);

				private:
					TString	fTmpDir;

				private:
					friend	class	Private::UsingModuleHelper;
			};



			class	TempFileLibrarian {
				public:
					TempFileLibrarian (const TString& privateDirectory, bool purgeDirectory, bool makeTMPDIRRel = true, bool deleteFilesOnDescruction = true);
					~TempFileLibrarian ();

				public:
					nonvirtual	TString	GetTempFile (const TString& fileNameBase);
					nonvirtual	TString	GetTempDir (const TString& fileNameBase);

				private:
					set<TString>					fFiles;
					TString							fPrivateDirectory;
					bool							fMakeTMPDIRRel;
					bool							fDeleteFilesOnDescruction;
					Execution::CriticalSection	fCriticalSection;
			};


			class	ScopedTmpDir {
				private:
					ScopedTmpDir (const ScopedTmpDir&);
					const ScopedTmpDir& operator= (const ScopedTmpDir&);
				public:
					ScopedTmpDir (const TString& fileNameBase);
					ScopedTmpDir (TempFileLibrarian& tfl, const TString& fileNameBase);
					~ScopedTmpDir ();
				public:
					operator TString () const;
				private:
					TString		fTmpDir;
			};

			class	ScopedTmpFile {
				private:
					ScopedTmpFile (const ScopedTmpFile&);
					const ScopedTmpFile& operator= (const ScopedTmpFile&);
				public:
					ScopedTmpFile (const TString& fileNameBase);
					ScopedTmpFile (TempFileLibrarian& tfl, const TString& fileNameBase);
					~ScopedTmpFile ();
				public:
					operator TString () const;
				private:
					TString	fTmpFile;
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
					ThroughTmpFileWriter (const TString& realFileName, const TString& tmpSuffix = _T (".tmp"));
					~ThroughTmpFileWriter ();

				private:	// NOT IMPLEMENTED
					ThroughTmpFileWriter (const ThroughTmpFileWriter&);
					const ThroughTmpFileWriter& operator= (const ThroughTmpFileWriter&);

				public:
					nonvirtual operator TString () const;

				public:
					// tmpfile must have been closed the time we call Commit, and it atomicly renames the file
					// to the target name. This CAN fail (in which case cleanup is handled automatically)
					nonvirtual	void	Commit ();

				private:
					TString	fRealFilePath;
					TString	fTmpFilePath;
			};



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
					DirectoryContentsIterator (const TString& pathExpr);		// can include wildcards - see ::FindFirstFile docs
					~DirectoryContentsIterator ();
				private:
					DirectoryContentsIterator (const DirectoryContentsIterator&);
					nonvirtual	void	operator= (const DirectoryContentsIterator&);

				public:
					nonvirtual	bool	NotAtEnd () const;
					nonvirtual	TString	operator *() const;
					nonvirtual	void	operator++ ();

				private:
					HANDLE			fHandle;
					WIN32_FIND_DATA	fFindFileData;
					TString			fDirectory;
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
#endif	/*_Stroika_Foundation_IO_FileUtils_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"FileUtils.inl"
