/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DateTime_inl_
#define _Stroika_Foundation_Time_DateTime_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Execution/Exceptions.h"
#include    "../Execution/ModuleInit.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Time {


            /*
             ********************************************************************************
             ********************************** DateTime ************************************
             ********************************************************************************
             */
            inline  constexpr   DateTime::DateTime () noexcept
                : fTimezone_ (Timezone::eUnknown)
                , fDate_ ()
                , fTimeOfDay_ ()
            {
            }
            inline  constexpr   DateTime::DateTime (const Date& d) noexcept
                : fTimezone_ (Timezone::eUnknown)
                , fDate_ (d)
                , fTimeOfDay_ ()
            {
            }
            inline  DateTime::DateTime (const DateTime& dt, const Date& updateDate) noexcept
                : fTimezone_ (dt.GetTimezone ())
                , fDate_ (updateDate)
                , fTimeOfDay_ (dt.GetTimeOfDay ())
            {
            }
            inline  DateTime::DateTime (const DateTime& dt, const TimeOfDay& updateTOD) noexcept
                : fTimezone_ (dt.GetTimezone ())
                , fDate_ (dt.GetDate ())
                , fTimeOfDay_ (updateTOD)
            {
            }
            inline  constexpr   DateTime::DateTime (const Date& date, const TimeOfDay& timeOfDay, Timezone tz) noexcept
                : fTimezone_ (tz)
                , fDate_ (date)
                , fTimeOfDay_ (timeOfDay)
            {
            }
            inline  constexpr   bool    DateTime::empty () const noexcept
            {
                // Risky change so late in the game - but this logic seems wrong (and causes some trouble).
                // DateTime is NOT empty just because date part is empty. We CAN use a DateTime record to store JUST a time!
                //      -- LGP 2006-04-26
                //          return fDate_.empty () and fTimeOfDay_.empty ();
                //
                // Maybe also risky - but see my comments in the header for this funciton. I Think it DOES make sense to treat the DateTime as empty
                // if the Date part is empty...
                return fDate_.empty ();
            }
            inline  constexpr   Date    DateTime::GetDate () const noexcept
            {
                return fDate_;
            }
            inline  constexpr   TimeOfDay   DateTime::GetTimeOfDay () const noexcept
            {
                return fTimeOfDay_;
            }
            template    <>
            inline  Date    DateTime::As () const
            {
                return fDate_;
            }
            inline  Date    DateTime::GetToday () noexcept
            {
                return Now ().GetDate ();
            }
            inline  DateTime::Timezone  DateTime::GetTimezone () const noexcept
            {
                return fTimezone_;
            }
            inline  String  DateTime::ToString () const
            {
                return Format ();
            }
#if     !qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
            constexpr   DateTime    DateTime::kMin  { Date::kMin, TimeOfDay::kMin };
            constexpr   DateTime    DateTime::kMax  { Date::kMax, TimeOfDay::kMax };
#endif

            /*
             ********************************************************************************
             ************************** DateTime operators **********************************
             ********************************************************************************
             */
            inline  bool    operator< (DateTime lhs, DateTime rhs)
            {
                return lhs.Compare (rhs) < 0;
            }
            inline  bool    operator<= (DateTime lhs, DateTime rhs)
            {
                return lhs.Compare (rhs) <= 0;
            }
            inline  bool    operator== (DateTime lhs, DateTime rhs)
            {
                return lhs.Compare (rhs) == 0;
            }
            inline  bool    operator!= (DateTime lhs, DateTime rhs)
            {
                return lhs.Compare (rhs) != 0;
            }
            inline  bool    operator>= (DateTime lhs, DateTime rhs)
            {
                return lhs.Compare (rhs) >= 0;
            }
            inline  bool    operator> (DateTime lhs, DateTime rhs)
            {
                return lhs.Compare (rhs) > 0;
            }


#if     qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
            namespace Private_ {


                struct DateTime_ModuleData_ {
                    DateTime_ModuleData_ ();
                    DateTime    fMin;
                    DateTime    fMax;
                };


            }
#endif


        }
    }
}
#if     qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Time::Private_::DateTime_ModuleData_>    _Stroika_Foundation_Time_DateTime_ModuleData_;   // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif
#endif  /*_Stroika_Foundation_Time_DateTime_inl_*/
