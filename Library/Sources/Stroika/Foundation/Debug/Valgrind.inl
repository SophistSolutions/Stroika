/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#if defined(__cplusplus)
namespace Stroika::Foundation::Debug {

    /*
     ********************************************************************************
     ***************************** IsRunningUnderValgrind ***************************
     ********************************************************************************
     */
    inline bool IsRunningUnderValgrind ()
    {
#if qStroika_FeatureSupported_Valgrind
        return RUNNING_ON_VALGRIND;
#else
        return false;
#endif
    }

}
#endif
