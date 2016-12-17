/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Timezone_inl_
#define _Stroika_Foundation_Time_Timezone_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Time {


            /*
             ********************************************************************************
             *********************************** Timezone ***********************************
             ********************************************************************************
             */
#if     !qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
            constexpr   const   Timezone                    Timezone::kUTC { Timezone::TZ_::eUTC };
            constexpr   const   Timezone                    Timezone::kLocalTime { Timezone::TZ_::eLocalTime };
            constexpr   const   Memory::Optional<Timezone>  Timezone::kUnknown{};
#endif


        }
    }
}
#endif  /*_Stroika_Foundation_Time_Timezone_inl_*/
