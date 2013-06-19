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

#include    "BinaryFileInputStream.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::FileSystem;


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
    NO_DEFAULT_CONSTRUCTOR(Rep_);
    NO_COPY_CONSTRUCTOR(Rep_);
    NO_ASSIGNMENT_OPERATOR(Rep_);

public:
    Rep_ (const String& fileName)
        : fCriticalSection_ ()
        , fFD_ (-1) {
#if     qPlatform_Windows
        Execution::ThrowErrNoIfNegative (fFD_ = _tsopen_s (&fFD_, fileName.AsTString ().c_str (), (O_RDONLY | O_BINARY), _SH_DENYNO, 0));
        ThrowIfFalseGetLastError (fFD_ != -1);
#else
        Execution::ThrowErrNoIfNegative (fFD_ = open (fileName.AsNarrowSDKString ().c_str (), O_RDONLY));
#endif
    }
    ~Rep_ () {
        ::_close (fFD_);
    }
    virtual size_t   Read (Byte* intoStart, Byte* intoEnd) override {
        RequireNotNull (intoStart);
        RequireNotNull (intoEnd);
        Require (intoStart < intoEnd);
        size_t  nRequested  =   intoEnd - intoStart;
        lock_guard<mutex>  critSec (fCriticalSection_);
        int r = ::_read (fFD_, intoStart, nRequested);
        Execution::ThrowErrNoIfNegative (r);
        return static_cast<size_t> (r);
    }
    virtual Streams::SeekOffsetType  GetOffset () const override {
        lock_guard<mutex>  critSec (fCriticalSection_);
        off_t n =  _lseek (fFD_, 0, SEEK_CUR);
        Execution::ThrowErrNoIfNegative (n);
        return static_cast<Streams::SeekOffsetType> (n);
    }
    virtual Streams::SeekOffsetType    Seek (Streams::Whence whence, Streams::SignedSeekOffsetType offset) override {
        using namespace Streams;
        lock_guard<mutex>  critSec (fCriticalSection_);
        switch (whence) {
            case    Whence::eFromStart: {
                    if (offset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    off_t n =  _lseek (fFD_, offset, SEEK_SET);
                    Execution::ThrowErrNoIfNegative (n);
                    return static_cast<Streams::SeekOffsetType> (n);
                }
                break;
            case    Whence::eFromCurrent: {
                    off_t n =  _lseek (fFD_, offset, SEEK_CUR);
                    Execution::ThrowErrNoIfNegative (n);
                    return static_cast<Streams::SeekOffsetType> (n);
                }
                break;
            case    Whence::eFromEnd: {
                    off_t n =  _lseek (fFD_, offset, SEEK_END);
                    Execution::ThrowErrNoIfNegative (n);
                    return static_cast<Streams::SeekOffsetType> (n);
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









BinaryFileInputStream::BinaryFileInputStream (const String& fileName)
    : inherited (_SharedIRep (new Rep_ (fileName)))
{
}
