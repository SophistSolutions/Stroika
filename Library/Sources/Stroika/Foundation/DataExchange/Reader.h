/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Reader_h_
#define _Stroika_Foundation_DataExchange_Reader_h_    1

#include    "../StroikaPreComp.h"

#include    "VariantReader.h"


#error ("DEPRECATED in v2.0a106 - use Reader instead of VariantReader");

namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            using Reader = VariantReader;
        }
    }
}

#endif  /*_Stroika_Foundation_DataExchange_Reader_h_*/
