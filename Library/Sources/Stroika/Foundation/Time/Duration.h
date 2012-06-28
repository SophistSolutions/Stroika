/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Duration_h_
#define _Stroika_Foundation_Time_Duration_h_    1

#include    "../StroikaPreComp.h"

#include    <climits>
#include    <string>

#if     qCompilerAndStdLib_Supports_stdchrono
#include    <chrono>
#endif

#if     qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../Configuration/Common.h"
#include    "../Execution/StringException.h"



/*
 * TODO:
 *
 *      o   Do better job rounding. Right now we round (?)properly for seconds, but nothing else.
 *
 *      o   Add support for long double (and perhaps others?). And consider using long double for InternalNumericFormatType_;
 *
 *      o   Consider adding 'precision' property to PrettyPrintInfo. Think about precision support/design of
 *          boost (maybe use  bignum or rational?). Probably no - but document clearly why or why not.
 *
 *      o   Add PRECISION support to PrettyPrintInfo argument to PrettyPrint () function.
 *          o   Number of seconds. Anything less than that number is truncated.
 *          o   So .001 ‘precision’ means show 3.44444 as 3.444 and 60 means show 67 seconds as “one minute”
 *          o   Maybe add option so can show > or < as in < one minute or > one minute for being passed sentainl values?
 *
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Time {


            /*
             * (basic) support for ISO 8601 Durations
             *      http://en.wikipedia.org/wiki/ISO_8601#Durations
             *
             *  Note: according to glibc docs - year is always 365 days, month always 30 days, etc, as far as these duration objects
             *  go - at least for conversion to/from time_t. Seek a better reference for this claim!
             *
             * Note also - there are two iso 8601 duration formats - one date like (YYYY-MM....) and this one P...T...3S).
             * As far as I know - XML always uses the later. For now - this implementation only supports the later.
             *
             *      See also: http://bugzilla/show_bug.cgi?id=468
             *
             *  This class is roughly equivilent to the .Net Timespan class, and also might be called "time period".
             *
             *  Note that a Duration maybe negative.
             *
             *  It is best to logically think of Duration as a number of seconds (at least lossly) – since for comparisons –
             *  that’s how things are normalized. #days etc are dumbed down to number of seconds for comparison sakes.
             */
            class   Duration {
            public:
                // Throws (FormatException) if bad format
                Duration ();
                explicit Duration (const wstring& durationStr);
                explicit Duration (int32_t duration);
                explicit Duration (int64_t duration);
                explicit Duration (double duration);
#if     qCompilerAndStdLib_Supports_stdchrono
                Duration (const std::chrono::duration<double>& d);
#endif

            public:
                nonvirtual  void    clear ();
                nonvirtual  bool    empty () const;

            public:
                /*
                 * Defined for
                 *      time_t
                 *      wstring
                 *      double
                 *      std::chrono::duration<double>           (if qCompilerAndStdLib_Supports_stdchrono)
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
                // Unary negation
                nonvirtual  Duration    operator- () const;

            public:
                class   FormatException;

            public:
                // Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs. Note - for the purpose of
                // this comparison function - see the notes about 'empty' in the class description.
                nonvirtual  int Compare (const Duration& rhs) const;

            private:
                typedef double  InternalNumericFormatType_;
                static  InternalNumericFormatType_  ParseTime_ (const string& s);
                static  string                      UnParseTime_ (InternalNumericFormatType_ t);

            private:
                string  fDurationRep;
            };
            template    <>
            time_t  Duration::As () const;
            template    <>
            wstring Duration::As () const;
            template    <>
            double  Duration::As () const;

            bool operator< (const Duration& lhs, const Duration& rhs);
            bool operator<= (const Duration& lhs, const Duration& rhs);
            bool operator> (const Duration& lhs, const Duration& rhs);
            bool operator== (const Duration& lhs, const Duration& rhs);
            bool operator!= (const Duration& lhs, const Duration& rhs);


            class   Duration::FormatException : public Execution::StringException {
            public:
                FormatException ();
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Time_Duration_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Duration.inl"
