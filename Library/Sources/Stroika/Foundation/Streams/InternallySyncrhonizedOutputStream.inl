/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySyncrhonizedOutputStream_inl_
#define _Stroika_Foundation_Streams_InternallySyncrhonizedOutputStream_inl_ 1

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
             *********** InternallySyncrhonizedOutputStream<ELEMENT_TYPE>::Rep_ *************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            class InternallySyncrhonizedOutputStream<ELEMENT_TYPE>::Rep_ : public OutputStream<ELEMENT_TYPE>::_IRep {
            public:
                Rep_ (const typename OutputStream<ELEMENT_TYPE>::Ptr& realOut)
                    : OutputStream<ELEMENT_TYPE>::_IRep ()
                    , fRealOut_ (realOut)
                {
                }
                Rep_ ()            = delete;
                Rep_ (const Rep_&) = delete;
                virtual bool IsSeekable () const override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    return fRealOut_.IsSeekable ();
                }
                virtual SeekOffsetType GetWriteOffset () const override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    return fRealOut_.GetWriteOffset ();
                }
                virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    return fRealOut_.SeekWrite (whence, offset);
                }
                virtual void Write (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    fRealOut_.Write (start, end);
                }
                virtual void Flush () override
                {
                    lock_guard<const mutex> critSec{fCriticalSection_};
                    fRealOut_.Write (start, end);
                }

            private:
                mutex                                    fCriticalSection_;
                typename OutputStream<ELEMENT_TYPE>::Ptr fRealOut_;
            };

            /*
             ********************************************************************************
             *********** InternallySyncrhonizedOutputStream<ELEMENT_TYPE> *******************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            inline auto InternallySyncrhonizedOutputStream<ELEMENT_TYPE>::New (const typename OutputStream<ELEMENT_TYPE>::Ptr& stream2Wrap) -> Ptr
            {
                return make_shared<Rep_> (stream2Wrap);
            }

            /*
             ********************************************************************************
             ************* InternallySyncrhonizedOutputStream<ELEMENT_TYPE>::Ptr ************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            inline InternallySyncrhonizedOutputStream<ELEMENT_TYPE>::Ptr::Ptr (const shared_ptr<Rep_>& from)
                : inherited (from)
            {
            }
            template <typename ELEMENT_TYPE>
            inline typename InternallySyncrhonizedOutputStream<ELEMENT_TYPE>::Ptr& InternallySyncrhonizedOutputStream<ELEMENT_TYPE>::Ptr::operator= (const InternallySyncrhonizedOutputStream<ELEMENT_TYPE>& rhs)
            {
                inherited::Ptr::operator= (rhs);
                return *this;
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_InternallySyncrhonizedOutputStream_inl_*/
