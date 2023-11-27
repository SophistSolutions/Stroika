/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
            new (&fStringRep_) string{src.fStringRep_};
        }
    }
    inline Duration::Duration (Duration&& src) noexcept
        : inherited{move (src)}
        , fRepType_{src.fRepType_}
    {
        if (src.fRepType_ == eString_) {
            Assert (fRepType_ == eString_);
            new (&fStringRep_) string{move (src.fStringRep_)};
        }
        src.fRepType_ = eEmpty_;
    }
    template <Characters::IConvertibleToString STRINGISH_T>
    inline Duration::Duration (STRINGISH_T&& durationStr)
        : inherited{kValueWhenEmptyRenderedAsNumber_}
        , fNonStringRep_{}
    {
        Assert (fRepType_ == eEmpty_);
        string asciiRep;
        if constexpr (is_same_v<STRINGISH_T, string>) {
            asciiRep = durationStr;
        }
        else {
            asciiRep = Characters::String{durationStr}.AsASCII ();
        }
        if (not asciiRep.empty ()) {
            (*(inherited*)this) = inherited{ParseTime_ (asciiRep)};
            new (&fStringRep_) string{asciiRep};
            fRepType_ = eString_;
        }
    }
    constexpr Duration::Duration (int durationInSeconds)
        : inherited{durationInSeconds}
        , fRepType_{eNumeric_}
        , fNonStringRep_{}
    {
    }
    constexpr Duration::Duration (long durationInSeconds)
        : inherited{durationInSeconds}
        , fRepType_{eNumeric_}
        , fNonStringRep_{}
    {
    }
    constexpr Duration::Duration (long long durationInSeconds)
        : inherited{static_cast<InternalNumericFormatType_> (durationInSeconds)}
        , fRepType_{eNumeric_}
        , fNonStringRep_{}
    {
    }
    constexpr Duration::Duration (float durationInSeconds)
        : inherited{durationInSeconds}
        , fRepType_{eNumeric_}
        , fNonStringRep_{}
    {
        //Require (not isnan (duration)); // inf is allowed
    }
    constexpr Duration::Duration (double durationInSeconds)
        : inherited{durationInSeconds}
        , fRepType_{eNumeric_}
        , fNonStringRep_{}
    {
        //Require (not isnan (duration)); // inf is allowed
    }
    constexpr Duration::Duration (long double durationInSeconds) noexcept
        : inherited{static_cast<InternalNumericFormatType_> (durationInSeconds)}
        , fRepType_{eNumeric_}
        , fNonStringRep_{}
    {
        //Require (not isnan (duration)); // inf is allowed
    }
    inline u8string Duration::AsUTF8 () const
    {
        return As<Characters::String> ().AsUTF8 ();
    }
    inline constexpr Duration::~Duration ()
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
                    new (&fStringRep_) string{rhs.fStringRep_};
                    // fRepType_ = eString_;    done at end of procedure
                }
            }
            inherited::operator= (rhs);
            fRepType_ = rhs.fRepType_;
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
                    new (&fStringRep_) string{move (rhs.fStringRep_)};
                }
            }
            inherited::operator= (move (rhs));
            fRepType_     = rhs.fRepType_;
            rhs.fRepType_ = eEmpty_;
        }
        return *this;
    }
    inline constexpr void Duration::destroy_ ()
    {
        if (fRepType_ == eString_) {
            fStringRep_.~basic_string ();
        }
        fRepType_ = eEmpty_;
    }
    template <typename T>
    inline T Duration::As () const
        requires (same_as<T, timeval> or integral<T> or floating_point<T> or same_as<T, Characters::String> or
                  Configuration::IDuration<T> or Configuration::ITimePoint<T>)
    {
        if constexpr (integral<T> or floating_point<T>) {
            return static_cast<T> (count ());
        }
        else if constexpr (same_as<T, timeval>) {
            rep                        r       = count (); // @todo fix for negative case
            decltype (timeval::tv_sec) seconds = static_cast<long> (r);
            r -= seconds;
            return timeval{seconds, static_cast<decltype (timeval::tv_usec)> (r * 1000 * 1000)};
        }
        else if constexpr (same_as<T, Characters::String>) {
            using Characters::String;
            switch (fRepType_) {
                case eEmpty_:
                    return String{};
                case eString_:
                    return String{fStringRep_};
                case eNumeric_:
                    return String{UnParseTime_ (count ())};
            }
            AssertNotReached ();
            return String{};
        }
        else if constexpr (Configuration::IDuration<T>) {
            return chrono::duration_cast<T> (*this);
        }
        else if constexpr (Configuration::ITimePoint<T>) {
            return T{this->As<typename T::duration> ()};
        }
    }
    template <typename T>
    inline T Duration::AsPinned () const
        requires (same_as<T, timeval> or integral<T> or floating_point<T> or same_as<T, Characters::String> or
                  Configuration::IDuration<T> or Configuration::ITimePoint<T>)
    {
        if constexpr (integral<T> or floating_point<T>) {
            if (this->count () < numeric_limits<T>::min ()) [[unlikely]] {
                return numeric_limits<T>::min ();
            }
            else if (this->count () > numeric_limits<T>::max ()) [[unlikely]] {
                return numeric_limits<T>::max ();
            }
            return As<T> ();
        }
        else if constexpr (same_as<T, timeval>) {
            if (this->count () > numeric_limits<long>::max ()) [[unlikely]] {
                return timeval{numeric_limits<long>::max (), 0}; // close enuf for now - if that big, do the nanoseconds matter?
            }
            return As<T> ();
        }
        else if constexpr (same_as<T, Characters::String>) {
            return As<T> ();
        }
        else if constexpr (Configuration::IDuration<T>) {
#if (defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 10)) ||                                                        \
    (defined(__clang_major__) && defined(__APPLE__) && (__clang_major__ >= 12))
            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wimplicit-int-float-conversion\""); // warning: implicit conversion from 'std::__1::chrono::duration<long long, std::__1::ratio<1, 1> >::rep' (aka 'long long') to 'double' changes value from 9223372036854775807 to 9223372036854775808
#endif
            /*
             *  To convert, will do( my_ratio::num/my_ratio::den) * (target::den / target::num) .
             *  Question is - will any of that overflow.
             * 
             *  Hard to be sure as it depends a little on how you get there (deterministic, just complex).
             *  But easy approximation is:
             *      do math in floating point, and see if would 'fit' in target rep.
             */
            using TRatio   = typename T::period;
            auto targetRes = (static_cast<long double> (this->count ()) * period::num / period::den) * TRatio ::den / TRatio::num;
            if (targetRes < T::min ().count ()) [[unlikely]] {
                return T::min ();
            }
            if (targetRes > T::max ().count ()) [[unlikely]] {
                return T::max ();
            }
#if (defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 10)) ||                                                        \
    (defined(__clang_major__) && defined(__APPLE__) && (__clang_major__ >= 12))
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wimplicit-int-float-conversion\"");
#endif
            return As<T> ();
        }
        else if constexpr (Configuration::ITimePoint<T>) {
            return T{this->AsPinned<typename T::duration> ()};
        }
    }
    inline Characters::String Duration::Format (const PrettyPrintInfo& prettyPrintInfo) const
    {
        return PrettyPrint (prettyPrintInfo);
    }
    inline Characters::String Duration::ToString () const
    {
        return Format ();
    }
    inline constexpr Duration Duration::min ()
    {
        return inherited::min ();
    }
    inline constexpr Duration Duration::max ()
    {
        return inherited::max ();
    }

    /*
     ********************************************************************************
     ************************** operator "" _duration *******************************
     ********************************************************************************
     */
    [[nodiscard]] inline Duration operator"" _duration (const char* str, size_t len)
    {
        return Duration{string{str, str + len}};
    }
    [[nodiscard]] inline Duration operator"" _duration (const wchar_t* str, size_t len)
    {
        return Duration{Characters::String{span{str, len}}};
    }
    [[nodiscard]] inline Duration operator"" _duration (const char8_t* str, size_t len)
    {
        return Duration{Characters::String{span{str, len}}};
    }
    [[nodiscard]] inline Duration operator"" _duration (const char16_t* str, size_t len)
    {
        return Duration{Characters::String{span{str, len}}};
    }
    [[nodiscard]] inline Duration operator"" _duration (const char32_t* str, size_t len)
    {
        return Duration{Characters::String{span{str, len}}};
    }
    [[nodiscard]] inline Duration operator"" _duration (long double _Val) noexcept
    {
        return Duration{_Val};
    }

    /*
     ********************************************************************************
     ***************************** Duration operators *******************************
     ********************************************************************************
     */
    inline Duration operator+ (const DurationSeconds& lhs, const Duration& rhs)
    {
        return Duration{lhs + rhs.As<DurationSeconds> ()};
    }
    inline Duration operator* (long double lhs, const Duration& rhs)
    {
        return Duration{rhs.As<DurationSeconds> () * lhs};
    }

}

namespace Stroika::Foundation::Traversal::RangeTraits {

    /*
     ********************************************************************************
     ****************************** RangeTraits::Default ****************************
     ********************************************************************************
     */
    inline constexpr Time::Duration Default<Time::Duration>::kLowerBound = Time::Duration::min ();
    inline constexpr Time::Duration Default<Time::Duration>::kUpperBound = Time::Duration::max ();
    inline Time::Duration           Default<Time::Duration>::GetNext (Time::Duration i)
    {
        using Time::Duration;
        return Duration{::nextafter (i.As<Duration::rep> (), numeric_limits<Duration::rep>::max ())};
    }
    inline Time::Duration Default<Time::Duration>::GetPrevious (Time::Duration i)
    {
        using Time::Duration;
        return Duration{::nextafter (i.As<Duration::rep> (), numeric_limits<Duration::rep>::min ())};
    }

}

#endif /*_Stroika_Foundation_Time_Duration_inl_*/
