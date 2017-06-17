/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputOutputStream_inl_
#define _Stroika_Foundation_Streams_InputOutputStream_inl_ 1

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
             ********************** InputOutputStream<ELEMENT_TYPE> *************************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            inline InputOutputStream<ELEMENT_TYPE>::InputOutputStream (const _SharedIRep& rep)
                : InputStream<ELEMENT_TYPE>::Ptr (rep)
                , OutputStream<ELEMENT_TYPE>::Ptr (rep)
            {
                RequireNotNull (rep);
            }
            template <typename ELEMENT_TYPE>
            inline InputOutputStream<ELEMENT_TYPE>::InputOutputStream (nullptr_t)
                : InputStream<ELEMENT_TYPE>::Ptr (nullptr)
                , OutputStream<ELEMENT_TYPE>::Ptr (nullptr)
            {
            }
            template <typename ELEMENT_TYPE>
            inline auto InputOutputStream<ELEMENT_TYPE>::_GetSharedRep () const -> _SharedIRep
            {
                Ensure (dynamic_pointer_cast<_IRep> (InputStream<ELEMENT_TYPE>::Ptr::_GetSharedRep ()) == dynamic_pointer_cast<_IRep> (OutputStream<ELEMENT_TYPE>::Ptr::_GetSharedRep ()));
                return dynamic_pointer_cast<_IRep> (InputStream<ELEMENT_TYPE>::Ptr::_GetSharedRep ());
            }
            template <typename ELEMENT_TYPE>
            inline auto InputOutputStream<ELEMENT_TYPE>::_GetRepConstRef () const -> const _IRep&
            {
                Ensure (dynamic_cast<const _IRep*> (&InputStream<ELEMENT_TYPE>::Ptr::_GetRepConstRef ()) == dynamic_cast<const _IRep*> (&OutputStream<ELEMENT_TYPE>::Ptr::_GetRepConstRef ()));
                EnsureMember (&InputStream<ELEMENT_TYPE>::Ptr::_GetRepConstRef (), _IRep);
                return *reinterpret_cast<const _IRep*> (&InputStream<ELEMENT_TYPE>::Ptr::_GetRepConstRef ()); // faster than dynamic_cast, and if not equivilent, add caching later here
            }
            template <typename ELEMENT_TYPE>
            inline auto InputOutputStream<ELEMENT_TYPE>::_GetRepRWRef () const -> _IRep&
            {
                Ensure (&typename InputStream<ELEMENT_TYPE>::Ptr::_GetRepRWRef () == &OutputStream<ELEMENT_TYPE>::Ptr::_GetRepRWRef ());
                EnsureMember (&InputStream<ELEMENT_TYPE>::Ptr::_GetRepRWRef (), _IRep);
                return *reinterpret_cast<_IRep*> (&InputStream<ELEMENT_TYPE>::Ptr::_GetRepRWRef ()); // faster than dynamic_cast, and if not equivilent, add caching later here
            }
            template <typename ELEMENT_TYPE>
            inline bool InputOutputStream<ELEMENT_TYPE>::empty () const
            {
                Ensure (InputStream<ELEMENT_TYPE>::Ptr::empty () == OutputStream<ELEMENT_TYPE>::Ptr::empty ());
                return InputStream<ELEMENT_TYPE>::Ptr::empty ();
            }
            template <typename ELEMENT_TYPE>
            inline bool InputOutputStream<ELEMENT_TYPE>::IsSeekable () const
            {
                Require (typename InputStream<ELEMENT_TYPE>::Ptr::IsSeekable () == OutputStream<ELEMENT_TYPE>::Ptr::IsSeekable ());
                return typename InputStream<ELEMENT_TYPE>::Ptr::IsSeekable ();
            }
            template <typename ELEMENT_TYPE>
            inline SeekOffsetType InputOutputStream<ELEMENT_TYPE>::GetReadOffset () const
            {
                return InputStream<ELEMENT_TYPE>::Ptr::GetOffset ();
            }
            template <typename ELEMENT_TYPE>
            inline SeekOffsetType InputOutputStream<ELEMENT_TYPE>::GetWriteOffset () const
            {
                return OutputStream<ELEMENT_TYPE>::Ptr::GetOffset ();
            }
            template <typename ELEMENT_TYPE>
            inline SeekOffsetType InputOutputStream<ELEMENT_TYPE>::SeekWrite (SignedSeekOffsetType offset) const
            {
                return OutputStream<ELEMENT_TYPE>::Ptr::Seek (offset);
            }
            template <typename ELEMENT_TYPE>
            inline SeekOffsetType InputOutputStream<ELEMENT_TYPE>::SeekWrite (Whence whence, SignedSeekOffsetType offset) const
            {
                return OutputStream<ELEMENT_TYPE>::Ptr::Seek (whence, offset);
            }
            template <typename ELEMENT_TYPE>
            inline SeekOffsetType InputOutputStream<ELEMENT_TYPE>::SeekRead (SignedSeekOffsetType offset) const
            {
                return InputStream<ELEMENT_TYPE>::Ptr::Seek (offset);
            }
            template <typename ELEMENT_TYPE>
            inline SeekOffsetType InputOutputStream<ELEMENT_TYPE>::SeekRead (Whence whence, SignedSeekOffsetType offset) const
            {
                return InputStream<ELEMENT_TYPE>::Ptr::Seek (whence, offset);
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_InputOutputStream_inl_*/
