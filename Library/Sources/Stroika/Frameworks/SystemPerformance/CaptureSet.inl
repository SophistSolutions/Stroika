/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Frameworks_SystemPerformance_CaptureSet_inl_
#define _Stroika_Frameworks_SystemPerformance_CaptureSet_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::SystemPerformance {

    /*
     ********************************************************************************
     *********************** SystemPerformance::CaptureSet **************************
     ********************************************************************************
     */
    inline Duration CaptureSet::GetRunPeriod () const
    {
        return fPeriod_;
    }
    inline void CaptureSet::SetRunPeriod (const Duration& p)
    {
        fPeriod_ = p;
    }
    inline Set<Instrument> CaptureSet::GetInstrumentSet () const
    {
        return fInstruments_;
    }
    inline void CaptureSet::SetInstrumentSet (const Set<Instrument>& i)
    {
        fInstruments_ = i;
    }
    inline void CaptureSet::AddInstrument (const Instrument& i)
    {
        fInstruments_.Add (i);
    }

}

#endif /*_Stroika_Frameworks_SystemPerformance_CaptureSet_inl_*/
