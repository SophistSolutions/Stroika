/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_JSONRPC_h_
#define _Stroika_Framework_WebService_JSONRPC_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"

/*
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Frameworks::WebService::JSONRPC {

    using namespace Stroika::Foundation;

    using Characters::String;
    using Containers::Mapping;
    using Containers::Sequence;
    using DataExchange::VariantValue;

    /**
     *      See https://www.jsonrpc.org/specification
     */

    /**
     */
    struct Request {
        /**
         */
        String jsonrpc{"2.0"sv};

        /**
         */
        String method;

        /**
         *  This can be missing, or a Mapping (named params) or an array of params (Sequence)
         */
        optional<variant<Mapping<String, VariantValue>, Sequence<VariantValue>>> params;

        /**
        // Note if provided, this must be an integer, or string (can be float but discouraged)
         */
        optional<VariantValue> id;

        /**
         */
        static const Foundation::DataExchange::ObjectVariantMapper kMapper;
    };

    namespace ErrorCodes {
        constexpr int kParseError           = -32700;
        constexpr int kInvalidRequest       = -32600;
        constexpr int kMethodNotFound       = -32601;
        constexpr int kInvalidParameters    = -32602;
        constexpr int kInternalJSONRPCError = -32603;
    }

    /**
     */
    struct Error {
        /**
         * can be ErrorCodes, or other
         */
        int code{};

        /**
         */
        String message;

        /**
         */
        optional<VariantValue> data;

        /**
         */
        static const Foundation::DataExchange::ObjectVariantMapper kMapper;
    };

    /**
     * MAYBE redo result/error so variant object so can be one thing or other?
     */
    struct Response {
        String jsonrpc{"2.0"sv};

        /**
         *  If provided, can be any json value. result or error required, but not both
         */
        optional<VariantValue> result;

        /**
         *  If provided, can be any json value. result or error required, but not both
         */
        optional<Error> error;

        /*
         *  Must be same as value from request
         */
        VariantValue id;

        /**
         */
        static const Foundation::DataExchange::ObjectVariantMapper kMapper;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "JSONRPC.inl"

#endif /*_Stroika_Framework_WebService_JSONRPC_h_*/
