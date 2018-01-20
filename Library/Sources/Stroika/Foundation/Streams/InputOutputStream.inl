/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
             ***************** InputOutputStream<ELEMENT_TYPE>::Ptr *************************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            inline InputOutputStream<ELEMENT_TYPE>::Ptr::Ptr (const _SharedIRep& rep)
                : InputStream<ELEMENT_TYPE>::Ptr (rep)
                , OutputStream<ELEMENT_TYPE>::Ptr (rep)
            {
                RequireNotNull (rep);
            }
            template <typename ELEMENT_TYPE>
            inline InputOutputStream<ELEMENT_TYPE>::Ptr::Ptr (nullptr_t)
                : InputStream<ELEMENT_TYPE>::Ptr (nullptr)
                , OutputStream<ELEMENT_TYPE>::Ptr (nullptr)
            {
            }
            template <typename ELEMENT_TYPE>
            inline auto InputOutputStream<ELEMENT_TYPE>::Ptr::_GetSharedRep () const -> _SharedIRep
            {
                Ensure (dynamic_pointer_cast<_IRep> (InputStream<ELEMENT_TYPE>::Ptr::_GetSharedRep ()) == dynamic_pointer_cast<_IRep> (OutputStream<ELEMENT_TYPE>::Ptr::_GetSharedRep ()));
                return dynamic_pointer_cast<_IRep> (InputStream<ELEMENT_TYPE>::Ptr::_GetSharedRep ());
            }
            template <typename ELEMENT_TYPE>
            inline auto InputOutputStream<ELEMENT_TYPE>::Ptr::_GetRepConstRef () const -> const _IRep&
            {
                Ensure (dynamic_cast<const _IRep*> (&InputStream<ELEMENT_TYPE>::Ptr::_GetRepConstRef ()) == dynamic_cast<const _IRep*> (&OutputStream<ELEMENT_TYPE>::Ptr::_GetRepConstRef ()));
                EnsureMember (&InputStream<ELEMENT_TYPE>::Ptr::_GetRepConstRef (), _IRep);
                return *reinterpret_cast<const _IRep*> (&InputStream<ELEMENT_TYPE>::Ptr::_GetRepConstRef ()); // faster than dynamic_cast, and if not equivilent, add caching later here
            }
            template <typename ELEMENT_TYPE>
            inline auto InputOutputStream<ELEMENT_TYPE>::Ptr::_GetRepRWRef () const -> _IRep&
            {
                Ensure (&typename InputStream<ELEMENT_TYPE>::Ptr::_GetRepRWRef () == &OutputStream<ELEMENT_TYPE>::Ptr::_GetRepRWRef ());
                EnsureMember (&InputStream<ELEMENT_TYPE>::Ptr::_GetRepRWRef (), _IRep);
                return *reinterpret_cast<_IRep*> (&InputStream<ELEMENT_TYPE>::Ptr::_GetRepRWRef ()); // faster than dynamic_cast, and if not equivilent, add caching later here
            }
            template <typename ELEMENT_TYPE>
            inline bool InputOutputStream<ELEMENT_TYPE>::Ptr::IsSeekable () const
            {
                Require (InputStream<ELEMENT_TYPE>::Ptr::IsSeekable () == OutputStream<ELEMENT_TYPE>::Ptr::IsSeekable ());
                return InputStream<ELEMENT_TYPE>::Ptr::IsSeekable ();
            }
            template <typename ELEMENT_TYPE>
            inline void InputOutputStream<ELEMENT_TYPE>::Ptr::Close () const
            {
                CloseRead ();
                CloseWrite ();
            }
            template <typename ELEMENT_TYPE>
            inline void InputOutputStream<ELEMENT_TYPE>::Ptr::Close (bool reset)
            {
                CloseRead (reset);
                CloseWrite (reset);
            }
            template <typename ELEMENT_TYPE>
            inline void InputOutputStream<ELEMENT_TYPE>::Ptr::CloseRead () const
            {
                InputStream<ELEMENT_TYPE>::Ptr::Close ();
            }
            template <typename ELEMENT_TYPE>
            inline void InputOutputStream<ELEMENT_TYPE>::Ptr::CloseRead (bool reset)
            {
                InputStream<ELEMENT_TYPE>::Ptr::Close (reset);
            }
            template <typename ELEMENT_TYPE>
            inline void InputOutputStream<ELEMENT_TYPE>::Ptr::CloseWrite () const
            {
                OutputStream<ELEMENT_TYPE>::Ptr::Close ();
            }
            template <typename ELEMENT_TYPE>
            inline void InputOutputStream<ELEMENT_TYPE>::Ptr::CloseWrite (bool reset)
            {
                OutputStream<ELEMENT_TYPE>::Ptr::Close (reset);
            }
            template <typename ELEMENT_TYPE>
            inline SeekOffsetType InputOutputStream<ELEMENT_TYPE>::Ptr::GetReadOffset () const
            {
                return InputStream<ELEMENT_TYPE>::Ptr::GetOffset ();
            }
            template <typename ELEMENT_TYPE>
            inline SeekOffsetType InputOutputStream<ELEMENT_TYPE>::Ptr::GetWriteOffset () const
            {
                return OutputStream<ELEMENT_TYPE>::Ptr::GetOffset ();
            }
            template <typename ELEMENT_TYPE>
            inline SeekOffsetType InputOutputStream<ELEMENT_TYPE>::Ptr::SeekWrite (SignedSeekOffsetType offset) const
            {
                return OutputStream<ELEMENT_TYPE>::Ptr::Seek (offset);
            }
            template <typename ELEMENT_TYPE>
            inline SeekOffsetType InputOutputStream<ELEMENT_TYPE>::Ptr::SeekWrite (Whence whence, SignedSeekOffsetType offset) const
            {
                return OutputStream<ELEMENT_TYPE>::Ptr::Seek (whence, offset);
            }
            template <typename ELEMENT_TYPE>
            inline SeekOffsetType InputOutputStream<ELEMENT_TYPE>::Ptr::SeekRead (SignedSeekOffsetType offset) const
            {
                return InputStream<ELEMENT_TYPE>::Ptr::Seek (offset);
            }
            template <typename ELEMENT_TYPE>
            inline SeekOffsetType InputOutputStream<ELEMENT_TYPE>::Ptr::SeekRead (Whence whence, SignedSeekOffsetType offset) const
            {
                return InputStream<ELEMENT_TYPE>::Ptr::Seek (whence, offset);
            }
            template <typename ELEMENT_TYPE>
            inline bool InputOutputStream<ELEMENT_TYPE>::Ptr::operator== (nullptr_t) const
            {
                Ensure (InputStream<ELEMENT_TYPE>::Ptr::operator== (nullptr) == OutputStream<ELEMENT_TYPE>::Ptr::operator== (nullptr));
                return InputStream<ELEMENT_TYPE>::Ptr::operator== (nullptr);
            }
            template <typename ELEMENT_TYPE>
            inline bool InputOutputStream<ELEMENT_TYPE>::Ptr::operator!= (nullptr_t) const
            {
                Ensure (InputStream<ELEMENT_TYPE>::Ptr::operator!= (nullptr) == OutputStream<ELEMENT_TYPE>::Ptr::operator!= (nullptr));
                return InputStream<ELEMENT_TYPE>::Ptr::operator!= (nullptr);
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_InputOutputStream_inl_*/
