/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Support_InstrumentHelpers_inl_
#define _Stroika_Framework_SystemPerformance_Support_InstrumentHelpers_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::SystemPerformance::Support {

    /*
     ********************************************************************************
     ***************** SystemPerformance::Support::InstrumentHelpers ****************
     ********************************************************************************
     */
    template <typename OPTIONS>
    inline CapturerWithContext_COMMON<OPTIONS>::CapturerWithContext_COMMON (const OPTIONS& options, const shared_ptr<_Context>& context)
        : _fOptions{options}
        , _fContext{context}
    {
    }
    template <typename OPTIONS>
    template <typename T_SUBCLASS>
    inline const shared_ptr<T_SUBCLASS> CapturerWithContext_COMMON<OPTIONS>::cContextPtr (typename Foundation::Execution::Synchronized<shared_ptr<_Context>>::ReadableReference&& r)
    {
        return dynamic_pointer_cast<T_SUBCLASS> (r.cref ());
    }
    template <typename OPTIONS>
    template <typename T_SUBCLASS>
    inline  shared_ptr<T_SUBCLASS> CapturerWithContext_COMMON<OPTIONS>::rwContextPtr (typename Foundation::Execution::Synchronized<shared_ptr<_Context>>::WritableReference&& r)
    {
        return dynamic_pointer_cast<T_SUBCLASS> (r.rwref ());
    }
    template <typename OPTIONS>
    inline optional<DurationSecondsType> CapturerWithContext_COMMON<OPTIONS>::_GetCaptureContextTime () const
    {
        return _fContext.cget ().cref ()->fCaptureContextAt;
    }
    template <typename OPTIONS>
    bool CapturerWithContext_COMMON<OPTIONS>::_NoteCompletedCapture (DurationSecondsType at)
    {
        if (not _fContext.rwget ().rwref ()->fCaptureContextAt.has_value () or (at - *_fContext.rwget ().rwref ()->fCaptureContextAt) >= _fOptions.fMinimumAveragingInterval) {
            _fContext.rwget ().rwref ()->fCaptureContextAt = at;
            return true;
        }
        return false;
    }

}
#endif /*_Stroika_Framework_SystemPerformance_Support_InstrumentHelpers_inl_*/
