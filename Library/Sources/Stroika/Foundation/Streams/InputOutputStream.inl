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
     ***************** InputOutputStream::Ptr<ELEMENT_TYPE> *************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE>::Ptr (const shared_ptr<_IRep<ELEMENT_TYPE>>& rep)
        : InputStream::Ptr<ELEMENT_TYPE>{rep}
        , OutputStream::Ptr<ELEMENT_TYPE>{rep}
    {
        RequireNotNull (rep);
    }
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE>::Ptr (nullptr_t)
        : InputStream::Ptr<ELEMENT_TYPE>{nullptr}
        , OutputStream::Ptr<ELEMENT_TYPE>{nullptr}
    {
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::_GetSharedRep () const -> shared_ptr<_IRep<ELEMENT_TYPE>>
    {
        Ensure (dynamic_pointer_cast<_IRep<ELEMENT_TYPE>> (InputStream::Ptr<ELEMENT_TYPE>::_GetSharedRep ()) ==
                dynamic_pointer_cast<_IRep<ELEMENT_TYPE>> (OutputStream::Ptr<ELEMENT_TYPE>::_GetSharedRep ()));
        return Debug::UncheckedDynamicPointerCast<_IRep<ELEMENT_TYPE>> (InputStream::Ptr<ELEMENT_TYPE>::_GetSharedRep ());
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::_GetRepConstRef () const -> const _IRep<ELEMENT_TYPE>&
    {
        Ensure (dynamic_cast<const _IRep<ELEMENT_TYPE>*> (&InputStream::Ptr<ELEMENT_TYPE>::_GetRepConstRef ()) ==
                dynamic_cast<const _IRep<ELEMENT_TYPE>*> (&OutputStream::Ptr<ELEMENT_TYPE>::_GetRepConstRef ()));
        return *Debug::UncheckedDynamicCast<const _IRep<ELEMENT_TYPE>*> (&InputStream::Ptr<ELEMENT_TYPE>::_GetRepConstRef ());
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::_GetRepRWRef () const -> _IRep<ELEMENT_TYPE>&
    {
        Ensure (&typename InputStream::Ptr<ELEMENT_TYPE>::_GetRepRWRef () == &OutputStream::Ptr<ELEMENT_TYPE>::_GetRepRWRef ());
        return *Debug::UncheckedDynamicCast<_IRep<ELEMENT_TYPE>*> (&InputStream::Ptr<ELEMENT_TYPE>::_GetRepRWRef ());
    }
    template <typename ELEMENT_TYPE>
    inline bool Ptr<ELEMENT_TYPE>::IsSeekable () const
    {
        Require (InputStream::Ptr<ELEMENT_TYPE>::IsSeekable () == OutputStream::Ptr<ELEMENT_TYPE>::IsSeekable ());
        return InputStream::Ptr<ELEMENT_TYPE>::IsSeekable ();
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
        InputStream::Ptr<ELEMENT_TYPE>::Close ();
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::CloseRead (bool reset)
    {
        InputStream::Ptr<ELEMENT_TYPE>::Close (reset);
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::CloseWrite () const
    {
        OutputStream::Ptr<ELEMENT_TYPE>::Close ();
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::CloseWrite (bool reset)
    {
        OutputStream::Ptr<ELEMENT_TYPE>::Close (reset);
    }
    template <typename ELEMENT_TYPE>
    inline bool Ptr<ELEMENT_TYPE>::IsOpenRead () const
    {
        return InputStream::Ptr<ELEMENT_TYPE>::IsOpen ();
    }
    template <typename ELEMENT_TYPE>
    inline bool Ptr<ELEMENT_TYPE>::IsOpenWrite () const
    {
        return OutputStream::Ptr<ELEMENT_TYPE>::IsOpen ();
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType Ptr<ELEMENT_TYPE>::GetReadOffset () const
    {
        return InputStream::Ptr<ELEMENT_TYPE>::GetOffset ();
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType Ptr<ELEMENT_TYPE>::GetWriteOffset () const
    {
        return OutputStream::Ptr<ELEMENT_TYPE>::GetOffset ();
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType Ptr<ELEMENT_TYPE>::SeekWrite (SignedSeekOffsetType offset) const
    {
        return OutputStream::Ptr<ELEMENT_TYPE>::Seek (offset);
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType Ptr<ELEMENT_TYPE>::SeekWrite (Whence whence, SignedSeekOffsetType offset) const
    {
        return OutputStream::Ptr<ELEMENT_TYPE>::Seek (whence, offset);
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType Ptr<ELEMENT_TYPE>::SeekRead (SignedSeekOffsetType offset) const
    {
        return InputStream::Ptr<ELEMENT_TYPE>::Seek (offset);
    }
    template <typename ELEMENT_TYPE>
    inline SeekOffsetType Ptr<ELEMENT_TYPE>::SeekRead (Whence whence, SignedSeekOffsetType offset) const
    {
        return InputStream::Ptr<ELEMENT_TYPE>::Seek (whence, offset);
    }
    template <typename ELEMENT_TYPE>
    inline bool Ptr<ELEMENT_TYPE>::operator== (nullptr_t) const
    {
        Ensure (InputStream::Ptr<ELEMENT_TYPE>::operator== (nullptr) == OutputStream::Ptr<ELEMENT_TYPE>::operator== (nullptr));
        return InputStream::Ptr<ELEMENT_TYPE>::operator== (nullptr);
    }

}

#endif /*_Stroika_Foundation_Streams_InputOutputStream_inl_*/
