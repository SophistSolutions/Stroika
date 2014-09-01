/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Duration_h_
#define _Stroika_Foundation_Time_Duration_h_    1

#include    "../StroikaPreComp.h"

#include    <chrono>
#include    <climits>
#include    <string>

#if     qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../Characters/String.h"
#include    "../Configuration/Common.h"
#include    "../Execution/StringException.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html">Alpha-Late</a>
 *
 * TODO:
 *
 *      @todo   POSSIBLY add support for Precision (see Characters::Float2String) - once that module has clenaned up
 *              notion of precision. Not sure how to add unobtrusively. - for As<String>()? optional param?...
 *              Maybe Float2StringOptions is optional param to As<String> ()???
 *
 *      @todo   Debug why/if we can make work the qCompilerAndStdLib_constexpr_Buggy/constexpr
 *              stuff for kMin/kMax
 *
 *              For now using ModuleInit<> code to assure proper construction order.
 *
 *              After I get this working, consider fixing derivitate classes like DurationRange
 *              to also use constexpr - but this one must work first!
 *
 *      @todo   PT3,4S and PT3.4S both must  be interpretted as 3.4 seconds. I think we can generate
 *              either, but parser must accept either. Right now we use atof(), and I'm not sure that
 *              handles either form of decimal separator! Add to regression tests, and make sure
 *              it works.
 *
 *      @todo   Do better job converting to/from std::duration<>. Unclear if I should just use
 *              templated CTOR to map all the types or overload the predefined milliseconds/microseconds
 *              etc.
 *
 *              One issue with the template stuff is that when it goes wrong, its a horrid mess to understand
 *              the error. But thats probably a temporary issue.
 *
 *      @todo   Consider storing BOTH numeric and string representations - as a performance hack (both optionally).
 *              If we are handled a 'chrono' time, and convert to double, there is no reason to go through
 *              a string representation! (or the other way around).
 *
 *      @todo   Do better job rounding. Right now we round (?)properly for seconds, but nothing else.
 *
 *      @todo   Add support for long double (and perhaps others?). And consider using long double for
 *              InternalNumericFormatType_;
 *
 *      @todo   Consider adding 'precision' property to PrettyPrintInfo. Think about precision support/design of
 *              boost (maybe use  bignum or rational?). Probably no - but document clearly why or why not.
 *
 *      @todo   Add PRECISION support to PrettyPrintInfo argument to PrettyPrint () function.
 *          o   Number of seconds. Anything less than that number is truncated.
 *          o   So .001 ‘precision’ means show 3.44444 as 3.444 and 60 means show 67 seconds as “one minute”
 *          o   Maybe add option so can show > or < as in < one minute or > one minute for being passed sentainl values?
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Time {


            namespace Private_ {
                struct  Duration_ModuleData_;
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
             *      See also: http://bugzilla/show_bug.cgi?id=468
             *
             *  This class is roughly equivalent to the .Net Timespan class, and also might be called
             *  "time period", or "timespan".
             *
             *  Note that a Duration may be negative.
             *
             *  It is best to logically think of Duration as a number of seconds (at least lossly) –
             *  since for comparisons – that’s how things are normalized. #days etc are dumbed down
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
             */
            class   Duration {
            public:
                /**
                 *  The characterset of the std::string CTOR is expected to be all ascii, or the code throws FormatException
                 *
                 *   Throws (FormatException) if bad format
                 */
                Duration ();
                explicit Duration (const string& durationStr);
                explicit Duration (const Characters::String& durationStr);
                explicit Duration (int duration);
                explicit Duration (long duration);
                explicit Duration (long long duration);
                explicit Duration (float duration);
                explicit Duration (double duration);
                explicit Duration (long double duration);
                Duration (const std::chrono::milliseconds& d);
                Duration (const std::chrono::microseconds& d);
                Duration (const std::chrono::nanoseconds& d);
                Duration (const std::chrono::duration<double>& d);

            public:
                /**
                 */
                nonvirtual  void    clear ();

            public:
                /**
                 */
                nonvirtual  bool    empty () const;


            public:
                /**
                 *  Return the sum of the two durations.
                 */
                nonvirtual  Duration    operator+ (const Duration& rhs) const;

            public:
                /**
                 *  Add the given duration to this (equivalent to *this = *this + rhs;).
                 */
                nonvirtual  Duration&   operator+= (const Duration& rhs);

            public:
                /**
                 * Defined for
                 *      time_t
                 *      wstring
                 *      String
                 *      float
                 *      double
                 *      long double
                 *      std::chrono::duration<double>
                 *      std::chrono::milliseconds
                 *      std::chrono::microseconds
                 *      std::chrono::nanoseconds
                 *
                 *  Note - if 'empty' - As<> for numeric types returns 0.
                 */
                template    <typename T>
                nonvirtual  T   As () const;

            public:
                /**
                 *  Shorthand for As<String> ().AsUTF8 ()
                 */
                nonvirtual  string  AsUTF8 () const;

            public:
                struct  PrettyPrintInfo {
                    struct  Labels {
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
                    }   fLabels;
                };

            public:
                static  const   PrettyPrintInfo kDefaultPrettyPrintInfo;

            public:
                nonvirtual  Characters::String PrettyPrint (const PrettyPrintInfo& prettyPrintInfo = kDefaultPrettyPrintInfo) const;

            public:
                struct  AgePrettyPrintInfo {
                    struct  Labels  {
                        Characters::String fNow;
                        Characters::String fAgo;
                        Characters::String fFromNow;        // could use 'until' or 'from now'
                    }   fLabels;

                    double  fNowThreshold;
                };

            public:
                static  const   AgePrettyPrintInfo kDefaultAgePrettyPrintInfo;

            public:
                /**
                 *  Inspired by useful JQuery plugin http://ksylvest.github.io/jquery-age/
                 *
                 *  Technically, this isn't an 'age' but just a 'now-centric' pretty printing of durations.
                 *
                 *  Just a DRAFT impl for now ... --LGP 2014-09-01 -- @todo FIX/revise/test
                 */
                nonvirtual  Characters::String PrettyPrintAge (const AgePrettyPrintInfo& agePrettyPrintInfo = kDefaultAgePrettyPrintInfo, const PrettyPrintInfo& prettyPrintInfo = kDefaultPrettyPrintInfo) const;

            public:
                /**
                 * Duration::kMin is the least duration this Duration class supports representing.
                 */
                static  const   Duration&   kMin;

            public:
                /**
                 * DateTime::kMax is the largest duration this Duration class supports representing.
                 */
                static  const   Duration&   kMax;

            public:
                /**
                 *  Unary negation
                 */
                nonvirtual  Duration    operator- () const;

            public:
                class   FormatException;

            public:
                /**
                 *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs. Note - for the purpose of
                 * this comparison function - see the notes about 'empty' in the class description.
                 */
                nonvirtual  int     Compare (const Duration& rhs) const;

            public:
                /**
                 *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const Duration& rhs)
                 */
                nonvirtual  bool operator< (const Duration& rhs) const;
                nonvirtual  bool operator<= (const Duration& rhs) const;
                nonvirtual  bool operator> (const Duration& rhs) const;
                nonvirtual  bool operator>= (const Duration& rhs) const;
                nonvirtual  bool operator== (const Duration& rhs) const;
                nonvirtual  bool operator!= (const Duration& rhs) const;

            private:
                using   InternalNumericFormatType_  =   double;
                static  InternalNumericFormatType_  ParseTime_ (const string& s);
                static  string                      UnParseTime_ (InternalNumericFormatType_ t);

            private:
                friend  Stroika::Foundation::Time::Private_::Duration_ModuleData_;

            private:
                string  fDurationRep_;
            };
            template    <>
            time_t  Duration::As () const;
            template    <>
            wstring Duration::As () const;
            template    <>
            Characters::String Duration::As () const;
            template    <>
            float  Duration::As () const;
            template    <>
            double  Duration::As () const;
            template    <>
            long double  Duration::As () const;
            template    <>
            chrono::duration<double>  Duration::As () const;
            template    <>
            chrono::milliseconds  Duration::As () const;
            template    <>
            chrono::microseconds  Duration::As () const;
            template    <>
            chrono::nanoseconds  Duration::As () const;


            class   Duration::FormatException : public Execution::StringException {
            public:
                FormatException ();
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Duration.inl"

#endif  /*_Stroika_Foundation_Time_Duration_h_*/
