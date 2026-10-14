/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

#include "Binding.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Variant::XML;

namespace {
    // shared_ptr's == compares the pointers - but a Binding is a value, so compare what they point to
    bool SameBinding_ (const shared_ptr<Binding>& lhs, const shared_ptr<Binding>& rhs)
    {
        return (lhs == nullptr or rhs == nullptr) ? lhs == rhs : *lhs == *rhs;
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
        sb << ": "sv << *fBinding;
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

String Binding::KeyForName (const String& elementName) const
{
    for (const Member& i : fMembers) {
        if (i.fBinding != nullptr and i.fBinding->fName and i.fBinding->fName->fName == elementName) {
            return i.fKey;
        }
    }
    return elementName;
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

bool Binding::operator== (const Binding& rhs) const
{
    return fKind == rhs.fKind and fName == rhs.fName and fMembers == rhs.fMembers and SameBinding_ (fItems, rhs.fItems) and
           SameBinding_ (fDefaultArrayItems, rhs.fDefaultArrayItems) and fNamespacePrefixes == rhs.fNamespacePrefixes;
}

String Binding::ToString () const
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
        default:
            sb << "element"sv;
            break;
    }
    if (fName) {
        sb << ", Name: "sv << *fName;
    }
    if (not fMembers.empty ()) {
        sb << ", Members: "sv << fMembers;
    }
    if (fItems != nullptr) {
        sb << ", Items: "sv << *fItems;
    }
    if (fDefaultArrayItems != nullptr) {
        sb << ", DefaultItems: "sv << *fDefaultArrayItems;
    }
    if (not fNamespacePrefixes.empty ()) {
        sb << ", NamespacePrefixes: "sv << fNamespacePrefixes;
    }
    sb << "}"sv;
    return sb;
}
