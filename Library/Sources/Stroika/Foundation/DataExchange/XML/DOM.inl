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

#if qHasFeature_Xerces
namespace Stroika::Foundation::DataExchange::XML::DOM {

    /*
     ********************************************************************************
     ************************ XML::DOM::XPath::Expression ***************************
     ********************************************************************************
     */
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
        Require (GetRep () == nullptr or GetNodeType () == eElementNT);
    }
    inline Element::Ptr::Ptr (const shared_ptr<IRep>& rep)
        : Node::Ptr{rep}
    {
        Require (GetRep () == nullptr or GetNodeType () == eElementNT);
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
    inline Element::Ptr Element::Ptr::AppendIfNotEmpty (const NameWithNamespace& eltName, const optional<String>& v)
    {
        if (v) {
            auto r = Append (eltName);
            r.SetValue (*v);
            return r;
        }
        return Element::Ptr{nullptr};
    }
    inline Element::Ptr Element::Ptr::Replace ()
    {
        /**
         *  Disallow replacing the root element - create a new document instead. (@todo fix a nd allow docRoot)
         * 
         *  Basic algorithm:
         *      o   Save parent node
         *      o   Add new node just after this one (with same name/namespace)
         *      o   Delete this node (one just 'replaced')
         * 
         *  Note - to avoid issues with temporarily having two nodes of the same name in list of children - may need to modify this logic?
         */
        Element::Ptr parent = this->GetParent ();
        Element::Ptr newNode{nullptr};
        if (parent == nullptr) {
            auto n = this->GetName ();
            this->Delete ();
            // technically 'Append' is not right and we really should grab the 'next sibling' and insert before it...but that seems unlikely to matter --LGP 2024-01-06
            newNode = parent.Append (n);
        }
        else {
            newNode = parent.Insert (this->GetName (), *this); // @todo clone NS as well!!!
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
    inline auto Element::Ptr::LookupOne (const XPath::Expression& e) const -> optional<XPath::Result>
    {
        return GetRep ()->LookupOne (e);
    }
    inline auto Element::Ptr::Lookup (const XPath::Expression& e) const -> Traversal::Iterator<XPath::Result>
    {
        return GetRep ()->Lookup (e);
    }
    inline auto Element::Ptr::GetRep () const -> shared_ptr<IRep>
    {
        return dynamic_pointer_cast<IRep> (Node::Ptr::GetRep ());
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
        return fRep_;
    }
    inline void Document::Ptr::Write (const Streams::OutputStream::Ptr<byte>& to, const SerializationOptions& options) const
    {
        fRep_->Write (to, options);
    }
    inline String Document::Ptr::Write (const SerializationOptions& options) const
    {
        // @todo need a better Streams DESIGN here - were we can write and produce the string directly...
        Streams::MemoryStream::Ptr<byte> bufferedOutput = Streams::MemoryStream::New<byte> ();
        fRep_->Write (bufferedOutput, options);
        return Streams::TextReader::New (bufferedOutput).ReadAll ();
    }
    inline Iterable<Node::Ptr> Document::Ptr::GetChildren () const
    {
        RequireNotNull (fRep_);
        return fRep_->GetChildren ();
    }
    inline Element::Ptr Document::Ptr::GetRootElement () const
    {
        RequireNotNull (fRep_);
        // Should only be one in an XML document.
        for (Node::Ptr ni : fRep_->GetChildren ()) {
            if (ni.GetNodeType () == Node::eElementNT) {
                return ni;
            }
        }
        return Element::Ptr{nullptr};
    }

}
#endif

#endif
