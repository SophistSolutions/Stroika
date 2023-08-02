/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedInputStream_inl_
#define _Stroika_Foundation_Streams_BufferedInputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/AssertExternallySynchronizedMutex.h"

namespace Stroika::Foundation::Streams {

    /*
     ********************************************************************************
     ******************** Streams::BufferedInputStream::Rep_ ************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    class BufferedInputStream<ELEMENT_TYPE>::Rep_ : public InputStream<ELEMENT_TYPE>::_IRep {
    public:
        Rep_ (const typename InputStream<ELEMENT_TYPE>::Ptr& realIn)
            : InputStream<ELEMENT_TYPE>::_IRep{}
            , fRealIn_{realIn}
        {
        }
        virtual bool IsSeekable () const override
        {
            return false; // @todo - COULD be seekable if underlying fRealIn_ was!!!
        }
        virtual void CloseRead () override
        {
            Require (IsOpenRead ());
            fRealIn_.Close ();
            Assert (fRealIn_ == nullptr);
        }
        virtual bool IsOpenRead () const override
        {
            return fRealIn_ != nullptr;
        }
        virtual SeekOffsetType GetReadOffset () const override
        {
            Require (IsOpenRead ());
            return fRealIn_.GetOffset ();
        }
        virtual SeekOffsetType SeekRead ([[maybe_unused]] Whence whence, [[maybe_unused]] SignedSeekOffsetType offset) override
        {
            RequireNotReached (); // not seekable (could fix)
            Require (IsOpenRead ());
            return 0;
        }
        virtual size_t Read (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            Require (IsOpenRead ());
            return fRealIn_.Read (intoStart, intoEnd);
        }
        virtual optional<size_t> ReadNonBlocking (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
        {
            // easy todo while no real buffer implementation ;-)
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            Require (IsOpenRead ());
            return fRealIn_.ReadNonBlocking (intoStart, intoEnd);
        }

    private:
        typename InputStream<ELEMENT_TYPE>::Ptr                        fRealIn_;
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
    };

    /*
     ********************************************************************************
     ********************* Streams::BufferedInputStream *****************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline auto BufferedInputStream<ELEMENT_TYPE>::New (const typename InputStream<ELEMENT_TYPE>::Ptr& realIn) -> Ptr
    {
        return InputStream<ELEMENT_TYPE>::_mkPtr (make_shared<Rep_> (realIn));
    }
    template <typename ELEMENT_TYPE>
    inline auto BufferedInputStream<ELEMENT_TYPE>::New (Execution::InternallySynchronized              internallySynchronized,
                                                        const typename InputStream<ELEMENT_TYPE>::Ptr& realIn) -> Ptr
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                return _mkPtr (InternalSyncRep_::New (realIn));
            case Execution::eNotKnownInternallySynchronized:
                return New (realIn);
            default:
                RequireNotReached ();
                return nullptr;
        }
    }

}

#endif /*_Stroika_Foundation_Streams_BufferedInputStream_inl_*/
