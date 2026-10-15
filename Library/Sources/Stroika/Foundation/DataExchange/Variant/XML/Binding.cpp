/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/DataExchange/XML/WriterUtils.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

#include "Binding.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Variant::XML;

using Containers::Set;

namespace {
    // shared_ptr's == compares the pointers - but a Binding is a value, so compare what they point to
    bool SameBinding_ (const shared_ptr<Binding>& lhs, const shared_ptr<Binding>& rhs)
    {
        return (lhs == nullptr or rhs == nullptr) ? lhs == rhs : *lhs == *rhs;
    }

    bool IsLeaf_ (VariantValue::Type t)
    {
        return t != VariantValue::eNull and t != VariantValue::eMap and t != VariantValue::eArray;
    }

    // a member of this kind is written as one of its element's children (a presence marker is an empty one)
    bool IsElement_ (Binding::Kind k)
    {
        return k == Binding::Kind::eElement or k == Binding::Kind::ePresenceMarker;
    }

    // where a Binding sits, which decides what it may be: an array of repeated elements repeats its member's element inside the
    // object it is a member of - so as an array's item, or the document element, it has none to repeat
    enum class Place_ {
        eDocument,
        eMember,
        eItem
    };

    // why b, at where (a path, for the reason), is not complete - nullopt where it is. prefixes are the namespaces given a prefix
    // in scope at b's element, as the Writer binds them
    optional<String> WhyIncompleteAt_ (const Binding& b, const String& where, Place_ place, Mapping<URI, String> prefixes)
    {
        if (not b.fType) {
            return "{}: declares no fType"_f(where);
        }
        for (const auto& i : b.fNamespacePrefixes) {
            if (not DataExchange::XML::IsNCName (i.fValue)) {
                return "{}: the prefix '{}' is not an XML name"_f(where, i.fValue);
            }
            prefixes.RemoveAll ([&] (const Common::KeyValuePair<URI, String>& p) { return p.fValue == i.fValue; });
            prefixes.Add (i.fKey, i.fValue);
        }
        switch (*b.fType) {
            case VariantValue::eNull:
                return "{}: declares eNull, which no value but null has"_f(where);
            case VariantValue::eMap: {
                if (b.fItems != nullptr) {
                    return "{}: an object (eMap) with fItems"_f(where);
                }
                if (b.fUndeclaredMembers == Binding::UndeclaredMembers::eKeep) {
                    return "{}: keeps the members it does not declare (eKeep), untyped"_f(where);
                }
                Set<String> keys;
                Set<String> elementNames;
                Set<String> attributeNames;
                bool        hasText     = false;
                bool        hasElements = false;
                for (const Binding::Member& m : b.fMembers) {
                    String memberWhere = "{}/{}"_f(where, m.fKey);
                    if (keys.Contains (m.fKey)) {
                        return "{}: listed twice in fMembers"_f(memberWhere);
                    }
                    keys.Add (m.fKey);
                    if (m.fBinding == nullptr) {
                        return "{}: no Binding, so declares no fType"_f(memberWhere);
                    }
                    if (optional<String> why = WhyIncompleteAt_ (*m.fBinding, memberWhere, Place_::eMember, prefixes)) {
                        return why;
                    }
                    NameWithNamespace name = b.NameFor (m.fKey);
                    if (m.fBinding->fKind != Binding::Kind::eElement and not IsLeaf_ (*m.fBinding->fType)) {
                        return "{}: an attribute, text or presence marker that is not a leaf"_f(memberWhere);
                    }
                    if (m.fBinding->fKind == Binding::Kind::ePresenceMarker and m.fBinding->fType != VariantValue::eBoolean) {
                        return "{}: a presence marker (ePresenceMarker) that is not a Boolean"_f(memberWhere);
                    }
                    switch (m.fBinding->fKind) {
                        case Binding::Kind::eAttribute:
                            if (not DataExchange::XML::IsNCName (name.fName) or (name.fName == "xmlns"sv and not name.fNamespace)) {
                                return "{}: '{}' is not an XML attribute name"_f(memberWhere, name.fName);
                            }
                            if (name.fNamespace and not prefixes.ContainsKey (*name.fNamespace)) {
                                return "{}: an attribute in a namespace with no prefix in scope (fNamespacePrefixes)"_f(memberWhere);
                            }
                            if (attributeNames.Contains (name.fName)) {
                                return "{}: another attribute's local name too: '{}'"_f(memberWhere, name.fName);
                            }
                            attributeNames.Add (name.fName);
                            break;
                        case Binding::Kind::eText:
                            if (hasText) {
                                return "{}: a second text member"_f(memberWhere);
                            }
                            hasText = true;
                            break;
                        default:
                            if (not DataExchange::XML::IsNCName (name.fName)) {
                                return "{}: '{}' is not an XML name"_f(memberWhere, name.fName);
                            }
                            if (elementNames.Contains (name.fName)) {
                                return "{}: another member's element's local name too: '{}'"_f(memberWhere, name.fName);
                            }
                            elementNames.Add (name.fName);
                            hasElements = true;
                            break;
                    }
                }
                if (hasText and hasElements) {
                    return "{}: a text member beside child elements (mixed content)"_f(where);
                }
            } break;
            case VariantValue::eArray: {
                if (not b.fMembers.empty ()) {
                    return "{}: an array (eArray) with fMembers"_f(where);
                }
                if (b.fItems == nullptr) {
                    return "{}: an array with no fItems"_f(where);
                }
                if (b.fItems->fKind != Binding::Kind::eElement) {
                    return "{}[]: an array's item that is not an element"_f(where);
                }
                if (b.fItems->fName) {
                    if (not DataExchange::XML::IsNCName (b.fItems->fName->fName)) {
                        return "{}[]: '{}' is not an XML name"_f(where, b.fItems->fName->fName);
                    }
                }
                else if (place != Place_::eMember) {
                    return "{}: an array of repeated elements that is not an object's member, so has no element of its own to repeat"_f(where);
                }
                if (optional<String> why = WhyIncompleteAt_ (*b.fItems, "{}[]"_f(where), Place_::eItem, prefixes)) {
                    return why;
                }
            } break;
            default:
                if (not b.fMembers.empty () or b.fItems != nullptr) {
                    return "{}: a leaf ({}) with fMembers or fItems"_f(where, *b.fType);
                }
                break;
        }
        return nullopt;
    }
}

/*
 ********************************************************************************
 *********************** Variant::XML::Binding::Member **************************
 ********************************************************************************
 */
Binding::Member::Member (const String& key)
    : fKey{key}
{
}

Binding::Member::Member (const String& key, const Binding& binding)
    : fKey{key}
    , fBinding{Memory::MakeSharedPtr<Binding> (binding)}
{
}

bool Binding::Member::operator== (const Member& rhs) const
{
    return fKey == rhs.fKey and SameBinding_ (fBinding, rhs.fBinding);
}

String Binding::Member::ToString () const
{
    StringBuilder sb;
    sb << "{"sv << fKey;
    if (fBinding != nullptr) {
        sb << ": "sv << fBinding->ToString_ (false);
    }
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ***************************** Variant::XML::Binding ****************************
 ********************************************************************************
 */
const Binding Binding::kDefaultArrayItems{.fName = NameWithNamespace{"Array"sv}};
const Binding Binding::kRepeatedArrayItems{};

const Binding* Binding::MemberBinding (const String& key) const
{
    for (const Member& i : fMembers) {
        if (i.fKey == key) {
            return i.fBinding.get ();
        }
    }
    return nullptr;
}

NameWithNamespace Binding::NameFor (const String& key) const
{
    if (const Binding* b = MemberBinding (key); b != nullptr and b->fName) {
        return *b->fName;
    }
    return NameWithNamespace{key};
}

Binding::Kind Binding::KindFor (const String& key) const
{
    const Binding* b = MemberBinding (key);
    return b == nullptr ? Kind::eElement : b->fKind;
}

String Binding::KeyForName (Kind kind, const String& name) const
{
    for (const Member& i : fMembers) {
        if ((IsElement_ (kind) ? IsElement_ (KindFor (i.fKey)) : KindFor (i.fKey) == kind) and NameFor (i.fKey).fName == name) {
            return i.fKey;
        }
    }
    return name;
}

optional<String> Binding::TextMemberKey () const
{
    for (const Member& i : fMembers) {
        if (i.fBinding != nullptr and i.fBinding->fKind == Kind::eText) {
            return i.fKey;
        }
    }
    return nullopt;
}

auto Binding::GetUndeclaredMembers () const -> UndeclaredMembers
{
    return fUndeclaredMembers.value_or (fType ? UndeclaredMembers::eReject : UndeclaredMembers::eKeep);
}

bool Binding::IsComplete () const
{
    return not WhyIncomplete_ ().has_value ();
}

optional<String> Binding::WhyIncomplete_ () const
{
    // the document element has a name of its own, and is an element - and with no fName, the value is an object of exactly
    // one member, which an object with any other number of members is not
    if (not fName) {
        return String{"the document element is not named (fName)"sv};
    }
    if (not DataExchange::XML::IsNCName (fName->fName)) {
        return "the document element's name '{}' is not an XML name"_f(fName->fName);
    }
    if (fKind != Kind::eElement) {
        return String{"the document element is not an element (fKind)"sv};
    }
    return WhyIncompleteAt_ (*this, fName->fName, Place_::eDocument, {});
}

bool Binding::operator== (const Binding& rhs) const
{
    return fKind == rhs.fKind and fName == rhs.fName and fType == rhs.fType and fMembers == rhs.fMembers and
           fUndeclaredMembers == rhs.fUndeclaredMembers and SameBinding_ (fItems, rhs.fItems) and
           SameBinding_ (fDefaultArrayItems, rhs.fDefaultArrayItems) and fNamespacePrefixes == rhs.fNamespacePrefixes;
}

String Binding::ToString () const
{
    return ToString_ (true);
}

String Binding::ToString_ (bool top) const
{
    StringBuilder sb;
    sb << "{"sv;
    switch (fKind) {
        case Kind::eAttribute:
            sb << "attribute"sv;
            break;
        case Kind::eText:
            sb << "text"sv;
            break;
        case Kind::ePresenceMarker:
            sb << "presence marker"sv;
            break;
        default:
            sb << "element"sv;
            break;
    }
    if (fName) {
        sb << ", Name: "sv << *fName;
    }
    if (fType) {
        sb << ", Type: "sv << *fType;
    }
    if (not fMembers.empty ()) {
        sb << ", Members: "sv << fMembers;
    }
    if (fUndeclaredMembers) {
        switch (*fUndeclaredMembers) {
            case UndeclaredMembers::eReject:
                sb << ", UndeclaredMembers: reject"sv;
                break;
            case UndeclaredMembers::eSkip:
                sb << ", UndeclaredMembers: skip"sv;
                break;
            case UndeclaredMembers::eKeep:
                sb << ", UndeclaredMembers: keep"sv;
                break;
        }
    }
    if (fItems != nullptr) {
        sb << ", Items: "sv << fItems->ToString_ (false);
    }
    if (fDefaultArrayItems != nullptr) {
        sb << ", DefaultItems: "sv << fDefaultArrayItems->ToString_ (false);
    }
    if (not fNamespacePrefixes.empty ()) {
        sb << ", NamespacePrefixes: "sv << fNamespacePrefixes;
    }
    if (top) {
        if (optional<String> why = WhyIncomplete_ ()) {
            sb << ", Incomplete: "sv << *why;
        }
        else {
            sb << ", Complete"sv;
        }
    }
    sb << "}"sv;
    return sb;
}
