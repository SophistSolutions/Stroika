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

#include "InternallySynchronizedInputStream.h"

namespace Stroika::Foundation::Streams::BufferedInputStream {

    namespace Private_ {
        template <typename ELEMENT_TYPE>
        class Rep_ : public InputStream::IRep<ELEMENT_TYPE> {
        public:
            Rep_ (const typename InputStream::Ptr<ELEMENT_TYPE>& realIn)
                : fRealIn_{realIn}
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
            virtual optional<size_t> AvailableToRead () override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                return fRealIn_.AvailableToRead (); // since no actual buffering here yet
            }
            virtual optional<span<ELEMENT_TYPE>> Read (span<ELEMENT_TYPE> intoBuffer, NoDataAvailableHandling blockFlag) override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                return fRealIn_.Read (intoBuffer, blockFlag);
            }

        private:
            typename InputStream::Ptr<ELEMENT_TYPE>                        fRealIn_;
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
        };
    }

    /*
     ********************************************************************************
     ********************* Streams::BufferedInputStream *****************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline auto New (const typename InputStream::Ptr<ELEMENT_TYPE>& realIn) -> Ptr<ELEMENT_TYPE>
    {
        return typename InputStream::Ptr<ELEMENT_TYPE>{make_shared<Private_::Rep_<ELEMENT_TYPE>> (realIn)};
    }
    template <typename ELEMENT_TYPE>
    inline auto New (Execution::InternallySynchronized internallySynchronized, const typename InputStream::Ptr<ELEMENT_TYPE>& realIn)
        -> Ptr<ELEMENT_TYPE>
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                return InternallySynchronizedInputStream::New<Private_::Rep_<ELEMENT_TYPE>> ({}, realIn);
            case Execution::eNotKnownInternallySynchronized:
                return New<ELEMENT_TYPE> (realIn);
            default:
                RequireNotReached ();
                return nullptr;
        }
    }

}

#endif /*_Stroika_Foundation_Streams_BufferedInputStream_inl_*/
