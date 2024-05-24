/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_JSON_PATCH_h_
#define _Stroika_Foundation_DataExchange_JSON_PATCH_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/JSON/Pointer.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"

/**
 */
namespace Stroika::Foundation::DataExchange::JSON::Patch {

    using namespace Stroika::Foundation;

    using Characters::String;
    using DataExchange::VariantValue;

    // SEE https://jsonpatch.com/
    // SEE https://www.rfc-editor.org/rfc/rfc6902

    /**
     *  @todo more OperationType values  
     */
    enum class OperationType {
        eAdd,
        eRemove,

        Stroika_Define_Enum_Bounds (eAdd, eRemove)
    };

    /**
     */
    struct OperationItemType {
        OperationType          op;
        PointerType            path;
        optional<VariantValue> value;

        /**
         *  Apply the operation to the argument VariantValue object, and return the mutated result.
         * 
         *  \note - @todo - as of Stroika v3.0d6 - QUITE INCOMPLETE IMPLEMENTATION - but alot of the infrastucture there to complete it (jsonpointer search/update - but thats not 100%).
         */
        nonvirtual VariantValue Apply (const VariantValue& v) const;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

        static const DataExchange::ObjectVariantMapper kMapper;
    };

    /**
     */
    struct OperationItemsType : Containers::Sequence<OperationItemType> {

        /**
         *  Apply each operation in sequence to the argument VariantValue object, and return the mutated result.
         */
        nonvirtual VariantValue Apply (const VariantValue& v) const;

        static const DataExchange::ObjectVariantMapper kMapper;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Patch.inl"

#endif /*_Stroika_Foundation_DataExchange_JSON_PATCH_h_*/
