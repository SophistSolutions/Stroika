/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <windows.h>
#elif   qPlatform_POSIX
#include    <dirent.h>
#endif

#include    "../../Characters/CString/Utilities.h"
#include    "../../Debug/Trace.h"
#include    "../../Execution/ErrNoException.h"
#if     qPlatform_Windows
#include    "../../Execution/Platform/Windows/Exception.h"
#endif
#include    "../../IO/FileAccessException.h"

#include    "DirectoryIterator.h"





// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1





using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::FileSystem;
using   namespace   Stroika::Foundation::Traversal;

#if     qPlatform_Windows
using   Execution::Platform::Windows::ThrowIfFalseGetLastError;
#endif
using   Execution::ThrowIfError_errno_t;



class   DirectoryIterator::Rep_ : public Iterator<String>::IRep {
private:
#if     qPlatform_POSIX
    DIR*            fDirIt_             { nullptr };
    dirent          fDirEntBuf_;        // intentionally uninitialized (done by readdir)
    dirent*         fCur_               { nullptr };
#elif   qPlatform_Windows
    String          fDirName_;
    HANDLE          fHandle_            { INVALID_HANDLE_VALUE };
    WIN32_FIND_DATA fFindFileData_;
    int             fSeekOffset_        { 0 };
#endif

public:
    Rep_ (const String& dir)
#if     qPlatform_POSIX
        : fDirIt_ { ::opendir (dir.AsSDKString ().c_str ()) }
#elif   qPlatform_Windows
        :
        fDirName_ (dir)
#endif
    {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"Entering DirectoryIterator::Rep_::CTOR('%s')", dir.c_str ());
#endif
        try {
#if     qPlatform_POSIX
            if (fDirIt_ == nullptr) {
                Execution::ThrowIfError_errno_t ();
            }
            else {
                ThrowIfError_errno_t (::readdir_r (fDirIt_, &fDirEntBuf_, &fCur_));
                Assert (fCur_ == nullptr or fCur_ == &fDirEntBuf_);
            }
            if (fCur_ != nullptr and fCur_->d_name[0] == '.' and (CString::Equals (fCur_->d_name, SDKSTR (".")) or CString::Equals (fCur_->d_name, SDKSTR ("..")))) {
                Memory::Optional<String>    tmphack;
                More (&tmphack, true);
            }
#elif   qPlatform_Windows
            (void)::memset (&fFindFileData_, 0, sizeof (fFindFileData_));
            fHandle_ = ::FindFirstFile ((dir + L"\\*").AsSDKString ().c_str (), &fFindFileData_);
            if (fHandle_ != INVALID_HANDLE_VALUE and fFindFileData_.cFileName[0] == '.' and (CString::Equals (fFindFileData_.cFileName, SDKSTR (".")) or CString::Equals (fFindFileData_.cFileName, SDKSTR ("..")))) {
                Memory::Optional<String>    tmphack;
                More (&tmphack, true);
            }
#endif
        }
        Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAMESONLY_HELPER(dir);
    }
#if     qPlatform_POSIX
    Rep_ (DIR* dirObj)
        : fDirIt_ { dirObj }
    {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"Entering DirectoryIterator::Rep_::CTOR(dirObj=%x)", int(dirObj));
#endif
        if (fDirIt_ != nullptr) {
            ThrowIfError_errno_t (::readdir_r (fDirIt_, &fDirEntBuf_, &fCur_));
            Assert (fCur_ == nullptr or fCur_ == &fDirEntBuf_);
            if (fCur_ != nullptr and fCur_->d_name[0] == '.' and (CString::Equals (fCur_->d_name, SDKSTR (".")) or CString::Equals (fCur_->d_name, SDKSTR ("..")))) {
                Memory::Optional<String>    tmphack;
                More (&tmphack, true);
            }
        }
    }
#elif   qPlatform_Windows
    Rep_ (const String& dir, int seekPos)
        : fDirName_ (dir)
    {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"Entering DirectoryIterator::Rep_::CTOR('%s',seekPos=%d)", dir.c_str (), seekPos);
#endif
        (void)::memset (&fFindFileData_, 0, sizeof (fFindFileData_));
        fHandle_ = ::FindFirstFile ((dir + L"\\*").AsSDKString ().c_str (), &fFindFileData_);
        for (int i = 0; i < seekPos; ++i) {
            if (::FindNextFile (fHandle_, &fFindFileData_) == 0) {
                ::FindClose (fHandle_);
                fHandle_ = INVALID_HANDLE_VALUE;
            }
            else {
                fSeekOffset_++;
            }
        }
        if (fHandle_ != INVALID_HANDLE_VALUE and fFindFileData_.cFileName[0] == '.' and (CString::Equals (fFindFileData_.cFileName, SDKSTR (".")) or CString::Equals (fFindFileData_.cFileName, SDKSTR (".."))) ) {
            Memory::Optional<String>    tmphack;
            More (&tmphack, true);
        }
    }
#endif
    virtual ~Rep_ ()
    {
#if     qPlatform_POSIX
        if (fDirIt_ != nullptr) {
            ::closedir (fDirIt_);
        }
#elif   qPlatform_Windows
        if (fHandle_ != INVALID_HANDLE_VALUE) {
            ::FindClose (fHandle_);
        }
#endif
    }
    virtual void    More (Memory::Optional<String>* result, bool advance) override
    {
        RequireNotNull (result);
        result->clear ();
#if     qPlatform_POSIX
        if (advance) {
Again:
            RequireNotNull (fCur_);
            RequireNotNull (fDirIt_);
            int e = ::readdir_r (fDirIt_, &fDirEntBuf_, &fCur_);
            if (e == EBADF) {
                Assert (fCur_ == nullptr);
            }
            else {
                ThrowIfError_errno_t (e);
            }
            Assert (fCur_ == nullptr or fCur_ == &fDirEntBuf_);
            if (fCur_ != nullptr and fCur_->d_name[0] == '.' and (CString::Equals (fCur_->d_name, SDKSTR (".")) or CString::Equals (fCur_->d_name, SDKSTR ("..")))) {
                goto Again;
            }
        }
        if (fCur_ != nullptr) {
            *result = String::FromSDKString (fCur_->d_name);
        }
#elif   qPlatform_Windows
        if (advance) {
Again:
            Require (fHandle_ != INVALID_HANDLE_VALUE);
            memset (&fFindFileData_, 0, sizeof (fFindFileData_));
            if (::FindNextFile (fHandle_, &fFindFileData_) == 0) {
                ::FindClose (fHandle_);
                fHandle_ = INVALID_HANDLE_VALUE;
            }
            if (fHandle_ != INVALID_HANDLE_VALUE and fFindFileData_.cFileName[0] == '.' and (CString::Equals (fFindFileData_.cFileName, SDKSTR (".")) or CString::Equals (fFindFileData_.cFileName, SDKSTR (".."))) ) {
                goto Again;
            }
        }
        if (fHandle_ != INVALID_HANDLE_VALUE) {
            *result = String::FromSDKString (fFindFileData_.cFileName);
        }
#endif
    }
    virtual bool    Equals (const Iterator<String>::IRep* rhs) const override
    {
        RequireNotNull (rhs);
        RequireMember (rhs, Rep_);
        const Rep_&  rrhs = *dynamic_cast<const Rep_*> (rhs);
#if     qPlatform_POSIX
        return fDirIt_ == rrhs.fDirIt_;
#elif   qPlatform_Windows
        return fHandle_ == rrhs.fHandle_;
#endif
    }
    virtual SharedIRepPtr    Clone () const override
    {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"Entering DirectoryIterator::Rep_::Clone");
#endif
#if     qPlatform_POSIX
        if (fDirIt_ == nullptr) {
            return SharedIRepPtr (MakeSharedPtr<Rep_> (nullptr));
        }
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
         *      o   Before each 'readdir() - do a telldir() to capture its value.
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
         */
        // Note - NOT 100% sure its OK to look for identical value telldir in another dir...
        DIR*        dirObj          =   ::fdopendir (::dirfd (fDirIt_));
        if (dirObj == nullptr) {
            Execution::ThrowIfError_errno_t ();
        }

        dirent      dirEntBuf;      // intentionally uninitialized (done by readdir)
        if (fCur_ == nullptr) {
            // then we're past end end, the cloned fdopen dir one SB too!
            dirent* d = nullptr;
            dirent  db;// no init on purpose
            (void)::readdir_r (fDirIt_, &db, &d);
            Assert (d == nullptr);
        }
        else {
            ino_t   aBridgeTooFar   =   fCur_->d_ino;
            ::rewinddir (dirObj);
            long useOffset = ::telldir (dirObj);
            for (;;) {
                //dirent* tmp = ::readdir (dirObj);
                dirent* tmp = nullptr;
                // ThrowIfError_errno_t (::readdir_r (dirObj, &dirEntBuf, &tmp));        // @todo UNSURE if we want to check error here?
                ::readdir_r (dirObj, &dirEntBuf, &tmp);
                Assert (tmp == nullptr or tmp == &dirEntBuf);
                if (tmp == nullptr) {
                    // somehow the file went away, so no idea where to start, and the end is as reasonable as anywhere else???
                    useOffset = ::telldir (dirObj);
                    DbgTrace (L"WARN: possible bug? - ususual, and not handled well, but this can happen if the file disappears while we're cloning");
                    break;
                }
                else if (tmp->d_ino == aBridgeTooFar) {
                    // then we are now pointing at the right elt, and want to seek to the PRECEEDING one so when it does a readdir it gets that item
                    // so DONT update useOffset - just break!
                    break;
                }
                else {
                    // update to this reflects the last offset before we find d_ino
                    useOffset = ::telldir (dirObj);
                }
            }
            ::seekdir (dirObj, useOffset);
        }
        return SharedIRepPtr (MakeSharedPtr<Rep_> (dirObj));
#elif   qPlatform_Windows
        return SharedIRepPtr (MakeSharedPtr<Rep_> (fDirName_, fSeekOffset_));
#endif
    }
    virtual IteratorOwnerID GetOwner () const override
    {
        /*
         *  This return value allows any two DiscreteRange iterators (of the same type) to be compared.
         */
        return typeid (*this).name ();
    }
};








/*
 ********************************************************************************
 ******************** IO::FileSystem::DirectoryIterator *************************
 ********************************************************************************
 */
DirectoryIterator::DirectoryIterator (const String& directoryName)
    : Iterator<String> (MakeSharedPtr<Rep_> (directoryName))
{
}
