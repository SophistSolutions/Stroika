/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_ExternallyOwnedMemoryInputStream_inl_
#define _Stroika_Foundation_Streams_ExternallyOwnedMemoryInputStream_inl_ 1


#include    "../Debug/AssertExternallySynchronizedLock.h"
#include    "../Traversal/Iterator.h"


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /*
             ********************************************************************************
             ***************** Streams::ExternallyOwnedMemoryInputStream::Rep_ **************
             ********************************************************************************
             */
            template    <typename ELEMENT_TYPE>
            class   ExternallyOwnedMemoryInputStream<ELEMENT_TYPE>::Rep_ : public InputStream<ELEMENT_TYPE>::_IRep, private Debug::AssertExternallySynchronizedLock {
            public:
                Rep_ () = delete;
                Rep_ (const Rep_&) = delete;
                Rep_ (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end)
                    : fStart_ (start)
                    , fEnd_ (end)
                    , fCursor_ (start)
                {
                }

            public:
                nonvirtual  Rep_& operator= (const Rep_&) = delete;

            protected:
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
                    Assert ((fStart_ <= fCursor_) and (fCursor_ <= fEnd_));
                    size_t  nAvail      =   fEnd_ - fCursor_;
                    size_t  nCopied     =   min (nAvail, nRequested);
#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
                    Memory::Private::VC_BWA_std_copy (fCursor_, fCursor_ + nCopied, intoStart);
#else
                    std::copy (fCursor_, fCursor_ + nCopied, intoStart);
#endif
                    fCursor_ += nCopied;
                    return nCopied; // this can be zero on EOF
                }
                virtual SeekOffsetType  GetReadOffset () const override
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                    return fCursor_ - fStart_;
                }
                virtual SeekOffsetType            SeekRead (Whence whence, SignedSeekOffsetType offset) override
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
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
                    return fCursor_ - fStart_;
                }

            private:
                mutable recursive_mutex fCriticalSection_;
                const ELEMENT_TYPE*     fStart_;
                const ELEMENT_TYPE*     fEnd_;
                const ELEMENT_TYPE*     fCursor_;
            };


            /*
             ********************************************************************************
             ************* Streams::ExternallyOwnedMemoryInputStream<ELEMENT_TYPE> **********
             ********************************************************************************
             */
            template    <typename ELEMENT_TYPE>
            ExternallyOwnedMemoryInputStream<ELEMENT_TYPE>::ExternallyOwnedMemoryInputStream (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end)
                : inherited (make_shared<Rep_> (start, end))
            {
            }
            template    <typename ELEMENT_TYPE>
            template    <typename   BYTE_RANDOM_ACCESS_ITERATOR>
            inline  ExternallyOwnedMemoryInputStream<ELEMENT_TYPE>::ExternallyOwnedMemoryInputStream (BYTE_RANDOM_ACCESS_ITERATOR start, BYTE_RANDOM_ACCESS_ITERATOR end)
                : ExternallyOwnedMemoryInputStream<ELEMENT_TYPE> (static_cast<const Memory::Byte*> (Traversal::Iterator2Pointer (start)), static_cast<const Memory::Byte*> (Traversal::Iterator2Pointer (start) + (end - start)))
            {
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_ExternallyOwnedMemoryInputStream_inl_*/
