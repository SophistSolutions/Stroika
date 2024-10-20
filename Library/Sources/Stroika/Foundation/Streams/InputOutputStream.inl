/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Cast.h"

namespace Stroika::Foundation::Streams::InputOutputStream {

    /*
     ********************************************************************************
     ***************** InputOutputStream::Ptr<ELEMENT_TYPE> *************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE>::Ptr (const shared_ptr<IRep<ELEMENT_TYPE>>& rep)
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
    inline auto Ptr<ELEMENT_TYPE>::GetSharedRep () const -> shared_ptr<IRep<ELEMENT_TYPE>>
    {
        Ensure (dynamic_pointer_cast<IRep<ELEMENT_TYPE>> (InputStream::Ptr<ELEMENT_TYPE>::_GetSharedRep ()) ==
                dynamic_pointer_cast<IRep<ELEMENT_TYPE>> (OutputStream::Ptr<ELEMENT_TYPE>::GetSharedRep ()));
        return Debug::UncheckedDynamicPointerCast<IRep<ELEMENT_TYPE>> (InputStream::Ptr<ELEMENT_TYPE>::GetSharedRep ());
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::GetRepConstRef () const -> const IRep<ELEMENT_TYPE>&
    {
        Ensure (dynamic_cast<const IRep<ELEMENT_TYPE>*> (&InputStream::Ptr<ELEMENT_TYPE>::GetRepConstRef ()) ==
                dynamic_cast<const IRep<ELEMENT_TYPE>*> (&OutputStream::Ptr<ELEMENT_TYPE>::GetRepConstRef ()));
        return *Debug::UncheckedDynamicCast<const IRep<ELEMENT_TYPE>*> (&InputStream::Ptr<ELEMENT_TYPE>::GetRepConstRef ());
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::GetRepRWRef () const -> IRep<ELEMENT_TYPE>&
    {
        Ensure (&typename InputStream::Ptr<ELEMENT_TYPE>::GetRepRWRef () == &OutputStream::Ptr<ELEMENT_TYPE>::GetRepRWRef ());
        return *Debug::UncheckedDynamicCast<IRep<ELEMENT_TYPE>*> (&InputStream::Ptr<ELEMENT_TYPE>::GetRepRWRef ());
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
