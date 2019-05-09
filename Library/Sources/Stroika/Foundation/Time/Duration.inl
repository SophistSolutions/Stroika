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

namespace Stroika::Foundation::Time {

    /*
     ********************************************************************************
     ************************************ Duration **********************************
     ********************************************************************************
     */
    template <typename DURATION_REP, typename DURATION_PERIOD>
    constexpr Duration::Duration (const chrono::duration<DURATION_REP, DURATION_PERIOD>& d)
        : fRepType_ (eNumeric_)
        , fNonStringRep_{}
        , fNumericRepOrCache_ (chrono::duration<InternalNumericFormatType_> (d).count ())
    {
    }
    constexpr Duration::Duration ()
        : fRepType_ (eEmpty_)
        , fNonStringRep_{}
    {
    }
    inline Duration::Duration (const Duration& src)
        : fRepType_ (src.fRepType_)
        , fNumericRepOrCache_ (src.fNumericRepOrCache_)
    {
        if (fRepType_ == eString_) {
            Assert (not src.fStringRep_.empty ());
            new (&fStringRep_) string (src.fStringRep_);
        }
    }
    inline Duration::Duration (Duration&& src) noexcept
        : fRepType_ (src.fRepType_)
        , fNumericRepOrCache_ (src.fNumericRepOrCache_)
    {
        if (src.fRepType_ == eString_) {
            Assert (fRepType_ == eString_);
            new (&fStringRep_) string (move (src.fStringRep_));
        }
        src.fRepType_ = eEmpty_;
    }
    inline Duration::Duration (const string& durationStr)
        : fNonStringRep_{}
    {
        Assert (fRepType_ == eEmpty_);
        if (not durationStr.empty ()) {
            fNumericRepOrCache_ = ParseTime_ (durationStr);
            new (&fStringRep_) string (durationStr);
            fRepType_ = eString_;
        }
    }
    constexpr Duration::Duration (int duration)
        : fRepType_ (eNumeric_)
        , fNonStringRep_{}
        , fNumericRepOrCache_ (duration)
    {
    }
    constexpr Duration::Duration (long duration)
        : fRepType_ (eNumeric_)
        , fNonStringRep_{}
        , fNumericRepOrCache_ (duration)
    {
    }
    constexpr Duration::Duration (long long duration)
        : fRepType_ (eNumeric_)
        , fNonStringRep_{}
        , fNumericRepOrCache_ (static_cast<InternalNumericFormatType_> (duration))
    {
    }
    constexpr Duration::Duration (float duration)
        : fRepType_ (eNumeric_)
        , fNonStringRep_{}
        , fNumericRepOrCache_ (duration)
    {
        //Require (not isnan (duration)); // inf is allowed
    }
    constexpr Duration::Duration (double duration)
        : fRepType_ (eNumeric_)
        , fNonStringRep_{}
        , fNumericRepOrCache_ (duration)
    {
        //Require (not isnan (duration)); // inf is allowed
    }
    constexpr Duration::Duration (long double duration)
        : fRepType_ (eNumeric_)
        , fNonStringRep_{}
        , fNumericRepOrCache_ (static_cast<InternalNumericFormatType_> (duration))
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
            fNumericRepOrCache_ = rhs.fNumericRepOrCache_;
            fRepType_           = rhs.fRepType_;
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
            fNumericRepOrCache_ = rhs.fNumericRepOrCache_;
            fRepType_           = rhs.fRepType_;
            rhs.fRepType_       = eEmpty_;
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
        return static_cast<int> (fNumericRepOrCache_);
    }
    template <>
    inline long int Duration::As () const
    {
        return static_cast<long int> (fNumericRepOrCache_);
    }
    template <>
    inline long long int Duration::As () const
    {
        return static_cast<long long int> (fNumericRepOrCache_);
    }
    template <>
    inline float Duration::As () const
    {
        return static_cast<float> (fNumericRepOrCache_);
    }
    template <>
    inline double Duration::As () const
    {
        return fNumericRepOrCache_;
    }
    template <>
    inline long double Duration::As () const
    {
        return fNumericRepOrCache_;
    }
    template <>
    inline chrono::duration<double> Duration::As () const
    {
        return chrono::duration<double> (fNumericRepOrCache_);
    }
    template <>
    inline chrono::seconds Duration::As () const
    {
        return chrono::seconds (static_cast<chrono::seconds::rep> (fNumericRepOrCache_));
    }
    template <>
    inline chrono::milliseconds Duration::As () const
    {
        return chrono::milliseconds (static_cast<chrono::milliseconds::rep> (fNumericRepOrCache_ * 1000));
    }
    template <>
    inline chrono::microseconds Duration::As () const
    {
        return chrono::microseconds (static_cast<chrono::microseconds::rep> (fNumericRepOrCache_ * 1000 * 1000));
    }
    template <>
    inline chrono::nanoseconds Duration::As () const
    {
        return chrono::nanoseconds (static_cast<chrono::nanoseconds::rep> (fNumericRepOrCache_ * 1000.0 * 1000.0 * 1000.0));
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
                return String::FromASCII (UnParseTime_ (fNumericRepOrCache_));
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
    inline int Duration::Compare (const Duration& rhs) const
    {
        Duration::InternalNumericFormatType_ n = As<Duration::InternalNumericFormatType_> () - rhs.As<Duration::InternalNumericFormatType_> ();
        if (n < 0) {
            return -1;
        }
        if (n > 0) {
            return 1;
        }
        return 0;
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
     ************************** Duration::ThreeWayComparer **************************
     ********************************************************************************
     */
    inline int Duration::ThreeWayComparer::operator() (const Duration& lhs, const Duration& rhs) const
    {
        return Common::ThreeWayCompareNormalizer (lhs.As<Duration::InternalNumericFormatType_> (), rhs.As<Duration::InternalNumericFormatType_> ());
    }

    /*
     ********************************************************************************
     ********************** Duration comparison operators ***************************
     ********************************************************************************
     */
    inline bool operator< (const Duration& lhs, const Duration& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) < 0;
    }
    inline bool operator<= (const Duration& lhs, const Duration& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) <= 0;
    }
    inline bool operator== (const Duration& lhs, const Duration& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) == 0;
    }
    inline bool operator!= (const Duration& lhs, const Duration& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) != 0;
    }
    inline bool operator>= (const Duration& lhs, const Duration& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) >= 0;
    }
    inline bool operator> (const Duration& lhs, const Duration& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) > 0;
    }

    /*
     ********************************************************************************
     ***************************** Duration operators *******************************
     ********************************************************************************
     */
    inline Duration operator/ (const Duration& lhs, long double rhs)
    {
        Require (rhs != 0);
        return lhs * (1 / rhs);
    }
    inline Duration operator+ (const Duration& lhs, const Duration& rhs)
    {
        return Duration (lhs.As<Time::DurationSecondsType> () + rhs.As<DurationSecondsType> ());
    }
    inline Duration operator- (const Duration& lhs, const Duration& rhs)
    {
        return Duration (lhs.As<Time::DurationSecondsType> () - rhs.As<DurationSecondsType> ());
    }
    inline Duration operator* (const Duration& lhs, long double rhs)
    {
        return Duration (lhs.As<Time::DurationSecondsType> () * rhs);
    }
    inline Duration operator* (long double lhs, const Duration& rhs)
    {
        return Duration (rhs.As<Time::DurationSecondsType> () * lhs);
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
