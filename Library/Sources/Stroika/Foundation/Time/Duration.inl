/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Duration_inl_
#define _Stroika_Foundation_Time_Duration_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Execution/Exceptions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Time {

            template    <typename T>
            T   Duration::As () const
            {
                static_assert (false, "Only specifically specialized variants are supported");
            }

            inline  bool operator<= (const Duration& lhs, const Duration& rhs)
            {
                return lhs.Compare (rhs) <= 0;
            }
            inline  bool operator< (const Duration& lhs, const Duration& rhs)
            {
                return lhs.Compare (rhs) < 0;
            }
            inline  bool operator> (const Duration& lhs, const Duration& rhs)
            {
                return lhs.Compare (rhs) > 0;
            }
            inline  bool operator== (const Duration& lhs, const Duration& rhs)
            {
                return lhs.Compare (rhs) == 0;
            }
            inline  bool operator!= (const Duration& lhs, const Duration& rhs)
            {
                return lhs.Compare (rhs) != 0;
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
#endif  /*_Stroika_Foundation_Time_Duration_inl_*/
