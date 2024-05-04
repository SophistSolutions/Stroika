/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Frameworks::WebService::JSONRPC {

    /*
     ********************************************************************************
     ****************************** JSONRPC::Request ********************************
     ********************************************************************************
     */
    inline const Foundation::DataExchange::ObjectVariantMapper Request::kMapper = [] () {
        using namespace Foundation::DataExchange;
        ObjectVariantMapper m;
        using SEQ_OR_MAP_ = variant<Mapping<String, VariantValue>, Sequence<VariantValue>>;
        m.Add<SEQ_OR_MAP_> (
            [] (const ObjectVariantMapper& mapper, const SEQ_OR_MAP_* obj) -> VariantValue {
                if (auto om = get_if<Mapping<String, VariantValue>> (obj)) {
                    return VariantValue{*om};
                }
                else if (auto os = get_if < Sequence < VariantValue >> (obj)) {
                    return VariantValue{*os};
                }
                else {
                    RequireNotReached ();
                    return VariantValue{};
                }
            },
            [] (const ObjectVariantMapper& mapper, const VariantValue& d, SEQ_OR_MAP_* intoObj) -> void {
                if (d.GetType () == VariantValue::eMap) {
                    *intoObj = d.As<Mapping<String, VariantValue>> ();
                }
                else if (d.GetType () == VariantValue::eArray) {
                    *intoObj = d.As<Sequence<VariantValue>> ();
                }
                else {
                    // not sure here - maybe throw - or just let this do the throwing
                    *intoObj = d.As<Sequence<VariantValue>> ();
                }
            });
        m.AddCommonType<optional<SEQ_OR_MAP_>> ();
        m.AddClass<Request> ({
            {"jsonrpc"sv, StructFieldMetaInfo{&Request::jsonrpc}},
            {"params"sv, StructFieldMetaInfo{&Request::params}, ObjectVariantMapper::StructFieldInfo::eOmitNullFields},
            {"id"sv, StructFieldMetaInfo{&Request::id}, ObjectVariantMapper::StructFieldInfo::eOmitNullFields},
        });
        return m;
    }();

    /*
     ********************************************************************************
     ****************************** JSONRPC::Error **********************************
     ********************************************************************************
     */
    inline const Foundation::DataExchange::ObjectVariantMapper Error::kMapper = [] () {
        using namespace Foundation::DataExchange;
        ObjectVariantMapper m;
        m.AddClass<Request> ({
            {"code"sv, StructFieldMetaInfo{&Error::code}},
            {"message"sv, StructFieldMetaInfo{&Error::message}},
            {"data"sv, StructFieldMetaInfo{&Error::data}, ObjectVariantMapper::StructFieldInfo::eOmitNullFields},
        });
        return m;
    }();

    /*
     ********************************************************************************
     ***************************** JSONRPC::Response ********************************
     ********************************************************************************
     */
    inline const Foundation::DataExchange::ObjectVariantMapper Response::kMapper = [] () {
        using namespace Foundation::DataExchange;
        ObjectVariantMapper m;
        // todo - could 'subclass' this to assure exactly one of result or error but not both, or maybe redo as
        // variant object so can be one thing or other?
        m.AddClass<Response> ({
            {"jsonrpc"sv, StructFieldMetaInfo{&Response::jsonrpc}},
            {"result"sv, StructFieldMetaInfo{&Response::result}, ObjectVariantMapper::StructFieldInfo::eOmitNullFields},
            {"error"sv, StructFieldMetaInfo{&Response::error}, ObjectVariantMapper::StructFieldInfo::eOmitNullFields},
            {"id"sv, StructFieldMetaInfo{&Response::id}},
        });
        return m;
    }();

}
