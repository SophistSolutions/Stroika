/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "InternallySynchronizedOutputStream.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Memory/InlineBuffer.h"

namespace Stroika::Foundation::Streams::BufferedOutputStream {

    namespace Private_ {
        // note note using block-allocation cuz aggregates kDefaultBufSize_ sized buffer
        template <typename ELEMENT_TYPE, size_t INLINE_BUF_SIZE>
        class Rep_ : public IRep_<ELEMENT_TYPE> {
        public:
            Rep_ (const typename OutputStream::Ptr<ELEMENT_TYPE>& realOut)
                : fRealOut_{realOut}
            {
            }
            ~Rep_ ()
            {
                if (IsOpenWrite ()) {
                    IgnoreExceptionsForCall (Flush ());
                }
                WeakAssert (fUnwrittenAppends_.size () == 0); // advisory - not quite right - could happen if a flush exception was eaten (@todo clean this up)
            }

        public:
            virtual size_t GetBufferSize () const override
            {
                Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                return fUnwrittenAppends_.capacity ();
            }
            virtual void SetBufferSize (size_t bufSize) override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                bufSize = Math::AtLeast (bufSize, INLINE_BUF_SIZE);
                if (bufSize < fUnwrittenAppends_.size ()) {
                    Flush_ (); // this logic only write because stream not seekable, and buffer is for unwritten appends
                }
                fUnwrittenAppends_.reserve (bufSize);
            }

        public:
            // Throws away all data about to be written (buffered). Once this is called, its illegal to call Flush or another write
            nonvirtual void Abort ()
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                fAborted_ = true; // for debug sake track this
                fUnwrittenAppends_.clear ();
            }
            virtual bool IsSeekable () const override
            {
                return false; // @todo - COULD be seekable if underlying fRealOut_ was!!!
            }
            virtual void CloseWrite () override
            {
                if (IsOpenWrite ()) {
                    Flush ();
                    fRealOut_.Close ();
                }
                Assert (fRealOut_ == nullptr);
                Ensure (not IsOpenWrite ());
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
                RequireNotReached (); // cuz we are not seekable, but could be changed/improved
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
                size_t bufSpaceRemaining   = fUnwrittenAppends_.capacity () - fUnwrittenAppends_.size ();
                size_t size2WriteRemaining = elts.size ();
                size_t copy2Buffer         = min (bufSpaceRemaining, size2WriteRemaining);
#if qStroika_Foundation_Debug_AssertionsChecked
                size_t oldCap = fUnwrittenAppends_.capacity ();
#endif
                fUnwrittenAppends_.insert (fUnwrittenAppends_.end (), elts.data (), elts.data () + copy2Buffer);
#if qStroika_Foundation_Debug_AssertionsChecked
                Assert (oldCap == fUnwrittenAppends_.capacity ());
#endif

                Assert (size2WriteRemaining >= copy2Buffer);
                size2WriteRemaining -= copy2Buffer;

                /*
                 * At this point - either the buffer is full, OR we are done writing. EITHER way - if the buffer is full - we may as well write it now.
                 */
                if (fUnwrittenAppends_.capacity () == fUnwrittenAppends_.size ()) {
                    Flush_ ();
                    Assert (fUnwrittenAppends_.empty ());
                }
#if qStroika_Foundation_Debug_AssertionsChecked
                Assert (oldCap == fUnwrittenAppends_.capacity ());
#endif

                // If the remaining will fit in the buffer, then buffer. But if it won't - no point in using the buffer - just write directly to avoid the copy.
                // And no point - even if equal to buffer size - since it won't save any writes...
                if (size2WriteRemaining == 0) {
                    // DONE
                }
                else if (size2WriteRemaining < fUnwrittenAppends_.capacity ()) {
                    fUnwrittenAppends_.insert (fUnwrittenAppends_.end (), elts.data () + copy2Buffer, elts.data () + elts.size ());
                }
                else {
                    fRealOut_.Write (elts.subspan (copy2Buffer));
                }
            }

        private:
            nonvirtual void Flush_ ()
            {
                if (fAborted_) {
                    fUnwrittenAppends_.clear ();
                }
                else {
                    if (not fUnwrittenAppends_.empty ()) {
                        fRealOut_.Write (span{fUnwrittenAppends_});
                        fUnwrittenAppends_.clear ();
                    }
                    fRealOut_.Flush ();
                }
                Ensure (fUnwrittenAppends_.empty ());
            }

        private:
            Memory::InlineBuffer<ELEMENT_TYPE, INLINE_BUF_SIZE>            fUnwrittenAppends_{};
            typename OutputStream::Ptr<ELEMENT_TYPE>                       fRealOut_{};
            bool                                                           fAborted_{false};
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
        };
    }

    /*
     ********************************************************************************
     ********************* Streams::BufferedOutputStream::New ***********************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline auto New (const typename OutputStream::Ptr<ELEMENT_TYPE>& realOut, const optional<size_t>& bufferSize) -> Ptr<ELEMENT_TYPE>
    {
        if (bufferSize and *bufferSize == 0) {
            return Ptr<ELEMENT_TYPE>{Memory::MakeSharedPtr<Private_::Rep_<ELEMENT_TYPE, 0>> (realOut)};
        }
        else if (bufferSize and *bufferSize <= 4 * 1024) {
            return Ptr<ELEMENT_TYPE>{Memory::MakeSharedPtr<Private_::Rep_<ELEMENT_TYPE, 4 * 1024>> (realOut)};
        }
        else {
            Ptr<ELEMENT_TYPE> p{Memory::MakeSharedPtr<Private_::Rep_<ELEMENT_TYPE>> (realOut)};
            if (bufferSize) {
                p.SetBufferSize (*bufferSize);
            }
            return p;
        }
    }
    template <typename ELEMENT_TYPE>
    inline auto New (Execution::InternallySynchronized internallySynchronized, const typename OutputStream::Ptr<ELEMENT_TYPE>& realOut,
                     const optional<size_t>& bufferSize) -> Ptr<ELEMENT_TYPE>
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized: {
                if (bufferSize and *bufferSize == 0) {
                    return Ptr<ELEMENT_TYPE>{InternallySynchronizedOutputStream::New<Private_::Rep_<ELEMENT_TYPE, 0>> ({}, realOut)};
                }
                else if (bufferSize and *bufferSize <= 4 * 1024) {
                    return Ptr<ELEMENT_TYPE>{InternallySynchronizedOutputStream::New<Private_::Rep_<ELEMENT_TYPE, 4 * 1024>> ({}, realOut)};
                }
                else {
                    Ptr<ELEMENT_TYPE> p{InternallySynchronizedOutputStream::New<Private_::Rep_<ELEMENT_TYPE>> ({}, realOut)};
                    if (bufferSize) {
                        p.SetBufferSize (*bufferSize);
                    }
                    return p;
                }
            }
            case Execution::eNotKnownInternallySynchronized:
                return New<ELEMENT_TYPE> (realOut, bufferSize);
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
    inline Ptr<ELEMENT_TYPE>::Ptr (const shared_ptr<Private_::IRep_<ELEMENT_TYPE>>& from)
        : inherited{from}
    {
    }
    template <typename ELEMENT_TYPE>
    inline size_t Ptr<ELEMENT_TYPE>::GetBufferSize () const
    {
        auto                          rep = this->GetSharedRep_ ();
        Private_::Rep_<ELEMENT_TYPE>* r   = Debug::UncheckedDynamicCast<Private_::Rep_<ELEMENT_TYPE>*> (rep.get ());
        AssertNotNull (r);
        return r->GetBufferSize ();
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::SetBufferSize (size_t bufSize)
    {
        auto                          rep = this->GetSharedRep_ ();
        Private_::Rep_<ELEMENT_TYPE>* r   = Debug::UncheckedDynamicCast<Private_::Rep_<ELEMENT_TYPE>*> (rep.get ());
        AssertNotNull (r);
        r->SetBufferSize (bufSize);
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
    inline shared_ptr<Private_::IRep_<ELEMENT_TYPE>> Ptr<ELEMENT_TYPE>::GetSharedRep_ () const
    {
        return Debug::UncheckedDynamicPointerCast<Private_::IRep_<ELEMENT_TYPE>> (inherited::GetSharedRep ());
    }

}
