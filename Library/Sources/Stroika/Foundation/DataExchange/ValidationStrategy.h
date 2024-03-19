/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_ValidationStrategy_h_
#define _Stroika_Foundation_DataExchange_ValidationStrategy_h_ 1

#include "../StroikaPreComp.h"

/*
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 *
 */
namespace Stroika::Foundation::DataExchange {

    /**
     *  Many times, in code, the caller knows the arguments are in some valid type restricted range
     *  (like if the data comes from an algorithm).
     *  Many times, the code doesn't know (like if the data comes from a file).
     * 
     *  This enumeration/specification, allows SOME APIs to (usually by default) treat validation/constraint failures
     *  as compile time or runtime assertion failures (so not checked in release code).
     * 
     *  And sometimes, to indicate the source of the data is untrusted, and the data must be validated (typically resulting
     *  in a DataExchange::BadFormatException being thrown).
     * 
     *  Stroika has long had this sort of dichotomy/choice (with the aforementioned bias towards assertion checking). This just regularizes
     *  the names/types used to indicate that choice.
     * 
     *  Often times, and API will be a const, inline, or constexpr version that only does assertions, and will have an OVERLOAD
     *  taking this parameter, which does the slower checking (one way or the other).
    */
    enum class ValidationStrategy {
        /**
         *  For most APIs using ValidationStrategy, this will be the default.
         */
        eAssertion,

        /**
         *  Indicates the data comes from an untrusted source, should be validated, and an exception raised if its invalid.
         */
        eThrow
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ValidationStrategy.inl"

#endif /*_Stroika_Foundation_DataExchange_ValidationStrategy_h_*/
