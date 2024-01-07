/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
    template <typename OPTIONS, derived_from<Context> CONTEXT>
    inline InstrumentRep_COMMON<OPTIONS, CONTEXT>::InstrumentRep_COMMON (const OPTIONS& options, const shared_ptr<CONTEXT>& context)
        : _fOptions{options}
        , _fContext{context}
    {
        RequireNotNull (_fContext.cget ().cref ());
    }
    template <typename OPTIONS, derived_from<Context> CONTEXT>
    inline Time::TimePointSeconds InstrumentRep_COMMON<OPTIONS, CONTEXT>::_GetCaptureContextTime () const
    {
        AssertNotNull (_fContext.cget ().cref ());
        return _fContext.cget ().cref ()->fCaptureContextAt.value_or (fInstrumentCreatedAt_);
    }
    template <typename OPTIONS, derived_from<Context> CONTEXT>
    void InstrumentRep_COMMON<OPTIONS, CONTEXT>::_NoteCompletedCapture (Time::TimePointSeconds at)
    {
        AssertNotNull (_fContext.cget ().cref ());
        _fContext.rwget ().rwref ()->fCaptureContextAt = at;
    }

}
#endif /*_Stroika_Framework_SystemPerformance_Support_InstrumentHelpers_inl_*/
