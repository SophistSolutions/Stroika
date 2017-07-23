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
             * InternallySyncrhonizedInputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Rep_
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE, template <typename> class BASE_CLASS, typename BASE_REP_TYPE>
            class InternallySyncrhonizedInputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Rep_ : public BASE_REP_TYPE {
            public:
                Rep_ (const typename BASE_CLASS<ELEMENT_TYPE>::Ptr& realIn)
                    : BASE_REP_TYPE ()
                    , fRealIn_ (realIn)
                {
                }
                Rep_ ()            = delete;
                Rep_ (const Rep_&) = delete;
                virtual bool IsSeekable () const override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    return fRealIn_.IsSeekable ();
                }
                virtual SeekOffsetType GetReadOffset () const override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    return fRealIn_.GetReadOffset ();
                }
                virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    return fRealIn_.SeekRead (whence, offset);
                }
                virtual size_t Read (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    return fRealIn_.Read (intoStart, intoEnd);
                }
                virtual Memory::Optional<size_t> ReadNonBlocking (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    return fRealIn_.ReadNonBlocking (intoStart, intoEnd);
                }

            private:
                mutable mutex                          fCriticalSection_;
                typename BASE_CLASS<ELEMENT_TYPE>::Ptr fRealIn_;
            };

            /*
             ********************************************************************************
             ** InternallySyncrhonizedInputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE> **
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE, template <typename> class BASE_CLASS, typename BASE_REP_TYPE>
            inline auto InternallySyncrhonizedInputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::New (const typename BASE_CLASS<ELEMENT_TYPE>::Ptr& stream2Wrap) -> Ptr
            {
                return Ptr{make_shared<Rep_> (stream2Wrap)};
            }

            /*
             ********************************************************************************
             * InternallySyncrhonizedInputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Ptr
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE, template <typename> class BASE_CLASS, typename BASE_REP_TYPE>
            inline InternallySyncrhonizedInputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Ptr::Ptr (const shared_ptr<Rep_>& from)
                : inherited (from)
            {
            }
            template <typename ELEMENT_TYPE, template <typename> class BASE_CLASS, typename BASE_REP_TYPE>
            inline typename InternallySyncrhonizedInputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Ptr& InternallySyncrhonizedInputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Ptr::operator= (const InternallySyncrhonizedInputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>& rhs)
            {
                inherited::Ptr::operator= (rhs);
                return *this;
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_InternallySyncrhonizedInputStream_inl_*/
