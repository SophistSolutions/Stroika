/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#if qPlatform_Windows
#include <io.h>
#elif qPlatform_POSIX
#include <unistd.h>
#endif

#include "../../Characters/Format.h"
#include "../../Characters/ToString.h"
#include "../../Debug/AssertExternallySynchronizedLock.h"
#include "../../Execution/Activity.h"
#include "../../Execution/Common.h"
#include "../../Execution/Exceptions.h"
#include "../../Execution/Throw.h"
#if qPlatform_Windows
#include "../../Execution/Platform/Windows/Exception.h"
#endif

#include "FileOutputStream.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;

using Execution::ThrowPOSIXErrNo;
using Execution::ThrowPOSIXErrNoIfNegative;
using Execution::ThrowSystemErrNo;

#if qPlatform_Windows
using Execution::Platform::Windows::ThrowIfZeroGetLastError;
#endif

/*
 ********************************************************************************
 ************************* FileSystem::FileOutputStream *************************
 ********************************************************************************
 */
class FileOutputStream::Rep_ : public Streams::OutputStream<byte>::_IRep, private Debug::AssertExternallySynchronizedLock {
public:
    Rep_ ()            = delete;
    Rep_ (const Rep_&) = delete;
    Rep_ (const String& fileName, AppendFlag appendFlag, FlushFlag flushFlag)
        : fFD_ (-1)
        , fFlushFlag (flushFlag)
        , fFileName_ (fileName)
    {
        auto            activity = LazyEvalActivity ([&] () -> String { return Characters::Format (L"opening %s for write access", Characters::ToString (fFileName_).c_str ()); });
        DeclareActivity currentActivity{&activity};
#if qPlatform_Windows
        int     appendFlag2Or = appendFlag == eStartFromStart ? _O_TRUNC : _O_APPEND;
        errno_t e             = ::_wsopen_s (&fFD_, fileName.c_str (), _O_WRONLY | _O_CREAT | _O_BINARY | appendFlag2Or, _SH_DENYNO, _S_IREAD | _S_IWRITE);
        if (e != 0) {
            ThrowPOSIXErrNo (e);
        }
        if (fFD_ == -1) {
            ThrowSystemErrNo ();
        }
#else
        int          appendFlag2Or = appendFlag == eStartFromStart ? O_TRUNC : O_APPEND;
        const mode_t kCreateMode_  = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
        ThrowPOSIXErrNoIfNegative (fFD_ = ::open (fileName.AsNarrowSDKString ().c_str (), O_WRONLY | O_CREAT | appendFlag2Or, kCreateMode_));
#endif
    }
    Rep_ (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekableFlag, FlushFlag flushFlag)
        : fFD_ (fd)
        , fFlushFlag (flushFlag)
        , fAdoptFDPolicy_ (adoptFDPolicy)
        , fSeekable_ (seekableFlag == SeekableFlag::eSeekable)
    {
    }
    ~Rep_ ()
    {
        IgnoreExceptionsForCall (Flush ()); // for fFlushFlag == FlushFlag::eToDisk
        if (fAdoptFDPolicy_ == AdoptFDPolicy::eCloseOnDestruction and IsOpenWrite ()) {
#if qPlatform_Windows
            ::_close (fFD_);
#else
            ::close (fFD_);
#endif
        }
    }
    nonvirtual Rep_& operator= (const Rep_&) = delete;
    virtual bool     IsSeekable () const override
    {
        return fSeekable_;
    }
    virtual void CloseWrite () override
    {
        Require (IsOpenWrite ());
        if (fAdoptFDPolicy_ == AdoptFDPolicy::eCloseOnDestruction) {
#if qPlatform_Windows
            ::_close (fFD_);
#else
            ::close (fFD_);
#endif
        }
        fFD_ = -1;
    }
    virtual bool IsOpenWrite () const override
    {
        return fFD_ >= 0;
    }
    virtual void Write (const byte* start, const byte* end) override
    {
        Require (start != nullptr or start == end);
        Require (end != nullptr or start == end);

        if (start != end) {
            lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
            auto                                               activity = LazyEvalActivity ([&] () -> String { return Characters::Format (L"writing to %s", Characters::ToString (fFileName_).c_str ()); });
            DeclareActivity                                    currentActivity{&activity};
            const byte*                                        i = start;
            while (i < end) {
#if qPlatform_Windows
                int n = ThrowPOSIXErrNoIfNegative (_write (fFD_, i, Math::PinToMaxForType<unsigned int> (end - i)));
#else
                int n = ThrowPOSIXErrNoIfNegative (write (fFD_, i, end - i));
#endif
                Assert (n <= (end - i));
                i += n;
            }
        }
    }
    virtual void Flush () override
    {
        // normally nothing todo - write 'writes thru' (except if fFlushFlag)
        if (fFlushFlag == FlushFlag::eToDisk) {
            lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
            auto                                               activity = LazyEvalActivity ([&] () -> String { return Characters::Format (L"flushing data to %s", Characters::ToString (fFileName_).c_str ()); });
            DeclareActivity                                    currentActivity{&activity};
#if qPlatform_Windows
            ThrowIfZeroGetLastError (::FlushFileBuffers (reinterpret_cast<HANDLE> (::_get_osfhandle (fFD_))));
#elif qPlatform_POSIX
            ThrowPOSIXErrNoIfNegative (::fsync (fFD_));
#else
            AssertNotImplemented ();
#endif
        }
    }
    virtual Streams::SeekOffsetType GetWriteOffset () const override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
#if qPlatform_Windows
        return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::_lseeki64 (fFD_, 0, SEEK_CUR)));
#elif qPlatform_Linux
        return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek64 (fFD_, 0, SEEK_CUR)));
#else
        return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek (fFD_, 0, SEEK_CUR)));
#endif
    }
    virtual Streams::SeekOffsetType SeekWrite (Streams::Whence whence, Streams::SignedSeekOffsetType offset) override
    {
        Require (fSeekable_);
        using namespace Streams;
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        switch (whence) {
            case Whence::eFromStart: {
                if (offset < 0)
                    [[UNLIKELY_ATTR]]
                    {
                        Execution::Throw (range_error ("seek"));
                    }
#if qPlatform_Windows
                return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::_lseeki64 (fFD_, offset, SEEK_SET)));
#elif qPlatform_Linux
                return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek64 (fFD_, offset, SEEK_SET)));
#else
                return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek (fFD_, offset, SEEK_SET)));
#endif
            } break;
            case Whence::eFromCurrent: {
#if qPlatform_Windows
                return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::_lseeki64 (fFD_, offset, SEEK_CUR)));
#elif qPlatform_Linux
                return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek64 (fFD_, offset, SEEK_CUR)));
#else
                return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek (fFD_, offset, SEEK_CUR)));
#endif
            } break;
            case Whence::eFromEnd: {
#if qPlatform_Windows
                return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::_lseeki64 (fFD_, offset, SEEK_END)));
#elif qPlatform_Linux
                return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek64 (fFD_, offset, SEEK_END)));
#else
                return static_cast<Streams::SeekOffsetType> (ThrowPOSIXErrNoIfNegative (::lseek (fFD_, offset, SEEK_END)));
#endif
            } break;
        }
        RequireNotReached ();
        return 0;
    }

private:
    int              fFD_;
    FlushFlag        fFlushFlag;
    AdoptFDPolicy    fAdoptFDPolicy_{AdoptFDPolicy::eCloseOnDestruction};
    bool             fSeekable_{true};
    optional<String> fFileName_;
};

auto FileOutputStream::New (const String& fileName, FlushFlag flushFlag) -> Ptr
{
    return make_shared<Rep_> (fileName, AppendFlag::eDEFAULT, flushFlag);
}

auto FileOutputStream::New (const String& fileName, AppendFlag appendFlag, FlushFlag flushFlag) -> Ptr
{
    return make_shared<Rep_> (fileName, appendFlag, flushFlag);
}

auto FileOutputStream::New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekableFlag, FlushFlag flushFlag) -> Ptr
{
    return make_shared<Rep_> (fd, adoptFDPolicy, seekableFlag, flushFlag);
}

auto FileOutputStream::New (Execution::InternallySynchronized internallySynchronized, const String& fileName, FlushFlag flushFlag) -> Ptr
{
    switch (internallySynchronized) {
        case Execution::eInternallySynchronized:
            return InternalSyncRep_::New (fileName, AppendFlag::eDEFAULT, flushFlag);
        case Execution::eNotKnownInternallySynchronized:
            return New (fileName, AppendFlag::eDEFAULT, flushFlag);
        default:
            RequireNotReached ();
            return New (fileName, AppendFlag::eDEFAULT, flushFlag);
    }
}

auto FileOutputStream::New (Execution::InternallySynchronized internallySynchronized, const String& fileName, AppendFlag appendFlag, FlushFlag flushFlag) -> Ptr
{
    switch (internallySynchronized) {
        case Execution::eInternallySynchronized:
            return InternalSyncRep_::New (fileName, appendFlag, flushFlag);
        case Execution::eNotKnownInternallySynchronized:
            return New (fileName, appendFlag, flushFlag);
        default:
            RequireNotReached ();
            return New (fileName, appendFlag, flushFlag);
    }
}

auto FileOutputStream::New (Execution::InternallySynchronized internallySynchronized, FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekableFlag, FlushFlag flushFlag) -> Ptr
{
    switch (internallySynchronized) {
        case Execution::eInternallySynchronized:
            return InternalSyncRep_::New (fd, adoptFDPolicy, seekableFlag, flushFlag);
        case Execution::eNotKnownInternallySynchronized:
            return New (fd, adoptFDPolicy, seekableFlag, flushFlag);
        default:
            RequireNotReached ();
            return New (fd, adoptFDPolicy, seekableFlag, flushFlag);
    }
}

/*
 ********************************************************************************
 ******************** IO::FileSystem::FileOutputStream::Ptr *********************
 ********************************************************************************
 */
IO::FileSystem::FileOutputStream::Ptr::Ptr (const shared_ptr<Rep_>& from)
    : inherited (from)
{
}
