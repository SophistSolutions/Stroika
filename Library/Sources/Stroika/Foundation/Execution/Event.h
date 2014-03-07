/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Event_h_
#define _Stroika_Foundation_Execution_Event_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"

#include    "WaitableEvent.h"



/**
 * TODO:
 *      DEPRECATED
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            typedef WaitableEvent   _DeprecatedFunction_ (Event, "Deprecated in v2.0a19 - use WaitableEvent");

            namespace {
                Event   file_IsDeprecated_Event;
            }


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_Execution_Event_h_*/
