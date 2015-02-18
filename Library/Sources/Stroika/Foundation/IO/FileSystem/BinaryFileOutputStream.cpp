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

#include    "../../Execution/Common.h"
#include    "../../Execution/ErrNoException.h"
#include    "../../Execution/Exceptions.h"
#if     qPlatform_Windows
#include    "../../Execution/Platform/Windows/Exception.h"
#endif

#include    "BinaryFileOutputStream.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::FileSystem;

using   Execution::make_unique_lock;


#if     qPlatform_Windows
using   Execution::Platform::Windows::ThrowIfFalseGetLastError;
#endif



/*
 ********************************************************************************
 *********************** FileSystem::BinaryFileOutputStream **********************
 ********************************************************************************
 */





class   BinaryFileOutputStream::Rep_ : public BinaryOutputStream::_IRep, public Seekable::_IRep {
public:
    Rep_ () = delete;
    Rep_ (const Rep_&) = delete;
    Rep_ (const String& fileName, FlushFlag flushFlag)
        : fCriticalSection_ ()
        , fFD_ (-1)
        , fFlushFlag (flushFlag)
    {
#if     qPlatform_Windows
        errno_t e = ::_wsopen_s (&fFD_, fileName.c_str (), _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
        if (e != 0) {
            Execution::errno_ErrorException::DoThrow (e);
        }
        ThrowIfFalseGetLastError (fFD_ != -1);
#else
        const mode_t kCreateMode_ = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
        Execution::ThrowErrNoIfNegative (fFD_ = open (fileName.AsNarrowSDKString ().c_str (), O_WRONLY | O_CREAT | O_TRUNC, kCreateMode_));
#endif
    }
    ~Rep_ ()
    {
        IgnoreExceptionsForCall (Flush ()); // for fFlushFlag == FlushFlag::eToDisk
#if     qPlatform_Windows
        ::_close (fFD_);
#else
        ::close (fFD_);
#endif
    }
    nonvirtual  Rep_& operator= (const Rep_&) = delete;
    virtual void    Write (const Byte* start, const Byte* end) override
    {
        Require (start != nullptr or start == end);
        Require (end != nullptr or start == end);

        if (start != end) {
            auto    critSec { make_unique_lock (fCriticalSection_) };

            const Byte* i = start;
            while (i < end) {
#if     qPlatform_Windows
                int n = Execution::ThrowErrNoIfNegative (_write (fFD_, i, Math::PinToMaxForType<unsigned int> (end - i)));
#else
                int n = Execution::ThrowErrNoIfNegative (write (fFD_, i, end - i));
#endif
                Assert (n <= (end - i));
                i += n;
            }
        }
    }
    virtual void     Flush () override
    {
        // nothing todo - write 'writes thru'
        if (fFlushFlag == FlushFlag::eToDisk) {
#if     qPlatform_Windows
            ThrowIfFalseGetLastError (::FlushFileBuffers (reinterpret_cast<HANDLE> (_get_osfhandle (fFD_))));
#elif   qPlatform_POSIX
            Execution::ThrowErrNoIfNegative (fsync (fFD_));
#else
            AssertNotImplemented ();
#endif
        }
    }
    virtual Streams::SeekOffsetType  GetOffset () const override
    {
        auto    critSec { make_unique_lock (fCriticalSection_) };
#if     qPlatform_Windows
        return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (_lseeki64 (fFD_, 0, SEEK_CUR)));
#else
        return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (lseek64 (fFD_, 0, SEEK_CUR)));
#endif
    }
    virtual Streams::SeekOffsetType    Seek (Streams::Whence whence, Streams::SignedSeekOffsetType offset) override
    {
        using namespace Streams;
        auto    critSec { make_unique_lock (fCriticalSection_) };
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
    mutable mutex   fCriticalSection_;
    int             fFD_;
    FlushFlag       fFlushFlag;
};

BinaryFileOutputStream::BinaryFileOutputStream (const String& fileName, FlushFlag flushFlag)
    : inherited (_SharedIRep (new Rep_ (fileName, flushFlag)))
{
}
