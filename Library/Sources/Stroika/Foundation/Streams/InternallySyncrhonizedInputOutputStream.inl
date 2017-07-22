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
             **** InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS>::Rep_ ***
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE, template <typename> typename BASE_CLASS>
            class InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS>::Rep_ : public InputOutputStream<ELEMENT_TYPE>::_IRep {
            public:
                Rep_ (const typename InputOutputStream<ELEMENT_TYPE>::Ptr& realInOut)
                    : InputOutputStream<ELEMENT_TYPE>::_IRep ()
                    , fRealInOut_ (realInOut)
                {
                }
                Rep_ ()            = delete;
                Rep_ (const Rep_&) = delete;
                virtual bool IsSeekable () const override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    return fRealInOut_.IsSeekable ();
                }
                virtual SeekOffsetType GetReadOffset () const override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    return fRealInOut_.GetReadOffset ();
                }
                virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    return fRealInOut_.SeekRead (whence, offset);
                }
                virtual size_t Read (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    return fRealInOut_.Read (intoStart, intoEnd);
                }
                virtual Memory::Optional<size_t> ReadNonBlocking (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    return fRealInOut_.ReadNonBlocking (intoStart, intoEnd);
                }
                virtual SeekOffsetType GetWriteOffset () const override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    return fRealInOut_.GetWriteOffset ();
                }
                virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    return fRealInOut_.SeekWrite (whence, offset);
                }
                virtual void Write (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    fRealInOut_.Write (start, end);
                }
                virtual void Flush () override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    fRealInOut_.Flush ();
                }

            private:
                mutable mutex                          fCriticalSection_;
                typename BASE_CLASS<ELEMENT_TYPE>::Ptr fRealInOut_;
            };

            /*
             ********************************************************************************
             ******* InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS> ******
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE, template <typename> typename BASE_CLASS>
            inline auto InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS>::New (const typename BASE_CLASS<ELEMENT_TYPE>::Ptr& stream2Wrap) -> Ptr
            {
                return make_shared<Rep_> (stream2Wrap);
            }

            /*
             ********************************************************************************
             *** InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS>::Ptr *****
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE, template <typename> typename BASE_CLASS>
            inline InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS>::Ptr::Ptr (const shared_ptr<Rep_>& from)
                : inherited (from)
            {
            }
            template <typename ELEMENT_TYPE, template <typename> typename BASE_CLASS>
            inline typename InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS>::Ptr& InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS>::Ptr::operator= (const InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS>& rhs)
            {
                inherited::Ptr::operator= (rhs);
                return *this;
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_InternallySyncrhonizedInputOutputStream_inl_*/
