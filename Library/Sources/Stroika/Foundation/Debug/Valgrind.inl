/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Debug_Valgrind_inl_
#define _Stroika_Foundation_Debug_Valgrind_inl_ 1

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

#endif /*_Stroika_Foundation_Debug_Valgrind_inl_*/
