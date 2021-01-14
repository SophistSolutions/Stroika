/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
#include "../../Execution/Exceptions.h"
#include "../../Execution/Finally.h"

#include "../../IO/FileSystem/Exception.h"
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
using Execution::Platform::Windows::ThrowIfZeroGetLastError;
#endif

using Memory::SmallStackBuffer;
using Memory::SmallStackBufferCommon;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

/*
 ********************************************************************************
 ********************************** FileSystem::Ptr *****************************
 ********************************************************************************
 */
bool IO::FileSystem::Ptr::Access (const filesystem::path& fileFullPath, AccessMode accessMode) const noexcept
{
// @todo FIX to only do ONE system call, not two!!!
#if qPlatform_Windows
    if ((accessMode & AccessMode::eRead) == AccessMode::eRead) {
        DWORD attribs = ::GetFileAttributesW (fileFullPath.c_str ());
        if (attribs == INVALID_FILE_ATTRIBUTES) {
            return false;
        }
    }
    if ((accessMode & AccessMode::eWrite) == AccessMode::eWrite) {
        DWORD attribs = ::GetFileAttributesW (fileFullPath.c_str ());
        if ((attribs == INVALID_FILE_ATTRIBUTES) or (attribs & FILE_ATTRIBUTE_READONLY)) {
            return false;
        }
    }
    return true;
#elif qPlatform_POSIX
    // Not REALLY right - but an OK hack for now... -- LGP 2011-09-26
    //http://linux.die.net/man/2/access
    if ((accessMode & AccessMode::eRead) == AccessMode::eRead) {
        if (access (fileFullPath.c_str (), R_OK) != 0) {
            return false;
        }
    }
    if ((accessMode & AccessMode::eWrite) == AccessMode::eWrite) {
        if (access (fileFullPath.c_str (), W_OK) != 0) {
            return false;
        }
    }
    return true;
#else
    AssertNotImplemented ();
    return false;
#endif
}

void IO::FileSystem::Ptr::CheckAccess (const filesystem::path& fileFullPath, AccessMode accessMode)
{
    // quick hack - not fully implemented - but since advsiory only - not too important...
    if (not Access (fileFullPath, accessMode)) {
        // @todo take into account 'accessMode' in the resulting message
        Execution::Throw (IO::FileSystem::Exception{make_error_code (errc::permission_denied), fileFullPath});
    }
}

void IO::FileSystem::Ptr::CheckAccess (const filesystem::path& fileFullPath, bool checkCanRead, bool checkCanWrite)
{
    if (checkCanRead and checkCanWrite) {
        CheckAccess (fileFullPath, IO::AccessMode::eReadWrite);
    }
    else if (checkCanRead) {
        CheckAccess (fileFullPath, IO::AccessMode::eRead);
    }
    else if (checkCanWrite) {
        CheckAccess (fileFullPath, IO::AccessMode::eWrite);
    }
}

optional<filesystem::path> IO::FileSystem::Ptr::FindExecutableInPath (const filesystem::path& /*filename*/) const
{
    // @TODO A
    //   const char* pPath = ::getenv ("PATH");
    // @todo windows https://msdn.microsoft.com/en-us/library/windows/desktop/aa365527(v=vs.85).aspx
    // on unix - get env path and just prepend and run access (EEXEC PERM)
    AssertNotImplemented ();
    return nullopt;
}

filesystem::path IO::FileSystem::Ptr::ResolveShortcut (const filesystem::path& path2FileOrShortcut)
{
#if qPlatform_POSIX
    SmallStackBuffer<Characters::SDKChar> buf (SmallStackBufferCommon::eUninitialized, 1024);
    ssize_t                               n;
    while ((n = ::readlink (path2FileOrShortcut.generic_string ().c_str (), buf, buf.GetSize ())) == buf.GetSize ()) {
        buf.GrowToSize_uninitialized (buf.GetSize () * 2);
    }
    if (n < 0) {
        auto e = errno;
        if (e == EINVAL) {
            // According to http://linux.die.net/man/2/readlink - this means the target is not a shortcut which is OK
            return path2FileOrShortcut;
        }
        else {
            FileSystem::Exception::ThrowPOSIXErrNo (e, path2FileOrShortcut);
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
    return SDKString (buf.begin (), buf.begin () + n);
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
        if (::SHGetFileInfo (path2FileOrShortcut.c_str (), 0, &info, sizeof (info), SHGFI_ATTRIBUTES) == 0) {
            return path2FileOrShortcut;
        }
        // not a shortcut?
        if (not(info.dwAttributes & SFGAO_LINK)) {
            return path2FileOrShortcut;
        }
    }

    // obtain the IShellLink interface
    IShellLink*             psl     = nullptr;
    IPersistFile*           ppf     = nullptr;
    [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept {
        ppf->Release ();
        ppf = nullptr;
        psl->Release ();
        psl = nullptr;
    });
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
                    return path;
                }
                DISABLE_COMPILER_MSC_WARNING_END (4267)
            }
        }
    }
    return path2FileOrShortcut;
#endif
}

DISABLE_COMPILER_MSC_WARNING_START (4996) // use deprecated
filesystem::path IO::FileSystem::Ptr::CanonicalizeName (const filesystem::path& path2FileOrShortcut, [[maybe_unused]] bool throwIfComponentsNotFound)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"IO::FileSystem::Ptr::CanonicalizeName", L"path2FileOrShortcut='%s', throwIfComponentsNotFound=%s", path2FileOrShortcut.c_str (), Characters::ToString (throwIfComponentsNotFound).c_str ());
#endif
#if qPlatform_POSIX
    //  We used to call canonicalize_file_name() - but this doesn't work with AIX 7.1/g++4.9.2, and
    //  according to http://man7.org/linux/man-pages/man3/canonicalize_file_name.3.html:
    //  The call canonicalize_file_name(path) is equivalent to the call:
    //      realpath(path, NULL)
    char* tmp{::realpath (path2FileOrShortcut.generic_string ().c_str (), nullptr)};
    if (tmp == nullptr) {
        FileSystem::Exception::ThrowPOSIXErrNo (errno, path2FileOrShortcut);
    }
    [[maybe_unused]] auto&& cleanup = Execution::Finally ([tmp] () noexcept { ::free (tmp); });
    return tmp;
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
    String        tmp = FromPath (ResolveShortcut (path2FileOrShortcut));
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
            Execution::Throw (FileSystem::Exception{make_error_code (errc::no_such_file_or_directory), String{L"for absolute path need drive letter or server/share"sv}, path2FileOrShortcut});
        }
    }
    else {
        if (c.fDriveLetter) {
            sb += *c.fDriveLetter + L":"sv;
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
    return ToPath (sb.str ());
#else
    AssertNotImplemented ();
    return path2FileOrShortcut;
#endif
}
DISABLE_COMPILER_MSC_WARNING_END (4996) // use deprecated

filesystem::path IO::FileSystem::Ptr::CanonicalizeName (const filesystem::path& path2FileOrShortcut, const filesystem::path& /*relativeToDirectory*/, bool /*throwIfComponentsNotFound*/)
{
    AssertNotImplemented ();
    return path2FileOrShortcut;
}

filesystem::path IO::FileSystem::Ptr::GetFullPathName (const filesystem::path& pathName)
{
    if (pathName.is_absolute ()) {
        return pathName;
    }
#if qPlatform_POSIX
    if (pathName.empty ())
        [[UNLIKELY_ATTR]] {
        //throw bad path name @todo improve exception
        Execution::Throw (Execution::Exception (L"invalid pathname"sv));
    }
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
    return filesystem::current_path () / pathName;
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#elif qPlatform_Windows
    String        name2Use          = FromPath (pathName);
    const wchar_t kAnySizePrefix_[] = L"\\\\?\\";
    if (not name2Use.StartsWith (kAnySizePrefix_)) {
        name2Use = kAnySizePrefix_ + name2Use;
    }
    DWORD                     sz = ::GetFullPathNameW (name2Use.c_str (), 0, nullptr, nullptr);
    SmallStackBuffer<wchar_t> buf (SmallStackBufferCommon::eUninitialized, sz + 1);
    Execution::Platform::Windows::ThrowIfZeroGetLastError (::GetFullPathNameW (name2Use.c_str (), static_cast<DWORD> (buf.GetSize ()), buf.begin (), nullptr));
    return buf.begin ();
#endif
}

IO::FileSystem::Ptr::Components IO::FileSystem::Ptr::GetPathComponents (const filesystem::path& fileNamex)
{
    String fileName = FromPath (fileNamex);
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
    bool isUNCName = fileName.length () > 2 and fileName.StartsWith (L"\\\\");
    //bool isAbsolutePath = fileName.length () >= 1 and fileName.StartsWith (L"\\");
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

FileOffset_t IO::FileSystem::Ptr::GetFileSize (const filesystem::path& fileName)
{
#if qPlatform_POSIX
    struct stat s {
    };
    FileSystem::Exception::ThrowPOSIXErrNoIfNegative (::stat (fileName.generic_string ().c_str (), &s), fileName);
    return s.st_size;
#elif qPlatform_Windows
    WIN32_FILE_ATTRIBUTE_DATA fileAttrData{};
    FileSystem::Exception::ThrowIfZeroGetLastError (::GetFileAttributesExW (fileName.c_str (), GetFileExInfoStandard, &fileAttrData), fileName);
    return fileAttrData.nFileSizeLow + (static_cast<FileOffset_t> (fileAttrData.nFileSizeHigh) << 32);
#else
    AssertNotImplemented ();
    return 0;
#endif
}

DateTime IO::FileSystem::Ptr::GetFileLastModificationDate (const filesystem::path& fileName)
{
#if qPlatform_POSIX
    struct stat s {
    };
    FileSystem::Exception::ThrowPOSIXErrNoIfNegative (::stat (fileName.generic_string ().c_str (), &s), fileName);
    return DateTime (s.st_mtime);
#elif qPlatform_Windows
    WIN32_FILE_ATTRIBUTE_DATA fileAttrData{};
    FileSystem::Exception::ThrowIfZeroGetLastError (::GetFileAttributesExW (fileName.c_str (), GetFileExInfoStandard, &fileAttrData), fileName);
    return DateTime (fileAttrData.ftLastWriteTime);
#else
    AssertNotImplemented ();
    return DateTime ();
#endif
}

DateTime IO::FileSystem::Ptr::GetFileLastAccessDate (const filesystem::path& fileName)
{
#if qPlatform_POSIX
    struct stat s {
    };
    FileSystem::Exception::ThrowPOSIXErrNoIfNegative (::stat (fileName.generic_string ().c_str (), &s), fileName);
    return DateTime{s.st_atime};
#elif qPlatform_Windows
    WIN32_FILE_ATTRIBUTE_DATA fileAttrData{};
    FileSystem::Exception::ThrowIfZeroGetLastError (::GetFileAttributesExW (fileName.c_str (), GetFileExInfoStandard, &fileAttrData), fileName);
    return DateTime{fileAttrData.ftLastAccessTime};
#else
    AssertNotImplemented ();
    return DateTime{};
#endif
}

void IO::FileSystem::Ptr::RemoveFile (const filesystem::path& fileName)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"IO::FileSystem::Ptr::RemoveFile", L"fileName='%s'", fileName.c_str ());
#endif
#if qPlatform_Windows && qTargetPlatformSDKUseswchar_t
    FileSystem::Exception::ThrowPOSIXErrNoIfNegative (::_wunlink (fileName.c_str ()), fileName);
#else
    FileSystem::Exception::ThrowPOSIXErrNoIfNegative (::unlink (fileName.generic_string ().c_str ()), fileName);
#endif
}

bool IO::FileSystem::Ptr::RemoveFileIf (const filesystem::path& fileName)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"IO::FileSystem::Ptr::RemoveFileIf", L"fileName='%s'", fileName.c_str ());
#endif
#if qPlatform_Windows && qTargetPlatformSDKUseswchar_t
    int r = ::_wunlink (fileName.c_str ());
#else
    int r = ::unlink (fileName.generic_string ().c_str ());
#endif
    if (r < 0) {
        if (errno != ENOENT) {
            FileSystem::Exception::ThrowPOSIXErrNo (errno, fileName);
        }
    }
    return r == 0;
}

DISABLE_COMPILER_MSC_WARNING_START (4996) // use deprecated
DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
void IO::FileSystem::Ptr::RemoveDirectory (const filesystem::path& directory, RemoveDirectoryPolicy policy)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"IO::FileSystem::Ptr::RemoveDirectory", L"directory='%s', policy=%s", directory.c_str (), Characters::ToString (policy).c_str ());
#endif
    bool triedRMRF{false};
Again:
#if qPlatform_Windows && qTargetPlatformSDKUseswchar_t
    int r = ::_wrmdir (directory.c_str ());
#else
    int r = ::rmdir (directory.generic_string ().c_str ());
#endif
    if (r < 0) {
        if (not triedRMRF and policy == RemoveDirectoryPolicy::eRemoveAnyContainedFiles and errno == ENOTEMPTY) {
            // Note: Subtle - we convert DirectoryIterable to Collection<String> - because we don't care about order, and we do care that we
            // don't lose track of where we are in iteration, just because we delete files in the directory, which can invalidate our cursor
            for (filesystem::path i : DirectoryIterable{directory, DirectoryIterable::IteratorReturnType::eFullPathName}) {
                if (filesystem::is_directory (i)) {
                    RemoveDirectory (i, RemoveDirectoryPolicy::eRemoveAnyContainedFiles);
                }
                else {
                    RemoveFile (i);
                }
            }
            triedRMRF = true;
            goto Again;
        }
        FileSystem::Exception::ThrowPOSIXErrNo (errno, directory);
    }
}
DISABLE_COMPILER_MSC_WARNING_END (4996)
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")

DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
DISABLE_COMPILER_MSC_WARNING_START (4996) // use deprecated
bool IO::FileSystem::Ptr::RemoveDirectoryIf (const filesystem::path& directory, RemoveDirectoryPolicy policy)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"IO::FileSystem::Ptr::RemoveDirectoryIf", L"directory='%s', policy=%s", directory.c_str (), Characters::ToString (policy).c_str ());
#endif
    bool triedRMRF{false};
Again:
#if qPlatform_Windows && qTargetPlatformSDKUseswchar_t
    int r = ::_wrmdir (directory.c_str ());
#else
    int r = ::rmdir (directory.generic_string ().c_str ());
#endif
    if (r < 0) {
        if (not triedRMRF and policy == RemoveDirectoryPolicy::eRemoveAnyContainedFiles and errno == ENOTEMPTY) {
            // Note: Subtle - we convert DirectoryIterable to Collection<String> - because we don't care about order, and we do care that we
            // don't lose track of where we are in iteration, just because we delete files in the directory, which can invalidate our cursor
            for (filesystem::path i : DirectoryIterable (directory, DirectoryIterable::IteratorReturnType::eFullPathName)) {
                if (filesystem::is_directory (i)) {
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
            FileSystem::Exception::ThrowPOSIXErrNo (errno, directory);
        }
    }
    return r == 0;
}
DISABLE_COMPILER_MSC_WARNING_END (4996)
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")

void IO::FileSystem::Ptr::CreateSymbolicLink ([[maybe_unused]] const filesystem::path& linkName, [[maybe_unused]] const filesystem::path& target)
{
#if qPlatform_POSIX
    Execution::ThrowPOSIXErrNoIfNegative (::symlink (target.generic_string ().c_str (), linkName.generic_string ().c_str ()));
#else
    AssertNotReached ();
#endif
}

filesystem::path IO::FileSystem::Ptr::GetCurrentDirectory () const
{
#if qPlatform_POSIX
    SDKChar buf[PATH_MAX];
    Execution::ThrowPOSIXErrNoIfNull (::getcwd (buf, NEltsOf (buf)));
    return buf;
#elif qPlatform_Windows
    SDKChar buf[MAX_PATH];
    ThrowIfZeroGetLastError (::GetCurrentDirectory (static_cast<DWORD> (NEltsOf (buf)), buf));
    return buf;
#else
    AssertNotImplemented ();
#endif
}

void IO::FileSystem::Ptr::SetCurrentDirectory (const filesystem::path& newDir)
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"IO::FileSystem::Ptr::SetCurrentDirectory", L"directory='%s'", newDir.c_str ())};
#if qPlatform_POSIX
    Execution::ThrowPOSIXErrNoIfNegative (::chdir (newDir.generic_string ().c_str ()));
#elif qPlatform_Windows
    ::SetCurrentDirectory (newDir.c_str ());
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
