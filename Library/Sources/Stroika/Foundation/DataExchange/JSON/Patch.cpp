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
    sb << "op: "sv << op << ", "sv;
    sb << "path: "sv << path << ", "sv;
    if (value) {
        sb << "value: "sv << value;
    }
    sb << "}"sv;
    return sb;
}

VariantValue JSON::Patch::OperationItemType::Apply (const VariantValue& v) const
{
    VariantValue result = v;
    // @todo - very rough draft
    switch (op) {
        case OperationType::eAdd: {
            if (auto oMatch = this->path.ApplyWithContext (v)) {
                //roughly - take the current value (or stack top) - and apply add result to it and pop stack back to new value
            }
            else {
                Execution::Throw (Execution::RuntimeErrorException{"operator add target not found"});
            }
            AssertNotImplemented ();
        } break;
        case OperationType::eRemove: {
            AssertNotImplemented ();
        } break;
        default: {
            AssertNotImplemented ();
        } break;
    }
    return result;
}

const DataExchange::ObjectVariantMapper JSON::Patch::OperationItemType::kMapper = [] () {
    ObjectVariantMapper mapper;
    mapper += JSON::PointerType::kMapper;
    mapper.AddCommonType<OperationType> ();
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

VariantValue JSON::Patch::OperationItemsType::Apply (const VariantValue& v) const
{
    VariantValue result = v;
    this->Sequence<OperationItemType>::Apply ([&] (const OperationItemType& op) { result = op.Apply (result); });
    return result;
}
