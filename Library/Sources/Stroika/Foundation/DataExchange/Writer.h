/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Writer_h_
#define _Stroika_Foundation_DataExchange_Writer_h_    1

#include    "../StroikaPreComp.h"

#include    "VariantWriter.h"


#error ("DEPRECATED in v2.0a106 - use Writer instead of VariantWriter");

namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            using Writer = VariantWriter;
        }
    }
}

#endif  /*_Stroika_Foundation_DataExchange_Writer_h_*/
