/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    <sys/types.h>
#include    <sys/stat.h>
#include    <fcntl.h>

#if     qPlatform_Windows
#include    <io.h>
#elif   qPlatform_POSIX
#include    <unistd.h>
#endif

#include    "../../Debug/AssertExternallySynchronizedLock.h"
#include    "../../Debug/Trace.h"
#include    "../../Execution/Common.h"
#include    "../../Execution/ErrNoException.h"
#include    "../../Execution/Exceptions.h"
#if     qPlatform_Windows
#include    "../../Execution/Platform/Windows/Exception.h"
#endif
#include    "../../IO/FileAccessException.h"
#include    "../../Streams/BufferedInputStream.h"

#include    "FileInputStream.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::FileSystem;

using   Execution::make_unique_lock;
using   Streams::InputStream;
using   Streams::SeekOffsetType;



#if     qPlatform_Windows
using   Execution::Platform::Windows::ThrowIfFalseGetLastError;
#endif









// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1






/*
 ********************************************************************************
 **************************** FileSystem::FileInputStream ***********************
 ********************************************************************************
 */
class   FileInputStream::Rep_ : public InputStream<Byte>::_IRep, private Debug::AssertExternallySynchronizedLock {
public:
    Rep_ () = delete;
    Rep_ (const Rep_&) = delete;
    Rep_ (const String& fileName, SeekableFlag seekable)
        : fFD_ (-1)
        , fSeekable_ (seekable)
    {
        try {
#if     qPlatform_Windows
            errno_t e = ::_wsopen_s (&fFD_, fileName.c_str (), (O_RDONLY | O_BINARY), _SH_DENYNO, 0);
            if (e != 0) {
                Execution::errno_ErrorException::Throw (e);
            }
            ThrowIfFalseGetLastError (fFD_ != -1);
#else
            Execution::ThrowErrNoIfNegative (fFD_ = ::open (fileName.AsNarrowSDKString ().c_str (), O_RDONLY));
#endif
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"opened fd: %d", fFD_);
#endif
        }
        Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER(fileName, FileAccessMode::eRead);
    }
    Rep_ (FileDescriptorType fd, SeekableFlag seekable)
        : fFD_ (fd)
        , fSeekable_ (seekable)
    {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"attached fd: %d", fFD_);
#endif
    }
    ~Rep_ ()
    {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx (L"FileInputStream::Rep_::~Rep_");
        DbgTrace (L"closing %d", fFD_);
#endif
#if     qPlatform_Windows
        ::_close (fFD_);
#else
        ::close (fFD_);
#endif
    }
    nonvirtual  Rep_& operator= (const Rep_&) = delete;

    virtual bool    IsSeekable () const override
    {
        return fSeekable_ == eSeekable;
    }
    virtual size_t  Read (Byte* intoStart, Byte* intoEnd) override
    {
        RequireNotNull (intoStart);
        RequireNotNull (intoEnd);
        Require (intoStart < intoEnd);
        size_t  nRequested  =   intoEnd - intoStart;
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx (L"FileInputStream::Rep_::Read");
        DbgTrace (L"(nRequested: %llu)", static_cast<unsigned long long> (nRequested));
#endif
        lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
#if     qPlatform_Windows
        return static_cast<size_t> (Execution::ThrowErrNoIfNegative (::_read (fFD_, intoStart, Math::PinToMaxForType<unsigned int> (nRequested))));
#else
        return static_cast<size_t> (Execution::ThrowErrNoIfNegative (::read (fFD_, intoStart, nRequested)));
#endif
    }
    virtual Streams::SeekOffsetType  GetReadOffset () const override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
#if     qPlatform_Windows
        return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::_lseeki64 (fFD_, 0, SEEK_CUR)));
#elif   qPlatform_Linux
        return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::lseek64 (fFD_, 0, SEEK_CUR)));
#else
        return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::lseek (fFD_, 0, SEEK_CUR)));
#endif
    }
    virtual Streams::SeekOffsetType    SeekRead (Streams::Whence whence, Streams::SignedSeekOffsetType offset) override
    {
        using   namespace Streams;
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx (L"FileInputStream::Rep_::SeekRead");
        DbgTrace (L"(whence: %d, offset: %lld)", whence, static_cast<long long> (offset));
#endif
        lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
        switch (whence) {
            case    Whence::eFromStart: {
                    if (offset < 0) {
                        Execution::Throw (std::range_error ("seek"));
                    }
#if     qPlatform_Windows
                    return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::_lseeki64 (fFD_, offset, SEEK_SET)));
#elif   qPlatform_Linux
                    return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::lseek64 (fFD_, offset, SEEK_SET)));
#else
                    return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::lseek (fFD_, offset, SEEK_SET)));
#endif
                }
                break;
            case    Whence::eFromCurrent: {
#if     qPlatform_Windows
                    return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::_lseeki64 (fFD_, offset, SEEK_CUR)));
#elif   qPlatform_Linux
                    return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::lseek64 (fFD_, offset, SEEK_CUR)));
#else
                    return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::lseek (fFD_, offset, SEEK_CUR)));
#endif
                }
                break;
            case    Whence::eFromEnd: {
#if     qPlatform_Windows
                    return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::_lseeki64 (fFD_, offset, SEEK_END)));
#elif   qPlatform_Linux
                    return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::lseek64 (fFD_, offset, SEEK_END)));
#else
                    return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (::lseek (fFD_, offset, SEEK_END)));
#endif
                }
                break;
        }
        RequireNotReached ();
        return 0;
    }

private:
    int             fFD_;
    SeekableFlag    fSeekable_;
};









FileInputStream::FileInputStream (const String& fileName, SeekableFlag seekable)
    : inherited (make_shared<Rep_> (fileName, seekable))
{
}

FileInputStream::FileInputStream (FileDescriptorType fd, SeekableFlag seekable)
    : inherited (make_shared<Rep_> (fd, seekable))
{
}

InputStream<Byte>   FileInputStream::mk (const String& fileName, SeekableFlag seekable, BufferFlag bufferFlag)
{
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"FileInputStream::mk");
    DbgTrace (L"(fileName: %s, seekable: %d, bufferFlag: %d)", fileName.c_str (), seekable, bufferFlag);
#endif
    InputStream<Byte>   in  =   FileInputStream (fileName, seekable);
    switch (bufferFlag) {
        case eBuffered:
            return Streams::BufferedInputStream<Byte> (in);
        case eUnbuffered:
            return in;
        default:
            AssertNotReached ();
            return in;
    }
}

InputStream<Byte>   FileInputStream::mk (FileDescriptorType fd, SeekableFlag seekable, BufferFlag bufferFlag)
{
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"FileInputStream::mk");
    DbgTrace (L"(fd: %d, seekable: %d, bufferFlag: %d)", fd, seekable, bufferFlag);
#endif
    InputStream<Byte>   in  =   FileInputStream (fd, seekable);
    switch (bufferFlag) {
        case eBuffered:
            return Streams::BufferedInputStream<Byte> (in);
        case eUnbuffered:
            return in;
        default:
            AssertNotReached ();
            return in;
    }
}
