/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_JSON_Reader_h_
#define _Stroika_Foundation_DataExchangeFormat_JSON_Reader_h_   1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Memory/VariantValue.h"
#include    "../../Streams/BinaryInputStream.h"
#include    "../../Streams/TextInputStream.h"



/**
 *  \file
 *
 * TODO:
 *      @todo   NotYetImplemented
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {
            namespace   XML {


                Memory::VariantValue    Reader (const Streams::BinaryInputStream& in);
                Memory::VariantValue    Reader (const Streams::TextInputStream& in);


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_DataExchangeFormat_JSON_Reader_h_*/
