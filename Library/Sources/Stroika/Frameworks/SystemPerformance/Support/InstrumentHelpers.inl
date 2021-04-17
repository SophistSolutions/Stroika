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
        : fOptions_{options}
        , _fContext{context}
    {
    }
    template <typename OPTIONS>
    optional<DurationSecondsType> CapturerWithContext_COMMON<OPTIONS>::GetCaptureContextTime () const
    {
        return _fContext.cget ().cref ()->fCaptureContextAt;
    }
    template <typename OPTIONS>
    bool CapturerWithContext_COMMON<OPTIONS>::_NoteCompletedCapture (DurationSecondsType at)
    {
        if (not _fContext.rwget ().rwref ()->fCaptureContextAt.has_value () or (at - *_fContext.rwget ().rwref ()->fCaptureContextAt) >= fOptions_.fMinimumAveragingInterval) {
            _fContext.rwget ().rwref ()->fCaptureContextAt = at;
            return true;
        }
        return false;
    }

}
#endif /*_Stroika_Framework_SystemPerformance_Support_InstrumentHelpers_inl_*/
