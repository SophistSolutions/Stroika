/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Duration_inl_
#define _Stroika_Foundation_Time_Duration_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include <limits>

namespace Stroika::Foundation::Time {

    /*
     ********************************************************************************
     ************************************ Duration **********************************
     ********************************************************************************
     */
    template <typename DURATION_REP, typename DURATION_PERIOD>
    constexpr Duration::Duration (const chrono::duration<DURATION_REP, DURATION_PERIOD>& d)
        : inherited{chrono::duration<InternalNumericFormatType_> (d).count ()}
        , fRepType_{eNumeric_}
        , fNonStringRep_{}
    {
    }
    constexpr Duration::Duration ()
        : inherited{kValueWhenEmptyRenderedAsNumber_}
        , fRepType_{eEmpty_}
        , fNonStringRep_{}
    {
    }
    inline Duration::Duration (const Duration& src)
        : inherited{src}
        , fRepType_{src.fRepType_}
    {
        if (fRepType_ == eString_) {
            Assert (not src.fStringRep_.empty ());
            new (&fStringRep_) string (src.fStringRep_);
        }
    }
    inline Duration::Duration (Duration&& src) noexcept
        : inherited{move (src)}
        , fRepType_{src.fRepType_}
    {
        if (src.fRepType_ == eString_) {
            Assert (fRepType_ == eString_);
            new (&fStringRep_) string (move (src.fStringRep_));
        }
        src.fRepType_ = eEmpty_;
    }
    inline Duration::Duration (const string& durationStr)
        : inherited{kValueWhenEmptyRenderedAsNumber_}
        , fNonStringRep_{}
    {
        Assert (fRepType_ == eEmpty_);
        if (not durationStr.empty ()) {
            (*(inherited*)this) = inherited{ParseTime_ (durationStr)};
            new (&fStringRep_) string (durationStr);
            fRepType_ = eString_;
        }
    }
    constexpr Duration::Duration (int duration)
        : inherited{duration}
        , fRepType_{eNumeric_}
        , fNonStringRep_{}
    {
    }
    constexpr Duration::Duration (long duration)
        : inherited{duration}
        , fRepType_{eNumeric_}
        , fNonStringRep_{}
    {
    }
    constexpr Duration::Duration (long long duration)
        : inherited{static_cast<InternalNumericFormatType_> (duration)}
        , fRepType_{eNumeric_}
        , fNonStringRep_{}
    {
    }
    constexpr Duration::Duration (float duration)
        : inherited{duration}
        , fRepType_{eNumeric_}
        , fNonStringRep_{}
    {
        //Require (not isnan (duration)); // inf is allowed
    }
    constexpr Duration::Duration (double duration)
        : inherited{duration}
        , fRepType_{eNumeric_}
        , fNonStringRep_{}
    {
        //Require (not isnan (duration)); // inf is allowed
    }
    constexpr Duration::Duration (long double duration)
        : inherited{static_cast<InternalNumericFormatType_> (duration)}
        , fRepType_{eNumeric_}
        , fNonStringRep_{}
    {
        //Require (not isnan (duration)); // inf is allowed
    }
    inline string Duration::AsUTF8 () const
    {
        return As<Characters::String> ().AsUTF8 ();
    }
    inline Duration::~Duration ()
    {
        destroy_ ();
    }
    inline void Duration::clear ()
    {
        destroy_ ();
    }
    constexpr bool Duration::empty () const
    {
        // on construction with an empty string, this produces type eEmpty_
        return fRepType_ == eEmpty_;
    }
    inline Duration& Duration::operator= (const Duration& rhs)
    {
        if (this != &rhs) {
            if (fRepType_ == rhs.fRepType_) {
                if (rhs.fRepType_ == eString_) {
                    // if both unions have string active - just assign
                    fStringRep_ = rhs.fStringRep_;
                }
            }
            else {
                // if reps different, destroy in case this is string type
                destroy_ ();
                if (rhs.fRepType_ == eString_) {
                    new (&fStringRep_) string (rhs.fStringRep_);
                    // fRepType_ = eString_;    done at end of procedure
                }
            }
            inherited::operator= (rhs);
            fRepType_          = rhs.fRepType_;
        }
        return *this;
    }
    inline Duration& Duration::operator= (Duration&& rhs) noexcept
    {
        if (this != &rhs) {
            if (fRepType_ == rhs.fRepType_) {
                // if both unions have string active - just move assign
                if (rhs.fRepType_ == eString_) {
                    fStringRep_ = move (rhs.fStringRep_);
                    // setting our type, and STEALING type of rhs at bottom of procedure
                }
            }
            else {
                // if reps different, destroy in case this is string type
                destroy_ ();
                if (rhs.fRepType_ == eString_) {
                    new (&fStringRep_) string (move (rhs.fStringRep_));
                }
            }
            inherited::operator= (move (rhs));
            fRepType_          = rhs.fRepType_;
            rhs.fRepType_      = eEmpty_;
        }
        return *this;
    }
    inline void Duration::destroy_ ()
    {
        if (fRepType_ == eString_) {
            fStringRep_.~basic_string ();
        }
        fRepType_ = eEmpty_;
    }
    template <>
    inline int Duration::As () const
    {
        return static_cast<int> (count ());
    }
    template <>
    inline long int Duration::As () const
    {
        return static_cast<long int> (count ());
    }
    template <>
    inline long long int Duration::As () const
    {
        return static_cast<long long int> (count ());
    }
    template <>
    inline float Duration::As () const
    {
        return static_cast<float> (count ());
    }
    template <>
    inline double Duration::As () const
    {
        return count ();
    }
    template <>
    inline long double Duration::As () const
    {
        return count ();
    }
    template <>
    inline chrono::duration<double> Duration::As () const
    {
        return chrono::duration<double> (count ());
    }
    template <>
    inline chrono::seconds Duration::As () const
    {
        return chrono::seconds (static_cast<chrono::seconds::rep> (count ()));
    }
    template <>
    inline chrono::milliseconds Duration::As () const
    {
        return chrono::milliseconds (static_cast<chrono::milliseconds::rep> (count () * 1000));
    }
    template <>
    inline chrono::microseconds Duration::As () const
    {
        return chrono::microseconds (static_cast<chrono::microseconds::rep> (count () * 1000 * 1000));
    }
    template <>
    inline chrono::nanoseconds Duration::As () const
    {
        return chrono::nanoseconds (static_cast<chrono::nanoseconds::rep> (count () * 1000.0 * 1000.0 * 1000.0));
    }
    template <>
    inline timeval Duration::As () const
    {
        auto                       r       = count (); // @todo fix for negative case
        decltype (timeval::tv_sec) seconds = static_cast<long> (r);
        r -= seconds;
        return timeval{seconds, static_cast<decltype (timeval::tv_usec)> (r * 1000 * 1000)};
    }
    template <>
    inline Characters::String Duration::As () const
    {
        using Characters::String;
        switch (fRepType_) {
            case eEmpty_:
                return String{};
            case eString_:
                return String::FromASCII (fStringRep_);
            case eNumeric_:
                return String::FromASCII (UnParseTime_ (count ()));
        }
        AssertNotReached ();
        return String{};
    }
    inline Characters::String Duration::Format (const PrettyPrintInfo& prettyPrintInfo) const
    {
        return PrettyPrint (prettyPrintInfo);
    }
    inline Characters::String Duration::ToString () const
    {
        return Format ();
    }
    inline /*constexpr*/ Duration Duration::min ()
    {
        return Duration{numeric_limits<InternalNumericFormatType_>::lowest ()};
    }
    inline /*constexpr*/ Duration Duration::max ()
    {
        return Duration{numeric_limits<InternalNumericFormatType_>::max ()};
    }

    /*
     ********************************************************************************
     ************************** operator "" _duration *******************************
     ********************************************************************************
     */
    inline Duration operator"" _duration (const char* str, size_t len) noexcept
    {
        return Duration{string (str, str + len)};
    }
    inline Duration operator"" _duration (const wchar_t* str, size_t len) noexcept
    {
        return Duration{Characters::String{str, str + len}};
    }
#if __cpp_char8_t >= 201811L
    inline Duration operator"" _duration (const char8_t* str, size_t len) noexcept
    {
        return Duration{Characters::String{str, str + len}};
    }
#endif
    inline Duration operator"" _duration (const char16_t* str, size_t len) noexcept
    {
        return Duration{Characters::String{str, str + len}};
    }
    inline Duration operator"" _duration (const char32_t* str, size_t len) noexcept
    {
        return Duration{Characters::String{str, str + len}};
    }

    /*
     ********************************************************************************
     ***************************** Duration operators *******************************
     ********************************************************************************
     */
    inline Duration operator+ (const DurationSecondsType& lhs, const Duration& rhs)
    {
        return Duration{lhs + rhs.As<DurationSecondsType> ()};
    }
    inline Duration operator* (long double lhs, const Duration& rhs)
    {
        return Duration (rhs.As<Time::DurationSecondsType> () * lhs);
    }

}

namespace Stroika::Foundation::Traversal::RangeTraits {

    /*
     ********************************************************************************
     ****************************** RangeTraits::Default ****************************
     ********************************************************************************
     */
#if !qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy
    inline const Time::Duration Default<Time::Duration>::kLowerBound = Time::Duration::min ();
    inline const Time::Duration Default<Time::Duration>::kUpperBound = Time::Duration::max ();
#endif
    inline Time::Duration Default<Time::Duration>::GetNext (Time::Duration i)
    {
        return Time::Duration{::nextafter (i.As<double> (), numeric_limits<double>::max ())};
    }
    inline Time::Duration Default<Time::Duration>::GetPrevious (Time::Duration i)
    {
        return Time::Duration{::nextafter (i.As<double> (), numeric_limits<double>::min ())};
    }

}

#endif /*_Stroika_Foundation_Time_Duration_inl_*/
