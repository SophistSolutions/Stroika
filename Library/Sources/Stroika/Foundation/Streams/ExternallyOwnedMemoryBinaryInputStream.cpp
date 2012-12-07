/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>

#if     !qCompilerAndStdLib_Supports_ConstructorDelegation
#include    "../Containers/Common.h"
#endif

#include    "ExternallyOwnedMemoryBinaryInputStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;




class   ExternallyOwnedMemoryBinaryInputStream::IRep_ : public BinaryInputStream::_IRep, public Seekable::_IRep {
public:
    NO_DEFAULT_CONSTRUCTOR(IRep_);
    NO_COPY_CONSTRUCTOR(IRep_);
    NO_ASSIGNMENT_OPERATOR(IRep_);

public:
    IRep_ (const Byte* start, const Byte* end)
        : fCriticalSection_ ()
        , fCursor_ (start)
        , fStart_ (start)
        , fEnd_ (end) {
    }

protected:
    virtual size_t  Read (Byte* intoStart, Byte* intoEnd) override {
        RequireNotNull (intoStart);
        RequireNotNull (intoEnd);
        Require (intoStart < intoEnd);
        size_t  nRequested  =   intoEnd - intoStart;
        Execution::AutoCriticalSection  critSec (fCriticalSection_);
        Assert ((fStart_ <= fCursor_) and (fCursor_ <= fEnd_));
        size_t  nAvail      =   fEnd_ - fCursor_;
        size_t  nCopied     =   min (nAvail, nRequested);
        memcpy (intoStart, fCursor_, nCopied);
        fCursor_ += nCopied;
        return nCopied; // this can be zero on EOF
    }

    virtual SeekOffsetType  _GetOffset () const override {
        Execution::AutoCriticalSection  critSec (fCriticalSection_);    // This crit section only needed if fetch of fCursor_ is not gauranteed atomic
        return fCursor_ - fStart_;
    }

    virtual SeekOffsetType            _Seek (Whence whence, SignedSeekOffsetType offset) override {
        Execution::AutoCriticalSection  critSec (fCriticalSection_);
        switch (whence) {
            case    Whence::eFromStart: {
                    if (offset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    if (offset > (fEnd_ - fStart_)) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fCursor_ = fStart_ + offset;
                }
                break;
            case    Whence::eFromCurrent: {
                    Streams::SeekOffsetType         curOffset   =   fCursor_ - fStart_;
                    Streams::SignedSeekOffsetType   newOffset   =   curOffset + offset;
                    if (newOffset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    if (newOffset > (fEnd_ - fStart_)) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fCursor_ = fStart_ + newOffset;
                }
                break;
            case    Whence::eFromEnd: {
                    Streams::SeekOffsetType         curOffset   =   fCursor_ - fStart_;
                    Streams::SignedSeekOffsetType   newOffset   =   (fEnd_ - fStart_) + offset;
                    if (newOffset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    if (newOffset > (fEnd_ - fStart_)) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fCursor_ = fStart_ + newOffset;
                }
                break;
        }
        Ensure ((fStart_ <= fCursor_) and (fCursor_ <= fEnd_));
        return _GetOffset ();
    }

private:
    mutable Execution::CriticalSection  fCriticalSection_;
    const Byte*                         fStart_;
    const Byte*                         fEnd_;
    const Byte*                         fCursor_;
};





/*
 ********************************************************************************
 ***************** Streams::ExternallyOwnedMemoryBinaryInputStream **************
 ********************************************************************************
 */
ExternallyOwnedMemoryBinaryInputStream::ExternallyOwnedMemoryBinaryInputStream (const Byte* start, const Byte* end)
    : BinaryInputStream (shared_ptr<_IRep> (new IRep_ (start, end)))
{
}

#if     !qCompilerAndStdLib_Supports_ConstructorDelegation
ExternallyOwnedMemoryBinaryInputStream::ExternallyOwnedMemoryBinaryInputStream (const vector<Byte>& v)
    : BinaryInputStream (shared_ptr<_IRep> (new IRep_ (Containers::Start (v), Containers::End (v))))
{
}
#endif
