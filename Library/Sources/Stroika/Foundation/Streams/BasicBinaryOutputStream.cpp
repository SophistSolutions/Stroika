/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>

#include    "../Execution/Exceptions.h"
#include    "../Memory/BlockAllocated.h"

#include    "BasicBinaryOutputStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;




class   BasicBinaryOutputStream::IRep_ : public BinaryOutputStream::_IRep, public Seekable::_IRep {
public:
    DECLARE_USE_BLOCK_ALLOCATION(IRep_);

public:
    IRep_ ()
        : fCriticalSection_ ()
        , fData_ ()
        , fCursor_ (fData_.begin ())
    {
    }
    IRep_ (const IRep_&) = delete;
    nonvirtual  const IRep_& operator= (const IRep_&) = delete;

    virtual void    Write (const Byte* start, const Byte* end) override
    {
        Require (start != nullptr or start == end);
        Require (end != nullptr or start == end);
        if (start != end) {
            lock_guard<mutex>  critSec (fCriticalSection_);
            size_t  roomLeft        =   fData_.end () - fCursor_;
            size_t  roomRequired    =   end - start;
            if (roomLeft < roomRequired) {
                size_t  curOffset = fCursor_ - fData_.begin ();
                Containers::ReserveSpeedTweekAddN (fData_, roomRequired - roomLeft);
                fData_.resize (curOffset + roomRequired);
                fCursor_ = fData_.begin () + curOffset;
                Assert (fCursor_ < fData_.end ());
            }
            memcpy (&*fCursor_, start, roomRequired);
            fCursor_ += roomRequired;
            Assert (fCursor_ <= fData_.end ());
        }
    }

    virtual void     Flush () override
    {
        // nothing todo - write 'writes thru'
    }

    virtual SeekOffsetType  GetOffset () const override
    {
        lock_guard<mutex>  critSec (fCriticalSection_);    // needed only if fetch of pointer not atomic
        return fCursor_ - fData_.begin ();
    }

    virtual SeekOffsetType    Seek (Whence whence, SignedSeekOffsetType offset) override
    {
        lock_guard<mutex>  critSec (fCriticalSection_);
        switch (whence) {
            case    Whence::eFromStart: {
                    if (offset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    if (offset > fData_.size ()) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fCursor_ = fData_.begin () + static_cast<size_t> (offset);
                }
                break;
            case    Whence::eFromCurrent: {
                    Streams::SeekOffsetType curOffset   =   fCursor_ - fData_.begin ();
                    Streams::SeekOffsetType newOffset   =   curOffset + offset;
                    if (newOffset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    if (static_cast<size_t> (newOffset) > fData_.size ()) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fCursor_ = fData_.begin () + static_cast<size_t> (newOffset);
                }
                break;
            case    Whence::eFromEnd: {
                    Streams::SeekOffsetType curOffset   =   fCursor_ - fData_.begin ();
                    Streams::SeekOffsetType newOffset   =   fData_.size () + offset;
                    if (newOffset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    if (static_cast<size_t> (newOffset) > fData_.size ()) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fCursor_ = fData_.begin () + static_cast<size_t> (newOffset);
                }
                break;
        }
        Ensure ((fData_.begin () <= fCursor_) and (fCursor_ <= fData_.end ()));
        return fCursor_ - fData_.begin ();
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
    vector<Byte>::iterator  fCursor_;
};





/*
 ********************************************************************************
 ******************* Streams::iostream::BasicBinaryOutputStream *****************
 ********************************************************************************
 */
BasicBinaryOutputStream::BasicBinaryOutputStream ()
    : BinaryOutputStream (shared_ptr<_IRep> (new IRep_ ()))
{
}

template    <>
Memory::BLOB   BasicBinaryOutputStream::As () const
{
    RequireNotNull (_GetRep ().get ());
    const IRep_&    rep =   *reinterpret_cast<const IRep_*> (_GetRep ().get ());
    return rep.AsBLOB ();
}

template    <>
vector<Byte>   BasicBinaryOutputStream::As () const
{
    RequireNotNull (_GetRep ().get ());
    const IRep_&    rep =   *reinterpret_cast<const IRep_*> (_GetRep ().get ());
    return rep.AsVector ();
}

template    <>
string   BasicBinaryOutputStream::As () const
{
    RequireNotNull (_GetRep ().get ());
    const IRep_&    rep =   *reinterpret_cast<const IRep_*> (_GetRep ().get ());
    return rep.AsString ();
}

