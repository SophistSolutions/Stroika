/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Clock_inl_
#define _Stroika_Foundation_Time_Clock_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Time {

    namespace Private_ {
        // From https://stackoverflow.com/questions/35282308/convert-between-c11-clocks
        // EXPECIALLY read commentary...
        template <typename DstTimePointT, typename SrcTimePointT, typename DstClockT = typename DstTimePointT::clock, typename SrcClockT = typename SrcTimePointT::clock>
        DstTimePointT clock_cast_0th (const SrcTimePointT tp)
        {
            const auto src_now = SrcClockT::now ();
            const auto dst_now = DstClockT::now ();
            return dst_now + chrono::duration_cast<typename DstClockT::duration> (tp - src_now);
        }
        namespace detail {
            template <typename DurationT, typename ReprT = typename DurationT::rep>
            constexpr DurationT max_duration () noexcept
            {
                return DurationT{std::numeric_limits<ReprT>::max ()};
            }
            template <typename DurationT>
            constexpr DurationT abs_duration (const DurationT d) noexcept
            {
                return DurationT{(d.count () < 0) ? -d.count () : d.count ()};
            }
        }
        template <typename DstTimePointT, typename SrcTimePointT, typename DstDurationT = typename DstTimePointT::duration, typename SrcDurationT = typename SrcTimePointT::duration,
                  typename DstClockT = typename DstTimePointT::clock, typename SrcClockT = typename SrcTimePointT::clock>
        DstTimePointT clock_cast_2nd (const SrcTimePointT tp,
                                      const SrcDurationT tolerance = std::chrono::duration_cast<SrcDurationT> (std::chrono::nanoseconds{100}),
                                      const int limit = 4)
        {
            Require (limit > 0);
            auto itercnt = 0;
            auto src_now = SrcTimePointT{};
            auto dst_now = DstTimePointT{};
            auto epsilon = detail::max_duration<SrcDurationT> ();
            do {
                const auto src_before  = SrcClockT::now ();
                const auto dst_between = DstClockT::now ();
                const auto src_after   = SrcClockT::now ();
                const auto src_diff    = src_after - src_before;
                const auto delta       = detail::abs_duration (src_diff);
                if (delta < epsilon) {
                    src_now = src_before + src_diff / 2;
                    dst_now = dst_between;
                    epsilon = std::chrono::duration_cast<SrcDurationT> (delta);
                }
                if (++itercnt >= limit)
                    break;
            } while (epsilon > tolerance);
            return dst_now + chrono::duration_cast<typename DstClockT::duration> (tp - src_now);
        }
    }

    /*
     ********************************************************************************
     ****************************** Time::clock_cast ********************************
     ********************************************************************************
     */
    template <typename DESTINATION_CLOCK_T, typename SOURCE_CLOCK_T, typename DURATION_T>
    inline auto clock_cast (chrono::time_point<SOURCE_CLOCK_T, DURATION_T> tp) -> DESTINATION_CLOCK_T::time_point
    {
        using namespace std::chrono;
        // @todo find better way to check if should use clock_cast or my_private_clock_cast
        using DstTimePointT = typename DESTINATION_CLOCK_T::time_point;
#if __cpp_lib_chrono >= 201907L
        // clang-format off
        if constexpr (
            (
                same_as<DESTINATION_CLOCK_T,SOURCE_CLOCK_T> 
                or (
                    same_as<DESTINATION_CLOCK_T, system_clock> or same_as<DESTINATION_CLOCK_T, utc_clock> or same_as<DESTINATION_CLOCK_T, gps_clock> or same_as<DESTINATION_CLOCK_T, file_clock> or  same_as<DESTINATION_CLOCK_T, tai_clock>)
                        and (same_as<SOURCE_CLOCK_T, system_clock> or same_as<SOURCE_CLOCK_T, utc_clock> or same_as<SOURCE_CLOCK_T, gps_clock> or same_as<SOURCE_CLOCK_T, file_clock> or  same_as<SOURCE_CLOCK_T, tai_clock>)
                )
            ) {
            return chrono::clock_cast<DESTINATION_CLOCK_T> (tp);
        }
        else {
            return Private_::clock_cast_2nd<DstTimePointT> (tp);        //return clock_cast_0th<DstTimePointT> (tp);
        }
        // clang-format on
#else
        return Private_::clock_cast_2nd<DstTimePointT> (tp); //return clock_cast_0th<DstTimePointT> (tp);
#endif
    }

}
#endif /*_Stroika_Foundation_Time_Clock_inl_*/
