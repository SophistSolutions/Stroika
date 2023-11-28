/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Common_inl_
#define _Stroika_Foundation_Time_Common_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Time {

    template <typename CLOCK_T, typename DURATION_T>
    auto Pin2SafeSeconds (const chrono::time_point<CLOCK_T, DURATION_T>& tp) -> chrono::time_point<CLOCK_T, DURATION_T>
    {
        /*
         *  Intuitively, you might expect to be able to just use chrono::seconds::min () and chrono::seconds::max (). But
         *  converting big integers to / from floats, and back is lossy and imprecise. So a little fudge-factor appears needed
         *  to avoid failures, and pragmatically, should cause no problems.
         * 
         *      --LGP 2023-11-28
         */
#if 1
        static constexpr auto kMin_ = chrono::time_point_cast<chrono::seconds> (
            chrono::time_point<CLOCK_T, DURATION_T>{chrono::seconds{chrono::seconds::min ().count () + 1000}});
        static constexpr auto kMax_ = chrono::time_point_cast<chrono::seconds> (
            chrono::time_point<CLOCK_T, DURATION_T>{chrono::seconds{chrono::seconds::max ().count () - 1000}});
#else
        static constexpr auto kMin  = chrono::time_point<CLOCK_T, DURATION_T>{chrono::seconds::min ()};
        static constexpr auto kMax_ = chrono::time_point<CLOCK_T, DURATION_T>{chrono::seconds::max ()};
#endif
#if qDebug
        [[maybe_unused]] static auto test1 = chrono::time_point_cast<chrono::seconds> (kMax_); // verify the +1000/-1000 stuff enuf
        [[maybe_unused]] static auto test2 = chrono::time_point_cast<chrono::seconds> (kMin_);
#endif

        typename DURATION_T::rep tpSeconds = tp.time_since_epoch ().count () * DURATION_T::period::den / DURATION_T::period::num;
        if (tpSeconds > static_cast<typename DURATION_T::rep> (chrono::seconds::max ().count ())) {
            return kMax_;
        }
        if (tpSeconds < static_cast<typename DURATION_T::rep> (chrono::seconds::min ().count ())) [[unlikely]] {
            return kMin_;
        }
        return tp;
    }

}

#endif /*_Stroika_Foundation_Time_Common_inl_*/
