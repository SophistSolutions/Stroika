/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedOutputStream_inl_
#define _Stroika_Foundation_Streams_BufferedOutputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/AssertExternallySynchronizedLock.h"

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /*
             ********************************************************************************
             ************************ Streams::BufferedOutputStream *************************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            class BufferedOutputStream<ELEMENT_TYPE>::Rep_ : public OutputStream<ELEMENT_TYPE>::_IRep, private Debug::AssertExternallySynchronizedLock {
                static const size_t kMinBufSize_     = 1 * 1024;
                static const size_t kDefaultBufSize_ = 16 * 1024;

            public:
                Rep_ (const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut)
                    : OutputStream<ELEMENT_TYPE>::_IRep ()
                    , fBuffer_ ()
                    , fRealOut_ (realOut)
                    , fAborted_ (false)
                {
                    fBuffer_.reserve (kDefaultBufSize_);
                }
                ~Rep_ ()
                {
                    if (IsOpenWrite ()) {
                        IgnoreExceptionsForCall (Flush ());
                    }
                    Ensure (fBuffer_.size () == 0); // advisory - not quite right - could happen if a flush exception was eaten (@todo clean this up)
                }

            public:
                nonvirtual size_t GetBufferSize () const
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    return fBuffer_.capacity ();
                }
                nonvirtual void SetBufferSize (size_t bufSize)
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    bufSize = max (bufSize, kMinBufSize_);
                    if (bufSize < fBuffer_.size ()) {
                        Flush_ ();
                    }
                    fBuffer_.reserve (bufSize);
                }

            public:
                // Throws away all data about to be written (buffered). Once this is called, its illegal to call Flush or another write
                nonvirtual void Abort ()
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    fAborted_ = true; // for debug sake track this
                    fBuffer_.clear ();
                }
                virtual bool IsSeekable () const override
                {
                    return false; // @todo - COULD be seekable if underlying fRealOut_ was!!!
                }
                virtual void CloseWrite () override
                {
                    Require (IsOpenWrite ());
                    Flush ();
                    fRealOut_.Close ();
                    Assert (fRealOut_ == nullptr);
                }
                virtual bool IsOpenWrite () const override
                {
                    return fRealOut_ != nullptr;
                }
                virtual SeekOffsetType GetWriteOffset () const override
                {
                    RequireNotReached ();
                    Require (IsOpenWrite ());
                    return 0;
                }
                virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
                {
                    RequireNotReached ();
                    Require (IsOpenWrite ());
                    return 0;
                }
                virtual void Flush () override
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    Require (IsOpenWrite ());
                    Flush_ ();
                }
                // pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
                // Writes always succeed fully or throw.
                virtual void Write (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) override
                {
                    Require (start < end); // for OutputStream<Byte> - this funciton requires non-empty write
                    Require (not fAborted_);
                    Require (IsOpenWrite ());
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    /*
                     * Minimize the number of writes at the possible cost of extra copying.
                     *
                     * See if there is room in the buffer, and use it up. Only when no more room do we flush.
                     */
                    size_t bufSpaceRemaining   = fBuffer_.capacity () - fBuffer_.size ();
                    size_t size2WriteRemaining = end - start;

                    size_t copy2Buffer = min (bufSpaceRemaining, size2WriteRemaining);
#if qDebug
                    size_t oldCap = fBuffer_.capacity ();
#endif
                    fBuffer_.insert (fBuffer_.end (), start, start + copy2Buffer);
                    Assert (oldCap == fBuffer_.capacity ());

                    Assert (size2WriteRemaining >= copy2Buffer);
                    size2WriteRemaining -= copy2Buffer;

                    /*
                     * At this point - either the buffer is full, OR we are done writing. EITHER way - if the buffer is full - we may as well write it now.
                     */
                    if (fBuffer_.capacity () == fBuffer_.size ()) {
                        Flush_ ();
                        Assert (fBuffer_.empty ());
                    }
                    Assert (oldCap == fBuffer_.capacity ());

                    // If the remaining will fit in the buffer, then buffer. But if it won't - no point in using the buffer - just write directly to avoid the copy.
                    // And no point - even if equal to buffer size - since it won't save any writes...
                    if (size2WriteRemaining == 0) {
                        // DONE
                    }
                    else if (size2WriteRemaining < fBuffer_.capacity ()) {
                        fBuffer_.insert (fBuffer_.end (), start + copy2Buffer, end);
                    }
                    else {
                        fRealOut_.Write (start + copy2Buffer, end);
                    }
                }

            private:
                nonvirtual void Flush_ ()
                {
                    if (fAborted_) {
                        fBuffer_.clear ();
                    }
                    else {
                        if (not fBuffer_.empty ()) {
                            fRealOut_.Write (Containers::Start (fBuffer_), Containers::End (fBuffer_));
                            fBuffer_.clear ();
                        }
                        fRealOut_.Flush ();
                    }
                    Ensure (fBuffer_.empty ());
                }

            private:
                vector<ELEMENT_TYPE>                     fBuffer_;
                typename OutputStream<ELEMENT_TYPE>::Ptr fRealOut_;
                bool                                     fAborted_;
            };

            /*
             ********************************************************************************
             ************************* Streams::BufferedOutputStream ************************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            inline auto BufferedOutputStream<ELEMENT_TYPE>::New (const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut) -> Ptr
            {
                return make_shared<Rep_> (realOut);
            }
            template <typename ELEMENT_TYPE>
            inline auto BufferedOutputStream<ELEMENT_TYPE>::New (Execution::InternallySyncrhonized internallySyncrhonized, const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut) -> Ptr
            {
                switch (internallySyncrhonized) {
                    case Execution::eInternallySynchronized:
                        return InternalSyncRep_::New (realOut);
                    case Execution::eNotKnwonInternallySynchronized:
                        return New (realOut);
                    default:
                        RequireNotReached ();
                        return nullptr;
                }
            }

            /*
             ********************************************************************************
             ****************** BufferedOutputStream<ELEMENT_TYPE>::Ptr *********************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            inline BufferedOutputStream<ELEMENT_TYPE>::Ptr::Ptr (const shared_ptr<Rep_>& from)
                : inherited (from)
            {
            }
            template <typename ELEMENT_TYPE>
            void BufferedOutputStream<ELEMENT_TYPE>::Ptr::Abort ()
            {
                auto  rep = this->_GetSharedRep ();
                Rep_* r   = dynamic_cast<Rep_*> (rep.get ());
                AssertNotNull (r);
                r->Abort ();
            }
            template <typename ELEMENT_TYPE>
            inline typename BufferedOutputStream<ELEMENT_TYPE>::_SharedIRep BufferedOutputStream<ELEMENT_TYPE>::Ptr::_GetSharedRep () const
            {
                return dynamic_pointer_cast<Rep_> (inherited::_GetSharedRep ());
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_BufferedOutputStream_inl_*/
