/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_DOM_inl_
#define _Stroika_Foundation_DataExchange_XML_DOM_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../../Streams/MemoryStream.h"
#include "../../Streams/TextReader.h"

namespace Stroika::Foundation::DataExchange::XML::DOM {

    /*
     ********************************************************************************
     ************************ XML::DOM::XPath::Expression ***************************
     ********************************************************************************
     */
    template <Characters::IConvertibleToString ST>
    inline XPath::Expression::Expression (ST&& e, const Options& o)
        : Expression{String{e}, o}
    {
    }
    inline String XPath::Expression::GetExpression () const
    {
        return fRep_->GetExpression ();
    }
    inline auto XPath::Expression::GetOptions () const -> Options
    {
        return fRep_->GetOptions ();
    }
    inline auto XPath::Expression::GetRep () const -> shared_ptr<const IRep>
    {
        return fRep_;
    }

    /*
     ********************************************************************************
     ****************************** XML::DOM::Node::Ptr *****************************
     ********************************************************************************
     */
    inline Node::Ptr::Ptr (const shared_ptr<IRep>& from)
        : fRep_{from}
    {
    }
    inline Node::Ptr::Ptr (nullptr_t)
        : fRep_{}
    {
    }
    inline bool Node::Ptr::operator== (const Ptr& rhs) const
    {
        if (fRep_ == nullptr) [[unlikely]] {
            return rhs.fRep_ == nullptr;
        }
        if (rhs.fRep_ == nullptr) {
            return false;
        }
        return fRep_->Equals (rhs.fRep_.get ());
    }
    inline bool Node::Ptr::operator== (nullptr_t) const
    {
        return fRep_ == nullptr;
    }
    inline Node::Ptr::operator bool () const
    {
        return fRep_.operator bool ();
    }
    inline Node::Type Node::Ptr::GetNodeType () const
    {
        RequireNotNull (fRep_);
        return fRep_->GetNodeType ();
    }
    inline auto Node::Ptr::GetName () const -> NameWithNamespace
    {
        Require (GetNodeType () == eAttributeNT or GetNodeType () == eElementNT);
        return GetRep ()->GetName ();
    }
    inline void Node::Ptr::SetName (const NameWithNamespace& name)
    {
        Require (GetNodeType () == eAttributeNT or GetNodeType () == eElementNT);
        GetRep ()->SetName (name);
    }
    inline String Node::Ptr::GetValue () const
    {
        Require (GetNodeType () == eAttributeNT or GetNodeType () == eElementNT);
        return GetRep ()->GetValue ();
    }
    inline void Node::Ptr::SetValue (const String& v)
    {
        Require (GetNodeType () == eAttributeNT or GetNodeType () == eElementNT);
        return GetRep ()->SetValue (v);
    }
    inline void Node::Ptr::Delete ()
    {
        RequireNotNull (fRep_);
        fRep_->DeleteNode ();
        fRep_ = nullptr;
    }
    inline Node::Ptr Node::Ptr::GetParentNode () const
    {
        AssertNotNull (fRep_);
        return fRep_->GetParentNode ();
    }
    inline shared_ptr<Node::IRep> Node::Ptr::GetRep () const
    {
        EnsureNotNull (fRep_);
        return fRep_;
    }
    inline shared_ptr<Node::IRep> Node::Ptr::PeekRep () const
    {
        return fRep_;
    }

    /*
     ********************************************************************************
     ************************* XML::DOM::Element::Ptr *******************************
     ********************************************************************************
     */
    inline Element::Ptr::Ptr (nullptr_t)
        : Node::Ptr{nullptr}
    {
    }
    inline Element::Ptr::Ptr (const Node::Ptr& p)
        : Node::Ptr{p != nullptr and p.GetNodeType () == Node::eElementNT ? p : nullptr}
    {
        Require (PeekRep () == nullptr or GetNodeType () == eElementNT);
    }
    inline Element::Ptr::Ptr (const shared_ptr<IRep>& rep)
        : Node::Ptr{rep}
    {
        Require (PeekRep () == nullptr or GetNodeType () == eElementNT);
    }
    inline Element::Ptr::Ptr (const XPath::Result& r)
        : Node::Ptr{get<Node::Ptr> (r)}
    {
        Require (PeekRep () == nullptr or GetNodeType () == eElementNT);
    }
    inline optional<String> Element::Ptr::GetAttribute (const NameWithNamespace& attrName) const
    {
        return GetRep ()->GetAttribute (attrName);
    }
    inline bool Element::Ptr::HasAttribute (const NameWithNamespace& attrName) const
    {
        return GetRep ()->GetAttribute (attrName) != nullopt;
    }
    inline bool Element::Ptr::HasAttribute (const NameWithNamespace& attrName, const String& value) const
    {
        if (auto o = GetRep ()->GetAttribute (attrName)) {
            return *o == value;
        }
        return false;
    }
    inline void Element::Ptr::SetAttribute (const NameWithNamespace& attrName, const optional<String>& v)
    {
        GetRep ()->SetAttribute (attrName, v);
    }
    template <same_as<VariantValue> VV>
    inline void Element::Ptr::SetAttribute (const NameWithNamespace& attrName, const VV& v)
    {
        GetRep ()->SetAttribute (attrName, v == nullptr ? optional<String>{} : v.template As<String> ());
    }
    inline optional<String> Element::Ptr::GetID () const
    {
        static const NameWithNamespace kID_{"id"sv};
        return GetRep ()->GetAttribute (kID_);
    }
    inline optional<URI> Element::Ptr::GetDefaultNamespace () const
    {
        if (auto v = GetAttribute (kXMLNS)) {
            return URI{*v};
        }
        return nullopt;
    }
    inline void Element::Ptr::SetDefaultNamespace (const optional<URI> defaultNS)
    {
        SetAttribute (kXMLNS, defaultNS == nullopt ? optional<String>{} : defaultNS->As<String> ());
    }
    inline optional<String> Element::Ptr::GetValue (const XPath::Expression& e) const
    {
        Require (e.GetOptions ().fResultTypeIndex == XPath::ResultTypeIndex_v<Node::Ptr>);
        if (optional<XPath::Result> o = GetRep ()->LookupOne (e)) {
            Node::Ptr ee = get<Node::Ptr> (*o);
            if (ee != nullptr) {
                return ee.GetValue ();
            }
        }
        return nullopt;
    }
    inline Traversal::Iterable<String> Element::Ptr::GetValues (const XPath::Expression& e) const
    {
        Require (e.GetOptions ().fResultTypeIndex == XPath::ResultTypeIndex_v<Node::Ptr>);
        return GetRep ()->Lookup (e).Map<Iterable<String>> ([] (const XPath::Result& e) -> optional<String> {
            Node::Ptr ep = get<Node::Ptr> (e);
            switch (ep.GetNodeType ()) {
                case Node::Type::eAttributeNT:
                case Node::Type::eElementNT:
                    return ep.GetValue ();
                default:
                    return nullopt; // skip comment nodes etc...
            }
        });
    }
    inline void Element::Ptr::SetValue (const XPath::Expression& e, const String& v)
    {
        Require (e.GetOptions ().fResultTypeIndex == XPath::ResultTypeIndex_v<Node::Ptr>);
        if (optional<XPath::Result> o = GetRep ()->LookupOne (e)) {
            Node::Ptr ee = get<Node::Ptr> (*o);
            if (ee != nullptr) {
                ee.SetValue (v);
                return;
            }
        }
        static const auto kException_ = Execution::RuntimeErrorException<> ("Node not found relative to given element"sv);
        Execution::Throw (kException_);
    }
    template <same_as<VariantValue> VV>
    inline void Element::Ptr::SetValue (const VV& v)
    {
        SetValue (v.template As<String> ());
    }
    template <same_as<VariantValue> VV>
    inline void Element::Ptr::SetValue (const XPath::Expression& e, const VV& v)
    {
        SetValue (e, v.template As<String> ());
    }
    inline auto Element::Ptr::Insert (const NameWithNamespace& eltName, const Node::Ptr& afterNode) -> Ptr
    {
        return GetRep ()->InsertElement (eltName, afterNode);
    }
    inline Element::Ptr Element::Ptr::Append (const NameWithNamespace& eltName)
    {
        return GetRep ()->AppendElement (eltName);
    }
    inline Element::Ptr Element::Ptr::Append (const NameWithNamespace& eltName, const String& v)
    {
        auto r = Append (eltName);
        r.SetValue (v);
        return r;
    }
    template <same_as<VariantValue> VV>
    inline Element::Ptr Element::Ptr::Append (const NameWithNamespace& eltName, const VV& v)
    {
        auto r = Append (eltName);
        r.SetValue (v.template As<String> ());
        return r;
    }
    inline Element::Ptr Element::Ptr::AppendIf (const NameWithNamespace& eltName, const optional<String>& v)
    {
        if (v) {
            auto r = Append (eltName);
            r.SetValue (*v);
            return r;
        }
        return Element::Ptr{nullptr};
    }
    template <same_as<VariantValue> VV>
    inline Element::Ptr Element::Ptr::AppendIf (const NameWithNamespace& eltName, const VV& v)
    {
        if (v != nullptr) {
            return Append (eltName, v.template As<String> ());
        }
        return Element::Ptr{nullptr};
    }
    inline Element::Ptr Element::Ptr::Replace (const NameWithNamespace& newEltName)
    {
        /**
         *  Basic algorithm:
         *      o   If Root of document (no parent node)
         *          o   Delete this
         *          o   Append (this loses relative position of nodes, which we may want to fix, but should generally work OK)
         *      o   If middling (not root) node
         *          o   Add new node just after this one (with same name/namespace)
         *          o   Delete this node (one just 'replaced')
         *          Note - to avoid issues with temporarily having two nodes of the same name in list of children - may need to modify this logic?
         * 
         */
        Element::Ptr parent = this->GetParent ();
        Element::Ptr newNode{nullptr};
        if (parent == nullptr) {
            this->Delete ();
            // technically 'Append' is not right and we really should grab the 'next sibling' and insert before it...but that seems unlikely to matter --LGP 2024-01-06
            newNode = parent.Append (newEltName);
        }
        else {
            newNode = parent.Insert (newEltName, *this);
            this->Delete ();
        }
        Ensure (newNode.GetParent () == parent);
        *this = newNode;
        return newNode;
    }
    inline Element::Ptr Element::Ptr::GetParent () const
    {
        return Element::Ptr{GetRep ()->GetParentNode ()};
    }
    inline auto Element::Ptr::GetChildNodes () const -> Iterable<Node::Ptr>
    {
        AssertNotNull (GetRep ());
        return GetRep ()->GetChildren ();
    }
    inline auto Element::Ptr::GetChildElements () const -> Iterable<Ptr>
    {
        AssertNotNull (GetRep ());
        return GetRep ()->GetChildren ().Map<Iterable<Ptr>> ([] (Node::Ptr p) -> optional<Ptr> {
            Ptr eltNode{p};
            return eltNode == nullptr ? optional<Ptr>{} : eltNode;
        });
    }
    inline auto Element::Ptr::GetChildByID (const String& id) const -> Ptr
    {
        return Element::Ptr{GetRep ()->GetChildElementByID (id)};
    }
    inline auto Element::Ptr::LookupOneElement (const XPath::Expression& e) const -> Element::Ptr
    {
        return Element::Ptr{LookupOneNode (e)};
    }
    inline auto Element::Ptr::LookupOneNode (const XPath::Expression& e) const -> Node::Ptr
    {
        Require (e.GetOptions ().fResultTypeIndex == XPath::ResultTypeIndex_v<Node::Ptr>);
        if (optional<XPath::Result> o = GetRep ()->LookupOne (e)) {
            return get<Node::Ptr> (*o);
        }
        return Node::Ptr{};
    }
    inline auto Element::Ptr::Lookup (const XPath::Expression& e) const -> Traversal::Iterable<XPath::Result>
    {
        return GetRep ()->Lookup (e);
    }
    inline auto Element::Ptr::LookupElements (const XPath::Expression& e) const -> Traversal::Iterable<Element::Ptr>
    {
        return GetRep ()->Lookup (e).Map<Traversal::Iterable<Element::Ptr>> ([] (const XPath::Result& e) {
            Element::Ptr ep{e};
            return ep != nullptr ? ep : optional<Element::Ptr>{};
        });
    }
    inline auto Element::Ptr::GetRep () const -> shared_ptr<IRep>
    {
        auto r = PeekRep ();
        EnsureNotNull (r);
        return r;
    }
    inline auto Element::Ptr::PeekRep () const -> shared_ptr<IRep>
    {
        return dynamic_pointer_cast<IRep> (Node::Ptr::PeekRep ());
    }

    /*
     ********************************************************************************
     ************************ XML::DOM::Document::Ptr *******************************
     ********************************************************************************
     */
    inline Document::Ptr::Ptr (nullptr_t)
        : fRep_{}
    {
    }
    inline Document::Ptr::Ptr (const shared_ptr<IRep>& rep)
        : fRep_{rep}
    {
    }
    inline shared_ptr<Document::IRep> Document::Ptr::GetRep () const
    {
        RequireNotNull (fRep_);
        return fRep_;
    }
    inline bool Document::Ptr::operator== (nullptr_t) const
    {
        return fRep_ == nullptr;
    }
    inline void Document::Ptr::Write (const Streams::OutputStream::Ptr<byte>& to, const SerializationOptions& options) const
    {
        GetRep ()->Write (to, options);
    }
    inline String Document::Ptr::Write (const SerializationOptions& options) const
    {
        // @todo need a better Streams DESIGN here - were we can write and produce the string directly...
        Streams::MemoryStream::Ptr<byte> bufferedOutput = Streams::MemoryStream::New<byte> ();
        GetRep ()->Write (bufferedOutput, options);
        return Streams::TextReader::New (bufferedOutput).ReadAll ();
    }
    inline Iterable<Node::Ptr> Document::Ptr::GetChildren () const
    {
        return GetRep ()->GetChildren ();
    }
    inline Element::Ptr Document::Ptr::GetRootElement () const
    {
        // Should only be one in an XML document.
        for (Node::Ptr ni : GetRep ()->GetChildren ()) {
            if (ni.GetNodeType () == Node::eElementNT) {
                return ni;
            }
        }
        return Element::Ptr{nullptr};
    }
    inline Element::Ptr Document::Ptr::ReplaceRootElement (const NameWithNamespace& newEltName, bool childrenInheritNS) const
    {
        // Note this cannot be implemented using the existing Replace () mechanism for elements because the document could be created without a root.
        return GetRep ()->ReplaceRootElement (newEltName, childrenInheritNS);
    }
    inline Element::Ptr Document::Ptr::LookupOneElement (const XPath::Expression& e) const
    {
        RequireNotNull (GetRootElement ());
        return GetRootElement ().LookupOneElement (e);
    }
    inline Traversal::Iterable<XPath::Result> Document::Ptr::Lookup (const XPath::Expression& e) const
    {
        RequireNotNull (GetRootElement ());
        return GetRootElement ().Lookup (e);
    }
    inline Traversal::Iterable<Element::Ptr> Document::Ptr::LookupElements (const XPath::Expression& e) const
    {
        RequireNotNull (GetRootElement ());
        return GetRootElement ().LookupElements (e);
    }

}

#endif
