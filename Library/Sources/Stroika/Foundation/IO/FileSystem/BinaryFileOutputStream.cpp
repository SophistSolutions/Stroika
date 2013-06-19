/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
    NO_DEFAULT_CONSTRUCTOR(Rep_);
    NO_COPY_CONSTRUCTOR(Rep_);
    NO_ASSIGNMENT_OPERATOR(Rep_);

public:
    Rep_ (const String& fileName)
        : fCriticalSection_ ()
        , fFD_ (-1) {
#if     qPlatform_Windows
        errno_t e = ::_tsopen_s (&fFD_, fileName.AsTString ().c_str (), _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
        if (e != 0) {
            Execution::errno_ErrorException::DoThrow (e);
        }
        ThrowIfFalseGetLastError (fFD_ != -1);
#else
        Execution::ThrowErrNoIfNegative (fFD_ = open (fileName.AsNarrowSDKString ().c_str (), O_RDONLY));
#endif
    }
    ~Rep_ () {
        ::_close (fFD_);
    }
    virtual void    Write (const Byte* start, const Byte* end) override {
        Require (start != nullptr or start == end);
        Require (end != nullptr or start == end);

        if (start != end) {
            lock_guard<mutex>  critSec (fCriticalSection_);

            const Byte* i = start;
            while (i < end) {
                int n = Execution::ThrowErrNoIfNegative (_write (fFD_, i, end - i));
                Assert (n <= (end - i));
                i += n;
            }
        }
    }
    virtual void     Flush () override {
        // nothing todo - write 'writes thru'
    }
    virtual Streams::SeekOffsetType  GetOffset () const override {
        lock_guard<mutex>  critSec (fCriticalSection_);
        return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (_lseek (fFD_, 0, SEEK_CUR)));
    }
    virtual Streams::SeekOffsetType    Seek (Streams::Whence whence, Streams::SignedSeekOffsetType offset) override {
        using namespace Streams;
        lock_guard<mutex>  critSec (fCriticalSection_);
        switch (whence) {
            case    Whence::eFromStart: {
                    if (offset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (_lseek (fFD_, offset, SEEK_SET)));
                }
                break;
            case    Whence::eFromCurrent: {
                    return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (_lseek (fFD_, offset, SEEK_CUR)));
                }
                break;
            case    Whence::eFromEnd: {
                    return static_cast<Streams::SeekOffsetType> (Execution::ThrowErrNoIfNegative (_lseek (fFD_, offset, SEEK_END)));
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

BinaryFileOutputStream::BinaryFileOutputStream (const String& fileName)
    : inherited (_SharedIRep (new Rep_ (fileName)))
{
}
