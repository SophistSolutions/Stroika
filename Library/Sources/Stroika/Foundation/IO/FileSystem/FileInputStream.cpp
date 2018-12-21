/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#if qPlatform_Windows
#include <io.h>
#elif qPlatform_POSIX
#include <poll.h>
#include <unistd.h>
#endif

#include "../../Debug/AssertExternallySynchronizedLock.h"
#include "../../Debug/Trace.h"
#include "../../Execution/Common.h"
#include "../../Execution/ErrNoException.h"
#include "../../Execution/Exceptions.h"
#if qPlatform_POSIX
#include "../../Execution/ErrNoException.h"
#elif qPlatform_Windows
#include "../../Execution/Platform/Windows/Exception.h"
#endif
#include "../../IO/FileAccessException.h"
#include "../../Streams/BufferedInputStream.h"

#include "FileInputStream.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;

using Streams::InputStream;
using Streams::SeekOffsetType;

#if qPlatform_Windows
using Execution::Platform::Windows::ThrowIfFalseGetLastError;
#endif

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 **************************** FileSystem::FileInputStream ***********************
 ********************************************************************************
 */
class FileInputStream::Rep_ : public InputStream<byte>::_IRep, private Debug::AssertExternallySynchronizedLock {
public:
    Rep_ ()            = delete;
    Rep_ (const Rep_&) = delete;
    Rep_ (const String& fileName, SeekableFlag seekable)
        : fFD_ (-1)
        , fSeekable_ (seekable)
        , fFileName_ (fileName)
    {
        try {
#if qPlatform_Windows
            errno_t e = ::_wsopen_s (&fFD_, fileName.c_str (), (O_RDONLY | O_BINARY), _SH_DENYNO, 0);
            if (e != 0) {
                Execution::errno_ErrorException::Throw (e);
            }
            ThrowIfFalseGetLastError (fFD_ != -1);
#else
            Execution::ThrowErrNoIfNegative (fFD_ = ::open (fileName.AsNarrowSDKString ().c_str (), O_RDONLY));
#endif
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"opened fd: %d", fFD_);
#endif
        }
        Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER (fileName, FileAccessMode::eRead);
    }
    Rep_ (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekable)
        : fFD_ (fd)
        , fSeekable_ (seekable)
        , fAdoptFDPolicy_ (adoptFDPolicy)
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"attached fd: %d", fFD_);
#endif
    }
    ~Rep_ ()
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx (L"FileInputStream::Rep_::~Rep_");
        if (fAdoptFDPolicy_ == AdoptFDPolicy::eCloseOnDestruction and IsOpenRead ()) {
            DbgTrace (L"closing %d", fFD_);
        }
#endif
        if (fAdoptFDPolicy_ == AdoptFDPolicy::eCloseOnDestruction and IsOpenRead ()) {
#if qPlatform_Windows
            ::_close (fFD_);
#else
            ::close (fFD_);
#endif
        }
    }
    nonvirtual Rep_& operator= (const Rep_&) = delete;

    virtual bool IsSeekable () const override
    {
        return fSeekable_ == eSeekable;
    }
    virtual void CloseRead () override
    {
        Require (IsOpenRead ());
        if (fAdoptFDPolicy_ == AdoptFDPolicy::eCloseOnDestruction) {
#if qPlatform_Windows
            ::_close (fFD_);
#else
            ::close (fFD_);
#endif
        }
        fFD_ = -1;
    }
    virtual bool IsOpenRead () const override
    {
        return fFD_ >= 0;
    }
    virtual size_t Read (byte* intoStart, byte* intoEnd) override
    {
        RequireNotNull (intoStart);
        RequireNotNull (intoEnd);
        Require (intoStart < intoEnd);
        size_t nRequested = intoEnd - intoStart;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx (L"FileInputStream::Rep_::Read", L"nRequested: %llu", static_cast<unsigned long long> (nRequested));
#endif
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        try {
#if qPlatform_Windows
            return static_cast<size_t> (Execution::ThrowErrNoIfNegative (::_read (fFD_, intoStart, Math::PinToMaxForType<unsigned int> (nRequested))));
#else
            return static_cast<size_t> (Execution::ThrowErrNoIfNegative (::read (fFD_, intoStart, nRequested)));
#endif
        }
        Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER (fFileName_, FileAccessMode::eRead);
    }
    virtual optional<size_t> ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) override
    {
        Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
#if qPlatform_Windows
        /*
         *  For now, assume all FILE reads are already non-blocking. Not sure about this.
         *
         *  COULD use intptr_t _get_osfhandle (int fd);
         *  to use Windows APIs, but those all seem to require the file to be opened a special way to do async reads.
         *
         *  Tried:
         *      int oldFileFlags = ::fcntl (fFD_, F_GETFL, 0);
         *      if (fcntl (fFD_, F_SETFL, oldFileFlags | O_NONBLOCK))
         *          ;
         *      [[maybe_unused]] auto&& cleanup = Execution::Finally ([this]() noexcept {
         *          fcntl (fFD_, F_SETFL, oldFileFlags);
         *      });
         *
         *  but windows doesn't appear to support fcntl()
         */
        return Read (intoStart, intoEnd);
#elif qPlatform_POSIX
        pollfd pollData{fFD_, POLLIN, 0};
        int pollResult = Execution::ThrowErrNoIfNegative (Execution::Handle_ErrNoResultInterruption ([&]() { return ::poll (&pollData, 1, 0); }));
        Assert (pollResult >= 0);
        if (pollResult == 0) {
            return {}; // if no data available, return {}
        }
        else {
            // we don't know how much is available, but at least one byte. If not actually reading, just return 1
            if (intoStart == nullptr) {
                return 1;
            }
            else {
                // if there is data available, read as much as you can...
                return Read (intoStart, intoEnd);
            }
        }
#endif
        return {};
    }
    virtual Streams::SeekOffsetType GetReadOffset () const override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
#if qPlatform_Windows
        return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::_lseeki64 (fFD_, 0, SEEK_CUR)));
#elif qPlatform_Linux
        return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::lseek64 (fFD_, 0, SEEK_CUR)));
#else
        return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::lseek (fFD_, 0, SEEK_CUR)));
#endif
    }
    virtual Streams::SeekOffsetType SeekRead (Streams::Whence whence, Streams::SignedSeekOffsetType offset) override
    {
        using namespace Streams;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx (L"FileInputStream::Rep_::SeekRead", L"whence: %d, offset: %lld", whence, static_cast<long long> (offset));
#endif
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        switch (whence) {
            case Whence::eFromStart: {
                if (offset < 0)
                    [[UNLIKELY_ATTR]]
                    {
                        Execution::Throw (range_error ("seek"));
                    }
#if qPlatform_Windows
                return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::_lseeki64 (fFD_, offset, SEEK_SET)));
#elif qPlatform_Linux
                return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::lseek64 (fFD_, offset, SEEK_SET)));
#else
                return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::lseek (fFD_, offset, SEEK_SET)));
#endif
            } break;
            case Whence::eFromCurrent: {
#if qPlatform_Windows
                return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::_lseeki64 (fFD_, offset, SEEK_CUR)));
#elif qPlatform_Linux
                return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::lseek64 (fFD_, offset, SEEK_CUR)));
#else
                return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::lseek (fFD_, offset, SEEK_CUR)));
#endif
            } break;
            case Whence::eFromEnd: {
#if qPlatform_Windows
                return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::_lseeki64 (fFD_, offset, SEEK_END)));
#elif qPlatform_Linux
                return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::lseek64 (fFD_, offset, SEEK_END)));
#else
                return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::lseek (fFD_, offset, SEEK_END)));
#endif
            } break;
        }
        RequireNotReached ();
        return 0;
    }

private:
    int              fFD_;
    SeekableFlag     fSeekable_;
    AdoptFDPolicy    fAdoptFDPolicy_{AdoptFDPolicy::eCloseOnDestruction};
    optional<String> fFileName_;
};

auto FileInputStream::New (const String& fileName, SeekableFlag seekable) -> Ptr
{
    return _mkPtr (make_shared<Rep_> (fileName, seekable));
}

auto FileInputStream::New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekable) -> Ptr
{
    return _mkPtr (make_shared<Rep_> (fd, adoptFDPolicy, seekable));
}

auto FileInputStream::New (Execution::InternallySynchronized internallySynchronized, const String& fileName, SeekableFlag seekable) -> Ptr
{
    switch (internallySynchronized) {
        case Execution::eInternallySynchronized:
            return InternalSyncRep_::New (fileName, seekable);
        case Execution::eNotKnownInternallySynchronized:
            return New (fileName, seekable);
        default:
            RequireNotReached ();
            return New (fileName, seekable);
    }
}

auto FileInputStream::New (Execution::InternallySynchronized internallySynchronized, FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekable) -> Ptr
{
    switch (internallySynchronized) {
        case Execution::eInternallySynchronized:
            return InternalSyncRep_::New (fd, adoptFDPolicy, seekable);
        case Execution::eNotKnownInternallySynchronized:
            return New (fd, adoptFDPolicy, seekable);
        default:
            RequireNotReached ();
            return New (fd, adoptFDPolicy, seekable);
    }
}

InputStream<byte>::Ptr FileInputStream::New (const String& fileName, SeekableFlag seekable, BufferFlag bufferFlag)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"FileInputStream::New", L"fileName: %s, seekable: %d, bufferFlag: %d", fileName.c_str (), seekable, bufferFlag);
#endif
    InputStream<byte>::Ptr in = FileInputStream::New (fileName, seekable);
    switch (bufferFlag) {
        case eBuffered:
            return Streams::BufferedInputStream<byte>::New (in);
        case eUnbuffered:
            return in;
        default:
            RequireNotReached ();
            return in;
    }
}

InputStream<byte>::Ptr FileInputStream::New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekable, BufferFlag bufferFlag)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"FileInputStream::New", L"fd: %d, seekable: %d, bufferFlag: %d", fd, seekable, bufferFlag);
#endif
    InputStream<byte>::Ptr in = FileInputStream::New (fd, adoptFDPolicy, seekable);
    switch (bufferFlag) {
        case eBuffered:
            return Streams::BufferedInputStream<byte>::New (in);
        case eUnbuffered:
            return in;
        default:
            AssertNotReached ();
            return in;
    }
}

/*
 ********************************************************************************
 ******************** IO::FileSystem::FileInputStream::Ptr **********************
 ********************************************************************************
 */
IO::FileSystem::FileInputStream::Ptr::Ptr (const shared_ptr<Rep_>& from)
    : inherited (from)
{
}