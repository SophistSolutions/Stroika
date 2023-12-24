/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputOutputStream_inl_
#define _Stroika_Foundation_Streams_InputOutputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/Cast.h"

namespace Stroika::Foundation::Streams::InputOutputStream {

    /*
     ********************************************************************************
     ***************** InputOutputStream<ELEMENT_TYPE>::Ptr *************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE>::Ptr (const shared_ptr<_IRep<ELEMENT_TYPE>>& rep)
        : InputStream<ELEMENT_TYPE>::Ptr{rep}
        , OutputStream<ELEMENT_TYPE>::Ptr{rep}
    {
        RequireNotNull (rep);
    }
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE>::Ptr (nullptr_t)
        : InputStream<ELEMENT_TYPE>::Ptr{nullptr}
        , OutputStream<ELEMENT_TYPE>::Ptr{nullptr}
    {
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::_GetSharedRep () const -> shared_ptr<_IRep<ELEMENT_TYPE>>
    {
        Ensure (dynamic_pointer_cast<_IRep<ELEMENT_TYPE>> (InputStream<ELEMENT_TYPE>::Ptr::_GetSharedRep ()) ==
                dynamic_pointer_cast<_IRep<ELEMENT_TYPE>> (OutputStream<ELEMENT_TYPE>::Ptr::_GetSharedRep ()));
        return Debug::UncheckedDynamicPointerCast<_IRep<ELEMENT_TYPE>> (InputStream<ELEMENT_TYPE>::Ptr::_GetSharedRep ());
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::_GetRepConstRef () const -> const _IRep<ELEMENT_TYPE>&
    {
        Ensure (dynamic_cast<const _IRep<ELEMENT_TYPE>*> (&InputStream<ELEMENT_TYPE>::Ptr::_GetRepConstRef ()) ==
                dynamic_cast<const _IRep<ELEMENT_TYPE>*> (&OutputStream<ELEMENT_TYPE>::Ptr::_GetRepConstRef ()));
        return *Debug::UncheckedDynamicCast<const _IRep<ELEMENT_TYPE>*> (&InputStream<ELEMENT_TYPE>::Ptr::_GetRepConstRef ());
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::_GetRepRWRef () const -> _IRep<ELEMENT_TYPE>&
    {
        Ensure (&typename InputStream<ELEMENT_TYPE>::Ptr::_GetRepRWRef () == &OutputStream<ELEMENT_TYPE>::Ptr::_GetRepRWRef ());
        return *Debug::UncheckedDynamicCast<_IRep<ELEMENT_TYPE>*> (&InputStream<ELEMENT_TYPE>::Ptr::_GetRepRWRef ());
    }
    template <typename ELEMENT_TYPE>
    inline bool Ptr<ELEMENT_TYPE>::IsSeekable () const
    {
        Require (InputStream<ELEMENT_TYPE>::Ptr::IsSeekable () == OutputStream<ELEMENT_TYPE>::Ptr::IsSeekable ());
        return InputStream<ELEMENT_TYPE>::Ptr::IsSeekable ();
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::Close () const
    {
        CloseRead ();
        CloseWrite ();
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::Close (bool reset)
    {
        CloseRead (reset);
        CloseWrite (reset);
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::CloseRead () const
    {
        InputStream<ELEMENT_TYPE>::Ptr::Close ();
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::CloseRead (bool reset)
    {
        InputStream<ELEMENT_TYPE>::Ptr::Close (reset);
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::CloseWrite () const
    {
        OutputStream<ELEMENT_TYPE>::Ptr::Close ();
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::CloseWrite (bool reset)
    {
        OutputStream<ELEMENT_TYPE>::Ptr::Close (reset);
    }
    template <typename ELEMENT_TYPE>
    inline bool Ptr<ELEMENT_TYPE>::IsOpenRead () const
    {
        return InputStream<ELEMENT_TYPE>::Ptr::IsOpen ();
    }
    template <typename ELEMENT_TYPE>
    inline bool Ptr<ELEMENT_TYPE>::IsOpenWrite () const
    {
        return OutputStream<ELEMENT_TYPE>::Ptr::IsOpen ();
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType Ptr<ELEMENT_TYPE>::GetReadOffset () const
    {
        return InputStream<ELEMENT_TYPE>::Ptr::GetOffset ();
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType Ptr<ELEMENT_TYPE>::GetWriteOffset () const
    {
        return OutputStream<ELEMENT_TYPE>::Ptr::GetOffset ();
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType Ptr<ELEMENT_TYPE>::SeekWrite (SignedSeekOffsetType offset) const
    {
        return OutputStream<ELEMENT_TYPE>::Ptr::Seek (offset);
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType Ptr<ELEMENT_TYPE>::SeekWrite (Whence whence, SignedSeekOffsetType offset) const
    {
        return OutputStream<ELEMENT_TYPE>::Ptr::Seek (whence, offset);
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType Ptr<ELEMENT_TYPE>::SeekRead (SignedSeekOffsetType offset) const
    {
        return InputStream<ELEMENT_TYPE>::Ptr::Seek (offset);
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType Ptr<ELEMENT_TYPE>::SeekRead (Whence whence, SignedSeekOffsetType offset) const
    {
        return InputStream<ELEMENT_TYPE>::Ptr::Seek (whence, offset);
    }
    template <typename ELEMENT_TYPE>
    inline bool Ptr<ELEMENT_TYPE>::operator== (nullptr_t) const
    {
        Ensure (InputStream<ELEMENT_TYPE>::Ptr::operator== (nullptr) == OutputStream<ELEMENT_TYPE>::Ptr::operator== (nullptr));
        return InputStream<ELEMENT_TYPE>::Ptr::operator== (nullptr);
    }

}

#endif /*_Stroika_Foundation_Streams_InputOutputStream_inl_*/
