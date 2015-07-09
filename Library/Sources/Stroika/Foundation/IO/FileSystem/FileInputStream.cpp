/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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




/*
 ********************************************************************************
 *********************** FileSystem::FileInputStream **********************
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
            errno_t e = _wsopen_s (&fFD_, fileName.c_str (), (O_RDONLY | O_BINARY), _SH_DENYNO, 0);
            if (e != 0) {
                Execution::errno_ErrorException::DoThrow (e);
            }
            ThrowIfFalseGetLastError (fFD_ != -1);
#else
            Execution::ThrowErrNoIfNegative (fFD_ = open (fileName.AsNarrowSDKString ().c_str (), O_RDONLY));
#endif
        }
        Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER(fileName, FileAccessMode::eRead);
    }
    ~Rep_ ()
    {
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
    virtual size_t  Read (SeekOffsetType* offset, Byte* intoStart, Byte* intoEnd) override
    {
        // @todo implement 'offset' support
        RequireNotNull (intoStart);
        RequireNotNull (intoEnd);
        Require (intoStart < intoEnd);
        size_t  nRequested  =   intoEnd - intoStart;
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
        return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (_lseeki64 (fFD_, 0, SEEK_CUR)));
#else
        return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (lseek64 (fFD_, 0, SEEK_CUR)));
#endif
    }
    virtual Streams::SeekOffsetType    SeekRead (Streams::Whence whence, Streams::SignedSeekOffsetType offset) override
    {
        using namespace Streams;
        lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
        switch (whence) {
            case    Whence::eFromStart: {
                    if (offset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
#if     qPlatform_Windows
                    return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (_lseeki64 (fFD_, offset, SEEK_SET)));
#else
                    return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (lseek64 (fFD_, offset, SEEK_SET)));
#endif
                }
                break;
            case    Whence::eFromCurrent: {
#if     qPlatform_Windows
                    return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (_lseeki64 (fFD_, offset, SEEK_CUR)));
#else
                    return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (lseek64 (fFD_, offset, SEEK_CUR)));
#endif
                }
                break;
            case    Whence::eFromEnd: {
#if     qPlatform_Windows
                    return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (_lseeki64 (fFD_, offset, SEEK_END)));
#else
                    return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (lseek64 (fFD_, offset, SEEK_END)));
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
    : FileInputStream (make_shared<Rep_> (fileName, seekable))
{
}

FileInputStream::FileInputStream (const shared_ptr<Rep_>& rep)
    : inherited (rep)
{
}

InputStream<Byte>   FileInputStream::mk (const String& fileName, SeekableFlag seekable, BufferFlag bufferFlag)
{
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
