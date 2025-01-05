/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

namespace Stroika::Frameworks::SystemPerformance::Support {

    /*
     ********************************************************************************
     ******************** SystemPerformance::Support::WMICollector ******************
     ********************************************************************************
     */
    inline optional<Foundation::Time::TimePointSeconds> WMICollector::GetTimeOfLastCollection () const
    {
        return fTimeOfLastCollection_;
    }

}
