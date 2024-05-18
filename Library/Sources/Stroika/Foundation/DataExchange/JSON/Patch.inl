/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::DataExchange::JSON {
}

namespace Stroika::Foundation::Configuration {
    template <>
    constexpr EnumNames<Stroika::Foundation::DataExchange::JSON::Patch::OperationType> DefaultNames<Stroika::Foundation::DataExchange::JSON::Patch::OperationType>::k{
        EnumNames<Stroika::Foundation::DataExchange::JSON::Patch::OperationType>::BasicArrayInitializer{{
            {Stroika::Foundation::DataExchange::JSON::Patch::OperationType::eAdd, L"add"},
            {Stroika::Foundation::DataExchange::JSON::Patch::OperationType::eRemove, L"remove"},
        }}};
}
