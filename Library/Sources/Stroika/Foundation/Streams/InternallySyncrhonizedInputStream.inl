/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySyncrhonizedInputStream_inl_
#define _Stroika_Foundation_Streams_InternallySyncrhonizedInputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /*
             ********************************************************************************
             ************ InternallySyncrhonizedInputStream<ELEMENT_TYPE>::Rep_ *************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            class InternallySyncrhonizedInputStream<ELEMENT_TYPE>::Rep_ : public InputStream<ELEMENT_TYPE>::_IRep {
            public:
                Rep_ (const typename InputStream<ELEMENT_TYPE>::Ptr& realIn)
                    : InputStream<ELEMENT_TYPE>::_IRep ()
                    , fRealIn_ (realIn)
                {
                }
                Rep_ ()            = delete;
                Rep_ (const Rep_&) = delete;
                virtual bool IsSeekable () const override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    return fRealIn_.IsSeekable ();
                }
                virtual SeekOffsetType GetReadOffset () const override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    return fRealIn_.GetReadOffset ();
                }
                virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    return fRealIn_.SeekRead (whence, offset);
                }
                virtual size_t Read (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    return fRealIn_.Read (intoStart, intoEnd);
                }
                virtual Memory::Optional<size_t> ReadNonBlocking (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    return fRealIn_.ReadNonBlocking (intoStart, intoEnd);
                }

            private:
                mutex                                   fCriticalSection_;
                typename InputStream<ELEMENT_TYPE>::Ptr fRealIn_;
            };

            /*
             ********************************************************************************
             *********** InternallySyncrhonizedInputStream<ELEMENT_TYPE> ********************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            inline auto InternallySyncrhonizedInputStream<ELEMENT_TYPE>::New (const InputStream<T>::Ptr& stream2Wrap) -> Ptr
            {
                return make_shared<Rep_> (stream2Wrap);
            }

            /*
             ********************************************************************************
             ************** InternallySyncrhonizedInputStream<ELEMENT_TYPE>::Ptr ************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            inline InternallySyncrhonizedInputStream<ELEMENT_TYPE>::Ptr::Ptr (const shared_ptr<Rep_>& from)
                : inherited (from)
            {
            }
            template <typename ELEMENT_TYPE>
            inline typename InternallySyncrhonizedInputStream<ELEMENT_TYPE>::Ptr& InternallySyncrhonizedInputStream<ELEMENT_TYPE>::Ptr::operator= (const InternallySyncrhonizedInputStream<ELEMENT_TYPE>& rhs)
            {
                inherited::Ptr::operator= (rhs);
                return *this;
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_InternallySyncrhonizedInputStream_inl_*/
