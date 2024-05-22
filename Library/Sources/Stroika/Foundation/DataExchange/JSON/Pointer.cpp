/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"

#include "Pointer.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Foundation::DataExchange::JSON;

/*
 ********************************************************************************
 ******************************** JSON::PointerType *****************************
 ********************************************************************************
 */
JSON::PointerType::PointerType (const String& s)
{
    optional<Character> prevChar;
    bool                startedSection = false;
    StringBuilder       curSectionSB;
    // return true if handled, and throw if bad character following
    auto maybeHandleCharAfterTwiddle = [&] (Character c) -> bool {
        if (prevChar == '~') {
            switch (c.GetCharacterCode ()) {
                case '0':
                    curSectionSB << '~';
                    break;
                case '1':
                    curSectionSB << '/';
                    break;
                default:
                    static const auto kExcept_ = DataExchange::BadFormatException{"Expected 0 or 1 after ~ in JSON Pointer"sv};
                    Execution::Throw (kExcept_);
                    break;
            }
            prevChar = c;
            return true;
        }
        return false;
    };
    s.Apply ([&] (Character c) {
        if (maybeHandleCharAfterTwiddle (c)) {
            return;
        }
        if (c == '/') {
            if (startedSection) {
                fComponents_.Append (curSectionSB);
                curSectionSB.clear ();
            }
            startedSection = true;
        }
        else if (startedSection) {
            if (c != '~') {
                curSectionSB << c;
            }
        }
        else {
            static const auto kExcept_ = DataExchange::BadFormatException{"Expected / to start section of JSON Pointer"sv};
            Execution::Throw (kExcept_);
        }
        prevChar = c;
    });
    if (prevChar == '/' or not curSectionSB.empty ()) {
        fComponents_.Append (curSectionSB);
    }
}

optional<VariantValue> JSON::PointerType::Apply (const VariantValue& v) const
{
    VariantValue curNode = v;
    for (auto component : this->fComponents_) {
        switch (curNode.GetType ()) {
            case VariantValue::eArray: {
                size_t                 i  = Characters::String2Int<size_t> (component);
                Sequence<VariantValue> sv = curNode.As<Sequence<VariantValue>> ();
                if (i >= sv.size ()) {
                    return nullopt;
                }
                curNode = sv[i];
            } break;
            case VariantValue::eMap: {
                Mapping<String, VariantValue> mv = curNode.As<Mapping<String, VariantValue>> ();
                if (auto o = mv.Lookup (component)) {
                    curNode = *o;
                }
                else {
                    return nullopt;
                }
            } break;
            default: {
                return nullopt;
            } break;
        }
    }
    // if we run out of components, return everything that is left
    return curNode;
}

String JSON::PointerType::ToString () const
{
    return Characters::ToString (fComponents_);
}
