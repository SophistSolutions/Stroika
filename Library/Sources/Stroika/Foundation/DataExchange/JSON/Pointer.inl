/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#include "Stroika/Foundation/Characters/StringBuilder.h"

namespace Stroika::Foundation::DataExchange::JSON {

    /*
     ********************************************************************************
     ******************************** JSON::PointerType *****************************
     ********************************************************************************
     */
    template <Configuration::IAnyOf<String> T>
    inline T PointerType::As () const
    {
        if constexpr (same_as<T, String>) {
            Characters::StringBuilder sb;
            fComponents_.Apply ([&] (const String& s) { sb << "/"sv << s.ReplaceAll ("~"sv, "~0"sv).ReplaceAll ("/"sv, "~1"); });
            return sb;
        }
    }
    inline const DataExchange::ObjectVariantMapper PointerType::kMapper = [] () {
        DataExchange::ObjectVariantMapper mapper;
        mapper.Add<PointerType> ([] (const ObjectVariantMapper&, const PointerType* obj) -> VariantValue { return obj->As<String> (); },
                                 [] (const ObjectVariantMapper&, const VariantValue& d, PointerType* intoObj) -> void {
                                     *intoObj = PointerType{d.As<String> ()};
                                 });
        return mapper;
    }();
}
