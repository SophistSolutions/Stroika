/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Date_h_
#define _Stroika_Foundation_Time_Date_h_    1

#include    "../StroikaPreComp.h"

#include    <climits>
#include    <string>

#if     qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../Characters/String.h"
#include    "../Configuration/Common.h"
#include    "../Execution/StringException.h"



/**
 * TODO:
 *
 *      @todo   LCID stuff appears to be obsolete, and perhaps not supported by MSFT any longer. Consider
 *              de-supporting.
 *
 *      @todo   Consider using strptime/strftime (or more likely the existing locale-based APIs) -
 *              and possibly use that to replace windows formatting APIs? The problem with this is
 *              that the Windows ones seem to currntly produce BETTER answers (more closely follow
 *              changes to the control panel regional settings). MAYBE its a bug with the VisStudio
 *              locale stuff or maybe I'm not using it properly. Not super high priority to
 *              unravalel - but would be nice to lose (most of) the Windoze-specific code.
 *
 *      @todo   Some places I've found on the net claim one date for start of gregorgian calendar
 *              and others claim other date(s). Really all I care about
 *              is for what range of dates is my currnet date logic valid, and thats what I'll use.
 *              But this needs some digging.
 *
 *      @todo   Should we PIN or throw OVERFLOW exception on values/requests which are out of range?
 *
 *      @todo   Consider replacing eXML with eISO8601_PF?  Not 100% sure they are the same. Maybe we
 *              should support BOTH here?
 *              Maybe where they differ doesn't matter for this class?
 *              o   Replace (research first) use of name XML here with iso8601.
 *                  + maybe not quite. See http://www.w3.org/TR/xmlschema-2/#isoformats
 *                      XML really is its own - nearly identical to iso8601, but see deviations...
 *                      Maybe have iso8601, and XML support (and for our primitive purposes maybe the
 *                      com eto the same thing?)
 *
 *      @todo   It would be highly desirable to allow this date code to represent lareger/smaller dates
 *              (without the julian calendar restriction).
 *              That maybe a longer term issue.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Time {


            using   Characters::String;


            /**
             */
            enum class  DayOfWeek : uint8_t {
                eMonday = 1,
                eTuesday = 2,
                eWednesday = 3,
                eThursday = 4,
                eFriday = 5,
                eSaturday = 6,
                eSunday = 7,
                eFirstDayOfWeek = eMonday,
                eLastDayOfWeek = eSunday,

                Define_Start_End_Count(eFirstDayOfWeek, eLastDayOfWeek)
            };


            /**
             */
            enum class  MonthOfYear : uint8_t {
                eEmptyMonthOfYear   = 0,        // only zero if date empty
                eJanuary = 1,
                eFebruary = 2,
                eMarch = 3,
                eApril = 4,
                eMay = 5,
                eJune = 6,
                eJuly = 7,
                eAugust = 8,
                eSeptember = 9,
                eOctober = 10,
                eNovember = 11,
                eDecember = 12,
                eFirstMonthOfYear = eJanuary,
                eLastMonthOfYear = eDecember,

                // Can do for (i = eSTART; i != eEND; i = Inc (i));
                Define_Start_End_Count (eFirstMonthOfYear, eLastMonthOfYear)
            };


            /**
             */
            enum  class     DayOfMonth : uint8_t {
                eEmptyDayOfMonth = 0,       // only zero if date empty
                e1 = 1, e2, e3, e4, e5, e6, e7, e8, e9, e10,
                e11 = 11, e12, e13, e14, e15, e16, e17, e18, e19, e20,
                e21 = 21, e22, e23, e24, e25, e26, e27, e28, e29, e30,
                e31,
                eFirstDayOfMonth = 1,
                eLastDayOfMonth = 31,

                // Can do for (i = eSTART; i != eEND; i = Inc (i));
                Define_Start_End_Count (eFirstDayOfMonth, eLastDayOfMonth)
            };


            /**
             */
            enum class  DayOfYear : uint16_t {
                eFirstDayOfYear = 1,
                eLastDayOfYear = 366,
                Define_Start_End_Count (eFirstDayOfYear, eLastDayOfYear)
            };


            /**
             */
            enum class Year : short {
                eEmptyYear = SHRT_MIN,
                eFirstYear = 1752,      // Gregorian calendar started on Sep. 14, 1752.
                eLastfYear = SHRT_MAX - 1,

                // Can do for (i = eSTART; i != eEND; i = Inc (i));
                Define_Start_End_Count (eFirstYear, eLastfYear)
            };


            /**
             * Description:
             * Based on Stroika code from 1992 (Date.hh/Date.cpp). From that - we have the comment:
             *
             * The Date class is based on SmallTalk-80, The Language & Its Implementation,
             * page 108 (apx) - but changed to use gregorian instead of Julian calendar - evne thogh rep date is
            <<< EXPLAIN ISSUE BETTER - WHEN IN UNDDETAND --....
            -- LGP 2011-10-05
             *
             *      o   Date represents a specific data since the start of the Gregorian (1752-09-14).
             *  Class Date knows about some obvious information:
             *      ->  there are seven days in a week, each day having a symbolic name and
             *          an index 1..7
             *      ->  there are twelve months in a year, each having a symbolic name and
             *          an index 1..12.
             *      ->  months have 28..31 days and
             *      ->  a particular year might be a leap year."
             *
             *
             *          NB: Date implies NO NOTION of timezone.
             *
             *      'empty' concept:
             *          Treat it as DISTINCT from any other Date. However, when converting it to a number of seconds
             *          or days (JulienRep), treat empty as Date::kMin. For format routine, return empty string.
             *          And for COMPARIONS (=,<,<=, etc) treat it as LESS THAN Date::kMin. This is a bit like the floating
             *          point concept of negative infinity.
             */
            class   Date {
            public:
                typedef unsigned int    JulianRepType;
                DEFINE_CONSTEXPR_CONSTANT(JulianRepType, kMinJulianRep, 2361222);  // This number corresponds to 1752-09-14
                DEFINE_CONSTEXPR_CONSTANT(JulianRepType, kEmptyJulianRep, UINT_MAX);

            public:
                class   FormatException;

            public:
                /**
                 */
                Date ();
                explicit Date (JulianRepType julianRep);
                explicit Date (Year year, MonthOfYear month, DayOfMonth day);

            public:
                /**
                 *  \brief  DisplayFormat is a representation which a date can be transformed in and out of
                 *
                 *  eCurrentLocale
                 *      Note this is the current C++ locale, which may not be the same as the platform default locale.
                 *      @see Configuration::GetPlatformDefaultLocale, Configuration::UsePlatformDefaultLocaleAsDefaultLocale ()
                 */
                enum  class DisplayFormat : uint8_t {
                    eCurrentLocale,
                    eISO8601,
                    eXML,
                    eJavascript,
                    Define_Start_End_Count (eCurrentLocale, eJavascript)
                };

            public:
                /**
                 *  Conceptually subclasses from DisplayFormat
                 *
                 *      @see DisplayFormat
                 *      @see eCurrentLocale
                 */
                enum  class ParseFormat : uint8_t {
                    eCurrentLocale      =   DisplayFormat::eCurrentLocale,
                    eISO8601            =   DisplayFormat::eISO8601,
                    eXML                =   DisplayFormat::eXML,
                    eJavascript         =   DisplayFormat::eJavascript,

                    Define_Start_End_Count (eCurrentLocale, eJavascript)
                };

            public:
                static  Date    Parse (const String& rep, ParseFormat pf);
                static  Date    Parse (const String& rep, const locale& l);
#if     qPlatform_Windows
                static  Date    Parse (const String& rep, LCID lcid);
#endif

            public:
                /*
                 * Date::kMin is the first date this Date class supports representing.
                 */
                static  const   Date    kMin;
                /*
                 * Date::kMax is the last date this Date class supports representing.
                 */
                static  const   Date    kMax;

            public:
                nonvirtual  bool    empty () const;

            public:
                nonvirtual  Year            GetYear () const;
                nonvirtual  MonthOfYear     GetMonth () const;
                nonvirtual  DayOfMonth      GetDayOfMonth () const;
            public:
                nonvirtual  void            mdy (MonthOfYear* month, DayOfMonth* day, Year* year) const;

            public:
                /**
                 *  Conceptually subclasses from DisplayFormat
                 *
                 *  eCurrentLocaleWithZerosStripped
                 *      eCurrentLocaleWithZerosStripped is eCurrentLocale, but with many cases of trailing zero's,
                 *      and sometimes leading zeros, stripped, so for example, 01:03:05 PM will become 1:03:05 PM,
                 *      and 04:06:00 PM will become 4:06 PM.
                 *
                 *      @see DisplayFormat
                 *      @see eCurrentLocale
                 */
                enum class PrintFormat : uint8_t {
                    eCurrentLocale      =   DisplayFormat::eCurrentLocale,
                    eISO8601            =   DisplayFormat::eISO8601,
                    eXML                =   DisplayFormat::eXML,
                    eJavascript         =   DisplayFormat::eJavascript,


                    //?NYI eCurrentLocaleWithZerosStripped

                    Define_Start_End_Count (eCurrentLocale, eJavascript)
                };
            public:
                nonvirtual  String Format (PrintFormat pf = PrintFormat::eCurrentLocale) const;

            public:
                nonvirtual  String Format (const locale& l) const;

#if     qPlatform_Windows
            public:
                nonvirtual  String Format (LCID lcid) const;
                nonvirtual  String Format (const String& format, LCID lcid = LOCALE_USER_DEFAULT) const;              // See GetDateFormat () format args
                nonvirtual  String LongFormat (LCID lcid = LOCALE_USER_DEFAULT) const;
#endif

            public:
                /**
                 * Returns a new Date object based on this Date, with 'dayCount' days added.
                 *
                 *  In the special case where Date is 'empty' - the starting reference (for adding dayCount)
                 *  is DateTime::GetToday ();
                 */
                nonvirtual  Date    AddDays (int dayCount) const;

            public:
                /**
                 * Note - in the specail case of 'empty' - this returns Date::kMin.GetJulianRep ()
                 */
                nonvirtual  JulianRepType   GetJulianRep () const;

            public:
                /**
                 * returns number of days since this point - relative to NOW. Never less than zero
                 */
                nonvirtual  JulianRepType   DaysSince () const;

            public:
                // Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs. Note - for the purpose of
                // this comparison function - see the notes about 'empty' in the class description.
                nonvirtual  int Compare (const Date& rhs) const;

            private:
                JulianRepType   fJulianDateRep_;
            };


            class   Date::FormatException : public Execution::StringException {
            public:
                FormatException ();
            };


            bool operator< (const Date& lhs, const Date& rhs);
            bool operator<= (const Date& lhs, const Date& rhs);
            bool operator> (const Date& lhs, const Date& rhs);
            bool operator== (const Date& lhs, const Date& rhs);
            bool operator!= (const Date& lhs, const Date& rhs);

            int DayDifference (const Date& lhs, const Date& rhs);
            int YearDifference (const Date& lhs, const Date& rhs);
            float   YearDifferenceF (const Date& lhs, const Date& rhs);


            String  GetFormattedAge (const Date& birthDate, const Date& deathDate = Date ());   // returns ? if not a good src date
            String  GetFormattedAgeWithUnit (const Date& birthDate, const Date& deathDate = Date (), bool allowMonths = true, bool abbrevUnit = true);  // returns ? if not a good src date

        }
    }
}
#endif  /*_Stroika_Foundation_Time_Date_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Date.inl"
