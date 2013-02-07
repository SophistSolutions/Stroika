/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_CopyableCriticalSection_inl_
#define _Stroika_Foundation_Execution_CopyableCriticalSection_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../Memory/Common.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {

            //  class   CopyableCriticalSection
            inline  CopyableCriticalSection::CopyableCriticalSection ()
                : fCritSec (DEBUG_NEW recursive_mutex ())
            {
            }
            inline  void    CopyableCriticalSection::Lock ()
            {
                fCritSec->lock ();
            }
            inline  void CopyableCriticalSection::Unlock ()
            {
                fCritSec->unlock ();
            }

        }
    }
}
#endif  /*_Stroika_Foundation_Execution_CopyableCriticalSection_inl_*/
