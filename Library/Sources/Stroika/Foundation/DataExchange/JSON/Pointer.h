/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_JSON_Pointer_h_
#define _Stroika_Foundation_DataExchange_JSON_Pointer_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Concepts.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Stack.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"

/**
 */
namespace Stroika::Foundation::DataExchange::JSON {

    using namespace Stroika::Foundation;

    using Characters::String;
    using Containers::Sequence;
    using Containers::Stack;

    /**
     *  @see https://datatracker.ietf.org/doc/html/rfc6901/
     * 
     *  Note this really only supports fully looking up data in an VariantValue, not (yet) providing the ability
     *  to index to update a VariantValue (like add to an array elt - need fancier Apply API support for that).
     */
    class PointerType {
    public:
        /**
         */
        template <Characters::IConvertibleToString STRINGISH_TYPE = String>
        PointerType (STRINGISH_TYPE&& s = STRINGISH_TYPE{});

    public:
        /**
         *  When applying a JSONPointer to a VariantValue, sometimes you just want the result. Sometimes
         *  you want info about the surrounding object (to modify).
         */
        struct Context {
            struct MapElt {
                Mapping<String, VariantValue> fOrigValue;
                String                        fEltName;
                bool operator== (const MapElt&) const = default; // @todo understand why this declaration needed (Iterable<>find...)

                /**
                 */
                String ToString () const;
            };
            struct SeqElt {
                Sequence<VariantValue> fOrigValue;
                size_t                 fIndex;

                bool operator== (const SeqElt&) const = default; // @todo understand why this declaration needed (Iterable<>find...)

                /**
                 */
                String ToString () const;
            };
            Stack<variant<MapElt, SeqElt>> fStack;

            /**
             *  Take the context in 'this' - and treat all offsets as Set (not remove or add); leafToUse only applied
             *  to replace the 'value' that would have been returned by Apply (or ApplyWithContext).
             */
            optional<VariantValue> ConstructNewFrom (const optional<VariantValue>& leafToUse) const;

            /**
             */
            String ToString () const;
        };

    public:
        /**
         *  Some references might be to non-existent objects, so return nullopt in that case - like bad array reference, or missing object member.
         */
        nonvirtual optional<VariantValue> Apply (const VariantValue& v) const;

    public:
        /**
         *  Some references might be to non-existent objects, so return nullopt in that case - like bad array reference, or missing object member.
         */
        nonvirtual optional<tuple<Context, VariantValue>> ApplyWithContext (const VariantValue& v) const;
        nonvirtual optional<VariantValue> ApplyWithContext (const VariantValue& v, Context* contextOut) const;

    public:
        /**
         */
        template <Configuration::IAnyOf<String> T>
        nonvirtual T As () const;

    public:
        /**
         */
        nonvirtual bool operator== (const PointerType&) const = default;

    public:
        /**
         */
        nonvirtual auto operator<=> (const PointerType&) const = default;

    public:
        /**
         */
        nonvirtual String ToString () const;

    public:
        static const DataExchange::ObjectVariantMapper kMapper;

    private:
        // first component refers to top of tree;
        // components are raw strings and must match nodes exactly (unescaped already)
        Sequence<String> fComponents_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Pointer.inl"

#endif /*_Stroika_Foundation_DataExchange_JSON_Pointer_h_*/
