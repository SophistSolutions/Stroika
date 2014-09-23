/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
#if     !qCompilerAndStdLib_constexpr_Buggy
            inline  constexpr Date::Date ()
                : fJulianDateRep_ (kEmptyJulianRep)
            {
            }
#else
            inline  Date::Date ()
                : fJulianDateRep_ (kEmptyJulianRep)
            {
            }
#endif
#if     !qCompilerAndStdLib_constexpr_Buggy
            inline  constexpr Date::Date (JulianRepType julianRep)
                : fJulianDateRep_ (julianRep)
            {
            }
#else
            inline  Date::Date (JulianRepType julianRep)
                : fJulianDateRep_ (julianRep)
            {
            }
#endif
            inline  Date::JulianRepType Date::GetJulianRep () const
            {
                return (fJulianDateRep_ == kEmptyJulianRep ? kMinJulianRep : fJulianDateRep_);
            }
            inline  bool    Date::empty () const
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
            inline  bool    Date::operator< (const Date& rhs) const
            {
                return Compare (rhs) < 0;
            }
            inline  bool    Date::operator<= (const Date& rhs) const
            {
                return Compare (rhs) <= 0;
            }
            inline  bool    Date::operator> (const Date& rhs) const
            {
                return Compare (rhs) > 0;
            }
            inline  bool    Date::operator>= (const Date& rhs) const
            {
                return Compare (rhs) >= 0;
            }
            inline  bool    Date::operator== (const Date& rhs) const
            {
                return Compare (rhs) == 0;
            }
            inline  bool    Date::operator!= (const Date& rhs) const
            {
                return Compare (rhs) != 0;
            }
#if     !qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy
            constexpr   Date    Date::kMin      { Date::JulianRepType (kMinJulianRep) };
            constexpr   Date    Date::kMax      { Date::JulianRepType (UINT_MAX - 1) };
#endif


#if     qCompilerAndStdLib_constexpr_Buggy || qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy
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
            inline  void    _NoReturn_  DoThrow (const Time::Date::FormatException& e2Throw)
            {
                DbgTrace (L"Throwing Date::FormatException");
                throw e2Throw;
            }
        }


    }
}
#if     qCompilerAndStdLib_constexpr_Buggy || qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy
namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Time::Private_::Date_ModuleData_>    _Stroika_Foundation_Time_Date_ModuleData_;   // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif
#endif  /*_Stroika_Foundation_Time_Date_inl_*/
