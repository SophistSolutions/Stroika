/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_SplitterOutputStream_inl_
#define _Stroika_Foundation_Streams_SplitterOutputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/AssertExternallySynchronizedLock.h"

namespace Stroika::Foundation::Streams {

    /*
     ********************************************************************************
     ******************** Streams::SplitterOutputStream::Rep_ ***********************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    class SplitterOutputStream<ELEMENT_TYPE>::Rep_ : public OutputStream<ELEMENT_TYPE>::_IRep, private Debug::AssertExternallySynchronizedLock {
    public:
        Rep_ (const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut1, const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut2)
            : OutputStream<ELEMENT_TYPE>::_IRep ()
            , fRealOut1_ (realOut1)
            , fRealOut2_ (realOut2)
        {
        }

    public:
        virtual bool IsSeekable () const override
        {
            return fRealOut1_.IsSeekable () and fRealOut2_.IsSeekable ();
        }
        virtual void CloseWrite () override
        {
            Require (IsOpenWrite ());
            fRealOut1_.CloseWrite ();
            Assert (fRealOut1_ == nullptr);
            fRealOut2_.CloseWrite ();
            Assert (fRealOut2_ == nullptr);
        }
        virtual bool IsOpenWrite () const override
        {
            Assert (fRealOut1_.IsOpenWrite () == fRealOut2_.IsOpenWrite ());
            return fRealOut1_.IsOpenWrite ();
        }
        virtual SeekOffsetType GetWriteOffset () const override
        {
            Assert (fRealOut1_.GetWriteOffset () == fRealOut2_.GetWriteOffset ());
            return fRealOut1_.GetWriteOffset ();
        }
        virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
        {
            Require (IsOpenWrite ());
            SeekOffsetType o1 = fRealOut1_.SeekWrite (whence, offset);
            SeekOffsetType o2 = fRealOut2_.SeekWrite (whence, offset);
            Assert (o1 == o2);
            return o1;
        }
        virtual void Flush () override
        {
            lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
            Require (IsOpenWrite ());
            fRealOut1_.Flush ();
            fRealOut2_.Flush ();
        }
        // pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
        // Writes always succeed fully or throw.
        virtual void Write (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) override
        {
            Require (start < end); // for OutputStream<Byte> - this funciton requires non-empty write
            Require (IsOpenWrite ());
            lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
            fRealOut1_.Write (start, end);
            fRealOut2_.Write (start, end);
        }

    private:
        typename OutputStream<ELEMENT_TYPE>::Ptr fRealOut1_;
        typename OutputStream<ELEMENT_TYPE>::Ptr fRealOut2_;
    };

    /*
     ********************************************************************************
     ************************* Streams::SplitterOutputStream ************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline auto SplitterOutputStream<ELEMENT_TYPE>::New (const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut1, const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut2) -> Ptr
    {
        return make_shared<Rep_> (realOut1, realOut2);
    }
    template <typename ELEMENT_TYPE>
    inline auto SplitterOutputStream<ELEMENT_TYPE>::New (Execution::InternallySyncrhonized internallySyncrhonized, const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut1, const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut2) -> Ptr
    {
        switch (internallySyncrhonized) {
            case Execution::eInternallySynchronized:
                return InternalSyncRep_::New (realOut1, realOut2);
            case Execution::eNotKnownInternallySynchronized:
                return New (realOut1, realOut2);
            default:
                RequireNotReached ();
                return nullptr;
        }
    }

}

#endif /*_Stroika_Foundation_Streams_SplitterOutputStream_inl_*/
