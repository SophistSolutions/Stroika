/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>

#include    "../Execution/Common.h"
#include    "../Execution/Exceptions.h"
#include    "../Memory/BlockAllocated.h"
#include    "../Traversal/Iterator.h"

#include    "MemoryStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;

using   Execution::make_unique_lock;




class   MemoryStream::Rep_ : public InputOutputStream<Byte>::_IRep {
public:
    Rep_ ()
        : fCriticalSection_ ()
        , fData_ ()
        , fReadCursor_ (fData_.begin ())
        , fWriteCursor_ (fData_.begin ())
    {
    }
    Rep_ (const Byte* start, const Byte* end)
        : Rep_ ()
    {
        Write (start, end);
    }
    Rep_ (const Rep_&) = delete;
    nonvirtual  Rep_& operator= (const Rep_&) = delete;

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
        Assert ((fData_.begin () <= fReadCursor_) and (fReadCursor_ <= fData_.end ()));
        size_t  nAvail      =   fData_.end () - fReadCursor_;
        size_t  nCopied     =   min (nAvail, nRequested);
        if (nCopied != 0) {
            (void)::memcpy (intoStart, Traversal::Iterator2Pointer (fReadCursor_), nCopied);
        }
        fReadCursor_ += nCopied;
        return nCopied; // this can be zero on EOF
    }
    virtual void    Write (const Byte* start, const Byte* end) override
    {
        Require (start != nullptr or start == end);
        Require (end != nullptr or start == end);
        if (start != end) {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
            MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
            auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
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
            (void)::memcpy (Traversal::Iterator2Pointer (fWriteCursor_), start, roomRequired);
            fWriteCursor_ += roomRequired;
            Assert (fReadCursor_ <= fData_.end ());
            Assert (fWriteCursor_ <= fData_.end ());
        }
    }
    virtual void     Flush () override
    {
        // nothing todo - write 'writes thru'
    }
    virtual SeekOffsetType      GetReadOffset () const override
    {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
        auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
        return fReadCursor_ - fData_.begin ();
    }
    virtual SeekOffsetType      SeekRead (Whence whence, SignedSeekOffsetType offset) override
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
    virtual SeekOffsetType      GetWriteOffset () const override
    {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
        auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
        return fWriteCursor_ - fData_.begin ();
    }
    virtual SeekOffsetType      SeekWrite (Whence whence, SignedSeekOffsetType offset) override
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
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
        auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
        return Memory::BLOB (fData_);
    }
    vector<Byte>   AsVector () const
    {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
        auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
        return fData_;
    }
    string   AsString () const
    {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
        auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
        return string (reinterpret_cast<const char*> (Containers::Start (fData_)), reinterpret_cast<const char*> (Containers::End (fData_)));
    }

    // @todo - COULD redo using
    //      DEFINE_CONSTEXPR_CONSTANT(size_t, USE_BUFFER_BYTES, 1024 - sizeof(recursive_mutex) - sizeof(Byte*) - sizeof (BinaryInputStream::_IRep) - sizeof (Seekable::_IRep));
    //      Memory::SmallStackBuffer < Byte, USE_BUFFER_BYTES>  fData_;
    // Or Stroika chunked array code
private:
    mutable mutex           fCriticalSection_;
    vector<Byte>            fData_;
    vector<Byte>::iterator  fReadCursor_;
    vector<Byte>::iterator  fWriteCursor_;
};





/*
 ********************************************************************************
 ********************** Streams::Streams::MemoryStream **************************
 ********************************************************************************
 */
MemoryStream::MemoryStream ()
    : InputOutputStream<Byte> (make_shared<Rep_> ())
{
}

MemoryStream::MemoryStream (const Byte* start, const Byte* end)
    : InputOutputStream<Byte> (make_shared<Rep_> (start, end))
{
}

MemoryStream::MemoryStream (const Memory::BLOB& blob)
    : InputOutputStream<Byte> (make_shared<Rep_> (blob.begin (), blob.end ()))
{
}

template    <>
Memory::BLOB   MemoryStream::As () const
{
    RequireNotNull (_GetRep ().get ());
    AssertMember (_GetRep ().get (), Rep_);
    const Rep_&    rep =   *dynamic_cast<const Rep_*> (_GetRep ().get ());
    return rep.AsBLOB ();
}

template    <>
vector<Byte>   MemoryStream::As () const
{
    RequireNotNull (_GetRep ().get ());
    AssertMember (_GetRep ().get (), Rep_);
    const Rep_&    rep =   *dynamic_cast<const Rep_*> (_GetRep ().get ());
    return rep.AsVector ();
}

template    <>
string   MemoryStream::As () const
{
    RequireNotNull (_GetRep ().get ());
    AssertMember (_GetRep ().get (), Rep_);
    const Rep_&    rep =   *dynamic_cast<const Rep_*> (_GetRep ().get ());
    return rep.AsString ();
}

