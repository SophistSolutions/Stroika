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
     ***** SystemPerformance::Support::InstrumentHelpers::InstrumentRep_COMMON ******
     ********************************************************************************
     */
    template <typename OPTIONS, typename CONTEXT>
    inline InstrumentRep_COMMON<OPTIONS, CONTEXT>::InstrumentRep_COMMON (const OPTIONS& options, const shared_ptr<CONTEXT>& context)
        : _fOptions{options}
        , _fContext{context}
    {
        RequireNotNull (_fContext.cget ().cref ());
    }
    template <typename OPTIONS, typename CONTEXT>
    inline optional<DurationSecondsType> InstrumentRep_COMMON<OPTIONS, CONTEXT>::_GetCaptureContextTime () const
    {
        AssertNotNull (_fContext.cget ().cref ());
        return _fContext.cget ().cref ()->fCaptureContextAt;
    }
    template <typename OPTIONS, typename CONTEXT>
    bool InstrumentRep_COMMON<OPTIONS, CONTEXT>::_NoteCompletedCapture (DurationSecondsType at)
    {
        AssertNotNull (_fContext.cget ().cref ());
        if (not _fContext.rwget ().rwref ()->fCaptureContextAt.has_value () or (at - *_fContext.rwget ().rwref ()->fCaptureContextAt) >= _fOptions.fMinimumAveragingInterval) {
            _fContext.rwget ().rwref ()->fCaptureContextAt = at;
            return true;
        }
        return false;
    }

}
#endif /*_Stroika_Framework_SystemPerformance_Support_InstrumentHelpers_inl_*/
