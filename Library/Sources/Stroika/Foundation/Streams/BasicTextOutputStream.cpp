/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Containers/Common.h"
#include    "../Execution/Exceptions.h"

#include    "BasicTextOutputStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;



class   BasicTextOutputStream::IRep_ : public TextOutputStream::_IRep, public Seekable::_IRep {
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

    virtual void    Write (const Character* start, const Character* end) override
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
            memcpy (&*fCursor_, start, roomRequired * sizeof (Character));
            fCursor_ += roomRequired;
            Assert (fCursor_ <= fData_.end ());
        }
    }

#if 0
    // @todo - when we add to TextStream!@!! See TextOutStreamTODO
    virtual void     Flush () override
    {
        // nothing todo - write 'writes thru'
    }
#endif

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
                    SeekOffsetType  uoffset =   static_cast<SeekOffsetType> (offset);
                    if (uoffset > fData_.size ()) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fCursor_ = fData_.begin () + static_cast<size_t> (uoffset);
                }
                break;
            case    Whence::eFromCurrent: {
                    Streams::SeekOffsetType         curOffset   =   fCursor_ - fData_.begin ();
                    Streams::SignedSeekOffsetType   newOffset   =   curOffset + offset;
                    if (newOffset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    Streams::SeekOffsetType         uNewOffset   =      newOffset;
                    if (static_cast<size_t> (uNewOffset) > fData_.size ()) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fCursor_ = fData_.begin () + static_cast<size_t> (uNewOffset);
                }
                break;
            case    Whence::eFromEnd: {
                    Streams::SeekOffsetType         curOffset   =   fCursor_ - fData_.begin ();
                    Streams::SignedSeekOffsetType   newOffset   =   fData_.size () + offset;
                    if (newOffset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    Streams::SeekOffsetType         uNewOffset   =      newOffset;
                    if (static_cast<size_t> (uNewOffset) > fData_.size ()) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fCursor_ = fData_.begin () + static_cast<size_t> (uNewOffset);
                }
                break;
        }
        Ensure ((fData_.begin () <= fCursor_) and (fCursor_ <= fData_.end ()));
        return fCursor_ - fData_.begin ();
    }

#if 0
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
#endif

    String   AsString () const
    {
        lock_guard<mutex>  critSec (fCriticalSection_);
        return String (Containers::Start (fData_), Containers::End (fData_));
    }

private:
    mutable mutex               fCriticalSection_;
    vector<Character>           fData_;
    vector<Character>::iterator fCursor_;
};




/*
 ********************************************************************************
 ************************ Streams::BasicTextOutputStream ************************
 ********************************************************************************
 */
BasicTextOutputStream::BasicTextOutputStream ()
    : TextOutputStream (shared_ptr<_IRep> (new IRep_ ()))
{
}

template    <>
String   BasicTextOutputStream::As () const
{
    RequireNotNull (_GetRep ().get ());
    const IRep_&    rep =   *reinterpret_cast<const IRep_*> (_GetRep ().get ());
    return rep.AsString ();
}

