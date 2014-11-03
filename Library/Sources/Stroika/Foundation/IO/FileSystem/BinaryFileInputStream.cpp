/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
#include    "../../Streams/BufferedBinaryInputStream.h"

#include    "BinaryFileInputStream.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::FileSystem;

using   Execution::make_unique_lock;
using   Streams::BinaryInputStream;



#if     qPlatform_Windows
using   Execution::Platform::Windows::ThrowIfFalseGetLastError;
#endif




/*
 ********************************************************************************
 *********************** FileSystem::BinaryFileInputStream **********************
 ********************************************************************************
 */
class   BinaryFileInputStream::Rep_ : public BinaryInputStream::_IRep, public Seekable::_IRep {
public:
    Rep_ () = delete;
    Rep_ (const Rep_&) = delete;
    Rep_ (const String& fileName)
        : fCriticalSection_ ()
        , fFD_ (-1)
    {
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
    ~Rep_ ()
    {
#if     qPlatform_Windows
        ::_close (fFD_);
#else
        ::close (fFD_);
#endif
    }
    nonvirtual  Rep_& operator= (const Rep_&) = delete;

    virtual size_t   Read (Byte* intoStart, Byte* intoEnd) override
    {
        RequireNotNull (intoStart);
        RequireNotNull (intoEnd);
        Require (intoStart < intoEnd);
        size_t  nRequested  =   intoEnd - intoStart;
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
        auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
#if     qPlatform_Windows
        return static_cast<size_t> (Execution::ThrowErrNoIfNegative (::_read (fFD_, intoStart, Math::PinToMaxForType<unsigned int> (nRequested))));
#else
        return static_cast<size_t> (Execution::ThrowErrNoIfNegative (::read (fFD_, intoStart, nRequested)));
#endif
    }
    virtual Streams::SeekOffsetType  GetOffset () const override
    {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
        auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
#if     qPlatform_Windows
        return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (_lseeki64 (fFD_, 0, SEEK_CUR)));
#else
        return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (lseek64 (fFD_, 0, SEEK_CUR)));
#endif
    }
    virtual Streams::SeekOffsetType    Seek (Streams::Whence whence, Streams::SignedSeekOffsetType offset) override
    {
        using namespace Streams;
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
        auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
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
};








BinaryFileInputStream::BinaryFileInputStream (const String& fileName, SeekableFlag seekable)
    : BinaryFileInputStream (shared_ptr<Rep_> (new Rep_ (fileName)), seekable)
{
}

BinaryFileInputStream::BinaryFileInputStream (const shared_ptr<Rep_>& rep, SeekableFlag seekable)
    : inherited (rep, seekable == eSeekable ? rep.get () : nullptr)
{
}

BinaryInputStream   BinaryFileInputStream::mk (const String& fileName, SeekableFlag seekable, BufferFlag bufferFlag)
{
    BinaryInputStream   in  =   BinaryFileInputStream (fileName, seekable);
    switch (bufferFlag) {
        case eBuffered:
            return Streams::BufferedBinaryInputStream (in);
        case eUnbuffered:
            return in;
        default:
            AssertNotReached ();
    }
}
