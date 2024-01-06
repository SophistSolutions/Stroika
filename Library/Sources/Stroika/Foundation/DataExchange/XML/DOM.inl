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

#include "../../Streams/MemoryStream.h"
#include "../../Streams/TextReader.h"

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
    inline Node::Ptr::Ptr (nullptr_t)
        : fRep_{}
    {
    }
    inline Node::Type Node::Ptr::GetNodeType () const
    {
        RequireNotNull (fRep_);
        return fRep_->GetNodeType ();
    }
    inline optional<URI> Node::Ptr::GetNamespace () const
    {
        RequireNotNull (fRep_);
        return fRep_->GetNamespace ();
    }
    inline String Node::Ptr::GetName () const
    {
        RequireNotNull (fRep_);
        return fRep_->GetName ();
    }
    inline void Node::Ptr::SetName (const String& name)
    {
        RequireNotNull (fRep_);
        fRep_->SetName (name);
    }
    inline String Node::Ptr::GetValue () const
    {
        RequireNotNull (fRep_);
        return fRep_->GetValue ();
    }
    inline void Node::Ptr::SetValue (const String& v)
    {
        RequireNotNull (fRep_);
        fRep_->SetValue (v);
    }
    inline void Node::Ptr::SetAttribute (const String& attrName, const String& v)
    {
        RequireNotNull (fRep_);
        Require (GetNodeType () == Node::eElementNT);
        fRep_->SetAttribute (attrName, v);
    }
    inline bool Node::Ptr::HasAttribute (const String& attrName) const
    {
        RequireNotNull (fRep_);
        Require (GetNodeType () == Node::eElementNT);
        return fRep_->GetAttribute (attrName) != nullopt;
    }
    inline bool Node::Ptr::HasAttribute (const String& attrName, const String& value) const
    {
        RequireNotNull (fRep_);
        Require (GetNodeType () == Node::eElementNT);
        if (auto o = fRep_->GetAttribute (attrName)) {
            return *o == value;
        }
        return false;
    }
    inline optional<String> Node::Ptr::GetAttribute (const String& attrName) const
    {
        RequireNotNull (fRep_);
        Require (GetNodeType () == Node::eElementNT);
        return fRep_->GetAttribute (attrName);
    }
    inline Node::Ptr Node::Ptr::GetFirstAncestorNodeWithAttribute (const String& attrName) const
    {
        RequireNotNull (fRep_);
        for (Node::Ptr p = *this; p != nullptr; p = p.GetParentNode ()) {
            if (p.GetNodeType () == Type::eElementNT) {
                if (p.HasAttribute (attrName)) {
                    return p;
                }
            }
            return nullptr;
        }
    }
    inline Node::Ptr Node::Ptr::InsertElement (const String& name, const Node::Ptr& afterNode)
    {
        RequireNotNull (fRep_);
        Require (GetNodeType () == Node::eElementNT);
        return fRep_->InsertElement (name, nullopt, afterNode);
    }
    inline Node::Ptr Node::Ptr::InsertElement (const String& name, const optional<URI>& ns, const Node::Ptr& afterNode)
    {
        RequireNotNull (fRep_);
        Require (GetNodeType () == Node::eElementNT);
        return fRep_->InsertElement (name, ns, afterNode);
    }
    inline Node::Ptr Node::Ptr::AppendElement (const String& name, const optional<URI>& ns)
    {
        RequireNotNull (fRep_);
        Require (GetNodeType () == Node::eElementNT);
        return fRep_->AppendElement (name, ns);
    }
    inline Node::Ptr Node::Ptr::AppendElement (const String& name, const optional<URI>& ns, const String& v)
    {
        Require (GetNodeType () == Node::eElementNT);
        auto r = AppendElement (name, ns);
        r.SetValue (v);
        return r;
    }
    inline void Node::Ptr::AppendElementIfNotEmpty (const String& name, const optional<URI>& ns, const optional<String>& v)
    {
        RequireNotNull (fRep_);
        Require (GetNodeType () == Node::eElementNT);
        if (v != nullopt and not v->empty ()) {
            fRep_->AppendElement (name, ns).SetValue (*v);
        }
    }
    inline void Node::Ptr::DeleteNode ()
    {
        RequireNotNull (fRep_);
        fRep_->DeleteNode ();
        fRep_ = nullptr;
    }
    inline Node::Ptr Node::Ptr::ReplaceNode ()
    {
        AssertNotNull (fRep_);
        return fRep_->ReplaceNode ();
    }
    inline Node::Ptr Node::Ptr::GetParentNode () const
    {
        AssertNotNull (fRep_);
        return fRep_->GetParentNode ();
    }
    inline Iterable<Node::Ptr> Node::Ptr::GetChildren () const
    {
        AssertNotNull (fRep_);
        return fRep_->GetChildren ();
    }
    inline Node::Ptr Node::Ptr::GetChildNodeByID (const String& id) const
    {
        RequireNotNull (fRep_);
        Require (GetNodeType () == Node::eElementNT);
        return fRep_->GetChildNodeByID (id);
    }
    inline shared_ptr<Node::IRep> Node::Ptr::GetRep () const
    {
        return fRep_;
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
    inline Node::Ptr Document::Ptr::GetRootElement () const
    {
        RequireNotNull (fRep_);
        // Should only be one in an XML document.
        for (Node::Ptr ni : fRep_->GetChildren ()) {
            if (ni.GetNodeType () == Node::eElementNT) {
                return ni;
            }
        }
        return nullptr;
    }

}
#endif

#endif
