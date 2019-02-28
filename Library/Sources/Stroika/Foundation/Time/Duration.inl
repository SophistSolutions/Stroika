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
        , fNumericRepOrCache_ (chrono::duration<InternalNumericFormatType_> (d).count ())
    {
    }
    constexpr Duration::Duration ()
        : fRepType_ (eEmpty_)
        , fNonStringRep_{}
    {
    }
    inline Duration::Duration (const Duration& src)
        : fNumericRepOrCache_ (src.fNumericRepOrCache_)
    {
        switch (src.fRepType_) {
            case eString_:
                construct_ (src.fStringRep_);
                break;
        }
        fRepType_ = src.fRepType_;
    }
    inline Duration::Duration (const string& durationStr)
		: Duration ()
    {
        if (not durationStr.empty ()) {
            fNumericRepOrCache_ = ParseTime_ (durationStr);
            construct_ (durationStr);
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
                switch (fRepType_) {
                    case eString_:
                        fStringRep_ = rhs.fStringRep_;
                        break;
                }
            }
            else {
                destroy_ ();
                switch (fRepType_) {
                    case eString_:
                        construct_ (rhs.fStringRep_);
                        break;
                }
            }
            fNumericRepOrCache_ = rhs.fNumericRepOrCache_;
            fRepType_           = rhs.fRepType_;
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
    inline void Duration::construct_ (const string& s)
    {
        Require (fRepType_ == eEmpty_);
		Require (not s.empty ());
        new (&fStringRep_) string (s);
        fRepType_ = eString_;
    }
    inline void Duration::construct_ (InternalNumericFormatType_ n)
    {
        Require (fRepType_ == eEmpty_);
        fNumericRepOrCache_ = n;
        fRepType_           = eNumeric_;
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
     ***************************** Duration operators *******************************
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
