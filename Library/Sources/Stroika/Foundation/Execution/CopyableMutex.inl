/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_CopyableMutex_inl_
#define _Stroika_Foundation_Execution_CopyableMutex_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../Memory/Common.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
             ********************************************************************************
             ******************** CopyableMutex<REAL_MUTUX> *********************************
             ********************************************************************************
             */
            template    <typename REAL_MUTUX>
            inline  CopyableMutex<REAL_MUTUX>::CopyableMutex ()
                : fCritSec_ (DEBUG_NEW REAL_MUTUX ())
            {
            }
            template    <typename REAL_MUTUX>
            inline  void    CopyableMutex<REAL_MUTUX>::lock ()
            {
                fCritSec_->lock ();
            }
            template    <typename REAL_MUTUX>
            inline  void    CopyableMutex<REAL_MUTUX>::unlock ()
            {
                fCritSec_->unlock ();
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_CopyableMutex_inl_*/
