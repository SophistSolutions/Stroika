/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Frameworks::SystemPerformance {

    /*
     ********************************************************************************
     *********************** SystemPerformance::CaptureSet **************************
     ********************************************************************************
     */
    inline void CaptureSet::AddInstrument (const Instrument& i)
    {
        fInstruments_.Add (i);
    }

}
