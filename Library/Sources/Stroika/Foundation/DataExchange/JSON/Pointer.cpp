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
 ************************ JSON::PointerType::MapElt *****************************
 ********************************************************************************
 */
String JSON::PointerType::Context::MapElt::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "orig: " << fOrigValue << ", "sv;
    sb << "eltName: " << fEltName;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 *********************** JSON::PointerType::SeqElt ******************************
 ********************************************************************************
 */
String JSON::PointerType::Context::SeqElt::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "orig: " << fOrigValue << ", "sv;
    sb << "index: " << this->fIndex;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 *********************** JSON::PointerType::Context *****************************
 ********************************************************************************
 */
namespace {
    using MapElt = JSON::PointerType::Context::MapElt;
    using SeqElt = JSON::PointerType::Context::SeqElt;
    auto PopOneAtAtATPopOneAtAtATime_ (Stack<variant<MapElt, SeqElt>> stack, const optional<VariantValue>& leafToUse) -> optional<VariantValue>
    {
        if (stack.empty ()) {
            return leafToUse;
        }
        variant<MapElt, SeqElt> cur = stack.Pop ();
        if (auto om = get_if<MapElt> (&cur)) {
            Mapping<String, VariantValue> r = om->fOrigValue;
            if (leafToUse) {
                r.Add (om->fEltName, leafToUse);
            }
            return PopOneAtAtATPopOneAtAtATime_ (stack, VariantValue{r});
        }
        else if (auto os = get_if<SeqElt> (&cur)) {
            Sequence<VariantValue> r = os->fOrigValue;
            if (os->fIndex > r.size ()) {
                static const auto kExcept_ = DataExchange::BadFormatException{"JSON Patch had array reference outside bounds of array"sv};
                Execution::Throw (kExcept_);
            }
            if (leafToUse) {
                r.SetAt (os->fIndex, *leafToUse);
            }
            return PopOneAtAtATPopOneAtAtATime_ (stack, VariantValue{r});
        }
        else {
            AssertNotReached (); // always one kind of context or another
            return nullopt;
        }
    }
}

optional<VariantValue> JSON::PointerType::Context::ConstructNewFrom (const optional<VariantValue>& leafToUse) const
{
    return PopOneAtAtATPopOneAtAtATime_ (fStack, leafToUse);
}

String JSON::PointerType::Context::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "stack: " << fStack;
    sb << "}"sv;
    return sb;
}

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

auto JSON::PointerType::ApplyWithContext (const VariantValue& v, Context* contextOut) const -> optional<VariantValue>
{
    VariantValue curNode = v;
    for (String component : this->fComponents_) {
        switch (curNode.GetType ()) {
            case VariantValue::eArray: {
                Sequence<VariantValue> sv = curNode.As<Sequence<VariantValue>> ();
                size_t                 i  = component == "-"sv ? sv.size () : Characters::String2Int<size_t> (component);
                if (i > sv.size ()) {
                    return nullopt;
                }
                curNode = i == sv.size () ? VariantValue{} : sv[i];
                if (contextOut != nullptr) {
                    contextOut->fStack.Push (Context::SeqElt{.fOrigValue = sv, .fIndex = i});
                }
            } break;
            case VariantValue::eMap: {
                Mapping<String, VariantValue> mv = curNode.As<Mapping<String, VariantValue>> ();
                if (auto o = mv.Lookup (component)) {
                    curNode = *o;
                }
                else {
                    return nullopt;
                }
                if (contextOut != nullptr) {
                    contextOut->fStack.Push (Context::MapElt{.fOrigValue = mv, .fEltName = component});
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
