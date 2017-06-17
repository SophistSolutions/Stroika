/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_Stream_inl_
#define _Stroika_Foundation_Streams_Stream_inl_ 1

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
             ************************* Stream<ELEMENT_TYPE>::Ptr ****************************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            inline Stream<ELEMENT_TYPE>::Ptr::Ptr (const _SharedIRep& rep)
                : fRep_ ((RequireNotNull (rep), rep))
                , fSeekable_ (rep->IsSeekable ())
            {
                RequireNotNull (rep);
            }
            template <typename ELEMENT_TYPE>
            inline Stream<ELEMENT_TYPE>::Ptr::Ptr (nullptr_t)
                : fRep_ ()
                , fSeekable_ (false)
            {
            }
            template <typename ELEMENT_TYPE>
            inline auto Stream<ELEMENT_TYPE>::Ptr::_GetSharedRep () const -> _SharedIRep
            {
                return fRep_;
            }
            template <typename ELEMENT_TYPE>
            inline auto Stream<ELEMENT_TYPE>::Ptr::_GetRepConstRef () const -> const _IRep&
            {
                RequireNotNull (fRep_);
                return *fRep_.get ();
            }
            template <typename ELEMENT_TYPE>
            inline auto Stream<ELEMENT_TYPE>::Ptr::_GetRepRWRef () const -> _IRep&
            {
                RequireNotNull (fRep_);
                return *fRep_.get ();
            }
            template <typename ELEMENT_TYPE>
            inline bool Stream<ELEMENT_TYPE>::Ptr::IsSeekable () const
            {
                return fSeekable_;
            }
            template <typename ELEMENT_TYPE>
            inline bool Stream<ELEMENT_TYPE>::Ptr::empty () const
            {
                return fRep_.get () == nullptr;
            }
            template <typename ELEMENT_TYPE>
            inline void Stream<ELEMENT_TYPE>::Ptr::clear ()
            {
                fRep_.reset ();
            }

            template <typename ELEMENT_TYPE>
            inline bool operator== (const typename Stream<ELEMENT_TYPE>::Ptr& s, nullptr_t)
            {
                return s.empty ();
            }
            template <typename ELEMENT_TYPE>
            inline bool operator== (nullptr_t, const typename Stream<ELEMENT_TYPE>::Ptr& s)
            {
                return s.empty ();
            }
            template <typename ELEMENT_TYPE>
            inline bool operator!= (const typename Stream<ELEMENT_TYPE>::Ptr& s, nullptr_t)
            {
                return not s.empty ();
            }
            template <typename ELEMENT_TYPE>
            inline bool operator!= (nullptr_t, const typename Stream<ELEMENT_TYPE>::Ptr& s)
            {
                return not s.empty ();
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_Stream_inl_*/
