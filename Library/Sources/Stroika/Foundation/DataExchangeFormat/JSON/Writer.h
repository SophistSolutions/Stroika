/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_JSON_Writer_h_
#define _Stroika_Foundation_DataExchangeFormat_JSON_Writer_h_   1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Memory/VariantValue.h"
#include    "../../Streams/BinaryOutputStream.h"


/*
 * TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {
            namespace   JSON {


                /*
                 *
                 * The arguemnt VariantValue must be composed of any combination of these types:
                 *          o   Memory::VariantValue::eBoolean
                 *          o   Memory::VariantValue::eInteger
                 *          o   Memory::VariantValue::eFloat
                 *          o   Memory::VariantValue::eString
                 *          o   Memory::VariantValue::eMap
                 *          o   Memory::VariantValue::eArray
                 *  or it can be the type:
                 *          o   Memory::VariantValue::eNull
                 *
                 *  Other types are illegal an JSON and will trigger a 'Require' failure.
                 *
                 * Note that PrettyPrint () writes in UTF-8 format to the output stream.
                 */
                void    PrettyPrint (const Memory::VariantValue& v, const Streams::BinaryOutputStream& out);

            }
        }
    }
}
#endif  /*_Stroika_Foundation_DataExchangeFormat_JSON_Writer_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
