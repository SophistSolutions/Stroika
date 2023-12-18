/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_DOM_inl_
#define _Stroika_Foundation_DataExchange_XML_DOM_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#if qHasFeature_Xerces
namespace Stroika::Foundation::DataExchange::XML::DOM {

    /*
     ********************************************************************************
     ****************************** XML::DOM::Node::Ptr *****************************
     ********************************************************************************
     */
    inline Node::Ptr::Ptr (const shared_ptr<IRep>& from)
        : fRep_{from}
    {
    }
    inline Node::Type Node::Ptr::GetNodeType () const
    {
        AssertNotNull (fRep_);
        return fRep_->GetNodeType ();
    }
    inline optional<URI> Node::Ptr::GetNamespace () const
    {
        AssertNotNull (fRep_);
        return fRep_->GetNamespace ();
    }
    inline String Node::Ptr::GetName () const
    {
        AssertNotNull (fRep_);
        return fRep_->GetName ();
    }
    inline void Node::Ptr::SetName (const String& name)
    {
        AssertNotNull (fRep_);
        fRep_->SetName (name);
    }
    inline VariantValue Node::Ptr::GetValue () const
    {
        AssertNotNull (fRep_);
        return fRep_->GetValue ();
    }
    inline void Node::Ptr::SetValue (const VariantValue& v)
    {
        AssertNotNull (fRep_);
        fRep_->SetValue (v);
    }
    inline void Node::Ptr::SetAttribute (const String& attrName, const String& v)
    {
        AssertNotNull (fRep_);
        fRep_->SetAttribute (attrName, v);
    }
    inline bool Node::Ptr::HasAttribute (const String& attrName) const
    {
        AssertNotNull (fRep_);
        return fRep_->HasAttribute (attrName, nullptr);
    }
    inline bool Node::Ptr::HasAttribute (const String& attrName, const String& value) const
    {
        AssertNotNull (fRep_);
        return fRep_->HasAttribute (attrName, &value);
    }
    inline String Node::Ptr::GetAttribute (const String& attrName) const
    {
        AssertNotNull (fRep_);
        return fRep_->GetAttribute (attrName);
    }
    inline optional<Node::Ptr> Node::Ptr::GetFirstAncestorNodeWithAttribute (const String& attrName) const
    {
        AssertNotNull (fRep_);
        return fRep_->GetFirstAncestorNodeWithAttribute (attrName);
    }
    inline Node::Ptr Node::Ptr::InsertChild (const String& name, optional<Ptr> afterNode)
    {
        AssertNotNull (fRep_);
        return fRep_->InsertChild (name, nullptr, afterNode);
    }
    inline Node::Ptr Node::Ptr::InsertChild (const String& name, const String& ns, optional<Node::Ptr> afterNode)
    {
        AssertNotNull (fRep_);
        return fRep_->InsertChild (name, &ns, afterNode);
    }
    inline Node::Ptr Node::Ptr::AppendChild (const String& name)
    {
        AssertNotNull (fRep_);
        return fRep_->AppendChild (name);
    }
    inline void Node::Ptr::AppendChild (const String& name, const VariantValue& v)
    {
        AssertNotNull (fRep_);
        fRep_->AppendChild (name, nullptr, v);
    }
    inline void Node::Ptr::AppendChild (const String& name, const String& ns, const VariantValue& v)
    {
        AssertNotNull (fRep_);
        fRep_->AppendChild (name, &ns, v);
    }
    inline void Node::Ptr::AppendChildIfNotEmpty (const String& name, const VariantValue& v)
    {
        AssertNotNull (fRep_);
        fRep_->AppendChildIfNotEmpty (name, nullptr, v);
    }
    inline void Node::Ptr::AppendChildIfNotEmpty (const String& name, const String& ns, const VariantValue& v)
    {
        AssertNotNull (fRep_);
        fRep_->AppendChildIfNotEmpty (name, &ns, v);
    }
    inline Node::Ptr Node::Ptr::InsertNode (const Node::Ptr& n, const optional<Node::Ptr>& afterNode, bool inheritNamespaceFromInsertionPoint)
    {
        AssertNotNull (fRep_);
        return fRep_->InsertNode (n, afterNode, inheritNamespaceFromInsertionPoint);
    }
    inline Node::Ptr Node::Ptr::AppendNode (const Node::Ptr& n, bool inheritNamespaceFromInsertionPoint)
    {
        AssertNotNull (fRep_);
        return fRep_->AppendNode (n, inheritNamespaceFromInsertionPoint);
    }
    inline void Node::Ptr::DeleteNode ()
    {
        AssertNotNull (fRep_);
        fRep_->DeleteNode ();
    }
    inline Node::Ptr Node::Ptr::ReplaceNode ()
    {
        AssertNotNull (fRep_);
        return fRep_->ReplaceNode ();
    }
    inline optional<Node::Ptr> Node::Ptr::GetParentNode () const
    {
        AssertNotNull (fRep_);
        return fRep_->GetParentNode ();
    }
    inline Iterable<Node::Ptr> Node::Ptr::GetChildren () const
    {
        AssertNotNull (fRep_);
        return fRep_->GetChildren ();
    }
    inline optional<Node::Ptr> Node::Ptr::GetChildNodeByID (const String& id) const
    {
        return fRep_->GetChildNodeByID (id);
    }

    /*
     ********************************************************************************
     *********************************** XML::DOM::Node *****************************
     ********************************************************************************
     */
    inline Document::Ptr::Ptr (const shared_ptr<IRep>& rep)
        : fRep_{rep}
    {
        RequireNotNull (rep);
    }
    inline shared_ptr<Document::IRep> Document::Ptr::GetRep () const
    {
        return fRep_;
    }
    inline void Document::Ptr::WritePrettyPrinted (ostream& out) const
    {
        /*
     * Write pretty printed XML - where we generate the whitespace around nodes - ignoring any text fragments - except in leaf nodes.
     */
        fRep_->WritePrettyPrinted (out);
    }
    inline void Document::Ptr::WriteAsIs (ostream& out) const
    {
        /*
     * Write - respecting all the little #text fragment nodes throughout the XML node tree
     */
        fRep_->WriteAsIs (out);
    }
    inline Iterable<Node::Ptr> Document::Ptr::GetChildren () const
    {
        return fRep_->GetChildren ();
    }
    inline Node::Ptr Document::Ptr::GetRootElement () const
    {
        // Should only be one in an XML document.
        for (Node::Ptr ni : fRep_->GetChildren ()) {
            if (ni.GetNodeType () == Node::eElementNT) {
                return ni;
            }
        }
        static const auto kException_ = Execution::RuntimeErrorException{"No root element"};
        Execution::Throw (kException_);
    }
    inline void Document::Ptr::Validate (const Schema::Ptr& schema) const
    {
        fRep_->Validate (schema);
    }
    inline Node::Ptr Document::Ptr::CreateDocumentElement (const String& name, const optional<URI>& ns)
    {
        return fRep_->CreateDocumentElement (name, ns);
    }
    inline void Document::Ptr::SetRootElement (const Node::Ptr& newRoot)
    {
        return fRep_->SetRootElement (newRoot);
    }

}
#endif

#endif
