/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Finally_inl_
#define _Stroika_Foundation_Execution_Finally_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "Exceptions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
             ********************************************************************************
             ********************************* Execution::Finally ***************************
             ********************************************************************************
             */
            inline  Finally::Finally (const function<void()>& cleanupCodeBlock)
                : fCleanupCodeBlock_(cleanupCodeBlock)
            {
            }
            template <typename FUNCTION>
            inline  Finally::Finally (FUNCTION f, typename is_function<FUNCTION>::type*) :
                Finally (function<void()>(f))
            {
            }
            inline  Finally::~Finally()
            {
                IgnoreExceptionsForCall (fCleanupCodeBlock_());
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_Finally_inl_*/
