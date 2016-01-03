/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_MemoryStream_inl_
#define _Stroika_Foundation_Streams_MemoryStream_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../Debug/AssertExternallySynchronizedLock.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /*
             ********************************************************************************
             ********************** MemoryStream<ELEMENT_TYPE>::Rep_ ************************
             ********************************************************************************
             */
            template    <typename   ELEMENT_TYPE>
            class   MemoryStream<ELEMENT_TYPE>::Rep_ : public InputOutputStream<ELEMENT_TYPE>::_IRep, private Debug::AssertExternallySynchronizedLock {
            public:
                using   ElementType = ELEMENT_TYPE;

            public:
                Rep_ ()
                    : fData_ ()
                    , fReadCursor_ (fData_.begin ())
                    , fWriteCursor_ (fData_.begin ())
                {
                }
                Rep_ (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end)
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
                virtual size_t  Read (SeekOffsetType* offset, ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    // @todo implement 'offset' support
                    RequireNotNull (intoStart);
                    RequireNotNull (intoEnd);
                    Require (intoStart < intoEnd);
                    size_t  nRequested  =   intoEnd - intoStart;
                    lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                    Assert ((fData_.begin () <= fReadCursor_) and (fReadCursor_ <= fData_.end ()));
                    size_t  nAvail      =   fData_.end () - fReadCursor_;
                    size_t  nCopied     =   min (nAvail, nRequested);
                    {
#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
                        Memory::Private::VC_BWA_std_copy (fReadCursor_, fReadCursor_ + nCopied, intoStart);
#else
                        std::copy (fReadCursor_, fReadCursor_ + nCopied, intoStart);
#endif
                        fReadCursor_ = fReadCursor_ + nCopied;
                    }
                    return nCopied; // this can be zero on EOF
                }
                virtual void    Write (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) override
                {
                    Require (start != nullptr or start == end);
                    Require (end != nullptr or start == end);
                    if (start != end) {
                        lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
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
#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
                        Memory::Private::VC_BWA_std_copy (start, start + roomRequired, fWriteCursor_);
#else
                        std::copy (start, start + roomRequired, fWriteCursor_);
#endif
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
                    lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                    return fReadCursor_ - fData_.begin ();
                }
                virtual SeekOffsetType      SeekRead (Whence whence, SignedSeekOffsetType offset) override
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
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
                    lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                    return fWriteCursor_ - fData_.begin ();
                }
                virtual SeekOffsetType      SeekWrite (Whence whence, SignedSeekOffsetType offset) override
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
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
                vector<ElementType>   AsVector () const
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                    return fData_;
                }
                string   AsString () const
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                    return string (reinterpret_cast<const char*> (Containers::Start (fData_)), reinterpret_cast<const char*> (Containers::End (fData_)));
                }

            private:
                // @todo - COULD redo using
                //      DEFINE_CONSTEXPR_CONSTANT(size_t, USE_BUFFER_BYTES, 1024 - sizeof(recursive_mutex) - sizeof(Byte*) - sizeof (BinaryInputStream::_IRep) - sizeof (Seekable::_IRep));
                //      Memory::SmallStackBuffer < Byte, USE_BUFFER_BYTES>  fData_;
                // Or Stroika chunked array code

            private:
                vector<ElementType>                     fData_;
                typename vector<ElementType>::iterator  fReadCursor_;
                typename vector<ElementType>::iterator  fWriteCursor_;
            };


            /*
            ********************************************************************************
            **************************** MemoryStream<ELEMENT_TYPE> ************************
            ********************************************************************************
            */
            template    <typename   ELEMENT_TYPE>
            MemoryStream<ELEMENT_TYPE>::MemoryStream ()
                : InputOutputStream<ELEMENT_TYPE> (make_shared<Rep_> ())
            {
            }
            template    <typename   ELEMENT_TYPE>
            MemoryStream<ELEMENT_TYPE>::MemoryStream (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end)
                : InputOutputStream<ELEMENT_TYPE> (make_shared<Rep_> (start, end))
            {
            }
            template    <typename   ELEMENT_TYPE>
            template    <typename TEST_TYPE, typename   ENABLE_IF_TEST>
            inline  MemoryStream<ELEMENT_TYPE>::MemoryStream (const Memory::BLOB& blob)
                : MemoryStream<ELEMENT_TYPE> (blob.begin (), blob.end ())
            {
            }
            template    <typename   ELEMENT_TYPE>
            template    <typename   T>
            T   MemoryStream<ELEMENT_TYPE>::As () const
            {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                RequireNotReached ();
#else
                static_assert (false, "Only specifically specialized variants are supported");
#endif
            }
            template    <>
            template    <>
            inline  vector<Memory::Byte>   MemoryStream<Memory::Byte>::As () const
            {
                RequireNotNull (_GetRep ().get ());
                AssertMember (_GetRep ().get (), Rep_);
                const Rep_&    rep =   *dynamic_cast<const Rep_*> (_GetRep ().get ());
                return rep.AsVector ();
            }
            template    <>
            template    <>
            inline  vector<Characters::Character>   MemoryStream<Characters::Character>::As () const
            {
                RequireNotNull (_GetRep ().get ());
                AssertMember (_GetRep ().get (), Rep_);
                const Rep_&    rep =   *dynamic_cast<const Rep_*> (_GetRep ().get ());
                return rep.AsVector ();
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_MemoryStream_inl_*/
