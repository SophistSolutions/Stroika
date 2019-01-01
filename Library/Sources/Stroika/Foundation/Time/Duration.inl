/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Duration_inl_
#define _Stroika_Foundation_Time_Duration_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Execution/Exceptions.h"

namespace Stroika::Foundation::Time {

    /*
     ********************************************************************************
     ************************************ Duration **********************************
     ********************************************************************************
     */
    template <typename DURATION_REP, typename DURATION_PERIOD>
    Duration::Duration (const chrono::duration<DURATION_REP, DURATION_PERIOD>& d)
        : fDurationRep_ (UnParseTime_ (static_cast<InternalNumericFormatType_> (chrono::duration<InternalNumericFormatType_> (d).count ())))
    {
    }
    inline string Duration::AsUTF8 () const
    {
        return As<Characters::String> ().AsUTF8 ();
    }

    /*
     ********************************************************************************
     ************************** Duration operators **********************************
     ********************************************************************************
     */
    inline bool operator< (const Duration& lhs, const Duration& rhs)
    {
        return lhs.Compare (rhs) < 0;
    }
    inline bool operator<= (const Duration& lhs, const Duration& rhs)
    {
        return lhs.Compare (rhs) <= 0;
    }
    inline bool operator== (const Duration& lhs, const Duration& rhs)
    {
        return lhs.Compare (rhs) == 0;
    }
    inline bool operator!= (const Duration& lhs, const Duration& rhs)
    {
        return lhs.Compare (rhs) != 0;
    }
    inline bool operator>= (const Duration& lhs, const Duration& rhs)
    {
        return lhs.Compare (rhs) >= 0;
    }
    inline bool operator> (const Duration& lhs, const Duration& rhs)
    {
        return lhs.Compare (rhs) > 0;
    }
    inline Duration operator/ (const Duration& lhs, long double rhs)
    {
        Require (rhs != 0);
        return lhs * (1 / rhs);
    }

    namespace Private_ {

        struct Duration_ModuleData_ {
            Duration_ModuleData_ ();
            Duration fMin;
            Duration fMax;
        };
    }

}

namespace {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Time::Private_::Duration_ModuleData_> _Stroika_Foundation_Time_Duration_ModuleData_; // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif /*_Stroika_Foundation_Time_Duration_inl_*/
