/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <windows.h>
#elif   qPlatform_POSIX
#include    <dirent.h>
#endif

#include    "../../Execution/ErrNoException.h"
#if     qPlatform_Windows
#include    "../../Execution/Platform/Windows/Exception.h"
#endif

#include    "../../Traversal/IterableFromIterator.h"

#include    "DirectoryIterator.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::FileSystem;
using   namespace   Stroika::Foundation::Traversal;

#if     qPlatform_Windows
using   Execution::Platform::Windows::ThrowIfFalseGetLastError;
#endif




class   DirectoryIterator::Rep_ : public Iterator<String>::IRep {
private:
#if     qPlatform_Windows
    String          fDirName_;
    HANDLE          fHandle_ { INVALID_HANDLE_VALUE };
    WIN32_FIND_DATA fFindFileData_;
    int             fSeekOffset_ = 0;
#elif   qPlatform_POSIX
    DIR*       fDirIt_ = nullptr;
    dirent*    fCur_   =   nullptr;
#endif

public:
    Rep_ (const String& dir)
#if     qPlatform_Windows
        : fDirName_ (dir)
#elif   qPlatform_POSIX
        : fDirIt_ { ::opendir (dir.AsSDKString ().c_str ()) }
#endif
    {
#if     qPlatform_Windows
        memset (&fFindFileData_, 0, sizeof (fFindFileData_));
        fHandle_ = ::FindFirstFile ((dir + L"\\*").AsSDKString ().c_str (), &fFindFileData_);
#elif   qPlatform_POSIX
        if (fDirIt_ == nullptr)
        {
            ThrowIfError_errno_t ();
        }
#endif
    }
#if     qPlatform_Windows
    Rep_ (const String& dir, int seekPos)
        : fDirName_ (dir)
    {
        memset (&fFindFileData_, 0, sizeof (fFindFileData_));
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
    }
#elif   qPlatform_POSIX
    Rep_ (DIR* dirObj, long seekPos)
        : fDirIt_ { dirObj } {
        if (fDirIt_ != nullptr)
        {
            seekdir(fDirIt_, seekPos);
            fCur_ = ::readdir (fDirIt_);
        }
    }
#endif
    virtual ~Rep_ ()
    {
#if         qPlatform_Windows
        if (fHandle_ != INVALID_HANDLE_VALUE) {
            ::FindClose (fHandle_);
        }
#elif   qPlatform_POSIX
        if (fDirIt_ != nullptr) {
            ::closedir (fDirIt_);
        }
#endif
    }
    virtual void    More (Memory::Optional<String>* result, bool advance) override
    {
        RequireNotNull (result);
        result->clear ();
#if     qPlatform_Windows
        if (advance) {
            Require (fHandle_ != INVALID_HANDLE_VALUE);
            memset (&fFindFileData_, 0, sizeof (fFindFileData_));
            if (::FindNextFile (fHandle_, &fFindFileData_) == 0) {
                ::FindClose (fHandle_);
                fHandle_ = INVALID_HANDLE_VALUE;
            }
        }
        if (fHandle_ != INVALID_HANDLE_VALUE) {
            *result = String::FromSDKString (fFindFileData_.cFileName);
        }
#elif   qPlatform_POSIX
        if (advance) {
            fCur_ = ::readdir (fDirIt_);
        }
        if (fCur_ != nullptr) {
            *result = fCur_->name;
        }
#endif
    }
    virtual bool    Equals (const Iterator<String>::IRep* rhs) const override
    {
        RequireNotNull (rhs);
        RequireMember (rhs, Rep_);
        const Rep_&  rrhs = *dynamic_cast<const Rep_*> (rhs);
#if     qPlatform_Windows
        return fHandle_ == rrhs.fHandle_;
#elif   qPlatform_POSIX
        return fDirIt_ == rrhs.fDirIt_;
#endif
    }
    virtual SharedIRepPtr    Clone () const override
    {
#if     qPlatform_Windows
        return SharedIRepPtr (new Rep_ (fDirName_, fSeekOffset_));
#elif   qPlatform_POSIX
        if (fDirIt_ == nullptr) {
            return SharedIRepPtr (new Rep_ (nullptr, 0)));
        }
        return SharedIRepPtr (new Rep_ (fdopendir(::dirfd (fDirIt_)), ::telldir (fDirIt_))));
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
    : Iterator<String> (SharedIRepPtr (new Rep_ (directoryName)))
{
}



