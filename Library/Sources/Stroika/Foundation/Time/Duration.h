/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md">Beta</a>
 *
 * TODO:
 *      @todo   See if I can get constexpr Duration working.
 *              This appears tricky because we need a DESTRUCTOR, and this cannot be constexpr which prevents
 *              you from having a constexpr object of that type. The closest I can get is that I could lose
 *              the string, and ALWAYS convert to float (or chron::duration) representation. Or I could make
 *              the class into a template class (but thats too close to hte existing chron::duration - so whats the point).
 *
 *                  1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Time\Duration.inl(253,34): error C3615:  constexpr function 'Stroika::Foundation::Time::Duration::min' cannot result in a constant expression (compiling source file ..\..\Sources\Stroika\Foundation\DataExchange\OptionsFile.cpp)
 *                  1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Time\Duration.inl(253,34): message :  failure was because type 'Stroika::Foundation::Time::Duration' is not a literal type (compiling source file ..\..\Sources\Stroika\Foundation\DataExchange\OptionsFile.cpp)
 *                  1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Time\Duration.inl(106,23): message :  type 'Stroika::Foundation::Time::Duration' is not a literal type because it has a user-defined destructor (compiling source file ..\..\Sources\Stroika\Foundation\DataExchange\OptionsFile.cpp)
 *                  1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Time\Duration.h(378,20): message :  type 'Stroika::Foundation::Time::Duration' is not a literal type because its data member 'fStringRep_' is of non-literal type 'std::basic_string<char,std::char_traits<char>,std::allocator<char>>' (compiling source file ..\..\Sources\Stroika\Foundation\DataExchange\OptionsFile.cpp)
 *
 *              Even with this restriction, I COULD store the string elsewhere to eliminate the DTOR. A kludge, but akin
 *              to garbage collection. Keep a list (std::map) of all allocated Duration objects which were given a string.
 *              Store the PTR as key, and the actual string pointer as value; We cannot always (maybe not even often) catch
 *              the leaked objects, but we can check each time a new Duration is constructed. If its constructed, that address
 *              was once destructed, so the associated string can be removed. OR - use a BLOB-BUFFER (stringpool) thing. Each time
 *              we get constructed, look for that string, and re-use the address (readonly afterall). If fail to find, append.
 *              For some cases, this grows indefinitely and could be bad. But for most common cases, it would probably work out fine.
 *
 *              Maybe even better, create a base class 'DurationLite' - that doesn't own a string (could make DurationLite just be
 *              Duration). Just always convert to numbers. THEN - THAT can be constexpr. And subclass Duration inherits all the functionality
 *              except overrides/replaces all the functions that return the underlying string (essentially faking the virtual part).
 *
 *              That gets us 95% of what we want - the same API we have no for Duration, and something just like and interoperable with
 *              Duration that is constexpr. Though - why not just call that chrono::duration? That maybe good enuf (esp since we typically
 *              use stuff like 2ms, etc). Maybe that's good enuf. Making conversion from 3ms to Duration 'super fast' (which I've largely
 *              done in 2.1d18
 *
 *     @todo    Then, when constexpr works, debug why/if we can make work the qCompilerAndStdLib_constexpr_Buggy/constexpr
 *              stuff for min () / max  ()
 *
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
 *      @todo   template    <typename   DURATION_REP, typename DURATION_PERIOD>
 *              chrono::duration<DURATION_REP, DURATION_PERIOD>  As () const;
 *
 *              Didn't work (at least on msvc). Try again, and see if I can combine template<> and template partial
 *              specialization to do an As<> function for any duration template. Maybe use SFINAE tricks! Yes, thats
 *              probably it...
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
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *      o   comparisons (operator <=>) inherited from chrono::duration
     */
    class Duration final : public chrono::duration<double> {
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
        Duration (const Duration& src);
        Duration (Duration&& src) noexcept;
        explicit Duration (const string& durationStr);
        explicit Duration (const Characters::String& durationStr);
        constexpr explicit Duration (int duration);
        constexpr explicit Duration (long duration);
        constexpr explicit Duration (long long duration);
        constexpr explicit Duration (float duration);
        constexpr explicit Duration (double duration);
        constexpr explicit Duration (long double duration);
        template <typename DURATION_REP, typename DURATION_PERIOD>
        constexpr Duration (const chrono::duration<DURATION_REP, DURATION_PERIOD>& d);

    public:
        ~Duration ();

    public:
        nonvirtual Duration& operator= (const Duration& rhs);
        nonvirtual Duration& operator= (Duration&& rhs) noexcept;

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
         *      time_t
         *      timeval
         *      int
         *      long int
         *      long long int
         *      wstring
         *      String
         *      float
         *      double
         *      long double
         *      chrono::duration<double>
         *      std::chrono::seconds
         *      std::chrono::milliseconds
         *      std::chrono::microseconds
         *      std::chrono::nanoseconds
         *
         *  Note - if 'empty' - As<> for numeric types returns 0.
         */
        template <typename T>
        nonvirtual T As () const;

    public:
        /**
         * Defined for
         *      std::chrono::seconds
         *      std::chrono::milliseconds
         *      std::chrono::microseconds
         *      std::chrono::nanoseconds
         *
         *  Same as As<> - except that it handles overflows, so if you pass in Duration {numeric_limits<long double>::max ()} and convert to seconds, you wont overflow,
         *  but get chrono::seconds::max
         */
        template <typename T>
        nonvirtual T AsPinned () const;

    public:
        /**
         *  Shorthand for As<String> ().AsUTF8 ()
         */
        nonvirtual string AsUTF8 () const;

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
         *          VerifyTestResult (Duration{L"PT1.4S"}.PrettyPrintAge () == L"now");
         *          VerifyTestResult (Duration{L"-PT9M"}.PrettyPrintAge () == L"now");
         *          VerifyTestResult (Duration{L"-PT20M"}.PrettyPrintAge () == L"20 minutes ago");
         *          VerifyTestResult (Duration{L"PT20M"}.PrettyPrintAge () == L"20 minutes from now");
         *          VerifyTestResult (Duration{L"PT4H"}.PrettyPrintAge () == L"4 hours from now");
         *      \endcode
         */
        nonvirtual Characters::String PrettyPrintAge (const AgePrettyPrintInfo& agePrettyPrintInfo = kDefaultAgePrettyPrintInfo, const PrettyPrintInfo& prettyPrintInfo = kDefaultPrettyPrintInfo) const;

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual Characters::String ToString () const;

    public:
        /**
         * Duration::kMin is the least duration this Duration class supports representing.
         *
         *  @todo get constexpr working here (I THINK MAYBE POSSIBLE IN C++20 since you can have constexpr dtor - not sure how but I think can work)
         *  THEN MAKE kMin
         */
        static /*constexpr*/ Duration min ();

    public:
        /**
         * Duration::kMax is the largest duration this Duration class supports representing
         *
         *  @todo get constexpr working here (I THINK MAYBE POSSIBLE IN C++20 since you can have constexpr dtor - not sure how but I think can work)
         *  THEN MAKE kMax
         */
        static /*constexpr*/ Duration max ();

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
            char   fNonStringRep_{}; // unused except to allow constexpr initialization (allow selecting non fStringRep_ to initialize since union must be initialized)
            string fStringRep_;
        };
        void destroy_ (); // allow call if already empty
    };
    template <>
    int Duration::As () const;
    template <>
    long int Duration::As () const;
    template <>
    long long int Duration::As () const;
    template <>
    wstring Duration::As () const;
    template <>
    Characters::String Duration::As () const;
    template <>
    float Duration::As () const;
    template <>
    double Duration::As () const;
    template <>
    long double Duration::As () const;
    template <>
    chrono::duration<double> Duration::As () const;
    template <>
    chrono::seconds Duration::As () const;
    template <>
    chrono::milliseconds Duration::As () const;
    template <>
    chrono::microseconds Duration::As () const;
    template <>
    chrono::nanoseconds Duration::As () const;

    template <>
    chrono::seconds Duration::AsPinned () const;
    template <>
    chrono::milliseconds Duration::AsPinned () const;
    template <>
    chrono::microseconds Duration::AsPinned () const;
    template <>
    chrono::nanoseconds Duration::AsPinned () const;

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
    Duration operator"" _duration (const char* str, size_t len) noexcept;
    Duration operator"" _duration (const wchar_t* str, size_t len) noexcept;
#if __cpp_char8_t >= 201811L
    Duration operator"" _duration (const char8_t* str, size_t len) noexcept;
#endif
    Duration operator"" _duration (const char16_t* str, size_t len) noexcept;
    Duration operator"" _duration (const char32_t* str, size_t len) noexcept;

    /**
     *  Return the sum of the two durations.
     *
     *  Must operators not needed (inherited from duration<>) - but these needed when LHS of operator is not a duration type
     */
    Duration operator+ (const DurationSecondsType& lhs, const Duration& rhs);

    /**
     *  Multiply the duration by the floating point argument
     *
     *  Must operators not needed (inherited from duration<>) - but these needed when LHS of operator is not a duration type
     */
    Duration operator* (long double lhs, const Duration& rhs);

}

namespace Stroika::Foundation::Traversal::RangeTraits {

    template <>
    struct DefaultOpenness<Time::Duration> : ExplicitOpenness<Openness::eClosed, Openness::eClosed> {
    };
    template <>
    struct DefaultDifferenceTypes<Time::Duration> : ExplicitDifferenceTypes<Time::DurationSecondsType> {
    };
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
