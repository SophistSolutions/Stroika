/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include <limits>

namespace Stroika::Foundation::Time {

    /*
     ********************************************************************************
     ***************************** Duration::AsStringOptions ************************
     ********************************************************************************
     */
    namespace Private_ {
        // the innermost ScopedThreadDefault's options, or nullptr: AsStringOptions{}. constinit - constant initialized - so a read
        // needs no initialization guard (nor does g++'s bug 66944, two guarded inline thread_locals in one translation unit, apply)
        inline constinit thread_local const Duration::AsStringOptions* tDurationAsStringOptions_{nullptr};
    }
    inline auto Duration::AsStringOptions::GetThreadDefault () -> AsStringOptions
    {
        const AsStringOptions* o = Private_::tDurationAsStringOptions_;
        return o == nullptr ? AsStringOptions{} : *o;
    }
    inline Duration::AsStringOptions::ScopedThreadDefault::ScopedThreadDefault (const AsStringOptions& options)
        : fOptions_{options}
        , fPrevious_{Private_::tDurationAsStringOptions_}
    {
        Private_::tDurationAsStringOptions_ = &fOptions_;
    }
    inline Duration::AsStringOptions::ScopedThreadDefault::~ScopedThreadDefault ()
    {
        Assert (Private_::tDurationAsStringOptions_ == &fOptions_); // scopes end innermost first
        Private_::tDurationAsStringOptions_ = fPrevious_;
    }

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
        if (not is_constant_evaluated ()) {
            if constexpr (floating_point<DURATION_REP>) {
                Require (not isnan (d.count ())); // inf is allowed, isnan not yet constexpr as of c++20, fixed in c++23
            }
        }
    }
    constexpr Duration::Duration ()
        : inherited{0}
        , fRepType_{eNumeric_}
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
            src.destroy_ (); // src keeps its numeric value
        }
    }
    template <Characters::IConvertibleToString STRINGISH_T>
    inline Duration::Duration (STRINGISH_T&& durationStr, const ParseOptions& options)
        : Duration{}
    {
        if constexpr (same_as<remove_cvref_t<STRINGISH_T>, string>) {
            *this = Execution::ThrowIfFailed (Parse_ (durationStr, options));
        }
        else {
            *this = Execution::ThrowIfFailed (Parse (Characters::String{forward<STRINGISH_T> (durationStr)}, options));
        }
    }
#if qCompilerAndStdLib_default_member_initializer_needed_within_enclosing_class_Buggy
    template <Characters::IConvertibleToString STRINGISH_T>
    inline Duration::Duration (STRINGISH_T&& durationStr)
        : Duration{forward<STRINGISH_T> (durationStr), ParseOptions{}}
    {
    }
#endif
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
        if (not is_constant_evaluated ()) {
            Require (not isnan (durationInSeconds)); // inf is allowed, isnan not yet constexpr as of c++20, fixed in c++23
        }
    }
    constexpr Duration::Duration (double durationInSeconds)
        : inherited{durationInSeconds}
        , fRepType_{eNumeric_}
        , fNonStringRep_{}
    {
        if (not is_constant_evaluated ()) {
            Require (not isnan (durationInSeconds)); // inf is allowed, isnan not yet constexpr as of c++20, fixed in c++23
        }
    }
    constexpr Duration::Duration (long double durationInSeconds) noexcept
        : inherited{static_cast<InternalNumericFormatType_> (durationInSeconds)}
        , fRepType_{eNumeric_}
        , fNonStringRep_{}
    {
        if (not is_constant_evaluated ()) {
            Require (not isnan (durationInSeconds)); // inf is allowed, isnan not yet constexpr as of c++20, fixed in c++23
        }
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
        *this = Duration{};
    }
    constexpr bool Duration::empty () const
    {
        return count () == 0;
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
            fRepType_ = rhs.fRepType_;
            rhs.destroy_ (); // rhs keeps its numeric value
        }
        return *this;
    }
    inline constexpr void Duration::destroy_ ()
    {
        if (fRepType_ == eString_) {
            fStringRep_.~basic_string ();
        }
        fRepType_ = eNumeric_;
    }
    template <typename T>
    inline T Duration::As () const
        requires (Common::IAnyOf<T, timeval, Characters::String> or integral<T> or floating_point<T> or Common::IDuration<T> or Common::ITimePoint<T>)
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
                case eString_:
                    return String{fStringRep_};
                case eNumeric_:
                    return UnParseTime_ (count (), AsStringOptions::GetThreadDefault ());
            }
            AssertNotReached ();
            return String{};
        }
        else if constexpr (Common::IDuration<T>) {
            return chrono::duration_cast<T> (*this);
        }
        else if constexpr (Common::ITimePoint<T>) {
            return T{this->As<typename T::duration> ()};
        }
    }
    template <typename T>
    inline T Duration::As (const AsStringOptions& options) const
        requires (same_as<T, Characters::String>)
    {
        return UnParseTime_ (count (), options);
    }
    DISABLE_COMPILER_MSC_WARNING_START (4996)
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    template <typename T>
    inline T Duration::As (Characters::Number::SignificantFigures p) const
        requires (same_as<T, Characters::String>)
    {
        return As<T> (AsStringOptions{.fSecondsPrecision = p});
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    template <typename T>
    inline T Duration::AsPinned () const
        requires (same_as<T, timeval> or integral<T> or floating_point<T> or same_as<T, Characters::String> or Common::IDuration<T> or
                  Common::ITimePoint<T>)
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
            if (this->count () > static_cast<rep> (numeric_limits<long>::max ())) [[unlikely]] {
                return timeval{numeric_limits<long>::max (), 0}; // close enuf for now - if that big, do the nanoseconds matter?
            }
            return As<T> ();
        }
        else if constexpr (same_as<T, Characters::String>) {
            return As<T> ();
        }
        else if constexpr (Common::IDuration<T>) {
            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wimplicit-int-float-conversion\""); // warning: implicit conversion from 'std::__1::chrono::duration<long long, std::__1::ratio<1, 1> >::rep' (aka 'long long') to 'double' changes value from 9223372036854775807 to 9223372036854775808
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
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wimplicit-int-float-conversion\"");
            return As<T> ();
        }
        else if constexpr (Common::ITimePoint<T>) {
            return T{this->AsPinned<typename T::duration> ()};
        }
    }
    inline Characters::String Duration::PrettyPrint () const
    {
        return Format ();
    }
    inline Characters::String Duration::PrettyPrint (const FormatOptions& options) const
    {
        return Format (options);
    }
    inline Characters::String Duration::PrettyPrintAge () const
    {
        return FormatAge ();
    }
    inline Characters::String Duration::PrettyPrintAge (const AgeFormatOptions& ageOptions) const
    {
        return FormatAge (ageOptions);
    }
    inline Characters::String Duration::PrettyPrintAge (const AgeFormatOptions& ageOptions, const FormatOptions& options) const
    {
        return FormatAge (ageOptions, options);
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
     ******************** Literals::operator "" _duration ***************************
     ********************************************************************************
     */
    inline namespace Literals {
        [[nodiscard]] inline Duration operator""_duration (const char* str, size_t len)
        {
            return Duration{string{str, str + len}};
        }
        [[nodiscard]] inline Duration operator""_duration (const wchar_t* str, size_t len)
        {
            return Duration{Characters::String{span{str, len}}};
        }
        [[nodiscard]] inline Duration operator""_duration (const char8_t* str, size_t len)
        {
            return Duration{Characters::String{span{str, len}}};
        }
        [[nodiscard]] inline Duration operator""_duration (const char16_t* str, size_t len)
        {
            return Duration{Characters::String{span{str, len}}};
        }
        [[nodiscard]] inline Duration operator""_duration (const char32_t* str, size_t len)
        {
            return Duration{Characters::String{span{str, len}}};
        }
        [[nodiscard]] inline Duration operator""_duration (long double _Val) noexcept
        {
            return Duration{_Val};
        }
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
    template <Common::IBuiltinArithmetic T>
    inline Duration operator* (T lhs, const Duration& rhs)
    {
        return Duration{rhs.As<DurationSeconds> () * lhs};
    }
    template <Common::IBuiltinArithmetic T>
    inline Duration operator* (const Duration& lhs, T rhs)
    {
        return Duration{lhs.As<DurationSeconds> () * rhs};
    }
    template <Common::IBuiltinArithmetic T>
    inline Duration operator/ (const Duration& lhs, T rhs)
    {
        return Duration{lhs.As<DurationSeconds> () / rhs};
    }
    inline Duration::rep operator/ (const Duration& lhs, const Duration& rhs)
    {
        return lhs.As<DurationSeconds> () / rhs.As<DurationSeconds> ();
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
