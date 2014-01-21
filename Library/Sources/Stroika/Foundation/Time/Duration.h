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
 *      @todo   I think we either need to use constexpr for kMin/kMax and declare stuff in headers, or
 *              use ModuleInit<> code to assure proper construction order.
 *
 *              So far this doesnt appear to have caused problems by the DurationRange<> code refrences
 *              these constants at its module init time.
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
 *          o   So .001 �precision� means show 3.44444 as 3.444 and 60 means show 67 seconds as �one minute�
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
             *  This class is roughly equivilent to the .Net Timespan class, and also might be called
             *  "time period", or "timespan".
             *
             *  Note that a Duration may be negative.
             *
             *  It is best to logically think of Duration as a number of seconds (at least lossly) �
             *  since for comparisons � that�s how things are normalized. #days etc are dumbed down
             *  to number of seconds for comparison sakes.
             */
            class   Duration {
            public:
                // Throws (FormatException) if bad format
                Duration ();
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
                nonvirtual  void    clear ();
                nonvirtual  bool    empty () const;

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
                struct  PrettyPrintInfo {
                    struct  Labels {
                        wstring fYear;
                        wstring fYears;
                        wstring fMonth;
                        wstring fMonths;
                        wstring fDay;
                        wstring fDays;
                        wstring fHour;
                        wstring fHours;
                        wstring fMinute;
                        wstring fMinutes;
                        wstring fSecond;
                        wstring fSeconds;
                        wstring fMilliSecond;
                        wstring fMilliSeconds;
                        wstring fMicroSecond;
                        wstring fMicroSeconds;
                        wstring fNanoSecond;
                        wstring fNanoSeconds;
                    }   fLabels;
                };
                static  const   PrettyPrintInfo kDefaultPrettyPrintInfo;
                nonvirtual  wstring PrettyPrint (const PrettyPrintInfo& prettyPrintInfo = kDefaultPrettyPrintInfo) const;


            public:
                /*
                 * Duration::kMin is the least duration this Duration class supports representing.
                 */
                static  const   Duration&   kMin;
                /*
                 * DateTime::kMax is the largest duration this Duration class supports representing.
                 */
                static  const   Duration&   kMax;

            public:
                // Unary negation
                nonvirtual  Duration    operator- () const;

            public:
                class   FormatException;

            public:
                /**
                 *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs. Note - for the purpose of
                 * this comparison function - see the notes about 'empty' in the class description.
                 */
                nonvirtual  int Compare (const Duration& rhs) const;

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
