/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>

#include    "../Containers/Common.h"
#include    "../Execution/Exceptions.h"

#include    "ExternallyOwnedMemoryBinaryInputStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;

using   Execution::make_unique_lock;



class   ExternallyOwnedMemoryBinaryInputStream::IRep_ : public InputStream<Byte>::_IRep {
public:
    IRep_ () = delete;
    IRep_ (const IRep_&) = delete;
    IRep_ (const Byte* start, const Byte* end)
        : fCriticalSection_ ()
        , fStart_ (start)
        , fEnd_ (end)
        , fCursor_ (start)
    {
    }

public:
    nonvirtual  IRep_& operator= (const IRep_&) = delete;

protected:
    virtual bool    IsSeekable () const override
    {
        return true;
    }
    virtual size_t  Read (SeekOffsetType* offset, Byte* intoStart, Byte* intoEnd) override
    {
        // @todo implement 'offset' support
        RequireNotNull (intoStart);
        RequireNotNull (intoEnd);
        Require (intoStart < intoEnd);
        size_t  nRequested  =   intoEnd - intoStart;
        auto    critSec { make_unique_lock (fCriticalSection_) };
        Assert ((fStart_ <= fCursor_) and (fCursor_ <= fEnd_));
        size_t  nAvail      =   fEnd_ - fCursor_;
        size_t  nCopied     =   min (nAvail, nRequested);
        (void)::memcpy (intoStart, fCursor_, nCopied);
        fCursor_ += nCopied;
        return nCopied; // this can be zero on EOF
    }
    virtual SeekOffsetType  GetReadOffset () const override
    {
        auto    critSec { make_unique_lock (fCriticalSection_) };  // This crit section only needed if fetch of fCursor_ is not gauranteed atomic
        return fCursor_ - fStart_;
    }
    virtual SeekOffsetType            SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        auto    critSec { make_unique_lock (fCriticalSection_) };
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
        return GetReadOffset ();
    }

private:
    mutable recursive_mutex  fCriticalSection_;
    const Byte*              fStart_;
    const Byte*              fEnd_;
    const Byte*              fCursor_;
};





/*
 ********************************************************************************
 ***************** Streams::ExternallyOwnedMemoryBinaryInputStream **************
 ********************************************************************************
 */
ExternallyOwnedMemoryBinaryInputStream::ExternallyOwnedMemoryBinaryInputStream (const Byte* start, const Byte* end)
    : InputStream<Byte> (shared_ptr<_IRep> (new IRep_ (start, end)))
{
}
