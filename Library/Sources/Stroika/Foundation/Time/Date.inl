/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Date_inl_
#define _Stroika_Foundation_Time_Date_inl_  1


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
             *************************************** Date ***********************************
             ********************************************************************************
             */
            inline  constexpr Date::Date ()
                : fJulianDateRep_ (kEmptyJulianRep)
            {
            }
            inline  constexpr Date::Date (JulianRepType julianRep)
                : fJulianDateRep_ (julianRep)
            {
            }
            inline  constexpr   Date::JulianRepType Date::GetJulianRep () const
            {
                return fJulianDateRep_ == kEmptyJulianRep ? kMinJulianRep : fJulianDateRep_;
            }
            inline  constexpr   bool    Date::empty () const
            {
                return fJulianDateRep_ == kEmptyJulianRep;
            }
            inline  int Date::Compare (const Date& rhs) const
            {
                if (empty ()) {
                    return rhs.empty () ? 0 : -1;
                }
                else {
                    if (rhs.empty ()) {
                        return 1;
                    }
                    // careful of signed/unsigned converstions - esp because of kMax which is very large
                    JulianRepType   l   =   GetJulianRep ();
                    JulianRepType   r   =   rhs.GetJulianRep ();
                    if (l == r) {
                        return 0;
                    }
                    return l < r ? -1 : 1;
                }
            }
            inline  String  Date::ToString () const
            {
                return Format ();
            }
            inline  Date& Date::operator++ ()
            {
                *this = this->AddDays (1);
                return *this;
            }
#if     !qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
            constexpr   Date    Date::kMin  { Date::JulianRepType (Date::kMinJulianRep) };
            constexpr   Date    Date::kMax  { Date::JulianRepType (UINT_MAX - 1) };
#endif


            /*
             ********************************************************************************
             ************************* Time::Date operators *********************************
             ********************************************************************************
             */
            inline  bool    operator< (const Date& lhs, const Date& rhs)
            {
                return lhs.Compare (rhs) < 0;
            }
            inline  bool    operator<= (const Date& lhs, const Date& rhs)
            {
                return lhs.Compare (rhs) <= 0;
            }
            inline  bool    operator== (const Date& lhs, const Date& rhs)
            {
                return lhs.Compare (rhs) == 0;
            }
            inline  bool    operator!= (const Date& lhs, const Date& rhs)
            {
                return lhs.Compare (rhs) != 0;
            }
            inline  bool    operator>= (const Date& lhs, const Date& rhs)
            {
                return lhs.Compare (rhs) >= 0;
            }
            inline  bool    operator> (const Date& lhs, const Date& rhs)
            {
                return lhs.Compare (rhs) > 0;
            }


#if     qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
            namespace Private_ {


                struct Date_ModuleData_ {
                    Date_ModuleData_ ();
                    Date    fMin;
                    Date    fMax;
                };


            }
#endif
        }


        namespace   Execution {
            template    <>
            [[noreturn]]    inline  void    Throw (const Time::Date::FormatException& e2Throw)
            {
                DbgTrace (L"Throwing Date::FormatException");
                throw e2Throw;
            }
        }


    }
}
#if     qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Time::Private_::Date_ModuleData_>    _Stroika_Foundation_Time_Date_ModuleData_;   // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif
#endif  /*_Stroika_Foundation_Time_Date_inl_*/
