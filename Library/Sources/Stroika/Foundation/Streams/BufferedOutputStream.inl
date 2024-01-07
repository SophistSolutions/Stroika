/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
#include "InternallySynchronizedOutputStream.h"

namespace Stroika::Foundation::Streams::BufferedOutputStream {

    namespace Private_ {
        template <typename ELEMENT_TYPE>
        class Rep_ : public OutputStream::IRep<ELEMENT_TYPE> {
            static constexpr size_t kMinBufSize_{1 * 1024};
            static constexpr size_t kDefaultBufSize_{16 * 1024};

        public:
            Rep_ (const typename OutputStream::Ptr<ELEMENT_TYPE>& realOut)
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
                Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                return fBuffer_.capacity ();
            }
            nonvirtual void SetBufferSize (size_t bufSize)
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
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
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
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
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenWrite ());
                Flush_ ();
            }
            // pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
            // Writes always succeed fully or throw.
            virtual void Write (span<const ELEMENT_TYPE> elts) override
            {
                Require (not elts.empty ()); // for OutputStream<byte> - this function requires non-empty write
                Require (not fAborted_);
                Require (IsOpenWrite ());
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                /*
                 * Minimize the number of writes at the possible cost of extra copying.
                 *
                 * See if there is room in the buffer, and use it up. Only when no more room do we flush.
                 */
                size_t bufSpaceRemaining   = fBuffer_.capacity () - fBuffer_.size ();
                size_t size2WriteRemaining = elts.size ();
                size_t copy2Buffer         = min (bufSpaceRemaining, size2WriteRemaining);
#if qDebug
                size_t oldCap = fBuffer_.capacity ();
#endif
                fBuffer_.insert (fBuffer_.end (), elts.data (), elts.data () + copy2Buffer);
#if qDebug
                Assert (oldCap == fBuffer_.capacity ());
#endif

                Assert (size2WriteRemaining >= copy2Buffer);
                size2WriteRemaining -= copy2Buffer;

                /*
                 * At this point - either the buffer is full, OR we are done writing. EITHER way - if the buffer is full - we may as well write it now.
                 */
                if (fBuffer_.capacity () == fBuffer_.size ()) {
                    Flush_ ();
                    Assert (fBuffer_.empty ());
                }
#if qDebug
                Assert (oldCap == fBuffer_.capacity ());
#endif

                // If the remaining will fit in the buffer, then buffer. But if it won't - no point in using the buffer - just write directly to avoid the copy.
                // And no point - even if equal to buffer size - since it won't save any writes...
                if (size2WriteRemaining == 0) {
                    // DONE
                }
                else if (size2WriteRemaining < fBuffer_.capacity ()) {
                    fBuffer_.insert (fBuffer_.end (), elts.data () + copy2Buffer, elts.data () + elts.size ());
                }
                else {
                    fRealOut_.Write (elts.subspan (copy2Buffer));
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
                        fRealOut_.Write (span{fBuffer_});
                        fBuffer_.clear ();
                    }
                    fRealOut_.Flush ();
                }
                Ensure (fBuffer_.empty ());
            }

        private:
            vector<ELEMENT_TYPE>                                           fBuffer_{};
            typename OutputStream::Ptr<ELEMENT_TYPE>                       fRealOut_{};
            bool                                                           fAborted_{false};
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
        };
    }

    /*
     ********************************************************************************
     ************************* Streams::BufferedOutputStream ************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline auto New (const typename OutputStream::Ptr<ELEMENT_TYPE>& realOut) -> Ptr<ELEMENT_TYPE>
    {
        return make_shared<Private_::Rep_<ELEMENT_TYPE>> (realOut);
    }
    template <typename ELEMENT_TYPE>
    inline auto New (Execution::InternallySynchronized internallySynchronized, const typename OutputStream::Ptr<ELEMENT_TYPE>& realOut)
        -> Ptr<ELEMENT_TYPE>
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                return InternallySynchronizedOutputStream::New<Private_::Rep_<ELEMENT_TYPE>> ({}, realOut);
            case Execution::eNotKnownInternallySynchronized:
                return New<ELEMENT_TYPE> (realOut);
            default:
                RequireNotReached ();
                return nullptr;
        }
    }

    /*
     ********************************************************************************
     ****************** BufferedOutputStream::Ptr<ELEMENT_TYPE> *********************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE>::Ptr (const shared_ptr<Private_::Rep_<ELEMENT_TYPE>>& from)
        : inherited{from}
    {
    }
    template <typename ELEMENT_TYPE>
    void Ptr<ELEMENT_TYPE>::Abort ()
    {
        auto                          rep = this->GetSharedRep_ ();
        Private_::Rep_<ELEMENT_TYPE>* r   = Debug::UncheckedDynamicCast<Private_::Rep_<ELEMENT_TYPE>*> (rep.get ());
        AssertNotNull (r);
        r->Abort ();
    }
    template <typename ELEMENT_TYPE>
    inline shared_ptr<Private_::Rep_<ELEMENT_TYPE>> Ptr<ELEMENT_TYPE>::GetSharedRep_ () const
    {
        return Debug::UncheckedDynamicPointerCast<Private_::Rep_<ELEMENT_TYPE>> (inherited::GetSharedRep ());
    }

}

#endif /*_Stroika_Foundation_Streams_BufferedOutputStream_inl_*/
