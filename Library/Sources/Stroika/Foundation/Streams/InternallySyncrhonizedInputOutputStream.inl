/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySyncrhonizedInputOutputStream_inl_
#define _Stroika_Foundation_Streams_InternallySyncrhonizedInputOutputStream_inl_ 1

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
             * InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Rep_ ***
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE, template <typename> typename BASE_CLASS, typename BASE_REP_TYPE>
            class InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Rep_ : public BASE_REP_TYPE {
            public:
                Rep_ (const typename BASE_CLASS<ELEMENT_TYPE>::Ptr& realInOut)
                    : BASE_REP_TYPE ()
                    , fRealInOut_ (realInOut)
                {
                }
                Rep_ ()            = delete;
                Rep_ (const Rep_&) = delete;
                virtual bool IsSeekable () const override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    return fRealInOut_.IsSeekable ();
                }
                virtual SeekOffsetType GetReadOffset () const override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    return fRealInOut_.GetReadOffset ();
                }
                virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    return fRealInOut_.SeekRead (whence, offset);
                }
                virtual size_t Read (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    return fRealInOut_.Read (intoStart, intoEnd);
                }
                virtual Memory::Optional<size_t> ReadNonBlocking (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    return fRealInOut_.ReadNonBlocking (intoStart, intoEnd);
                }
                virtual SeekOffsetType GetWriteOffset () const override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    return fRealInOut_.GetWriteOffset ();
                }
                virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    return fRealInOut_.SeekWrite (whence, offset);
                }
                virtual void Write (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    fRealInOut_.Write (start, end);
                }
                virtual void Flush () override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    fRealInOut_.Flush ();
                }

            private:
                mutable mutex                          fCriticalSection_;
                typename BASE_CLASS<ELEMENT_TYPE>::Ptr fRealInOut_;
            };

            /*
             ********************************************************************************
             * InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE, template <typename> typename BASE_CLASS, typename BASE_REP_TYPE>
            inline auto InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::New (const typename BASE_CLASS<ELEMENT_TYPE>::Ptr& stream2Wrap) -> Ptr
            {
                return Ptr{make_shared<Rep_> (stream2Wrap)};
            }

            /*
             ********************************************************************************
             * InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Ptr
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE, template <typename> typename BASE_CLASS, typename BASE_REP_TYPE>
            inline InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Ptr::Ptr (const shared_ptr<Rep_>& from)
                : inherited (from)
            {
            }
            template <typename ELEMENT_TYPE, template <typename> typename BASE_CLASS, typename BASE_REP_TYPE>
            inline typename InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Ptr& InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Ptr::operator= (const InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>& rhs)
            {
                inherited::Ptr::operator= (rhs);
                return *this;
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_InternallySyncrhonizedInputOutputStream_inl_*/
