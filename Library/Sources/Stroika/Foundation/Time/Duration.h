/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Duration_h_
#define _Stroika_Foundation_Time_Duration_h_ 1

#include "../StroikaPreComp.h"

#include <chrono>
#include <climits>
#include <string>

#if qPlatform_POSIX
#include <sys/time.h>
#elif qPlatform_Windows
#include <Winsock2.h>
#endif

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Execution/Exceptions.h"
#include "../Traversal/Range.h"

#include "Common.h"
#include "Realtime.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md">Beta</a>
 *
 * TODO:
 *      @todo   POSSIBLY add support for Precision (see Characters::Float2String) - once that module has clenaned up
 *              notion of precision. Not sure how to add unobtrusively. - for As<String>()? optional param?...
 *              Maybe Float2StringOptions is optional param to As<String> ()???
 *
 *      @todo   PT3,4S and PT3.4S both must  be interpretted as 3.4 seconds. I think we can generate
 *              either, but parser must accept either. Right now we use atof(), and I'm not sure that
 *              handles either form of decimal separator! Add to regression tests, and make sure
 *              it works.
 *
 *      @todo   Do better job rounding. Right now we round (?)properly for seconds, but nothing else.
 *
 *      @todo   Consider using long double for InternalNumericFormatType_;
 *
 *      @todo   Consider adding 'precision' property to PrettyPrintInfo. Think about precision support/design of
 *              boost (maybe use  bignum or rational?). Probably no - but document clearly why or why not.
 *
 *      @todo   Add PRECISION support to PrettyPrintInfo argument to PrettyPrint () function.
 *          o   Number of seconds. Anything less than that number is truncated.
 *          o   So .001 'precision' means show 3.44444 as 3.444 and 60 means show 67 seconds as 'one minute'
 *          o   Maybe add option so can show > or < as in < one minute or > one minute for being passed sentainl values?
 *
 */

namespace Stroika::Foundation::Time {

    /**
     * (basic) support for ISO 8601 Durations
     *      http://en.wikipedia.org/wiki/ISO_8601#Durations
     *
     *  Note: according to glibc docs - year is always 365 days, month always 30 days, etc, as far
     *  as these duration objects go - at least for conversion to/from time_t. Seek a better
     *  reference for this claim!
     *
     *  Note also - there are two iso 8601 duration formats - one date like (YYYY-MM....) and this
     *  one P...T...3S). As far as I know - XML always uses the later. For now - this implementation
     *  only supports the later.
     *
     *  @see    http://bugzilla/show_bug.cgi?id=468
     *
     *  This class is roughly equivalent to the .Net Timespan class, and also might be called
     *  "time period", or "timespan".
     *
     *  Note that a Duration may be negative.
     *
     *  It is best to logically think of Duration as a number of seconds (perhaps lossily)
     *  since for comparisons that's how things are normalized. #days etc are dumbed down
     *  to number of seconds for comparison sakes.
     *
     *  \note constexpr not really working (though declared) - see @todo above
     *
     *  \note Design Note - why no c_str () method
     *      In order to implement c_str () - we would need to return an internal pointer. That would
     *      constrain the internal implementation, and would need careful definition of lifetime. The
     *      simplest way around this is to have the caller pass something in, or to return something
     *      whose lifetime is controlled (an object). So now - just call As<String> ().c_str () or
     *      As<wstring> ().c_str ()
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   comparisons (operator <=>) inherited from chrono::duration
     */
    class [[nodiscard]] Duration final : public chrono::duration<double> {
    private:
        using inherited = chrono::duration<double>;

    public:
        /**
         *  The characterset of the std::string CTOR is expected to be all ascii, or the code throws FormatException
         *
         *  Throws (FormatException) if bad format
         *
         *  \note constexpr not really working (though declared) - see @todo above
         *
         *  \note for numeric overloads, require (not isnan (src)) - but allow isinf()
         */
        constexpr Duration ();
        Duration (Duration&& src) noexcept;
        Duration (const Duration& src);
        template <Characters::IConvertibleToString STRINGISH_T>
        explicit Duration (STRINGISH_T&& durationStr);
        constexpr Duration (int durationInSeconds);
        constexpr Duration (long durationInSeconds);
        constexpr Duration (long long durationInSeconds);
        constexpr Duration (float durationInSeconds);
        constexpr Duration (double durationInSeconds);
        constexpr Duration (long double durationInSeconds) noexcept;
        template <typename DURATION_REP, typename DURATION_PERIOD>
        constexpr Duration (const chrono::duration<DURATION_REP, DURATION_PERIOD>& d);

    public:
        constexpr ~Duration ();

    public:
        nonvirtual Duration& operator= (Duration&& rhs) noexcept;
        nonvirtual Duration& operator= (const Duration& rhs);

    private:
        using InternalNumericFormatType_ = inherited::rep;

    public:
        /**
         */
        nonvirtual void clear ();

    public:
        /**
         */
        constexpr bool empty () const;

    public:
        /**
         *  Add the given duration to this (equivalent to *this = *this + rhs;).
         */
        nonvirtual Duration& operator+= (const Duration& rhs);

    public:
        /**
         */
        nonvirtual Duration& operator-= (const Duration& rhs);

    public:
        /**
         * Only specifically specialized variants are supported. Defined for
         *  o   timeval
         *  o   integral<T>
         *  o   floating_point<T>
         *  o   String
         *  o   Configuration::IDuration<T>
         *  o   Configuration::ITimePoint<T>
         * 
         *  Note this implies inclusion of:
         *      o   time_t
         *      o   int, float, etc...
         *      o   std::chrono::milliseconds etc...
         *
         *  Note - if 'empty' - As<> for numeric types returns 0.
         *
         *  \@todo unsafe if value out of range - decode how to handle - probably should throw if out of range, but unclear - see AsPinned()
         * 
         *      \note Stroika v2.1 also supproted wstring, which was (tentiatively) desupported in Stroika v3.0d5
         */
        template <typename T>
        nonvirtual T As () const
            requires (same_as<T, timeval> or integral<T> or floating_point<T> or same_as<T, Characters::String> or
                      Configuration::IDuration<T> or Configuration::ITimePoint<T>);

    public:
        /**
         *  \see As<T>, but automatically takes care of pinning values so always safe and in range.
         *
         *  Same as As<> - except that it handles overflows, so if you pass in Duration {numeric_limits<long double>::max ()} and convert to seconds, you wont overflow,
         *  but get chrono::seconds::max
         * 
         *      @todo same requires stuff as we have with As<T>()
         */
        template <typename T>
        nonvirtual T AsPinned () const
            requires (same_as<T, timeval> or integral<T> or floating_point<T> or same_as<T, Characters::String> or
                      Configuration::IDuration<T> or Configuration::ITimePoint<T>);

    public:
        /**
         *  Shorthand for As<String> ().AsUTF8 ()
         */
        nonvirtual u8string AsUTF8 () const;

    public:
        /**
         */
        struct PrettyPrintInfo {
            /**
             */
            struct Labels {
                Characters::String fYear;
                Characters::String fYears;
                Characters::String fMonth;
                Characters::String fMonths;
                Characters::String fWeek;
                Characters::String fWeeks;
                Characters::String fDay;
                Characters::String fDays;
                Characters::String fHour;
                Characters::String fHours;
                Characters::String fMinute;
                Characters::String fMinutes;
                Characters::String fSecond;
                Characters::String fSeconds;
                Characters::String fMilliSecond;
                Characters::String fMilliSeconds;
                Characters::String fMicroSecond;
                Characters::String fMicroSeconds;
                Characters::String fNanoSecond;
                Characters::String fNanoSeconds;
                Characters::String fPicoSecond;
                Characters::String fPicoSeconds;
            } fLabels;
        };

    public:
        static const PrettyPrintInfo kDefaultPrettyPrintInfo;

    public:
        /**
         *  Consider deprecating, and using 'Format'
         */
        nonvirtual Characters::String PrettyPrint (const PrettyPrintInfo& prettyPrintInfo = kDefaultPrettyPrintInfo) const;

    public:
        /**
         */
        nonvirtual Characters::String Format (const PrettyPrintInfo& prettyPrintInfo = kDefaultPrettyPrintInfo) const;

    public:
        struct AgePrettyPrintInfo {
            struct Labels {
                Characters::String fNow;
                Characters::String fAgo;
                Characters::String fFromNow; // could use 'until' or 'from now'
            } fLabels;
            double fNowThreshold{};
        };

    public:
        static const AgePrettyPrintInfo kDefaultAgePrettyPrintInfo;

    public:
        /**
         *  Inspired by useful JQuery plugin http://ksylvest.github.io/jquery-age/; or https://momentjs.com/
         *
         *  Technically, this isn't an 'age' but just a 'now-centric' pretty printing of durations.
         *
         *  \todo Just a DRAFT impl for now ... --LGP 2014-09-01 -- @todo FIX/revise/test
         *
         *  \par Example Usage
         *      \code
         *          VerifyTestResult (Duration{"PT1.4S"}.PrettyPrintAge () == "now");
         *          VerifyTestResult (Duration{"-PT9M"}.PrettyPrintAge () == "now");
         *          VerifyTestResult (Duration{"-PT20M"}.PrettyPrintAge () == "20 minutes ago");
         *          VerifyTestResult (Duration{"PT20M"}.PrettyPrintAge () == "20 minutes from now");
         *          VerifyTestResult (Duration{"PT4H"}.PrettyPrintAge () == "4 hours from now");
         *      \endcode
         */
        nonvirtual Characters::String PrettyPrintAge (const AgePrettyPrintInfo& agePrettyPrintInfo = kDefaultAgePrettyPrintInfo,
                                                      const PrettyPrintInfo&    prettyPrintInfo    = kDefaultPrettyPrintInfo) const;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual Characters::String ToString () const;

    public:
        /**
         * Duration::kMin is the least duration this Duration class supports representing.
         */
        static constexpr Duration min ();

    public:
        /**
         * Duration::kMax is the largest duration this Duration class supports representing
         */
        static constexpr Duration max ();

    public:
        /**
         *  Unary negation
         */
        nonvirtual Duration operator- () const;

    public:
        class FormatException;

    private:
        static InternalNumericFormatType_ ParseTime_ (const string& s);
        static string                     UnParseTime_ (InternalNumericFormatType_ t);

    private:
        /**
         *  3 types - 'empty' (possibly we can lose this due to presence of optional) - and 
         *  string, and numeric.
         *
         *  We ALWAYS store the numeric value. We OPTIONALLY store the string value. So fNumericRepOrCache_
         *  is ALWAYS valid (unless fRepType==eEmpty_).
         */
        static constexpr InternalNumericFormatType_ kValueWhenEmptyRenderedAsNumber_{0};
        enum RepType_ {
            eEmpty_,
            eString_,
            eNumeric_
        };
        RepType_ fRepType_{eEmpty_};
        union {
            char fNonStringRep_{}; // unused except to allow constexpr initialization (allow selecting non fStringRep_ to initialize since union must be initialized)
            string fStringRep_;
        };
        constexpr void destroy_ (); // allow call if already empty
    };

    class Duration::FormatException : public Execution::RuntimeErrorException<> {
    private:
        using inherited = Execution::RuntimeErrorException<>;

    public:
        FormatException ();

    public:
        static const FormatException kThe;
    };
    inline const Duration::FormatException Duration::FormatException::kThe;

    /**
     *  \brief user defined literal for Duration, specified in ISO8601 format.
     */
    [[nodiscard]] Duration operator"" _duration (const char* str, size_t len);
    [[nodiscard]] Duration operator"" _duration (const wchar_t* str, size_t len);
    [[nodiscard]] Duration operator"" _duration (const char8_t* str, size_t len);
    [[nodiscard]] Duration operator"" _duration (const char16_t* str, size_t len);
    [[nodiscard]] Duration operator"" _duration (const char32_t* str, size_t len);
    [[nodiscard]] Duration operator"" _duration (long double _Val) noexcept;

    /**
     *  Return the sum of the two durations.
     *
     *  Must operators not needed (inherited from duration<>) - but these needed when LHS of operator is not a duration type
     */
    Duration operator+ (const DurationSeconds& lhs, const Duration& rhs);

    /**
     *  Multiply the duration by the floating point argument
     *
     *  Must operators not needed (inherited from duration<>) - but these needed when LHS of operator is not a duration type
     */
    Duration operator* (long double lhs, const Duration& rhs);

}

namespace Stroika::Foundation::Traversal::RangeTraits {

    template <>
    struct DefaultOpenness<Time::Duration> : ExplicitOpenness<Openness::eClosed, Openness::eClosed> {};
    template <>
    struct DefaultDifferenceTypes<Time::Duration> : ExplicitDifferenceTypes<Time::DurationSeconds> {};
    /**
     *  \note   This type's properties (kLowerBound/kUpperBound) can only be used after static initialization, and before
     *          static de-initialization.
     */
    template <>
    struct Default<Time::Duration> : ExplicitOpennessAndDifferenceType<Time::Duration> {
        static const Time::Duration kLowerBound;
        static const Time::Duration kUpperBound;

        static Time::Duration GetNext (Time::Duration i);
        static Time::Duration GetPrevious (Time::Duration i);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Duration.inl"

#endif /*_Stroika_Foundation_Time_Duration_h_*/
