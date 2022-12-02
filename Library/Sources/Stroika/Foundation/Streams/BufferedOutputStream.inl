/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedOutputStream_inl_
#define _Stroika_Foundation_Streams_BufferedOutputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "../Debug/Cast.h"

namespace Stroika::Foundation ::Streams {

    /*
     ********************************************************************************
     ************************ Streams::BufferedOutputStream *************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    class BufferedOutputStream<ELEMENT_TYPE>::Rep_ : public OutputStream<ELEMENT_TYPE>::_IRep, private Debug::AssertExternallySynchronizedMutex {
        static constexpr size_t kMinBufSize_{1 * 1024};
        static constexpr size_t kDefaultBufSize_{16 * 1024};

    public:
        Rep_ (const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut)
            : fRealOut_{realOut}
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
            AssertExternallySynchronizedMutex::WriteLock critSec{*this};
            return fBuffer_.capacity ();
        }
        nonvirtual void SetBufferSize (size_t bufSize)
        {
            AssertExternallySynchronizedMutex::WriteLock critSec{*this};
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
            AssertExternallySynchronizedMutex::WriteLock critSec{*this};
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
        virtual SeekOffsetType SeekWrite ([[maybe_unused]] Whence whence, [[maybe_unused]] SignedSeekOffsetType offset) override
        {
            RequireNotReached (); // cuz we ar enot seekable, but could be changed/improved
            Require (IsOpenWrite ());
            return 0;
        }
        virtual void Flush () override
        {
            AssertExternallySynchronizedMutex::WriteLock critSec{*this};
            Require (IsOpenWrite ());
            Flush_ ();
        }
        // pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
        // Writes always succeed fully or throw.
        virtual void Write (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) override
        {
            Require (start < end); // for OutputStream<byte> - this function requires non-empty write
            Require (not fAborted_);
            Require (IsOpenWrite ());
            AssertExternallySynchronizedMutex::WriteLock critSec{*this};
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
        vector<ELEMENT_TYPE>                     fBuffer_{};
        typename OutputStream<ELEMENT_TYPE>::Ptr fRealOut_{};
        bool                                     fAborted_{false};
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
    inline auto BufferedOutputStream<ELEMENT_TYPE>::New (Execution::InternallySynchronized internallySynchronized, const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut) -> Ptr
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                return InternalSyncRep_::New (realOut);
            case Execution::eNotKnownInternallySynchronized:
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
        : inherited{from}
    {
    }
    template <typename ELEMENT_TYPE>
    void BufferedOutputStream<ELEMENT_TYPE>::Ptr::Abort ()
    {
        auto  rep = this->_GetSharedRep ();
        Rep_* r   = Debug::UncheckedDynamicCast<Rep_*> (rep.get ());
        AssertNotNull (r);
        r->Abort ();
    }
    template <typename ELEMENT_TYPE>
    inline typename BufferedOutputStream<ELEMENT_TYPE>::_SharedIRep BufferedOutputStream<ELEMENT_TYPE>::Ptr::_GetSharedRep () const
    {
        return dynamic_pointer_cast<Rep_> (inherited::_GetSharedRep ());
    }

}

#endif /*_Stroika_Foundation_Streams_BufferedOutputStream_inl_*/
