/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>

#include    "../Containers/Common.h"
#include    "../Execution/OperationNotSupportedException.h"
#include    "../Memory/BLOB.h"

#include    "BasicBinaryInputStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;

using   Execution::make_unique_lock;





class   BasicBinaryInputStream::IRep_ : public BinaryInputStream::_IRep, public Seekable::_IRep {
public:
    IRep_ (const Byte* start, const Byte* end)
        : fCriticalSection_ ()
        , fData_ (end - start)
        , fCursor_ ()
    {
        if (start == end) {
            fCursor_ = fData_.end ();
        }
        else {
            (void)::memcpy (fData_.begin (), start, end - start);
            fCursor_ = fData_.begin ();
        }
    }
    IRep_ () = delete;
    IRep_ (const IRep_&) = delete;
    nonvirtual  IRep_& operator= (const IRep_&) = delete;

    virtual size_t          Read (Byte* intoStart, Byte* intoEnd) override
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
        Assert ((fData_.begin () <= fCursor_) and (fCursor_ <= fData_.end ()));
        size_t  nAvail      =   fData_.end () - fCursor_;
        size_t  nCopied     =   min (nAvail, nRequested);
        (void)::memcpy (intoStart, fCursor_, nCopied);
        fCursor_ += nCopied;
        return nCopied; // this can be zero on EOF
    }

    virtual SeekOffsetType  GetOffset () const override
    {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
        auto    critSec { make_unique_lock (fCriticalSection_) };     // needed only if fetch of pointer not atomic
#endif
        return fCursor_ - fData_.begin ();
    }

    virtual SeekOffsetType    Seek (Whence whence, SignedSeekOffsetType offset) override
    {
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
                    SeekOffsetType  uoffset =   static_cast<SeekOffsetType> (offset);
                    if (uoffset > fData_.GetSize ()) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fCursor_ = fData_.begin () + uoffset;
                }
                break;
            case    Whence::eFromCurrent: {
                    Streams::SeekOffsetType         curOffset   =   fCursor_ - fData_.begin ();
                    Streams::SignedSeekOffsetType   newOffset   =   curOffset + offset;
                    if (newOffset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    SeekOffsetType  unewOffset  =   static_cast<SeekOffsetType> (newOffset);
                    if (unewOffset > fData_.GetSize ()) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fCursor_ = fData_.begin () + unewOffset;
                }
                break;
            case    Whence::eFromEnd: {
                    Streams::SeekOffsetType         curOffset   =   fCursor_ - fData_.begin ();
                    Streams::SignedSeekOffsetType   newOffset   =   fData_.GetSize () + offset;
                    if (newOffset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    SeekOffsetType  unewOffset  =   static_cast<SeekOffsetType> (newOffset);
                    if (unewOffset > fData_.GetSize ()) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fCursor_ = fData_.begin () + unewOffset;
                }
                break;
        }
        Ensure ((fData_.begin () <= fCursor_) and (fCursor_ <= fData_.end ()));
        return GetOffset ();
    }

private:
    // round size of usage up to around 1k (include vtableptr) - no real good reason - # doesnt matter much...
    DEFINE_CONSTEXPR_CONSTANT(size_t, USE_BUFFER_BYTES, 1024 - sizeof(recursive_mutex) - sizeof(Byte*) - sizeof (BinaryInputStream::_IRep) - sizeof (Seekable::_IRep));

    mutable recursive_mutex                             fCriticalSection_;
    Memory::SmallStackBuffer < Byte, USE_BUFFER_BYTES>  fData_;
    const Byte*                                         fCursor_;
};





/*
 ********************************************************************************
 ******************* Streams::iostream::BasicBinaryInputStream ******************
 ********************************************************************************
 */
BasicBinaryInputStream::BasicBinaryInputStream (const Byte* start, const Byte* end)
    : BinaryInputStream (shared_ptr<_IRep> (new IRep_ (start, end)))
{
}

BasicBinaryInputStream::BasicBinaryInputStream (const Memory::BLOB& blob)
    : BinaryInputStream (shared_ptr<_IRep> (new IRep_ (blob.begin (), blob.end ())))
{
}

#if     1
// See why still needed - TO DEBUG @todo
BasicBinaryInputStream::BasicBinaryInputStream (const vector<Byte>& v)
    : BinaryInputStream (shared_ptr<_IRep> (new IRep_ (Containers::Start (v), Containers::End (v))))
{
}
#endif
