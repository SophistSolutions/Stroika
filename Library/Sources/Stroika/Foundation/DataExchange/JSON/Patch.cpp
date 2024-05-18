/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Patch.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Foundation::DataExchange::JSON;
using namespace Stroika::Foundation::DataExchange::JSON::Patch;

/*
 ********************************************************************************
 ********************** JSON::Patch::OperationItemType **************************
 ********************************************************************************
 */
String JSON::Patch::OperationItemType::ToString () const
{
    Characters::StringBuilder sb;
    sb << "{"sv;
    sb << "op: "sv << Characters::ToString (op) << ", "sv;
    sb << "path: "sv << Characters::ToString (path) << ", "sv;
    if (value) {
        sb << "value: "sv << value;
    }
    sb << "}"sv;
    return sb;
}

const DataExchange::ObjectVariantMapper JSON::Patch::OperationItemType::kMapper = [] () {
    ObjectVariantMapper mapper;
    mapper.AddCommonType<OperationType> ();
    mapper.AddCommonType<String> ();
    mapper.AddCommonType<optional<String>> ();
    mapper.AddCommonType<optional<VariantValue>> ();
    mapper.AddClass<OperationItemType> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
        {"op"sv, StructFieldMetaInfo{&OperationItemType::op}},
        {"path"sv, StructFieldMetaInfo{&OperationItemType::path}},
        {"value"sv, StructFieldMetaInfo{&OperationItemType::value}, ObjectVariantMapper::StructFieldInfo::eOmitNullFields},
    });
    return mapper;
}();

/*
 ********************************************************************************
 ********************** JSON::Patch::OperationItemsType *************************
 ********************************************************************************
 */
const DataExchange::ObjectVariantMapper JSON::Patch::OperationItemsType::kMapper = [] () {
    ObjectVariantMapper mapper;
    mapper += OperationItemType::kMapper;
    mapper.AddCommonType<OperationItemsType> ();
    return mapper;
}();
