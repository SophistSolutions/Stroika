/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <windows.h>
#include    <shellapi.h>
#include    <shlobj.h>
#elif   qPlatform_POSIX
#include    <unistd.h>
#endif

#if     qPlatform_Windows
#include    "../../Execution/Platform/Windows/Exception.h"
#include    "../../Execution/Platform/Windows/HRESULTErrorException.h"
#endif

#include    "../../IO/FileAccessException.h"
#include    "../../IO/FileBusyException.h"
#include    "../../IO/FileFormatException.h"

#include    "FileUtils.h"

#include    "FileSystem.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::FileSystem;


#if     qPlatform_Windows
using   Execution::Platform::Windows::ThrowIfFalseGetLastError;
#endif





IO::FileSystem::FileSystem  IO::FileSystem::FileSystem::Default ()
{
    static  IO::FileSystem::FileSystem  sThe_;
    return sThe_;
}

bool    IO::FileSystem::FileSystem::Access (const String& fileFullPath, FileAccessMode accessMode) const
{
    // quick hack - not fully implemented - but since advsiory only - not too important...
    return FileExists (fileFullPath);
}

void    IO::FileSystem::FileSystem::CheckFileAccess (const String& fileFullPath, FileAccessMode accessMode)
{
    // quick hack - not fully implemented - but since advsiory only - not too important...

    if (not Access (fileFullPath, accessMode)) {
        // FOR NOW - MIMIC OLD CODE - BUT FIX TO CHECK READ AND WRITE (AND BOTH) ACCESS DEPENDING ON ARGS) -- LGP 2009-08-15
        Execution::DoThrow (FileAccessException (fileFullPath, accessMode));
    }
}

void    IO::FileSystem::FileSystem::CheckFileAccess (const String& fileFullPath, bool checkCanRead, bool checkCanWrite)
{
    if (checkCanRead and checkCanWrite) {
        CheckFileAccess (fileFullPath, IO::FileAccessMode::eReadWrite_FAM);
    }
    else if (checkCanRead) {
        CheckFileAccess (fileFullPath, IO::FileAccessMode::eRead_FAM);
    }
    else if (checkCanWrite) {
        CheckFileAccess (fileFullPath, IO::FileAccessMode::eWrite_FAM);
    }
}







/*
 ********************************************************************************
 ************************* FileSystem::ResolveShortcut **************************
 ********************************************************************************
 */
String IO::FileSystem::FileSystem::ResolveShortcut (const String& path2FileOrShortcut)
{
#if     qPlatform_Windows
    // NB: this requires COM, and for now - I don't want the support module depending on the COM module,
    // so just allow this to fail if COM isn't initialized.
    //      -- LGP 2007-09-23
    //
    {
        SHFILEINFO   info;
        memset (&info, 0, sizeof (info));
        if (::SHGetFileInfo (path2FileOrShortcut.AsSDKString ().c_str (), 0, &info, sizeof (info), SHGFI_ATTRIBUTES) == 0)
        {
            return path2FileOrShortcut;
        }
        // not a shortcut?
        if (!(info.dwAttributes & SFGAO_LINK))
        {
            return path2FileOrShortcut;
        }
    }

    // obtain the IShellLink interface
    IShellLink*     psl =   nullptr;
    IPersistFile*   ppf =   nullptr;
    try {
        if (FAILED (::CoCreateInstance (CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl))) {
            return path2FileOrShortcut;
        }
        if (SUCCEEDED (psl->QueryInterface (IID_IPersistFile, (LPVOID*)&ppf))) {
            if (SUCCEEDED (ppf->Load (path2FileOrShortcut.c_str (), STGM_READ))) {
                // Resolve the link, this may post UI to find the link
                if (SUCCEEDED (psl->Resolve(0, SLR_NO_UI))) {
                    TCHAR   path[MAX_PATH + 1];
                    memset (path, 0, sizeof (path));
                    if (SUCCEEDED (psl->GetPath (path, NEltsOf (path), nullptr, 0))) {
                        ppf->Release ();
                        ppf = nullptr;
                        psl->Release ();
                        psl = nullptr;
                        return String::FromSDKString (path);
                    }
                }
            }
        }
    }
    catch (...) {
        if (ppf != nullptr) {
            ppf->Release ();
        }
        if (psl != nullptr) {
            psl->Release ();
        }
        Execution::DoReThrow ();
    }
    if (ppf != nullptr) {
        ppf->Release ();
    }
    if (psl != nullptr) {
        psl->Release ();
    }
    return path2FileOrShortcut;
#else
    // interpret as slink follow - readlink
    AssertNotImplemented ();
    return String ();
#endif
}








/*
 ********************************************************************************
 *************************** FileSystem::GetFileSize ****************************
 ********************************************************************************
 */
FileOffset_t    IO::FileSystem::FileSystem::GetFileSize (const String& fileName)
{
#if     qPlatform_Windows
    WIN32_FILE_ATTRIBUTE_DATA   fileAttrData;
    (void)::memset (&fileAttrData, 0, sizeof (fileAttrData));
    Execution::Platform::Windows::ThrowIfFalseGetLastError (::GetFileAttributesExW (fileName.c_str (), GetFileExInfoStandard, &fileAttrData));
    return fileAttrData.nFileSizeLow + (static_cast<FileOffset_t> (fileAttrData.nFileSizeHigh) << 32);
#else
    AssertNotImplemented ();
    return 0;
#endif
}






/*
 ********************************************************************************
 ********************* FileSystem::GetFileLastModificationDate ******************
 ********************************************************************************
 */
DateTime        IO::FileSystem::FileSystem::GetFileLastModificationDate (const String& fileName)
{
#if     qPlatform_Windows
    WIN32_FILE_ATTRIBUTE_DATA   fileAttrData;
    (void)::memset (&fileAttrData, 0, sizeof (fileAttrData));
    ThrowIfFalseGetLastError (::GetFileAttributesExW (fileName.c_str (), GetFileExInfoStandard, &fileAttrData));
    return DateTime (fileAttrData.ftLastWriteTime);
#else
    AssertNotImplemented ();
    return DateTime ();
#endif
}






/*
 ********************************************************************************
 *********************** FileSystem::GetFileLastAccessDate **********************
 ********************************************************************************
 */
DateTime    IO::FileSystem::FileSystem::GetFileLastAccessDate (const String& fileName)
{
#if     qPlatform_Windows
    WIN32_FILE_ATTRIBUTE_DATA   fileAttrData;
    (void)::memset (&fileAttrData, 0, sizeof (fileAttrData));
    ThrowIfFalseGetLastError (::GetFileAttributesExW (fileName.c_str (), GetFileExInfoStandard, &fileAttrData));
    return DateTime (fileAttrData.ftLastAccessTime);
#else
    AssertNotImplemented ();
    return DateTime ();
#endif
}



