/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_Stream_inl_
#define _Stroika_Foundation_Streams_Stream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Streams {

    /*
     ********************************************************************************
     ************************* Stream<ELEMENT_TYPE>::Ptr ****************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline Stream<ELEMENT_TYPE>::Ptr::Ptr (const _SharedIRep& rep) noexcept
        : fRep_{(RequireNotNull (rep), rep)}
        , fSeekable_{rep->IsSeekable ()}
    {
        RequireNotNull (rep);
    }
    template <typename ELEMENT_TYPE>
    inline Stream<ELEMENT_TYPE>::Ptr::Ptr (nullptr_t) noexcept
    {
    }
    template <typename ELEMENT_TYPE>
    inline auto Stream<ELEMENT_TYPE>::Ptr::_GetSharedRep () const -> _SharedIRep
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return fRep_;
    }
    template <typename ELEMENT_TYPE>
    inline auto Stream<ELEMENT_TYPE>::Ptr::_GetRepConstRef () const -> const _IRep&
    {
        RequireNotNull (fRep_);
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return *fRep_.get ();
    }
    template <typename ELEMENT_TYPE>
    inline auto Stream<ELEMENT_TYPE>::Ptr::_GetRepRWRef () const -> _IRep&
    {
        RequireNotNull (fRep_);
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return *fRep_.get ();
    }
    template <typename ELEMENT_TYPE>
    inline bool Stream<ELEMENT_TYPE>::Ptr::IsSeekable () const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return fSeekable_;
    }
    template <typename ELEMENT_TYPE>
    inline SeekableFlag Stream<ELEMENT_TYPE>::Ptr::GetSeekability () const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return fSeekable_ ? SeekableFlag::eSeekable : SeekableFlag::eNotSeekable;
    }
    template <typename ELEMENT_TYPE>
    inline void Stream<ELEMENT_TYPE>::Ptr::reset () noexcept
    {
        AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        fRep_.reset ();
    }
    template <typename ELEMENT_TYPE>
    inline bool Stream<ELEMENT_TYPE>::Ptr::operator== (nullptr_t) const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return fRep_.get () == nullptr;
    }
    template <typename ELEMENT_TYPE>
    inline Stream<ELEMENT_TYPE>::Ptr::operator bool () const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return fRep_.get () != nullptr;
    }

}

namespace Stroika::Foundation::Configuration {
#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Streams::SeekableFlag> DefaultNames<Streams::SeekableFlag>::k{
        EnumNames<Streams::SeekableFlag>::BasicArrayInitializer{{
            {Streams::SeekableFlag::eSeekable, L"Seekable"},
            {Streams::SeekableFlag::eNotSeekable, L"Not-Seekable"},
        }}};
}

#endif /*_Stroika_Foundation_Streams_Stream_inl_*/
