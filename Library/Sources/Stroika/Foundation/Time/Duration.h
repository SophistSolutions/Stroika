/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Duration_h_
#define _Stroika_Foundation_Time_Duration_h_ 1

#include "../StroikaPreComp.h"

#include <chrono>
#include <climits>
#include <string>

#if qPlatform_Windows
#include <Windows.h>
#endif

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Execution/Exceptions.h"

#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md">Alpha-Late</a>
 *
 * TODO:
 *      @todo   now that we have constexpr Duration
 *              debug why/if we can make work the qCompilerAndStdLib_constexpr_Buggy/constexpr
 *              stuff for kMin/kMax
 *
 *              For now using ModuleInit<> code to assure proper construction order.
 *
 *              After I get this working, consider fixing derivitate classes like DurationRange
 *              to also use constexpr - but this one must work first!
 *
 *      @todo   Consider possibly storing chrono::duration<> as the representation for 'numeric' type instead of double/float. This might
 *              produce less overflow problems.
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
 *          o   So .001 �precision� means show 3.44444 as 3.444 and 60 means show 67 seconds as �one minute�
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

    namespace Private_ {
        struct Duration_ModuleData_;
    }

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
     *  It is best to logically think of Duration as a number of seconds (perhaps lossily) �
     *  since for comparisons that's how things are normalized. #days etc are dumbed down
     *  to number of seconds for comparison sakes.
     *
     *  \note Design Note - why no c_str () method
     *      In order to implement c_str () - we would need to return an internal pointer. That would
     *      constrain the internal implementation, and would need careful definition of lifetime. The
     *      simplest way around this is to have the caller pass something in, or to return something
     *      whose lifetime is controlled (an object). So now - just call As<String> ().c_str () or
     *      As<wstring> ().c_str ()
     *
     *  \note   This type properties (kMin/kMax) can only be used after static initialization, and before
     *          static de-initializaiton.
     *
     *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
     */
    class Duration final {
    public:
        /**
         *  The characterset of the std::string CTOR is expected to be all ascii, or the code throws FormatException
         *
         *  Throws (FormatException) if bad format
         *
         *  \note for numeric overloads, require (not isnan (src)) - but allow isinf()
         */
        constexpr Duration ();
        Duration (const Duration& src);
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
         *  Inspired by useful JQuery plugin http://ksylvest.github.io/jquery-age/
         *
         *  Technically, this isn't an 'age' but just a 'now-centric' pretty printing of durations.
         *
         *  Just a DRAFT impl for now ... --LGP 2014-09-01 -- @todo FIX/revise/test
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
         */
        static const Duration& kMin;

    public:
        /**
         * Duration::kMax is the largest duration this Duration class supports representing.
         */
        static const Duration& kMax;

    public:
        /**
         *  Unary negation
         */
        nonvirtual Duration operator- () const;

    public:
        class FormatException;

    public:
        /**
         *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs. Note - for the purpose of
         * this comparison function - see the notes about 'empty' in the class description.
         */
        nonvirtual int Compare (const Duration& rhs) const;

    private:
        using InternalNumericFormatType_ = double;
        static InternalNumericFormatType_ ParseTime_ (const string& s);
        static string                     UnParseTime_ (InternalNumericFormatType_ t);

    private:
        friend Stroika::Foundation::Time::Private_::Duration_ModuleData_;

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
        InternalNumericFormatType_ fNumericRepOrCache_{kValueWhenEmptyRenderedAsNumber_}; // we ALWAYS compute this (even if string rep) since frequently used
        void                       destroy_ ();                                           // allow call if already empty
        void                       construct_ (const string& s);
        void                       construct_ (InternalNumericFormatType_ n);
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

    /**
     *  operator indirects to Duration::Compare()
     */
    bool operator< (const Duration& lhs, const Duration& rhs);

    /**
     *  operator indirects to Duration::Compare()
     */
    bool operator<= (const Duration& lhs, const Duration& rhs);

    /**
     *  operator indirects to Duration::Compare()
     */
    bool operator== (const Duration& lhs, const Duration& rhs);

    /**
     *  operator indirects to Duration::Compare()
     */
    bool operator!= (const Duration& lhs, const Duration& rhs);

    /**
     *  operator indirects to Duration::Compare()
     */
    bool operator>= (const Duration& lhs, const Duration& rhs);

    /**
     *  operator indirects to Duration::Compare()
     */
    bool operator> (const Duration& lhs, const Duration& rhs);

    /**
     *  Return the sum of the two durations.
     */
    Duration operator+ (const Duration& lhs, const Duration& rhs);

    /**
     *  Return the difference of the two durations.
     */
    Duration operator- (const Duration& lhs, const Duration& rhs);

    /**
     *  Multiply the duration by the floating point argument
     */
    Duration operator* (const Duration& lhs, long double rhs);
    Duration operator* (long double lhs, const Duration& rhs);

    /**
     *  Divide the duration by the floating point argument
     */
    Duration operator/ (const Duration& lhs, long double rhs);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Duration.inl"

#endif /*_Stroika_Foundation_Time_Duration_h_*/
