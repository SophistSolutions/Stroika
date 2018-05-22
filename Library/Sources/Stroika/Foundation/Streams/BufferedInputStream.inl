/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedInputStream_inl_
#define _Stroika_Foundation_Streams_BufferedInputStream_inl_ 1

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
             ******************** Streams::BufferedInputStream::Rep_ ************************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            class BufferedInputStream<ELEMENT_TYPE>::Rep_ : public InputStream<ELEMENT_TYPE>::_IRep, private Debug::AssertExternallySynchronizedLock {
            public:
                Rep_ (const typename InputStream<ELEMENT_TYPE>::Ptr& realIn)
                    : InputStream<ELEMENT_TYPE>::_IRep ()
                    , fRealIn_ (realIn)
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
                virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
                {
                    RequireNotReached ();
                    Require (IsOpenRead ());
                    return 0;
                }
                virtual size_t Read (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    Require (IsOpenRead ());
                    return fRealIn_.Read (intoStart, intoEnd);
                }
                virtual Memory::Optional<size_t> ReadNonBlocking (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    // easy todo while no real buffer implementation ;-)
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    Require (IsOpenRead ());
                    return fRealIn_.ReadNonBlocking (intoStart, intoEnd);
                }

            private:
                typename InputStream<ELEMENT_TYPE>::Ptr fRealIn_;
            };

            /*
             ********************************************************************************
             ********************* Streams::BufferedInputStream *****************************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            inline auto BufferedInputStream<ELEMENT_TYPE>::New (const typename InputStream<ELEMENT_TYPE>::Ptr& realIn) -> Ptr
            {
                return make_shared<Rep_> (realIn);
            }
            template <typename ELEMENT_TYPE>
            inline auto BufferedInputStream<ELEMENT_TYPE>::New (Execution::InternallySyncrhonized internallySyncrhonized, const typename InputStream<ELEMENT_TYPE>::Ptr& realIn) -> Ptr
            {
                switch (internallySyncrhonized) {
                    case Execution::eInternallySynchronized:
                        return InternalSyncRep_::New (realIn);
                    case Execution::eNotKnownInternallySynchronized:
                        return New (realIn);
                    default:
                        RequireNotReached ();
                        return nullptr;
                }
            }

            /*
             ********************************************************************************
             ****************** BufferedInputStream<ELEMENT_TYPE>::Ptr **********************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            inline BufferedInputStream<ELEMENT_TYPE>::Ptr::Ptr (const shared_ptr<Rep_>& from)
                : inherited (from)
            {
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_BufferedInputStream_inl_*/
