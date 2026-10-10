/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::DataExchange::XML {

    /*
     ********************************************************************************
     ***************************** XML::BindingMember *******************************
     ********************************************************************************
     */
    inline BindingMember::BindingMember (const String& key)
        : fKey{key}
    {
    }
    inline BindingMember::BindingMember (const String& key, const Binding& binding)
        : fKey{key}
        , fBinding{Memory::MakeSharedPtr<Binding> (binding)}
    {
    }
    inline bool BindingMember::operator== (const BindingMember& rhs) const
    {
        if (fKey != rhs.fKey) {
            return false;
        }
        if (fBinding == nullptr or rhs.fBinding == nullptr) {
            return fBinding == rhs.fBinding;
        }
        return *fBinding == *rhs.fBinding;
    }
    inline String BindingMember::ToString () const
    {
        Characters::StringBuilder sb;
        sb << "{"sv << fKey;
        if (fBinding != nullptr) {
            sb << ": "sv << *fBinding;
        }
        sb << "}"sv;
        return sb;
    }

    /*
     ********************************************************************************
     ******************************** XML::Binding **********************************
     ********************************************************************************
     */
    inline const Binding* Binding::MemberBinding (const String& key) const
    {
        for (const BindingMember& i : fMembers) {
            if (i.fKey == key) {
                return i.fBinding.get ();
            }
        }
        return nullptr;
    }
    inline NameWithNamespace Binding::NameFor (const String& key) const
    {
        if (const Binding* b = MemberBinding (key); b != nullptr and b->fName) {
            return *b->fName;
        }
        return NameWithNamespace{key};
    }
    inline Binding::Kind Binding::KindFor (const String& key) const
    {
        const Binding* b = MemberBinding (key);
        return b == nullptr ? Kind::eElement : b->fKind;
    }
    inline String Binding::KeyForName (const String& elementName) const
    {
        for (const BindingMember& i : fMembers) {
            if (i.fBinding != nullptr and i.fBinding->fName and i.fBinding->fName->fName == elementName) {
                return i.fKey;
            }
        }
        return elementName;
    }
    inline optional<String> Binding::TextMemberKey () const
    {
        for (const BindingMember& i : fMembers) {
            if (i.fBinding != nullptr and i.fBinding->fKind == Kind::eText) {
                return i.fKey;
            }
        }
        return nullopt;
    }
    inline String Binding::ToString () const
    {
        Characters::StringBuilder sb;
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
        sb << "}"sv;
        return sb;
    }

}
