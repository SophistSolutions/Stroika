/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_SplitterOutputStream_inl_
#define _Stroika_Foundation_Streams_SplitterOutputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "InternallySynchronizedOutputStream.h"

namespace Stroika::Foundation::Streams::SplitterOutputStream {

    namespace Private_ {
        template <typename ELEMENT_TYPE>
        class Rep_ : public OutputStream::IRep<ELEMENT_TYPE> {
        public:
            Rep_ (const typename OutputStream::Ptr<ELEMENT_TYPE>& realOut1, const typename OutputStream::Ptr<ELEMENT_TYPE>& realOut2)
                : fRealOut1_{realOut1}
                , fRealOut2_{realOut2}
            {
            }

        public:
            virtual bool IsSeekable () const override
            {
                Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                return fRealOut1_.IsSeekable () and fRealOut2_.IsSeekable ();
            }
            virtual void CloseWrite () override
            {
                Require (IsOpenWrite ());
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                fRealOut1_.CloseWrite ();
                Assert (fRealOut1_ == nullptr);
                fRealOut2_.CloseWrite ();
                Assert (fRealOut2_ == nullptr);
            }
            virtual bool IsOpenWrite () const override
            {
                Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                Assert (fRealOut1_.IsOpenWrite () == fRealOut2_.IsOpenWrite ());
                return fRealOut1_.IsOpenWrite ();
            }
            virtual SeekOffsetType GetWriteOffset () const override
            {
                Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                Assert (fRealOut1_.GetWriteOffset () == fRealOut2_.GetWriteOffset ());
                return fRealOut1_.GetWriteOffset ();
            }
            virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
            {
                Require (IsOpenWrite ());
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                SeekOffsetType                                         o1 = fRealOut1_.SeekWrite (whence, offset);
                [[maybe_unused]] SeekOffsetType                        o2 = fRealOut2_.SeekWrite (whence, offset);
                Assert (o1 == o2);
                return o1;
            }
            virtual void Flush () override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenWrite ());
                fRealOut1_.Flush ();
                fRealOut2_.Flush ();
            }
            // pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
            // Writes always succeed fully or throw.
            virtual void Write (span<const ELEMENT_TYPE> elts) override
            {
                Require (start < end); // for OutputStream<byte> - this function requires non-empty write
                Require (IsOpenWrite ());
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                fRealOut1_.Write (start, end);
                fRealOut2_.Write (start, end);
            }

        private:
            typename OutputStream::Ptr<ELEMENT_TYPE>                       fRealOut1_;
            typename OutputStream::Ptr<ELEMENT_TYPE>                       fRealOut2_;
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
        };
    }

    /*
     ********************************************************************************
     ************************* Streams::SplitterOutputStream ************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline auto New (const typename OutputStream::Ptr<ELEMENT_TYPE>& realOut1, const typename OutputStream::Ptr<ELEMENT_TYPE>& realOut2)
        -> Ptr<ELEMENT_TYPE>
    {
        return make_shared<Private_::Rep_> (realOut1, realOut2);
    }
    template <typename ELEMENT_TYPE>
    inline auto New (Execution::InternallySynchronized internallySynchronized, const typename OutputStream::Ptr<ELEMENT_TYPE>& realOut1,
                     const typename OutputStream::Ptr<ELEMENT_TYPE>& realOut2) -> Ptr<ELEMENT_TYPE>
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                return Streams::InternallySynchronizedOutputStream::New<Private_::Rep_> ({}, fileName, seekable);
            case Execution::eNotKnownInternallySynchronized:
                return New<ELEMENT_TYPE> (realOut1, realOut2);
            default:
                RequireNotReached ();
                return nullptr;
        }
    }

}

#endif /*_Stroika_Foundation_Streams_SplitterOutputStream_inl_*/
