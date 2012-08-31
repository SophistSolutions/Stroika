/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>

#include    "../Execution/OperationNotSupportedException.h"

#include    "MemoryBinaryInputStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;




class   MemoryBinaryInputStream::IRep_ : public virtual BinaryInputStream::_IRep, public virtual Seekable {
public:
    NO_DEFAULT_CONSTRUCTOR(IRep_);
    NO_COPY_CONSTRUCTOR(IRep_);
    NO_ASSIGNMENT_OPERATOR(IRep_);

public:
    IRep_ (const Byte* start, const Byte* end)
        : fCriticalSection_ ()
        , fData_ (end - start)
        , fCursor_ () {
        memcpy (fData_.begin (), start, end - start);
        fCursor_ = fData_.begin ();
    }

    virtual size_t          Read (Byte* intoStart, Byte* intoEnd) override {
        RequireNotNull (intoStart);
        RequireNotNull (intoEnd);
        Require (intoStart < intoEnd);
        size_t  nRequested  =   intoEnd - intoStart;
        Execution::AutoCriticalSection  critSec (fCriticalSection_);
        Assert ((fData_.begin () <= fCursor_) and (fCursor_ <= fData_.end ()));
        size_t  nAvail      =   fData_.end () - fCursor_;
        size_t  nCopied     =   min (nAvail, nRequested);
        memcpy (intoStart, fCursor_, nCopied);
        fCursor_ += nCopied;
        return nCopied; // this can be zero on EOF
    }

    virtual SeekOffsetType  _GetOffset () const override {
        return fCursor_ - fData_.begin ();
    }

    virtual void            _Seek (Whence whence, SeekOffsetType offset) override {
        Execution::AutoCriticalSection  critSec (fCriticalSection_);
        switch (whence) {
            case    FromStart_W: {
                    if (offset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    if (static_cast<size_t> (offset) > fData_.GetSize ()) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fCursor_ = fData_.begin () + offset;
                }
                break;
            case    FromCurrent_W: {
                    Streams::SeekOffsetType curOffset   =   fCursor_ - fData_.begin ();
                    Streams::SeekOffsetType newOffset   =   curOffset + offset;
                    if (newOffset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    if (static_cast<size_t> (newOffset) > fData_.GetSize ()) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fCursor_ = fData_.begin () + newOffset;
                }
                break;
            case    FromEnd_W: {
                    Streams::SeekOffsetType curOffset   =   fCursor_ - fData_.begin ();
                    Streams::SeekOffsetType newOffset   =   fData_.GetSize () + offset;
                    if (newOffset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    if (static_cast<size_t> (newOffset) > fData_.GetSize ()) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fCursor_ = fData_.begin () + newOffset;
                }
                break;
        }
        Ensure ((fData_.begin () <= fCursor_) and (fCursor_ <= fData_.end ()));
    }

private:
    Execution::CriticalSection          fCriticalSection_;
    // round size of usage up to around 1k (include vtableptr) - no real good reason - # doesnt matter much...
    Memory::SmallStackBuffer < Byte, (1024 - sizeof(Execution::CriticalSection) - 2 * sizeof(Byte*)) >    fData_;
    const Byte*                         fCursor_;
};





/*
 ********************************************************************************
 ******************* Streams::iostream::MemoryBinaryInputStream *****************
 ********************************************************************************
 */
MemoryBinaryInputStream::MemoryBinaryInputStream (const Byte* start, const Byte* end)
    : BinaryInputStream (shared_ptr<_IRep> (new IRep_ (start, end)))
{
}




#if 0
/*
 ********************************************************************************
 ************************ Streams::MemoryBinaryInputStream **********************
 ********************************************************************************
 */
MemoryBinaryInputStream::MemoryBinaryInputStream (const Byte* start, const Byte* end)
    : fCriticalSection_ ()
    , fData_ (end - start)
    , fCursor_ ()
{
    memcpy (fData_.begin (), start, end - start);
    fCursor_ = fData_.begin ();
}

size_t  MemoryBinaryInputStream::_Read (Byte* intoStart, Byte* intoEnd)
{
    RequireNotNull (intoStart);
    RequireNotNull (intoEnd);
    Require (intoStart < intoEnd);
    size_t  nRequested  =   intoEnd - intoStart;
    Execution::AutoCriticalSection  critSec (fCriticalSection_);
    Assert ((fData_.begin () <= fCursor_) and (fCursor_ <= fData_.end ()));
    size_t  nAvail      =   fData_.end () - fCursor_;
    size_t  nCopied     =   min (nAvail, nRequested);
    memcpy (intoStart, fCursor_, nCopied);
    fCursor_ += nCopied;
    return nCopied; // this can be zero on EOF
}

Streams::SeekOffsetType MemoryBinaryInputStream::_GetOffset () const override
{
    return fCursor_ - fData_.begin ();
}

void    MemoryBinaryInputStream::_Seek (Streams::Whence whence, Streams::SeekOffsetType offset) override
{
    Execution::AutoCriticalSection  critSec (fCriticalSection_);
    switch (whence) {
        case    FromStart_W: {
                if (offset < 0) {
                    Execution::DoThrow (std::range_error ("seek"));
                }
                if (static_cast<size_t> (offset) > fData_.GetSize ()) {
                    Execution::DoThrow (std::range_error ("seek"));
                }
                fCursor_ = fData_.begin () + offset;
            }
            break;
        case    FromCurrent_W: {
                Streams::SeekOffsetType curOffset   =   fCursor_ - fData_.begin ();
                Streams::SeekOffsetType newOffset   =   curOffset + offset;
                if (newOffset < 0) {
                    Execution::DoThrow (std::range_error ("seek"));
                }
                if (static_cast<size_t> (newOffset) > fData_.GetSize ()) {
                    Execution::DoThrow (std::range_error ("seek"));
                }
                fCursor_ = fData_.begin () + newOffset;
            }
            break;
        case    FromEnd_W: {
                Streams::SeekOffsetType curOffset   =   fCursor_ - fData_.begin ();
                Streams::SeekOffsetType newOffset   =   fData_.GetSize () + offset;
                if (newOffset < 0) {
                    Execution::DoThrow (std::range_error ("seek"));
                }
                if (static_cast<size_t> (newOffset) > fData_.GetSize ()) {
                    Execution::DoThrow (std::range_error ("seek"));
                }
                fCursor_ = fData_.begin () + newOffset;
            }
            break;
    }
    Ensure ((fData_.begin () <= fCursor_) and (fCursor_ <= fData_.end ()));
}
#endif