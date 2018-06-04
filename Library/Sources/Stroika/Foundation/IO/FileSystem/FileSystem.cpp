/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qPlatform_Windows
#include <windows.h>

#include <shellapi.h>
#include <shlobj.h>
#elif qPlatform_POSIX
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#if qPlatform_Linux
#include <linux/limits.h>
#endif
#include "../../Characters/Format.h"
#include "../../Characters/StringBuilder.h"
#include "../../Characters/ToString.h"
#include "../../Containers/Collection.h"
#include "../../Containers/Set.h"
#include "../../Debug/Trace.h"
#if qPlatform_Windows
#include "../../Execution/Platform/Windows/Exception.h"
#include "../../Execution/Platform/Windows/HRESULTErrorException.h"
#endif
#include "../../Execution/ErrNoException.h"
#include "../../Execution/Finally.h"

#include "../../IO/FileAccessException.h"
#include "../../IO/FileBusyException.h"
#include "../../IO/FileFormatException.h"
#include "../../Memory/SmallStackBuffer.h"

#include "DirectoryIterable.h"
#include "FileUtils.h"
#include "PathName.h"

#include "FileSystem.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;

#if qPlatform_Windows
using Execution::Platform::Windows::ThrowIfFalseGetLastError;
using Execution::Platform::Windows::ThrowIfZeroGetLastError;
#endif

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

#if qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy
/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika::Foundation::Configuration {
    constexpr EnumNames<IO::FileSystem::RemoveDirectoryPolicy> DefaultNames<IO::FileSystem::RemoveDirectoryPolicy>::k;
}
#endif

/*
 ********************************************************************************
 ********************************** FileSystem::Ptr *****************************
 ********************************************************************************
 */
bool IO::FileSystem::Ptr::Access (const String& fileFullPath, FileAccessMode accessMode) const noexcept
{
// @todo FIX to only do ONE system call, not two!!!
#if qPlatform_Windows
    if ((accessMode & FileAccessMode::eRead) == FileAccessMode::eRead) {
        DWORD attribs = ::GetFileAttributesW (fileFullPath.c_str ());
        if (attribs == INVALID_FILE_ATTRIBUTES) {
            return false;
        }
    }
    if ((accessMode & FileAccessMode::eWrite) == FileAccessMode::eWrite) {
        DWORD attribs = ::GetFileAttributesW (fileFullPath.c_str ());
        if ((attribs == INVALID_FILE_ATTRIBUTES) or (attribs & FILE_ATTRIBUTE_READONLY)) {
            return false;
        }
    }
    return true;
#elif qPlatform_POSIX
    // Not REALLY right - but an OK hack for now... -- LGP 2011-09-26
    //http://linux.die.net/man/2/access
    if ((accessMode & FileAccessMode::eRead) == FileAccessMode::eRead) {
        if (access (fileFullPath.AsSDKString ().c_str (), R_OK) != 0) {
            return false;
        }
    }
    if ((accessMode & FileAccessMode::eWrite) == FileAccessMode::eWrite) {
        if (access (fileFullPath.AsSDKString ().c_str (), W_OK) != 0) {
            return false;
        }
    }
    return true;
#else
    AssertNotImplemented ();
    return false;
#endif
}

void IO::FileSystem::Ptr::CheckAccess (const String& fileFullPath, FileAccessMode accessMode)
{
    // quick hack - not fully implemented - but since advsiory only - not too important...

    if (not Access (fileFullPath, accessMode)) {
        Execution::Throw (FileAccessException (fileFullPath, accessMode));
    }
}

void IO::FileSystem::Ptr::CheckAccess (const String& fileFullPath, bool checkCanRead, bool checkCanWrite)
{
    if (checkCanRead and checkCanWrite) {
        CheckAccess (fileFullPath, IO::FileAccessMode::eReadWrite);
    }
    else if (checkCanRead) {
        CheckAccess (fileFullPath, IO::FileAccessMode::eRead);
    }
    else if (checkCanWrite) {
        CheckAccess (fileFullPath, IO::FileAccessMode::eWrite);
    }
}

Memory::Optional<String> IO::FileSystem::Ptr::FindExecutableInPath (const String& filename) const
{
    // @TODO A
    //   const char* pPath = ::getenv ("PATH");
    // @todo windows https://msdn.microsoft.com/en-us/library/windows/desktop/aa365527(v=vs.85).aspx
    // on unix - get env path and just prepend and run access (EEXEC PERM)
    AssertNotImplemented ();
    return Memory::Optional<String>{};
}

String IO::FileSystem::Ptr::ResolveShortcut (const String& path2FileOrShortcut)
{
    try {
#if qPlatform_POSIX
        Memory::SmallStackBuffer<Characters::SDKChar> buf (1024);
        ssize_t                                       n;
        while ((n = ::readlink (path2FileOrShortcut.AsSDKString ().c_str (), buf, buf.GetSize ())) == buf.GetSize ()) {
            buf.GrowToSize (buf.GetSize () * 2);
        }
        if (n < 0) {
            auto e = errno;
            if (e == EINVAL) {
                // According to http://linux.die.net/man/2/readlink - this means the target is not a shortcut which is OK
                return path2FileOrShortcut;
            }
            else {
                Execution::errno_ErrorException::Throw (e);
            }
        }
        Assert (n <= buf.GetSize ()); // could leave no room for NUL-byte, but not needed
        constexpr bool kWorkaroundBuggyCentos5ReturnsNulBytesInBuf_ = true;
        if (kWorkaroundBuggyCentos5ReturnsNulBytesInBuf_) {
            const Characters::SDKChar* b = buf.begin ();
            const Characters::SDKChar* e = b + n;
            const Characters::SDKChar* i = find (b, e, '\0');
            if (i != e) {
                size_t newN = i - buf.begin ();
                Assert (newN < n);
                n = newN;
            }
        }
        return String::FromSDKString (SDKString (buf.begin (), buf.begin () + n));
#elif qPlatform_Windows
        // @todo WRONG semantics if file doesn't exist. Wed should raise an exception here.
        // But OK if not a shortcut. THEN just rutn the givne file
        //
        //
        // NB: this requires COM, and for now - I don't want the support module depending on the COM module,
        // so just allow this to fail if COM isn't initialized.
        //      -- LGP 2007-09-23
        //
        {
            SHFILEINFO info{};
            if (::SHGetFileInfo (path2FileOrShortcut.AsSDKString ().c_str (), 0, &info, sizeof (info), SHGFI_ATTRIBUTES) == 0) {
                return path2FileOrShortcut;
            }
            // not a shortcut?
            if (not(info.dwAttributes & SFGAO_LINK)) {
                return path2FileOrShortcut;
            }
        }

        // obtain the IShellLink interface
        IShellLink*   psl = nullptr;
        IPersistFile* ppf = nullptr;
        try {
            if (FAILED (::CoCreateInstance (CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl))) {
                return path2FileOrShortcut;
            }
            if (SUCCEEDED (psl->QueryInterface (IID_IPersistFile, (LPVOID*)&ppf))) {
                if (SUCCEEDED (ppf->Load (path2FileOrShortcut.c_str (), STGM_READ))) {
                    // Resolve the link, this may post UI to find the link
                    if (SUCCEEDED (psl->Resolve (0, SLR_NO_UI))) {
                        TCHAR path[MAX_PATH + 1]{};
                        DISABLE_COMPILER_MSC_WARNING_START (4267)
                        if (SUCCEEDED (psl->GetPath (path, NEltsOf (path), nullptr, 0))) {
                            ppf->Release ();
                            ppf = nullptr;
                            psl->Release ();
                            psl = nullptr;
                            return String::FromSDKString (path);
                        }
                        DISABLE_COMPILER_MSC_WARNING_END (4267)
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
            Execution::ReThrow ();
        }
        if (ppf != nullptr) {
            ppf->Release ();
        }
        if (psl != nullptr) {
            psl->Release ();
        }
        return path2FileOrShortcut;
#endif
    }
    Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER (path2FileOrShortcut, FileAccessMode::eRead);
}

String IO::FileSystem::Ptr::CanonicalizeName (const String& path2FileOrShortcut, bool throwIfComponentsNotFound)
{
    try {
#if qPlatform_POSIX
        //  We used to call canonicalize_file_name() - but this doesn't work with AIX 7.1/g++4.9.2, and
        //  according to http://man7.org/linux/man-pages/man3/canonicalize_file_name.3.html:
        //  The call canonicalize_file_name(path) is equivalent to the call:
        //      realpath(path, NULL)
        char* tmp{::realpath (path2FileOrShortcut.AsSDKString ().c_str (), nullptr)};
        if (tmp == nullptr) {
            errno_ErrorException::Throw (errno);
        }
        auto&& cleanup = Execution::Finally ([tmp]() noexcept { ::free (tmp); });
        return String::FromNarrowSDKString (tmp);
#elif qPlatform_Windows
        // @todo MaYBE USE GetFinalPathNameByHandle
        //          https://msdn.microsoft.com/en-us/library/windows/desktop/aa364962(v=vs.85).aspx
        //  EXCEPT REQUIRES OPEN?

        // @todo LARGELY UNSTED ROUGH DRAFT - 2015-05-11
        /*
         *  Note:   PathCanonicalize has lots of problems, PathCanonicalizeCh, and
         *  PathCchCanonicalizeEx is better, but only works with windows 8 or later.
         */
        using Characters::StringBuilder;
        String        tmp = ResolveShortcut (path2FileOrShortcut);
        StringBuilder sb;
        Components    c = GetPathComponents (path2FileOrShortcut);
        if (c.fAbsolutePath == Components::eAbsolutePath) {
            // use UNC notation
            sb += L"\\\\?";
            if (c.fDriveLetter) {
                sb += *c.fDriveLetter;
            }
            else if (c.fServerAndShare) {
                sb += c.fServerAndShare->fServer + L"\\" + c.fServerAndShare->fShare;
            }
            else {
                Execution::Throw (Execution::StringException (L"for absolute path need drive letter or server/share"));
            }
        }
        else {
            if (c.fDriveLetter) {
                sb += *c.fDriveLetter + L":";
            }
        }
        bool prefixWIthSlash = false;
        for (String i : c.fPath) {
            if (prefixWIthSlash) {
                sb += L"\\";
            }
            sb += i;
            prefixWIthSlash = true;
        }
        return sb.str ();
#else
        AssertNotImplemented ();
        return path2FileOrShortcut;
#endif
    }
    Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER (path2FileOrShortcut, FileAccessMode::eRead);
}

String IO::FileSystem::Ptr::CanonicalizeName (const String& path2FileOrShortcut, const String& relativeToDirectory, bool throwIfComponentsNotFound)
{
    AssertNotImplemented ();
    return path2FileOrShortcut;
}

String IO::FileSystem::Ptr::GetFullPathName (const String& pathName)
{
#if qPlatform_POSIX
    if (pathName.empty ()) {
        //throw bad path name @todo improve exception
        Execution::Throw (Execution::StringException (L"invalid pathname"));
    }
    if (pathName[0] == '/') {
        return pathName;
    }
    return GetCurrentDirectory () + pathName;
#elif qPlatform_Windows
    String        name2Use          = pathName;
    const wchar_t kAnySizePrefix_[] = L"\\\\?\\";
    if (not name2Use.StartsWith (kAnySizePrefix_)) {
        name2Use = kAnySizePrefix_ + name2Use;
    }
    DWORD                             sz = ::GetFullPathNameW (name2Use.c_str (), 0, nullptr, nullptr);
    Memory::SmallStackBuffer<wchar_t> buf (sz + 1);
    Execution::Platform::Windows::ThrowIfZeroGetLastError (::GetFullPathNameW (name2Use.c_str (), static_cast<DWORD> (buf.GetSize ()), buf.begin (), nullptr));
    return buf.begin ();
#endif
}

IO::FileSystem::Ptr::Components IO::FileSystem::Ptr::GetPathComponents (const String& fileName)
{
// @todo LARGELY UNSTED ROUGH DRAFT - 2015-05-11
// See http://en.wikipedia.org/wiki/Path_%28computing%29 to write this
#if 0
//windows
C:
    \user\docs\Letter.txt
    / user / docs / Letter.txt
C:
    Letter.txt
    \\Server01\user\docs\Letter.txt
    \\ ? \UNC\Server01\user\docs\Letter.txt
    \\ ? \C : \user\docs\Letter.txt
    C : \user\docs\somefile.ext : alternate_stream_name
    . / inthisdir
    .. / .. / greatgrandparent
#endif
#if 0
// unix
    / home / user / docs / Letter.txt
    . / inthisdir
    .. / .. / greatgrandparent
    ~ / .rcinfo
#endif
    IO::FileSystem::Ptr::Components result;
    using Characters::Character;
    using Traversal::Iterator;

#if qPlatform_Windows
    bool isUNCName      = fileName.length () > 2 and fileName.StartsWith (L"\\\\");
    bool isAbsolutePath = fileName.length () >= 1 and fileName.StartsWith (L"\\");
#else
#endif
#if qPlatform_Windows
    const Set<Character> kSlashChars_ = {'\\', '/'};
#else
    const Set<Character> kSlashChars_ = {'/'};
#endif
    Sequence<String> rawComponents = fileName.Tokenize (kSlashChars_, false);
    Iterator<String> i             = rawComponents.begin ();
#if qPlatform_Windows
    if (isUNCName) {
        // work todo
    }
#endif
    for (; i != rawComponents.end (); ++i) {
        result.fPath.Append (*i);
    }
    AssertNotImplemented ();
    return result;
}

FileOffset_t IO::FileSystem::Ptr::GetFileSize (const String& fileName)
{
    try {
#if qPlatform_POSIX
        struct stat s {
        };
        ThrowErrNoIfNegative (::stat (fileName.AsNarrowSDKString ().c_str (), &s));
        return s.st_size;
#elif qPlatform_Windows
        WIN32_FILE_ATTRIBUTE_DATA fileAttrData{};
        Execution::Platform::Windows::ThrowIfFalseGetLastError (::GetFileAttributesExW (fileName.c_str (), GetFileExInfoStandard, &fileAttrData));
        return fileAttrData.nFileSizeLow + (static_cast<FileOffset_t> (fileAttrData.nFileSizeHigh) << 32);
#else
        AssertNotImplemented ();
        return 0;
#endif
    }
    Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER (fileName, FileAccessMode::eRead);
}

DateTime IO::FileSystem::Ptr::GetFileLastModificationDate (const String& fileName)
{
    try {
#if qPlatform_POSIX
        struct stat s {
        };
        ThrowErrNoIfNegative (::stat (fileName.AsNarrowSDKString ().c_str (), &s));
        return DateTime (s.st_mtime);
#elif qPlatform_Windows
        WIN32_FILE_ATTRIBUTE_DATA fileAttrData{};
        ThrowIfFalseGetLastError (::GetFileAttributesExW (fileName.c_str (), GetFileExInfoStandard, &fileAttrData));
        return DateTime (fileAttrData.ftLastWriteTime);
#else
        AssertNotImplemented ();
        return DateTime ();
#endif
    }
    Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER (fileName, FileAccessMode::eRead);
}

DateTime IO::FileSystem::Ptr::GetFileLastAccessDate (const String& fileName)
{
    try {
#if qPlatform_POSIX
        struct stat s {
        };
        ThrowErrNoIfNegative (::stat (fileName.AsNarrowSDKString ().c_str (), &s));
        return DateTime (s.st_atime);
#elif qPlatform_Windows
        WIN32_FILE_ATTRIBUTE_DATA fileAttrData{};
        ThrowIfFalseGetLastError (::GetFileAttributesExW (fileName.c_str (), GetFileExInfoStandard, &fileAttrData));
        return DateTime (fileAttrData.ftLastAccessTime);
#else
        AssertNotImplemented ();
        return DateTime ();
#endif
    }
    Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER (fileName, FileAccessMode::eRead);
}

void IO::FileSystem::Ptr::RemoveFile (const String& fileName)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"IO::FileSystem::Ptr::RemoveFile", L"fileName='%s'", fileName.c_str ());
#endif
    try {
#if qPlatform_Windows && qTargetPlatformSDKUseswchar_t
        Execution::ThrowErrNoIfNegative (::_wunlink (fileName.c_str ()));
#else
        Execution::ThrowErrNoIfNegative (::unlink (fileName.AsNarrowSDKString ().c_str ()));
#endif
    }
    Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER (fileName, FileAccessMode::eWrite);
}

bool IO::FileSystem::Ptr::RemoveFileIf (const String& fileName)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"IO::FileSystem::Ptr::RemoveFileIf", L"fileName='%s'", fileName.c_str ());
#endif
    try {
#if qPlatform_Windows && qTargetPlatformSDKUseswchar_t
        int r = ::_wunlink (fileName.c_str ());
#else
        int r = ::unlink (fileName.AsNarrowSDKString ().c_str ());
#endif
        if (r < 0) {
            if (errno != ENOENT) {
                errno_ErrorException::Throw (errno);
            }
        }
        return r == 0;
    }
    Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER (fileName, FileAccessMode::eWrite);
}

void IO::FileSystem::Ptr::RemoveDirectory (const String& directory, RemoveDirectoryPolicy policy)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("IO::FileSystem::Ptr::RemoveDirectory", L"directory='%s', policy=%s", directory.c_str (), Characters::ToString (policy).c_str ());
#endif
    bool triedRMRF{false};
Again:
    try {
#if qPlatform_Windows && qTargetPlatformSDKUseswchar_t
        int r = ::_wrmdir (directory.c_str ());
#else
        int r = ::rmdir (directory.AsNarrowSDKString ().c_str ());
#endif
        if (r < 0) {
            if (not triedRMRF and policy == RemoveDirectoryPolicy::eRemoveAnyContainedFiles and errno == ENOTEMPTY) {
                // Note: Subtle - we convert DirectoryIterable to Collection<String> - because we dont care about order, and we do care that we
                // don't lose track of where we are in iteration, just because we delete files in the directory, which can invalidate our cursor
                for (String i : DirectoryIterable (directory, DirectoryIterable::IteratorReturnType::eFullPathName).As<Collection<String>> ()) {
                    if (Directory{i}.Exists ()) {
                        RemoveDirectory (i, RemoveDirectoryPolicy::eRemoveAnyContainedFiles);
                    }
                    else {
                        RemoveFile (i);
                    }
                }
                triedRMRF = true;
                goto Again;
            }
            errno_ErrorException::Throw (errno);
        }
    }
    Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER (directory, FileAccessMode::eWrite);
}

bool IO::FileSystem::Ptr::RemoveDirectoryIf (const String& directory, RemoveDirectoryPolicy policy)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("IO::FileSystem::Ptr::RemoveDirectoryIf", L"directory='%s', policy=%s", directory.c_str (), Characters::ToString (policy).c_str ());
#endif
    bool triedRMRF{false};
Again:
    try {
#if qPlatform_Windows && qTargetPlatformSDKUseswchar_t
        int r = ::_wrmdir (directory.c_str ());
#else
        int r = ::rmdir (directory.AsNarrowSDKString ().c_str ());
#endif
        if (r < 0) {
            if (not triedRMRF and policy == RemoveDirectoryPolicy::eRemoveAnyContainedFiles and errno == ENOTEMPTY) {
                // Note: Subtle - we convert DirectoryIterable to Collection<String> - because we dont care about order, and we do care that we
                // don't lose track of where we are in iteration, just because we delete files in the directory, which can invalidate our cursor
                for (String i : DirectoryIterable (directory, DirectoryIterable::IteratorReturnType::eFullPathName).As<Collection<String>> ()) {
                    if (Directory{i}.Exists ()) {
                        RemoveDirectoryIf (i, RemoveDirectoryPolicy::eRemoveAnyContainedFiles);
                    }
                    else {
                        RemoveFileIf (i);
                    }
                }
                triedRMRF = true;
                goto Again;
            }
            if (errno != ENOENT) {
                errno_ErrorException::Throw (errno);
            }
        }
        return r == 0;
    }
    Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER (directory, FileAccessMode::eWrite);
}

void IO::FileSystem::Ptr::CreateSymbolicLink (const String& linkName, const String& target)
{
#if qPlatform_POSIX
    Execution::ThrowErrNoIfNegative (::symlink (target.AsNarrowSDKString ().c_str (), linkName.AsNarrowSDKString ().c_str ()));
#else
    AssertNotReached ();
#endif
}

String IO::FileSystem::Ptr::GetCurrentDirectory () const
{
#if qPlatform_POSIX
    SDKChar buf[PATH_MAX];
    Execution::ThrowErrNoIfNull (::getcwd (buf, NEltsOf (buf)));
    return AssureDirectoryPathSlashTerminated (String::FromSDKString (buf));
#elif qPlatform_Windows
    SDKChar buf[MAX_PATH];
    ThrowIfZeroGetLastError (::GetCurrentDirectory (static_cast<DWORD> (NEltsOf (buf)), buf));
    return AssureDirectoryPathSlashTerminated (String::FromSDKString (buf));
#else
    AssertNotImplemented ();
#endif
}

void IO::FileSystem::Ptr::SetCurrentDirectory (const String& newDir)
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"IO::FileSystem::Ptr::SetCurrentDirectory", L"directory='%s'", newDir.c_str ())};
#if qPlatform_POSIX
    Execution::ThrowErrNoIfNegative (::chdir (newDir.AsNarrowSDKString ().c_str ()));
#elif qPlatform_Windows
    ::SetCurrentDirectory (newDir.AsSDKString ().c_str ());
#else
    AssertNotReached ();
#endif
}

/*
 ********************************************************************************
 **************************** FileSystem::Default *******************************
 ********************************************************************************
 */
IO::FileSystem::Ptr IO::FileSystem::Default ()
{
    static IO::FileSystem::Ptr sThe_;
    return sThe_;
}
