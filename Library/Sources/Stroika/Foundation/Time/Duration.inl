/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Duration_inl_
#define _Stroika_Foundation_Time_Duration_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Execution/Exceptions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Time {


            /*
            ********************************************************************************
            ***************************** Implementation Details ***************************
            ********************************************************************************
            */
            template    <typename T>
            T   Duration::As () const
            {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                RequireNotReached ();
#else
                static_assert (false, "Only specifically specialized variants are supported");
#endif
            }
            inline  bool Duration::operator<= (const Duration& rhs) const
            {
                return Compare (rhs) <= 0;
            }
            inline  bool Duration::operator< (const Duration& rhs) const
            {
                return Compare (rhs) < 0;
            }
            inline  bool Duration::operator> (const Duration& rhs) const
            {
                return Compare (rhs) > 0;
            }
            inline  bool Duration::operator>= (const Duration& rhs) const
            {
                return Compare (rhs) >= 0;
            }
            inline  bool Duration::operator== (const Duration& rhs) const
            {
                return Compare (rhs) == 0;
            }
            inline  bool Duration::operator!= (const Duration& rhs) const
            {
                return Compare (rhs) != 0;
            }


            namespace Private_ {


                struct Duration_ModuleData_ {
                    Duration_ModuleData_ ();
                    Duration    fMin;
                    Duration    fMax;
                };


            }


        }

        namespace   Execution {
            template    <>
            inline  void    _NoReturn_  DoThrow (const Time::Duration::FormatException& e2Throw)
            {
                DbgTrace (L"Throwing Duration::FormatException");
                throw e2Throw;
            }
        }

    }
}
namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Time::Private_::Duration_ModuleData_>    _Stroika_Foundation_Time_Duration_ModuleData_;   // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif  /*_Stroika_Foundation_Time_Duration_inl_*/
