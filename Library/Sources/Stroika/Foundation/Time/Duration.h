/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Duration_h_
#define _Stroika_Foundation_Time_Duration_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <chrono>
#include <climits>
#include <string>

#if qStroika_Platform_POSIX
#include <sys/time.h>
#elif qStroika_Platform_Windows
#include <Winsock2.h>
#endif

#include "Stroika/Foundation/Characters/Number.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Time/Common.h"
#include "Stroika/Foundation/Time/Realtime.h"
#include "Stroika/Foundation/Traversal/Range.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md">Beta</a>
 *
 * TODO:
 *      @todo   PT3,4S and PT3.4S both must  be interpreted as 3.4 seconds. I think we can generate
 *              either, but parser must accept either. Right now we use atof(), and I'm not sure that
 *              handles either form of decimal separator! Add to regression tests, and make sure
 *              it works.
 *
 *      @todo   Do better job rounding. Right now we round (?)properly for seconds, but nothing else.
 *
 *      @todo   Consider using long double for InternalNumericFormatType_;
 *
 *      @todo   Consider adding 'significantFigures' property to FormatOptions. Think about significantFigures support/design of
 *              boost (maybe use  bignum or rational?). Probably no - but document clearly why or why not.
 *
 *      @todo   Add PRECISION support to FormatOptions.
 *          o   Number of seconds. Anything less than that number is truncated.
 *          o   So .001 'precision' means show 3.44444 as 3.444 and 60 means show 67 seconds as 'one minute'
 *          o   Maybe add option so can show > or < as in < one minute or > one minute for being passed sentinel values?
 *
 */

namespace Stroika::Foundation::Time {

    /**
     *  \brief Duration is a chrono::duration<double> (=\see DurationSeconds) - but adding ISO-8601 string representation support
     * 
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
     *  \note Reason Duration functionality not merged into DurationSeconds (why two separate types):
     *        Duration maintains the ability to recover the original string (in case conversion to float loses
     *        precision) - so Duration{"PT2.3M"} always returns that - As<String> () value, even if its converted
     *        to something else; DurationSeconds is more lightweight (and constexpr friendly).
     *
     *  \note Design Note - why no c_str () method
     *      In order to implement c_str () - we would need to return an internal pointer. That would
     *      constrain the internal implementation, and would need careful definition of lifetime. The
     *      simplest way around this is to have the caller pass something in, or to return something
     *      whose lifetime is controlled (an object). So now - just call As<String> ().c_str () or
     *      As<wstring> ().c_str ()
     *
     *  \note <a href="Design-Overview.md#Comparisons">Comparisons</a>:
     *      o   static_assert (totally_ordered<Duration>);
     */
    class Duration final : public chrono::duration<double> {
    private:
        using inherited = chrono::duration<double>;

    public:
        class FormatException;

    public:
        /**
         *  How text is read as a Duration - by Parse, the string constructor, and "..."_duration.
         *
         *  What is read is an ISO 8601 duration, in its usual format - what As<String> writes - and nothing else:
         *      o   an optional '-' (ISO 8601-2's negative duration), then a 'P'
         *      o   then elements, each a number and its designator: Y, M, D, then a T, then H, M, S - in that order, each at
         *          most once; at least one in all, and at least one after a T. Or a number of weeks alone: "P2W"
         *      o   a number is digits; the last element's may have a fraction - a '.' or ',' and digits ("PT0.5S", "P0,5D")
         *  No whitespace, around the text or within it: XML strips it from around an xs:duration before the value is read, as
         *  the XML ObjectReader does. Every xs:duration is read - its format is a subset of this one (no weeks, a '.' only, a
         *  fraction on the seconds only). The empty string is not read.
         *
         *  There are no options yet. Possible ones, should text that uses them need reading - each off by default:
         *      o   an exponent: "PT1E-3S" - ISO 8601-2 (https://www.iso.org/standard/70908.html). Stroika v3.0d12 to v3.0d22
         *          wrote one for a seconds part under 0.0001, with a lowercase 'e'
         *      o   negative elements: "PT-1S", "P-1Y-2M3DT-4H-5M-6S" - ISO 8601-2; Java's Duration and Period write them
         *          (https://docs.oracle.com/en/java/javase/21/docs/api/java.base/java/time/Duration.html#toString()), as does
         *          PostgreSQL's iso_8601 IntervalStyle (https://www.postgresql.org/docs/current/datatype-datetime.html)
         *      o   ISO 8601's alternative format: "P0003-06-04T12:30:05"
         *      o   what Stroika before v3.0d26 also read: designators out of place or repeated ("P30S", "PT1S1S"), weeks with
         *          other elements, a fraction on any element, ".5" and "5.", whitespace around or within the text
         */
        struct ParseOptions {};

    public:
        /**
         *  \brief How As<String> () writes a Duration: an ISO 8601 duration, in its usual format - what Parse reads
         *
         *  As<String> () - and so what is built on it, as ObjectVariantMapper's mapping of a Duration, and so its JSON - writes with
         *  the calling thread's AsStringOptions::GetThreadDefault (): AsStringOptions{}, unless a ScopedThreadDefault has set others.
         *  Per thread: a thread pool's worker does not see the scope of the thread that queued its task. As<String> (options) writes
         *  with the options given.
         *
         *  Extension points, each a field off by default: an exponent in the seconds ("PT1.5E-9S", ISO 8601-2) - the only way to keep
         *  a huge or tiny Duration short (Duration{numeric_limits<double>::max ()} writes about 300 digits of years, which no
         *  precision shortens); weeks ("P3W").
         *
         *  \note Satisfies Concepts:
         *      o   Common::IConstexprable<AsStringOptions>
         */
        struct AsStringOptions {
            /**
             *  The seconds - the one element written with a fraction - never with an exponent (ISO 8601 Part 1 has none). By default
             *  6 significant figures, for short text - so not read back exactly; Precision::ShortestRoundTrip () reads back as the
             *  same Duration.
             */
            Characters::Number::Precision fSecondsPrecision{Characters::Number::Precision::MaxSignificantDigits (6)};

            /**
             *  \brief The calling thread's: AsStringOptions{}, unless a ScopedThreadDefault has set others
             */
            static AsStringOptions GetThreadDefault ();

            class ScopedThreadDefault;
        };

    public:
        /**
         *  A string is read as Parse reads it (an ISO 8601 duration, as "P3DT2H" or "-PT0.5S"), throwing its
         *  FormatException if it cannot. A std::string is not checked for ASCII first: a character that is not ASCII is a
         *  FormatException too.
         *
         *  Duration{} is zero ("PT0S"), as chrono::duration's is. For no value, use optional<Duration>.
         *
         *  \note constexpr not really working (though declared) - see @todo above
         *
         *  \note for numeric overloads (includes chrono::duration), require (not isnan (src)) - but allow isinf()
         */
        constexpr Duration ();
        Duration (Duration&& src) noexcept;
        Duration (const Duration& src);
#if qCompilerAndStdLib_default_member_initializer_needed_within_enclosing_class_Buggy
        template <Characters::IConvertibleToString STRINGISH_T>
        explicit Duration (STRINGISH_T&& durationStr, const ParseOptions& options);
        template <Characters::IConvertibleToString STRINGISH_T>
        explicit Duration (STRINGISH_T&& durationStr); ///< \brief Same as Duration{durationStr, ParseOptions{}}
#else
        template <Characters::IConvertibleToString STRINGISH_T>
        explicit Duration (STRINGISH_T&& durationStr, const ParseOptions& options = {});
#endif
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
         *  \brief text as a Duration - an ISO 8601 duration, as ParseOptions describes - or a FormatException saying what is
         *  wrong with it
         *
         *  \par Example Usage
         *      \code
         *          Duration d = Execution::ThrowIfFailed (Duration::Parse (text));
         *          Duration e = Duration::Parse (text).value_or (Duration{0});
         *      \endcode
         */
#if qCompilerAndStdLib_default_member_initializer_needed_within_enclosing_class_Buggy
        static Common::StdCompat::expected<Duration, FormatException> Parse (const Characters::String& text, const ParseOptions& options);
        static Common::StdCompat::expected<Duration, FormatException> Parse (const Characters::String& text); ///< \brief Same as Parse (text, ParseOptions{})
#else
        static Common::StdCompat::expected<Duration, FormatException> Parse (const Characters::String& text, const ParseOptions& options = {});
#endif

    public:
        /**
         *  \brief Make this zero - the same as *this = Duration{}
         *
         *  \note Before Stroika v3.0d26, there was an empty Duration (Duration{} and Duration{""}), distinct from zero.
         */
        [[deprecated ("Since Stroika v3.0d26 - there is no empty Duration: assign Duration{}, or use optional<Duration> for no "
                      "value")]] nonvirtual void
        clear ();

    public:
        /**
         *  \brief Is this zero - the same as *this == Duration{}
         *
         *  \note Before Stroika v3.0d26, there was an empty Duration (Duration{} and Duration{""}), distinct from zero.
         */
        [[deprecated ("Since Stroika v3.0d26 - there is no empty Duration: compare with Duration{}, or use optional<Duration> for "
                      "no value")]] constexpr bool
        empty () const;

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
         *  \brief Multiply this by rhs - the same as *this = *this * rhs
         *
         *  Duration declares its own compound assignments and increments, though chrono::duration has them: chrono's change
         *  only the count, so a Duration read from text would go on writing that text (As<String>), not its new value.
         */
        template <Common::IBuiltinArithmetic T>
        nonvirtual Duration& operator*= (T rhs);

    public:
        /**
         *  \brief Divide this by rhs - the same as *this = *this / rhs
         */
        template <Common::IBuiltinArithmetic T>
        nonvirtual Duration& operator/= (T rhs);

    public:
        /**
         *  \brief Add one second - chrono::duration<double>'s tick
         */
        nonvirtual Duration& operator++ ();
        nonvirtual Duration  operator++ (int); ///< \brief Add one second, returning the Duration before

    public:
        /**
         *  \brief Subtract one second - chrono::duration<double>'s tick
         */
        nonvirtual Duration& operator-- ();
        nonvirtual Duration  operator-- (int); ///< \brief Subtract one second, returning the Duration before

    public:
        /**
         * Only specifically specialized variants are supported. Defined for
         *  o   timeval
         *  o   integral<T>
         *  o   floating_point<T>
         *  o   String
         *  o   Common::IDuration<T>
         *  o   Common::ITimePoint<T>
         * 
         *  Note this implies inclusion of:
         *      o   time_t
         *      o   int, float, etc...
         *      o   std::chrono::milliseconds etc...
         *
         *  As<String> () gives back the text a Duration was read from, if it was; else writes it as the calling thread's
         *  AsStringOptions say (AsStringOptions::GetThreadDefault ()). As<String> (options) writes it as options says.
         *
         *  \@todo unsafe if value out of range - decode how to handle - probably should throw if out of range, but unclear - see AsPinned()
         * 
         *      \note Stroika v2.1 also supported wstring, which was (tentatively) de-supported in Stroika v3.0d5
         * 
         *  \note No exponential numerical format is generated (nor read - ParseOptions). This IS supported in ISO 8601 in Part 2,
         *        but not the original ISO 8601, and the luxon JS library (probably among others) doesn't support it.
         */
        template <typename T>
        nonvirtual T As () const
            requires (Common::IAnyOf<T, timeval, Characters::String> or integral<T> or floating_point<T> or Common::IDuration<T> or
                      Common::ITimePoint<T>);
        template <typename T>
        nonvirtual T As (const AsStringOptions& options) const
            requires (same_as<T, Characters::String>);
        DISABLE_COMPILER_MSC_WARNING_START (4996)
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
        template <typename T>
        [[deprecated ("Since Stroika v3.0d26 use As<String> (AsStringOptions{.fSecondsPrecision = p})")]] nonvirtual T As (Characters::Number::SignificantFigures p) const
            requires (same_as<T, Characters::String>);
        DISABLE_COMPILER_MSC_WARNING_END (4996)
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

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
            requires (same_as<T, timeval> or integral<T> or floating_point<T> or same_as<T, Characters::String> or Common::IDuration<T> or
                      Common::ITimePoint<T>);

    public:
        /**
         *  Shorthand for As<String> ().AsUTF8 ()
         */
        nonvirtual u8string AsUTF8 () const;

    public:
        /**
         *  \brief The unit names Format () and FormatAge () write, singular and plural - English by default
         */
        struct FormatOptions {
            /**
             */
            struct Labels {
                Characters::String fYear{"year"sv};
                Characters::String fYears{"years"sv};
                Characters::String fMonth{"month"sv};
                Characters::String fMonths{"months"sv};
                Characters::String fWeek{"week"sv};
                Characters::String fWeeks{"weeks"sv};
                Characters::String fDay{"day"sv};
                Characters::String fDays{"days"sv};
                Characters::String fHour{"hour"sv};
                Characters::String fHours{"hours"sv};
                Characters::String fMinute{"minute"sv};
                Characters::String fMinutes{"minutes"sv};
                Characters::String fSecond{"second"sv};
                Characters::String fSeconds{"seconds"sv};
                Characters::String fMilliSecond{"ms"sv};
                Characters::String fMilliSeconds{"ms"sv};
                Characters::String fMicroSecond{L"\u00b5s"sv};
                Characters::String fMicroSeconds{L"\u00b5s"sv};
                Characters::String fNanoSecond{"ns"sv};
                Characters::String fNanoSeconds{"ns"sv};
                Characters::String fPicoSecond{"ps"sv};
                Characters::String fPicoSeconds{"ps"sv};
            } fLabels;
        };

    public:
        /**
         *  \brief The Duration in words, for people to read - "3 seconds", "1.003 seconds", "3 ms" - like JavaScript's 'humanize' APIs
         *
         *  \par Example Usage
         *      \code
         *          EXPECT_EQ (Duration{3}.Format (), "3 seconds");
         *          Duration::FormatOptions options;
         *          options.fLabels.fSeconds = "secs";
         *          EXPECT_EQ (Duration{3}.Format (options), "3 secs");
         *      \endcode
         *
         *  \note Format () formats with a FormatOptions{} built once, not per call: building one - its 22 labels - costs about as
         *        much as the formatting. So FormatOptions is not a default argument (= {}).
         */
        nonvirtual Characters::String Format () const;
        nonvirtual Characters::String Format (const FormatOptions& options) const; ///< \brief Format () with the labels given

    public:
        /**
         *  \brief The words FormatAge () puts around the number, and how near zero is "now"
         */
        struct AgeFormatOptions {
            /**
             */
            struct Labels {
                Characters::String fNow{"now"sv};
                Characters::String fAgo{"ago"sv};
                Characters::String fFromNow{"from now"sv}; // could use 'until' or 'from now'
            } fLabels;

            /**
             *  \brief In seconds: a Duration nearer zero than this, either way, is fLabels.fNow
             */
            double fNowThreshold{12 * 60};
        };

    public:
        /**
         *  \brief The Duration as a time relative to now, for people to read - "now", "20 minutes ago", "4 hours from now" - in
         *         one unit, rounded
         *
         *  Inspired by useful JQuery plugin http://ksylvest.github.io/jquery-age/; or https://momentjs.com/
         *
         *  Technically, this isn't an 'age' but just a 'now-centric' pretty printing of durations.
         *
         *  \todo Just a DRAFT impl for now ... --LGP 2014-09-01 -- @todo FIX/revise/test
         *
         *  \par Example Usage
         *      \code
         *          EXPECT_EQ (Duration{"PT1.4S"}.FormatAge (), "now");
         *          EXPECT_EQ (Duration{"-PT9M"}.FormatAge (), "now");
         *          EXPECT_EQ (Duration{"-PT20M"}.FormatAge (), "20 minutes ago");
         *          EXPECT_EQ (Duration{"PT20M"}.FormatAge (), "20 minutes from now");
         *          EXPECT_EQ (Duration{"PT4H"}.FormatAge (), "4 hours from now");
         *          EXPECT_EQ (Duration{"-PT9M"}.FormatAge (Duration::AgeFormatOptions{.fNowThreshold = 60}), "9 minutes ago");
         *      \endcode
         *
         *  \note Like Format (), FormatAge () formats with options built once, not per call.
         */
        nonvirtual Characters::String FormatAge () const;
        nonvirtual Characters::String FormatAge (const AgeFormatOptions& ageOptions) const; ///< \brief FormatAge () with the AgeFormatOptions given
        nonvirtual Characters::String FormatAge (const AgeFormatOptions& ageOptions, const FormatOptions& options) const; ///< \brief FormatAge () with both options given

    public:
        using PrettyPrintInfo [[deprecated ("Since Stroika v3.0d26 use FormatOptions")]]       = FormatOptions;
        using AgePrettyPrintInfo [[deprecated ("Since Stroika v3.0d26 use AgeFormatOptions")]] = AgeFormatOptions;
        [[deprecated ("Since Stroika v3.0d26 use FormatOptions{}")]] static const FormatOptions       kDefaultPrettyPrintInfo;
        [[deprecated ("Since Stroika v3.0d26 use AgeFormatOptions{}")]] static const AgeFormatOptions kDefaultAgePrettyPrintInfo;
        [[deprecated ("Since Stroika v3.0d26 use Format")]] nonvirtual Characters::String PrettyPrint () const;
        [[deprecated ("Since Stroika v3.0d26 use Format")]] nonvirtual Characters::String PrettyPrint (const FormatOptions& options) const;
        [[deprecated ("Since Stroika v3.0d26 use FormatAge")]] nonvirtual Characters::String PrettyPrintAge () const;
        [[deprecated ("Since Stroika v3.0d26 use FormatAge")]] nonvirtual Characters::String PrettyPrintAge (const AgeFormatOptions& ageOptions) const;
        [[deprecated ("Since Stroika v3.0d26 use FormatAge")]] nonvirtual Characters::String PrettyPrintAge (const AgeFormatOptions& ageOptions,
                                                                                                             const FormatOptions& options) const;

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

    private:
        static Common::StdCompat::expected<Duration, FormatException> Parse_ (string_view text, const ParseOptions& options);
        static Common::StdCompat::expected<InternalNumericFormatType_, FormatException> ParseTime_ (string_view s, const ParseOptions& options);
        static Characters::String UnParseTime_ (InternalNumericFormatType_ t, const AsStringOptions& options);

    private:
        /**
         *  We ALWAYS store the numeric value (inherited). With eString_, we also store the text it was read from, which
         *  As<String> gives back.
         */
        enum RepType_ {
            eNumeric_,
            eString_
        };
        RepType_ fRepType_{eNumeric_};
        union {
            char fNonStringRep_{}; // unused except to allow constexpr initialization (allow selecting non fStringRep_ to initialize since union must be initialized)
            string fStringRep_;
        };
        constexpr void destroy_ (); // drops the text, if any, leaving eNumeric_ (and the numeric value unchanged)
    };
    static_assert (totally_ordered<Duration>);
    static_assert (Common::IConstexprable<Duration::AsStringOptions>); // see Satisfies Concepts

    /**
     *  \brief Sets the calling thread's Duration::AsStringOptions for this object's lifetime, then the previous ones again: scopes nest
     *
     *  \par Example Usage
     *      \code
     *          Duration::AsStringOptions::ScopedThreadDefault exact{{.fSecondsPrecision = Characters::Number::Precision::ShortestRoundTrip ()}};
     *          String json = DataExchange::Variant::JSON::Writer{}.WriteAsString (mapper.FromObject (myObject)); // its Durations read back exactly
     *      \endcode
     *
     *  \note The thread's setting is a constinit thread_local pointer to the innermost scope's options (none: AsStringOptions{}) -
     *        a read costs a load or two (a call, on macOS), and a scope two stores.
     */
    class Duration::AsStringOptions::ScopedThreadDefault {
    public:
        ScopedThreadDefault (const AsStringOptions& options);
        ScopedThreadDefault (const ScopedThreadDefault&)            = delete;
        ScopedThreadDefault& operator= (const ScopedThreadDefault&) = delete;
        ~ScopedThreadDefault ();

    private:
        AsStringOptions        fOptions_;
        const AsStringOptions* fPrevious_;
    };

    class Duration::FormatException : public Execution::Exception<runtime_error> {
    private:
        using inherited = Execution::Exception<runtime_error>;

    public:
        FormatException ();
        FormatException (const Characters::String& details); ///< \brief "Invalid Duration Format: " and details

    public:
        static const FormatException kThe;
    };
    inline const Duration::FormatException Duration::FormatException::kThe;

    inline namespace Literals {

        /**
         *  \brief user defined literal for Duration, specified in ISO8601 format.
         */
        [[nodiscard]] Duration operator""_duration (const char* str, size_t len);
        [[nodiscard]] Duration operator""_duration (const wchar_t* str, size_t len);
        [[nodiscard]] Duration operator""_duration (const char8_t* str, size_t len);
        [[nodiscard]] Duration operator""_duration (const char16_t* str, size_t len);
        [[nodiscard]] Duration operator""_duration (const char32_t* str, size_t len);
        [[nodiscard]] Duration operator""_duration (long double _Val) noexcept;
    }

    /**
     *  Return the sum of the two durations.
     *
     *  Must operators not needed (inherited from duration<>) - but these needed when LHS of operator is not a duration type
     */
    Duration operator+ (const DurationSeconds& lhs, const Duration& rhs);

    /**
     *  \brief A Duration times a number, either way round - a Duration
     *
     *  chrono::duration's own give a chrono::duration. These take any arithmetic type: one taking a single type (long double,
     *  say) is ambiguous with chrono's for every other type, each matching one argument exactly.
     *
     *  \par Example Usage
     *      \code
     *          EXPECT_EQ (2 * Duration{3}, Duration{6});
     *          EXPECT_EQ ((Duration{"PT3S"} * 0.5).As<String> (), "PT1.5S");
     *      \endcode
     */
    template <Common::IBuiltinArithmetic T>
    Duration operator* (T lhs, const Duration& rhs);
    template <Common::IBuiltinArithmetic T>
    Duration operator* (const Duration& lhs, T rhs); ///< \brief A Duration times a number - a Duration

    /**
     *  \brief A Duration divided by a number - a Duration; or by a Duration - a number, how many times rhs goes into lhs
     *
     *  chrono::duration's give a chrono::duration, and Duration / Duration a chrono::duration<Duration>: Duration is not a
     *  chrono::duration specialization, so chrono reads the right-hand one as a number.
     */
    template <Common::IBuiltinArithmetic T>
    Duration      operator/ (const Duration& lhs, T rhs);
    Duration::rep operator/ (const Duration& lhs, const Duration& rhs); ///< \brief How many times rhs goes into lhs, as chrono::duration's

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
