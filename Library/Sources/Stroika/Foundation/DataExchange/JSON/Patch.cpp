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
    Debug::TraceContextBumper ctx{"JSON::Patch::OperationItemType::Apply", "*this={},v={}", *this, v};
    using Context       = JSON::PointerType::Context;
    using MapElt        = Context::MapElt;
    using SeqElt        = Context::SeqElt;
    VariantValue result = v;
    // @todo - very rough draft
    switch (op) {
        case OperationType::eAdd: {
            if (optional<tuple<Context, VariantValue>> oMatch = this->path.ApplyWithContext (v)) {
                //DbgTrace ("oMa={}"_f, oMatch);
                Context c = get<Context> (*oMatch);
                if (c.fStack.empty ()) {
                    Execution::Throw (Execution::RuntimeErrorException{"? maybe sb assert"sv});
                }
                auto         stackTop = c.fStack.Pop ();
                VariantValue vv       = get<VariantValue> (*oMatch); // not sure what this means? IGNORE?
                //DbgTrace ("c={}"_f, c);
                //DbgTrace ("vv={}"_f, vv);
                //DbgTrace ("value2add={}"_f, this->value);
                if (auto mo = get_if<MapElt> (&stackTop)) {
                    mo->fOrigValue.Add (mo->fEltName, this->value);
                    result = c.ConstructNewFrom (VariantValue{mo->fOrigValue});
                }
                else if (auto so = get_if<SeqElt> (&stackTop)) {
                    so->fOrigValue.Insert (so->fIndex, this->value);
                    result = c.ConstructNewFrom (VariantValue{so->fOrigValue});
                }
                else {
                    AssertNotReached (); // cuz must be one of those two types in variant
                }
                //DbgTrace ("result={}"_f, result);
            }
            else {
                Execution::Throw (Execution::RuntimeErrorException{"operator add target not found"sv});
            }
        } break;
        case OperationType::eRemove: {
            AssertNotImplemented ();
        } break;
        default: {
            AssertNotReached ();
        } break;
    }
    //DbgTrace ("returning {}"_f, result);
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
