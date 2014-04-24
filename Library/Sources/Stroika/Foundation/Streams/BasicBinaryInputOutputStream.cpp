/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>

#include    "../Execution/Exceptions.h"
#include    "../Memory/BlockAllocated.h"

#include    "BasicBinaryInputOutputStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;




class   BasicBinaryInputOutputStream::IRep_ : public BinaryInputOutputStream::_IRep {
public:
    DECLARE_USE_BLOCK_ALLOCATION(IRep_);

public:
    IRep_ ()
        : fCriticalSection_ ()
        , fData_ ()
        , fReadCursor_ (fData_.begin ())
        , fWriteCursor_ (fData_.begin ())
    {
    }
    IRep_ (const IRep_&) = delete;
    nonvirtual  IRep_& operator= (const IRep_&) = delete;
    virtual size_t  Read (Byte* intoStart, Byte* intoEnd) override
    {
        RequireNotNull (intoStart);
        RequireNotNull (intoEnd);
        Require (intoStart < intoEnd);
        size_t  nRequested  =   intoEnd - intoStart;
        lock_guard<mutex>  critSec (fCriticalSection_);
        Assert ((fData_.begin () <= fReadCursor_) and (fReadCursor_ <= fData_.end ()));
        size_t  nAvail      =   fData_.end () - fReadCursor_;
        size_t  nCopied     =   min (nAvail, nRequested);
        if (nCopied != 0) {
            memcpy (intoStart, &*fReadCursor_, nCopied);
        }
        fReadCursor_ += nCopied;
        return nCopied; // this can be zero on EOF
    }

    virtual void    Write (const Byte* start, const Byte* end) override
    {
        Require (start != nullptr or start == end);
        Require (end != nullptr or start == end);
        if (start != end) {
            lock_guard<mutex>  critSec (fCriticalSection_);
            size_t  roomLeft        =   fData_.end () - fWriteCursor_;
            size_t  roomRequired    =   end - start;
            if (roomLeft < roomRequired) {
                size_t  curReadOffset = fReadCursor_ - fData_.begin ();
                size_t  curWriteOffset = fWriteCursor_ - fData_.begin ();
                const size_t    kChunkSize_     =   128;        // WAG: @todo tune number...
                Containers::ReserveSpeedTweekAddN (fData_, roomRequired - roomLeft, kChunkSize_);
                fData_.resize (curWriteOffset + roomRequired);
                fReadCursor_ = fData_.begin () + curReadOffset;
                fWriteCursor_ = fData_.begin () + curWriteOffset;
                Assert (fWriteCursor_ < fData_.end ());
            }
            memcpy (&*fWriteCursor_, start, roomRequired);
            fWriteCursor_ += roomRequired;
            Assert (fReadCursor_ <= fData_.end ());
            Assert (fWriteCursor_ <= fData_.end ());
        }
    }

    virtual void     Flush () override
    {
        // nothing todo - write 'writes thru'
    }

    virtual SeekOffsetType      ReadGetOffset () const override
    {
        lock_guard<mutex>  critSec (fCriticalSection_);
        return fReadCursor_ - fData_.begin ();
    }

    virtual SeekOffsetType      ReadSeek (Whence whence, SignedSeekOffsetType offset) override
    {
        lock_guard<mutex>  critSec (fCriticalSection_);
        switch (whence) {
            case    Whence::eFromStart: {
                    if (offset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    SeekOffsetType  uOffset =   static_cast<SeekOffsetType> (offset);
                    if (uOffset > fData_.size ()) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fReadCursor_ = fData_.begin () + static_cast<size_t> (uOffset);
                }
                break;
            case    Whence::eFromCurrent: {
                    Streams::SeekOffsetType         curOffset   =   fReadCursor_ - fData_.begin ();
                    Streams::SignedSeekOffsetType   newOffset   =   curOffset + offset;
                    if (newOffset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    SeekOffsetType  uNewOffset =   static_cast<SeekOffsetType> (newOffset);
                    if (uNewOffset > fData_.size ()) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fReadCursor_ = fData_.begin () + static_cast<size_t> (uNewOffset);
                }
                break;
            case    Whence::eFromEnd: {
                    Streams::SeekOffsetType         curOffset   =   fReadCursor_ - fData_.begin ();
                    Streams::SignedSeekOffsetType   newOffset   =   fData_.size () + offset;
                    if (newOffset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    SeekOffsetType  uNewOffset =   static_cast<SeekOffsetType> (newOffset);
                    if (uNewOffset > fData_.size ()) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fReadCursor_ = fData_.begin () + static_cast<size_t> (uNewOffset);
                }
                break;
        }
        Ensure ((fData_.begin () <= fReadCursor_) and (fReadCursor_ <= fData_.end ()));
        return fReadCursor_ - fData_.begin ();
    }

    virtual SeekOffsetType      WriteGetOffset () const override
    {
        lock_guard<mutex>  critSec (fCriticalSection_);
        return fWriteCursor_ - fData_.begin ();
    }

    virtual SeekOffsetType      WriteSeek (Whence whence, SignedSeekOffsetType offset) override
    {
        lock_guard<mutex>  critSec (fCriticalSection_);
        switch (whence) {
            case    Whence::eFromStart: {
                    if (offset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    if (static_cast<SeekOffsetType> (offset) > fData_.size ()) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fWriteCursor_ = fData_.begin () + static_cast<size_t> (offset);
                }
                break;
            case    Whence::eFromCurrent: {
                    Streams::SeekOffsetType         curOffset   =   fWriteCursor_ - fData_.begin ();
                    Streams::SignedSeekOffsetType   newOffset   =   curOffset + offset;
                    if (newOffset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    if (static_cast<size_t> (newOffset) > fData_.size ()) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fWriteCursor_ = fData_.begin () + static_cast<size_t> (newOffset);
                }
                break;
            case    Whence::eFromEnd: {
                    Streams::SeekOffsetType     curOffset       =   fWriteCursor_ - fData_.begin ();
                    Streams::SignedSeekOffsetType newOffset     =   fData_.size () + offset;
                    if (newOffset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    if (static_cast<size_t> (newOffset) > fData_.size ()) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fWriteCursor_ = fData_.begin () + static_cast<size_t> (newOffset);
                }
                break;
        }
        Ensure ((fData_.begin () <= fWriteCursor_) and (fWriteCursor_ <= fData_.end ()));
        return fWriteCursor_ - fData_.begin ();
    }

    Memory::BLOB   AsBLOB () const
    {
        lock_guard<mutex>  critSec (fCriticalSection_);
        return Memory::BLOB (fData_);
    }

    vector<Byte>   AsVector () const
    {
        lock_guard<mutex>  critSec (fCriticalSection_);
        return fData_;
    }

    string   AsString () const
    {
        lock_guard<mutex>  critSec (fCriticalSection_);
        return string (reinterpret_cast<const char*> (Containers::Start (fData_)), reinterpret_cast<const char*> (Containers::End (fData_)));
    }

private:
    mutable mutex           fCriticalSection_;
    vector<Byte>            fData_;
    vector<Byte>::iterator  fReadCursor_;
    vector<Byte>::iterator  fWriteCursor_;
};





/*
 ********************************************************************************
 ************* Streams::iostream::BasicBinaryInputOutputStream ******************
 ********************************************************************************
 */
BasicBinaryInputOutputStream::BasicBinaryInputOutputStream ()
    : BinaryInputOutputStream (shared_ptr<IRep_> (new IRep_ ()))
{
}

template    <>
Memory::BLOB   BasicBinaryInputOutputStream::As () const
{
    RequireNotNull (_GetRep ().get ());
    AssertMember (_GetRep ().get (), IRep_);
    const IRep_&    rep =   *dynamic_cast<const IRep_*> (_GetRep ().get ());
    return rep.AsBLOB ();
}

template    <>
vector<Byte>   BasicBinaryInputOutputStream::As () const
{
    RequireNotNull (_GetRep ().get ());
    AssertMember (_GetRep ().get (), IRep_);
    const IRep_&    rep =   *dynamic_cast<const IRep_*> (_GetRep ().get ());
    return rep.AsVector ();
}

template    <>
string   BasicBinaryInputOutputStream::As () const
{
    RequireNotNull (_GetRep ().get ());
    AssertMember (_GetRep ().get (), IRep_);
    const IRep_&    rep =   *dynamic_cast<const IRep_*> (_GetRep ().get ());
    return rep.AsString ();
}

