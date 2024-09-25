/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Streams {

    /*
     ********************************************************************************
     ********************************* Ptr<ELEMENT_TYPE> ****************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE>::Ptr (const shared_ptr<IRep<ELEMENT_TYPE>>& rep) noexcept
        : fRep_{(RequireExpression (rep != nullptr), rep)}
        , fSeekable_{rep->IsSeekable ()}
    {
        RequireNotNull (rep);
    }
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE>::Ptr (nullptr_t) noexcept
    {
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::GetSharedRep () const -> shared_ptr<IRep<ELEMENT_TYPE>>
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fThisAssertExternallySynchronized};
        return fRep_;
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::GetRepConstRef () const -> const IRep<ELEMENT_TYPE>&
    {
        RequireNotNull (fRep_);
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fThisAssertExternallySynchronized};
        return *fRep_.get ();
    }
    template <typename ELEMENT_TYPE>
    inline auto Ptr<ELEMENT_TYPE>::GetRepRWRef () const -> IRep<ELEMENT_TYPE>&
    {
        RequireNotNull (fRep_);
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fThisAssertExternallySynchronized};
        return *fRep_.get ();
    }
    template <typename ELEMENT_TYPE>
    inline bool Ptr<ELEMENT_TYPE>::IsSeekable () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fThisAssertExternallySynchronized};
        return fSeekable_;
    }
    template <typename ELEMENT_TYPE>
    inline void Ptr<ELEMENT_TYPE>::reset () noexcept
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
        fRep_.reset ();
    }
    template <typename ELEMENT_TYPE>
    inline bool Ptr<ELEMENT_TYPE>::operator== (nullptr_t) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fThisAssertExternallySynchronized};
        return fRep_.get () == nullptr;
    }
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE>::operator bool () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fThisAssertExternallySynchronized};
        return fRep_.get () != nullptr;
    }

}

namespace Stroika::Foundation::Common {
    template <>
    constexpr EnumNames<Streams::SeekableFlag> DefaultNames<Streams::SeekableFlag>::k{{{
        {Streams::SeekableFlag::eNotSeekable, L"Not-Seekable"},
        {Streams::SeekableFlag::eSeekable, L"Seekable"},
    }}};
}
