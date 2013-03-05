/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>

#include    "../Execution/OperationNotSupportedException.h"

#include    "BasicBinaryOutputStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;




class   BasicBinaryOutputStream::IRep_ : public BinaryOutputStream::_IRep, public Seekable::_IRep {
public:
    NO_COPY_CONSTRUCTOR(IRep_);
    NO_ASSIGNMENT_OPERATOR(IRep_);

public:
    IRep_ ()
        : fCriticalSection_ ()
        , fData_ ()
        , fCursor_ (fData_.begin ()) {
    }

    virtual void    Write (const Byte* start, const Byte* end) override {
        Require (start != nullptr or start == end);
        Require (end != nullptr or start == end);
        if (start != end) {
            lock_guard<recursive_mutex>  critSec (fCriticalSection_);

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

    virtual SeekOffsetType  GetOffset () const override {
        lock_guard<recursive_mutex>  critSec (fCriticalSection_);    // needed only if fetch of pointer not atomic
        return fCursor_ - fData_.begin ();
    }

    virtual SeekOffsetType    Seek (Whence whence, SignedSeekOffsetType offset) override {
        lock_guard<recursive_mutex>  critSec (fCriticalSection_);
        switch (whence) {
            case    Whence::eFromStart: {
                    if (offset < 0) {
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    if (static_cast<size_t> (offset) > fData_.size ()) {
                        // REALLY ERROR
                        Execution::DoThrow (std::range_error ("seek"));
                    }
                    fCursor_ = fData_.begin () + offset;
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
                    fCursor_ = fData_.begin () + newOffset;
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
                    fCursor_ = fData_.begin () + newOffset;
                }
                break;
        }
        Ensure ((fData_.begin () <= fCursor_) and (fCursor_ <= fData_.end ()));
        return GetOffset ();
    }

private:
    mutable recursive_mutex     fCriticalSection_;
    vector<Byte>                fData_;
    vector<Byte>::iterator      fCursor_;
};





/*
 ********************************************************************************
 ******************* Streams::iostream::BasicBinaryOutputStream ******************
 ********************************************************************************
 */
BasicBinaryOutputStream::BasicBinaryOutputStream ()
    : BinaryOutputStream (shared_ptr<_IRep> (new IRep_ ()))
{
}
