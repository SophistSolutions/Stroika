/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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


                using   Characters::String;
                using   Memory::Byte;
                using   Time::DateTime;

                typedef int64_t FileOffset_t;


                // doesn't actually open the file. It's purely advisory. But its helpful to assure
                // a consistent set of error reporting across different styles of opens. Just call this first,
                // and it will throw exceptions if the file doesn't exist, or has access privileges issues.
                void    CheckFileAccess (const String& fileFullPath, bool checkCanRead = true, bool checkCanWrite = false);


                String ResolveShortcut (const String& path2FileOrShortcut);

                String FileSizeToDisplayString (FileOffset_t bytes);

                FileOffset_t    GetFileSize (const String& fileName);
                DateTime        GetFileLastModificationDate (const String& fileName);
                DateTime        GetFileLastAccessDate (const String& fileName);

                void    SetFileAccessWideOpened (const String& filePathName);


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
                void    CreateDirectory (const String& directoryPath, bool createParentComponentsIfNeeded = true);


                void    CreateDirectoryForFile (const String& filePath);


                String GetVolumeName (const String& driveLetterAbsPath);


                bool    FileExists (const String& filePath);


                // returns true iff given path exists, is accessible, and is a directory
                bool    DirectoryExists (const String& filePath);

                vector<String> FindFiles (const String& path, const String& fileNameToMatch = L"*.*");


                vector<String> FindFilesOneDirUnder (const String& path, const String& fileNameToMatch = L"*.*");


                void    DeleteAllFilesInDirectory (const String& path, bool ignoreErrors = true);


                void    CopyFile (const String& srcFile, const String& destPath);


                // COULD be made portable but alot of changes needed
#if         qPlatform_Windows
                class   DirectoryChangeWatcher {
                private:
                    DirectoryChangeWatcher (const DirectoryChangeWatcher&);     // declare but not defined, to prevent copies
                    void operator= (const DirectoryChangeWatcher&);             // ''
                public:
                    DirectoryChangeWatcher (const String& directoryName, bool watchSubTree = false, DWORD notifyFilter = FILE_NOTIFY_CHANGE_LAST_WRITE);
                    virtual ~DirectoryChangeWatcher ();

                protected:
                    virtual void    ValueChanged ();

                private:
                    static  void    ThreadProc (void* lpParameter);

                private:
                    String              fDirectory;
                    bool                fWatchSubTree;
                    Execution::Thread   fThread;
                    HANDLE              fDoneEvent;
                    HANDLE              fWatchEvent;
                    bool                fQuitting;
                };
#endif


                namespace   Private {
                    class   FileUtilsModuleData_;
                }
                class   AppTempFileManager {
                private:
                    AppTempFileManager ();
                    ~AppTempFileManager ();

                public:
                    static  AppTempFileManager& Get ();
                public:
                    nonvirtual  String GetMasterTempDir () const;

                public:
                    nonvirtual  String GetTempFile (const String& fileNameBase);
                    nonvirtual  String GetTempDir (const String& fileNameBase);

                private:
                    String fTmpDir;

                private:
                    friend  class   Private::FileUtilsModuleData_;
                };


                class   TempFileLibrarian {
                public:
                    TempFileLibrarian (const String& privateDirectory, bool purgeDirectory, bool makeTMPDIRRel = true, bool deleteFilesOnDescruction = true);
                    ~TempFileLibrarian ();

                public:
                    nonvirtual  String GetTempFile (const String& fileNameBase);
                    nonvirtual  String GetTempDir (const String& fileNameBase);

                private:
                    set<String>                 fFiles;
                    String                      fPrivateDirectory;
                    bool                        fMakeTMPDIRRel;
                    bool                        fDeleteFilesOnDescruction;
                    mutex                       fCriticalSection_;
                };


                class   ScopedTmpDir {
                private:
                    ScopedTmpDir (const ScopedTmpDir&);
                    const ScopedTmpDir& operator= (const ScopedTmpDir&);
                public:
                    ScopedTmpDir (const String& fileNameBase);
                    ScopedTmpDir (TempFileLibrarian& tfl, const String& fileNameBase);
                    ~ScopedTmpDir ();
                public:
                    operator String () const;
                private:
                    String     fTmpDir;
                };

                class   ScopedTmpFile {
                private:
                    ScopedTmpFile (const ScopedTmpFile&);
                    const ScopedTmpFile& operator= (const ScopedTmpFile&);
                public:
                    ScopedTmpFile (const String& fileNameBase);
                    ScopedTmpFile (TempFileLibrarian& tfl, const String& fileNameBase);
                    ~ScopedTmpFile ();
                public:
                    operator String () const;
                private:
                    String fTmpFile;
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
                    ThroughTmpFileWriter (const String& realFileName, const String& tmpSuffix = L".tmp");
                    ~ThroughTmpFileWriter ();

                private:
                    NO_COPY_CONSTRUCTOR (ThroughTmpFileWriter);
                    NO_ASSIGNMENT_OPERATOR (ThroughTmpFileWriter);

                public:
                    nonvirtual operator String () const;

                public:
                    // tmpfile must have been closed the time we call Commit, and it atomicly renames the file
                    // to the target name. This CAN fail (in which case cleanup is handled automatically)
                    nonvirtual  void    Commit ();

                private:
                    String fRealFilePath;
                    String fTmpFilePath;
                };


                /**
                  * @todo   This FileReader utility needs redesign, and probably should be deprecated.
                  *         It's implementation fails when built for 64 bit mode (we just hide the errors) and you read
                  *         using a 32-bit API, and fails to possibly work by API design reading a > 4GB file on a
                  *         32-bit system.
                  */
                class   FileReader {
                public:
                    FileReader (const String& fileName);
                    ~FileReader ();

                private:
                    NO_COPY_CONSTRUCTOR (FileReader);
                    NO_ASSIGNMENT_OPERATOR (FileReader);

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
                    FileWriter (const String& fileName);
                    ~FileWriter ();

                public:
                    void    Append (const Byte* data, size_t count);

                private:
                    int     fFD;
                };


                class   MemoryMappedFileReader {
                public:
                    MemoryMappedFileReader (const String& fileName);
                    ~MemoryMappedFileReader ();

                private:
                    NO_COPY_CONSTRUCTOR (MemoryMappedFileReader);
                    NO_ASSIGNMENT_OPERATOR (MemoryMappedFileReader);

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
                    DirectoryContentsIterator (const String& pathExpr);        // can include wildcards - see ::FindFirstFile docs
                    ~DirectoryContentsIterator ();

                private:
                    NO_COPY_CONSTRUCTOR (DirectoryContentsIterator);
                    NO_ASSIGNMENT_OPERATOR (DirectoryContentsIterator);

                public:
                    nonvirtual  bool    NotAtEnd () const;
                    nonvirtual  String  operator *() const;
                    nonvirtual  void    operator++ ();

#if         qPlatform_Windows
                private:
                    HANDLE          fHandle;
                    WIN32_FIND_DATA fFindFileData;
                    String          fDirectory;
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




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "FileUtils.inl"

#endif  /*_Stroika_Foundation_IO_FileSystem_FileUtils_h_*/
