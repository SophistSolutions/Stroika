/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::DataExchange::JSON {
}

namespace Stroika::Foundation::DataExchange::JSON::Patch {
    constexpr Common::EnumNameText EnumeratorName (OperationType e, Common::EnumNameText byRule)
    {
        switch (e) {
            case OperationType::eAdd:
                return "add";
            case OperationType::eRemove:
                return "remove";
            default:
                return byRule;
        }
    }
}
