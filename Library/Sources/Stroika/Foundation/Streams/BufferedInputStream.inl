/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
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
                virtual SeekOffsetType GetReadOffset () const override
                {
                    RequireNotReached ();
                    return 0;
                }
                virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
                {
                    RequireNotReached ();
                    return 0;
                }
                virtual size_t Read (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    return fRealIn_.Read (intoStart, intoEnd);
                }
                virtual Memory::Optional<size_t> ReadNonBlocking (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    // https://stroika.atlassian.net/browse/STK-567 EXPERIMENTAL DRAFT API
                    Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
                    WeakAssert (false);
                    // @todo - FIX TO REALLY CHECK
                    return {};
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
            BufferedInputStream<ELEMENT_TYPE>::BufferedInputStream (const typename InputStream<ELEMENT_TYPE>::Ptr& realIn)
                : InputStream<ELEMENT_TYPE>::Ptr (make_shared<Rep_> (realIn))
            {
            }

            /*
             ********************************************************************************
             ****************** BufferedInputStream<ELEMENT_TYPE>::Ptr **********************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            inline BufferedInputStream<ELEMENT_TYPE>::Ptr::Ptr (const BufferedInputStream& from)
                : InputStream<ELEMENT_TYPE>::Ptr (from)
            {
            }
            template <typename ELEMENT_TYPE>
            inline typename BufferedInputStream<ELEMENT_TYPE>::Ptr& BufferedInputStream<ELEMENT_TYPE>::Ptr::operator= (const BufferedInputStream<ELEMENT_TYPE>& rhs)
            {
                InputStream<ELEMENT_TYPE>::Ptr::operator= (rhs);
                return *this;
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_BufferedInputStream_inl_*/
