/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_JSON_Reader_h_
#define _Stroika_Foundation_DataExchangeFormat_JSON_Reader_h_   1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Memory/VariantValue.h"
#include    "../../Streams/BinaryInputStream.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {
            namespace   JSON {

                Memory::VariantValue    Reader (const Streams::BinaryInputStream& in);

            }
        }
    }
}
#endif  /*_Stroika_Foundation_DataExchangeFormat_JSON_Reader_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
