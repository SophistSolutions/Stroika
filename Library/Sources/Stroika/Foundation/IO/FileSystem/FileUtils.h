/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileUtils_h_
#define _Stroika_Foundation_IO_FileSystem_FileUtils_h_  1

#include    "../../StroikaPreComp.h"

#include    <set>
#include    <vector>

#if         qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../../Characters/TChar.h"
#include    "../../Configuration/Common.h"
#include    "../../Debug/Assertions.h"
#include    "../../Execution/Exceptions.h"
#include    "../../Execution/Thread.h"
#include    "../../Time/DateTime.h"

#include    "../FileAccessMode.h"



/**
 * TODO:
 *
 *          o   This file is obsolete, and will be gradually replaced - moving its contents out to other modules

 **         o   Great IDEA FROM KDJ. I USED TO support abstractfilesystem in stroika. Maybe in old code base. Used for Win32 FS versus UNIX versus MacOS FS.
 *              KDJ's point is this idea should be resurected cuz its useful for stuff like TARFILEs and ZIPFILES or ISO files which act like a FS, and can be treated
 *              that way.
 */




namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {

                using   Characters::TChar;
                using   Characters::TString;
                using   Time::DateTime;

                typedef int64_t FileOffset_t;



                // doesn't actually open the file. It's purely advisory. But its helpful to assure
                // a consistent set of error reporting across different styles of opens. Just call this first,
                // and it will throw exceptions if the file doesn't exist, or has access privileges issues.
                void    CheckFileAccess (const TString& fileFullPath, bool checkCanRead = true, bool checkCanWrite = false);


                TString ResolveShortcut (const TString& path2FileOrShortcut);

                wstring FileSizeToDisplayString (FileOffset_t bytes);

                FileOffset_t    GetFileSize (const TString& fileName);
                DateTime        GetFileLastModificationDate (const TString& fileName);
                DateTime        GetFileLastAccessDate (const TString& fileName);

                void    SetFileAccessWideOpened (const TString& filePathName);


                /*
                 * CreateDirectory makes sure the directory with the given name exists on the filesystem. If it already exists, this is NOT an error.
                 * If it already exists, but is a FILE (anything but a directory) - then it is an error).
                 *
                 * If createParentComponentsIfNeeded - the function braks the directory paoth into parts, and recursively applies itself to each segment from
                 * the root down.
                 *
                 * The net effect - this makes sure the given DIRECTORY exists or raises an exception.
                 *
                 *          TODO:
                 *              (o) We need an overload which takes the directory permissions as argument.
                 */
                void    CreateDirectory (const TString& directoryPath, bool createParentComponentsIfNeeded = true);




                void    CreateDirectoryForFile (const TString& filePath);

                TString GetVolumeName (const TString& driveLetterAbsPath);

                bool    FileExists (const TChar* filePath);
                bool    FileExists (const TString& filePath);

                // returns true iff given path exists, is accessible, and is a directory
                bool    DirectoryExists (const TChar* filePath);
                bool    DirectoryExists (const TString& filePath);

                vector<TString> FindFiles (const TString& path, const TString& fileNameToMatch = TSTR ("*.*"));
                vector<TString> FindFilesOneDirUnder (const TString& path, const TString& fileNameToMatch = TSTR ("*.*"));

                void    DeleteAllFilesInDirectory (const TString& path, bool ignoreErrors = true);

                void    CopyFile (const TString& srcFile, const TString& destPath);


                // COULD be made portable but alot of changes needed
#if         qPlatform_Windows
                class   DirectoryChangeWatcher {
                private:
                    DirectoryChangeWatcher (const DirectoryChangeWatcher&);     // declare but not defined, to prevent copies
                    void operator= (const DirectoryChangeWatcher&);             // ''
                public:
                    DirectoryChangeWatcher (const TString& directoryName, bool watchSubTree = false, DWORD notifyFilter = FILE_NOTIFY_CHANGE_LAST_WRITE);
                    virtual ~DirectoryChangeWatcher ();

                protected:
                    virtual void    ValueChanged ();

                private:
                    static  void    ThreadProc (void* lpParameter);

                private:
                    TString             fDirectory;
                    bool                fWatchSubTree;
                    Execution::Thread   fThread;
                    HANDLE              fDoneEvent;
                    HANDLE              fWatchEvent;
                    bool                fQuitting;
                };
#endif



                namespace   Private {
                    class   UsingModuleHelper;
                }
                class   AppTempFileManager {
                private:
                    AppTempFileManager ();
                    ~AppTempFileManager ();

                public:
                    static  AppTempFileManager& Get ();
                public:
                    nonvirtual  TString GetMasterTempDir () const;

                public:
                    nonvirtual  TString GetTempFile (const TString& fileNameBase);
                    nonvirtual  TString GetTempDir (const TString& fileNameBase);

                private:
                    TString fTmpDir;

                private:
                    friend  class   Private::UsingModuleHelper;
                };



                class   TempFileLibrarian {
                public:
                    TempFileLibrarian (const TString& privateDirectory, bool purgeDirectory, bool makeTMPDIRRel = true, bool deleteFilesOnDescruction = true);
                    ~TempFileLibrarian ();

                public:
                    nonvirtual  TString GetTempFile (const TString& fileNameBase);
                    nonvirtual  TString GetTempDir (const TString& fileNameBase);

                private:
                    set<TString>                fFiles;
                    TString                     fPrivateDirectory;
                    bool                        fMakeTMPDIRRel;
                    bool                        fDeleteFilesOnDescruction;
                    Execution::CriticalSection  fCriticalSection;
                };


                class   ScopedTmpDir {
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
                    TString     fTmpDir;
                };

                class   ScopedTmpFile {
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
                    TString fTmpFile;
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
                class   ThroughTmpFileWriter {
                public:
                    ThroughTmpFileWriter (const TString& realFileName, const TString& tmpSuffix = TSTR (".tmp"));
                    ~ThroughTmpFileWriter ();

                private:    // NOT IMPLEMENTED
                    ThroughTmpFileWriter (const ThroughTmpFileWriter&);
                    const ThroughTmpFileWriter& operator= (const ThroughTmpFileWriter&);

                public:
                    nonvirtual operator TString () const;

                public:
                    // tmpfile must have been closed the time we call Commit, and it atomicly renames the file
                    // to the target name. This CAN fail (in which case cleanup is handled automatically)
                    nonvirtual  void    Commit ();

                private:
                    TString fRealFilePath;
                    TString fTmpFilePath;
                };



                /**
                  * @todo   This FileReader utility needs redesign, and probably should be deprecated.
                  *         It's implementaiton fails when built for 64 bit mode (we just hide the errors) and you read
                  *         using a 32-bit API, and fails to possibly work by API design reading a > 4GB file on a
                  *         32-bit system.
                  */
                class   FileReader {
                public:
                    FileReader (const TChar* fileName);
                    ~FileReader ();

                public:
                    const Byte* GetFileStart () const;
                    const Byte* GetFileEnd () const;
                    size_t      GetFileSize () const;

                private:
                    const Byte* fFileDataStart;
                    const Byte* fFileDataEnd;
                };
                class   FileWriter {
                public:
                    FileWriter (const TChar* fileName);
                    ~FileWriter ();

                public:
                    void    Append (const Byte* data, size_t count);

                private:
                    int     fFD;
                };


                class   MemoryMappedFileReader {
                public:
                    MemoryMappedFileReader (const TChar* fileName);
                    ~MemoryMappedFileReader ();

                public:
                    const Byte* GetFileStart () const;
                    const Byte* GetFileEnd () const;
                    size_t      GetFileSize () const;

                private:
                    const Byte* fFileDataStart;
                    const Byte* fFileDataEnd;
#if         qPlatform_Windows
                    HANDLE      fFileHandle;
                    HANDLE      fFileMapping;
#endif
                };

                class   DirectoryContentsIterator {
                public:
                    DirectoryContentsIterator (const TString& pathExpr);        // can include wildcards - see ::FindFirstFile docs
                    ~DirectoryContentsIterator ();
                private:
                    DirectoryContentsIterator (const DirectoryContentsIterator&);
                    nonvirtual  void    operator= (const DirectoryContentsIterator&);

                public:
                    nonvirtual  bool    NotAtEnd () const;
                    nonvirtual  TString operator *() const;
                    nonvirtual  void    operator++ ();

#if         qPlatform_Windows
                private:
                    HANDLE          fHandle;
                    WIN32_FIND_DATA fFindFileData;
                    TString         fDirectory;
#endif
                };


                // Should be in a PLATFORM_WINDOWS subfile or sub-namespace... And DOCUMENT!!!!
#if         qPlatform_Windows
                struct  AdjustSysErrorMode {
                    static  UINT    GetErrorMode ();
                    AdjustSysErrorMode (UINT newErrorMode);
                    ~AdjustSysErrorMode ();
                    UINT    fSavedErrorMode;
                };
#endif
            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_FileSystem_FileUtils_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "FileUtils.inl"
