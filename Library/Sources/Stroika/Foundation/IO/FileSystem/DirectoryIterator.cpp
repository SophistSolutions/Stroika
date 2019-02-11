/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qPlatform_Windows
#include <windows.h>
#elif qPlatform_POSIX
#include <dirent.h>
#endif

#include "../../Characters/CString/Utilities.h"
#include "../../Characters/ToString.h"
#include "../../Debug/AssertExternallySynchronizedLock.h"
#include "../../Debug/Trace.h"
#include "../../Execution/ErrNoException.h"
#if qPlatform_Windows
#include "../../Execution/Platform/Windows/Exception.h"
#endif
#include "../../IO/FileAccessException.h"
#include "../../IO/FileSystem/FileSystem.h"

#include "PathName.h"

#include "DirectoryIterator.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;
using namespace Stroika::Foundation::Traversal;

#if qPlatform_Windows
using Execution::Platform::Windows::ThrowIfFalseGetLastError;
#endif
using Execution::ThrowErrNoIfNull;
using Execution::ThrowPOSIXErrNo;

// from https://www.gnu.org/software/libc/manual/html_node/Reading_002fClosing-Directory.html -
// To distinguish between an end-of-directory condition or an error, you must set errno to zero before calling readdir.

class DirectoryIterator::Rep_ : public Iterator<String>::IRep, private Debug::AssertExternallySynchronizedLock {
private:
    IteratorReturnType fIteratorReturnType_;
    String             fDirName_;
    String             fReportPrefix_;
#if qPlatform_POSIX
    DIR*    fDirIt_{nullptr};
    dirent* fCur_{nullptr};
#elif qPlatform_Windows
    HANDLE          fHandle_{INVALID_HANDLE_VALUE}; // after constructor - fHandle_ == INVALID_HANDLE_VALUE means iterator ATEND
    WIN32_FIND_DATA fFindFileData_{};
#endif

public:
    Rep_ (const String& dir, IteratorReturnType iteratorReturns)
        : fIteratorReturnType_ (iteratorReturns)
        , fDirName_ (dir)
        , fReportPrefix_ (mkReportPrefix_ (dir, iteratorReturns))
#if qPlatform_POSIX
        , fDirIt_
    {
        ::opendir (dir.AsSDKString ().c_str ())
    }
#endif
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"DirectoryIterator::Rep_::CTOR", L"'%s'", dir.c_str ()};
#endif
        try {
#if qPlatform_POSIX
            if (fDirIt_ == nullptr) {
                Execution::ThrowPOSIXErrNo ();
            }
            else {
                errno = 0;
                ThrowErrNoIfNull (fCur_ = ::readdir (fDirIt_));
            }
            if (fCur_ != nullptr and fCur_->d_name[0] == '.' and (CString::Equals (fCur_->d_name, SDKSTR (".")) or CString::Equals (fCur_->d_name, SDKSTR ("..")))) {
                optional<String> tmphack;
                More (&tmphack, true);
            }
#elif qPlatform_Windows
            fHandle_ = ::FindFirstFile ((dir + L"\\*").AsSDKString ().c_str (), &fFindFileData_);
            while (fHandle_ != INVALID_HANDLE_VALUE and (CString::Equals (fFindFileData_.cFileName, SDKSTR (".")) or CString::Equals (fFindFileData_.cFileName, SDKSTR ("..")))) {
                optional<String> tmphack;
                More (&tmphack, true);
            }
#endif
        }
        Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAMESONLY_HELPER (dir);
    }
#if qPlatform_POSIX
    Rep_ (const String& dirName, const optional<ino_t>& curInode, IteratorReturnType iteratorReturns)
        : fIteratorReturnType_ (iteratorReturns)
        , fDirName_ (dirName)
        , fReportPrefix_ (mkReportPrefix_ (dirName, iteratorReturns))
        , fDirIt_{::opendir (dirName.AsSDKString ().c_str ())}
    {
        if (fDirIt_ == nullptr) {
            Execution::ThrowPOSIXErrNo ();
        }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"DirectoryIterator::Rep_::CTOR", L"curInode=%s", Characters::ToString (curInode).c_str ()};
#endif
        if (curInode) {
            do {
                fCur_ = ::readdir (fDirIt_);
            } while (fCur_ != nullptr and fCur_->d_ino != *curInode);
        }
    }
#elif qPlatform_Windows
    // missing name implies Iterator::IsAtEnd ()
    Rep_ (const String& dir, const optional<String>& name, IteratorReturnType iteratorReturns)
        : fIteratorReturnType_ (iteratorReturns)
        , fDirName_ (dir)
        , fReportPrefix_ (mkReportPrefix_ (dir, iteratorReturns))
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"DirectoryIterator::Rep_::CTOR", L"'%s',name=%s", dir.c_str (), name.c_str ()};
#endif
        if (name) {
            fHandle_ = ::FindFirstFile ((dir + L"\\*").AsSDKString ().c_str (), &fFindFileData_);
            while (fHandle_ != INVALID_HANDLE_VALUE and String::FromSDKString (fFindFileData_.cFileName) != name) {
                optional<String> tmphack;
                More (&tmphack, true);
            }
        }
    }
#endif
    virtual ~Rep_ ()
    {
#if qPlatform_POSIX
        if (fDirIt_ != nullptr) {
            ::closedir (fDirIt_);
        }
#elif qPlatform_Windows
        if (fHandle_ != INVALID_HANDLE_VALUE) {
            ::FindClose (fHandle_);
        }
#endif
    }
    virtual void More (optional<String>* result, bool advance) override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        RequireNotNull (result);
        *result = nullopt;
#if qPlatform_POSIX
        if (advance) {
        Again:
            RequireNotNull (fCur_);
            RequireNotNull (fDirIt_);
            errno = 0;
            fCur_ = ::readdir (fDirIt_);
            if (fCur_ == nullptr) {
                // errno can be zero here at end of directory
                if (errno != EBADF and errno != 0) {
                    ThrowPOSIXErrNo ();
                }
            }
            if (fCur_ != nullptr and fCur_->d_name[0] == '.' and (CString::Equals (fCur_->d_name, SDKSTR (".")) or CString::Equals (fCur_->d_name, SDKSTR ("..")))) {
                goto Again;
            }
        }
        if (fCur_ != nullptr) {
            *result = fReportPrefix_ + String::FromSDKString (fCur_->d_name);
        }
#elif qPlatform_Windows
        if (advance) {
        Again:
            Require (fHandle_ != INVALID_HANDLE_VALUE);
            (void)::memset (&fFindFileData_, 0, sizeof (fFindFileData_));
            if (::FindNextFile (fHandle_, &fFindFileData_) == 0) {
                ::FindClose (fHandle_);
                fHandle_ = INVALID_HANDLE_VALUE;
            }
            if (fHandle_ != INVALID_HANDLE_VALUE and (CString::Equals (fFindFileData_.cFileName, SDKSTR (".")) or CString::Equals (fFindFileData_.cFileName, SDKSTR ("..")))) {
                goto Again;
            }
        }
        if (fHandle_ != INVALID_HANDLE_VALUE) {
            *result = fReportPrefix_ + String::FromSDKString (fFindFileData_.cFileName);
        }
#endif
    }
    virtual bool Equals (const Iterator<String>::IRep* rhs) const override
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        RequireNotNull (rhs);
        RequireMember (rhs, Rep_);
        const Rep_& rrhs = *dynamic_cast<const Rep_*> (rhs);
#if qPlatform_POSIX
        return fDirName_ == rrhs.fDirName_ and fIteratorReturnType_ == rrhs.fIteratorReturnType_ and ((fCur_ == rrhs.fCur_ and fCur_ == nullptr) or (rrhs.fCur_ != nullptr and fCur_->d_ino == rrhs.fCur_->d_ino));
#elif qPlatform_Windows
        return fHandle_ == rrhs.fHandle_;
#endif
    }
    virtual RepSmartPtr Clone () const override
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx (L"Entering DirectoryIterator::Rep_::Clone");
#endif
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
#if qPlatform_POSIX
        AssertNotNull (fDirIt_);
        /*
         *  must find telldir() returns the location of the NEXT read. We must pass along the value of telldir as
         *  of the PREVIOUS read. Essentially (seek CUR_OFF, -1);
         *
         *  But - this API doesn't support that.
         *
         *  This leaves us with several weak alternatives:
         *      o   save the telldir, and seek to the start of the dir, and repeatedly seek until
         *          we get the same telldir, and then return the previous stored value.
         *
         *      o   substract 1 from the value of telldir()
         *
         *      o   Before each readdir() - do a telldir() to capture its value.
         *
         *      o   When cloning - compute offset in file#s by rewinding and seeking til we find the same value by
         *          name or some such, and then use that same process to re-seek in the cloned dirEnt.
         *
         *  The 'subtract 1' approach, though cheap and simple, appears not supported by the telldir documentation,
         *  and looking at values in the current linux/gcc, seems unpromising.
         *
         *  Between the 'telldir' before each read, and re-seek approaches, the former adds overhead even when not cloning
         *  iterators, and the later only when you use the feature. Moreover, the seek-to-start and keep looking
         *  for telldir() approach probably works out efficeintly, as the most likely time to Clone () an iterator is
         *  when it is 'at start' anyhow (DirectoryIterable). So we'll start with that...
         *          -- LGP 2014-07-10
         *
         *  This above didn't work on macos, so use the (actually simpler) approach of just opening the dir again, and scanning til we
         *  find the same inode. Not perfect (in case that is deleted) - but not sure there is a guaranteed way then.
         */
        return RepSmartPtr (MakeSmartPtr<Rep_> (fDirName_, fCur_ == nullptr ? optional<ino_t>{} : fCur_->d_ino, fIteratorReturnType_));
#elif qPlatform_Windows
        return RepSmartPtr (MakeSmartPtr<Rep_> (fDirName_, fHandle_ == INVALID_HANDLE_VALUE ? optional<String>{} : String::FromSDKString (fFindFileData_.cFileName), fIteratorReturnType_));
#endif
    }
    virtual IteratorOwnerID GetOwner () const override
    {
        /*
         *  This return value allows any two DiscreteRange iterators (of the same type) to be compared.
         */
        return typeid (*this).name ();
    }

private:
    static String mkReportPrefix_ (const String& dirName, IteratorReturnType iteratorReturns)
    {
        switch (iteratorReturns) {
            case IteratorReturnType::eFilenameOnly:
                return String{};
            case IteratorReturnType::eDirPlusFilename:
                return AssureDirectoryPathSlashTerminated (dirName);
            case IteratorReturnType::eFullPathName:
                return AssureDirectoryPathSlashTerminated (IO::FileSystem::Default ().GetFullPathName (dirName));
            default:
                AssertNotReached ();
                return String{};
        }
    }
};

/*
 ********************************************************************************
 ******************** IO::FileSystem::DirectoryIterator *************************
 ********************************************************************************
 */
DirectoryIterator::DirectoryIterator (const String& directoryName, IteratorReturnType iteratorReturns)
    : Iterator<String> (MakeSmartPtr<Rep_> (directoryName, iteratorReturns))
{
}
