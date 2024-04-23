/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
#if __cpp_lib_chrono >= 201907L
        // @todo find better way to check if should use clock_cast or my_private_clock_cast
        // clang-format off
        template <typename DESTINATION_CLOCK_T, typename SOURCE_CLOCK_T, typename DURATION_T>
        constexpr bool kCanUseStdClockCnv_ = 
            same_as<DESTINATION_CLOCK_T,SOURCE_CLOCK_T> 
                or (
                    ( same_as<DESTINATION_CLOCK_T, chrono::system_clock> or same_as<DESTINATION_CLOCK_T, chrono::utc_clock> or same_as<DESTINATION_CLOCK_T, chrono::gps_clock> or same_as<DESTINATION_CLOCK_T, chrono::file_clock> or  same_as<DESTINATION_CLOCK_T, chrono::tai_clock>)
                        and (same_as<SOURCE_CLOCK_T, chrono::system_clock> or same_as<SOURCE_CLOCK_T, chrono::utc_clock> or same_as<SOURCE_CLOCK_T, chrono::gps_clock> or same_as<SOURCE_CLOCK_T, chrono::file_clock> or  same_as<SOURCE_CLOCK_T, chrono::tai_clock>)
                )
        ;
        // clang-format on
#else
        template <typename DESTINATION_CLOCK_T, typename SOURCE_CLOCK_T, typename DURATION_T>
        constexpr bool kCanUseStdClockCnv_ = false;
#endif
    }

    /*
     ********************************************************************************
     ****************************** Time::clock_cast ********************************
     ********************************************************************************
     */
    template <typename DESTINATION_CLOCK_T, typename SOURCE_CLOCK_T, typename DURATION_T>
    inline auto clock_cast (chrono::time_point<SOURCE_CLOCK_T, DURATION_T> tp) -> typename DESTINATION_CLOCK_T::time_point
    {
#if __cpp_lib_chrono >= 201907L
        if constexpr (Private_::kCanUseStdClockCnv_<DESTINATION_CLOCK_T, SOURCE_CLOCK_T, DURATION_T>) {
            return chrono::clock_cast<DESTINATION_CLOCK_T> (tp);
        }
#endif
        return Private_::clock_cast_2nd<typename DESTINATION_CLOCK_T::time_point> (tp); //return clock_cast_0th<DstTimePointT> (tp);
    }
    template <typename DESTINATION_CLOCK_T, template <typename> typename RANGE, typename SOURCE_CLOCK_T, typename DURATION_T>
    RANGE<typename DESTINATION_CLOCK_T::time_point> clock_cast (RANGE<chrono::time_point<SOURCE_CLOCK_T, DURATION_T>> tpRange)
    {
        using RESULT_TIMERANGE  = typename DESTINATION_CLOCK_T::time_point;
        using RESULT_RANGE_TYPE = RANGE<RESULT_TIMERANGE>;
        if (tpRange.empty ()) {
            return RESULT_RANGE_TYPE{};
        }
        /*
         *  Naive implementation:
         *      return Range<DisplayedRealtimeClock::time_point>{Time::clock_cast<DisplayedRealtimeClock> (tpRange.GetLowerBound ()),
         *                                                       Time::clock_cast<DisplayedRealtimeClock> (tpRange.GetUpperBound ())};
         *  wrong because jitter in estimate/conversion and can result in negative Range/assert error. Leverage fact that conversion
         *  of one is just an offset, and use same offset for the second one.
         * 
         *  But not all clock conversions have this jitter issue.
         */
        if constexpr (Private_::kCanUseStdClockCnv_<DESTINATION_CLOCK_T, SOURCE_CLOCK_T, DURATION_T>) {
            return RESULT_RANGE_TYPE{clock_cast<DESTINATION_CLOCK_T> (tpRange.GetLowerBound ()),
                                     Time::clock_cast<DESTINATION_CLOCK_T> (tpRange.GetUpperBound ())};
        }
        else {
            typename DESTINATION_CLOCK_T::time_point lb   = clock_cast<DESTINATION_CLOCK_T> (tpRange.GetLowerBound ());
            auto                                     diff = lb.time_since_epoch () - tpRange.GetLowerBound ().time_since_epoch ();
            return RESULT_RANGE_TYPE{lb, RESULT_TIMERANGE{tpRange.GetUpperBound ().time_since_epoch () + diff}};
        }
    }

    /*
     ********************************************************************************
     **************** AppStartZeroedClock<BASE_CLOCK_T, DURATION_T> *****************
     ********************************************************************************
     */
    template <typename BASE_CLOCK_T, typename DURATION_T>
    [[nodiscard]] inline auto AppStartZeroedClock<BASE_CLOCK_T, DURATION_T>::now () noexcept -> time_point
    {
        return time_point{Implementation_::now ().time_since_epoch ()} - kTimeAppStartedOffset_;
    }

}
