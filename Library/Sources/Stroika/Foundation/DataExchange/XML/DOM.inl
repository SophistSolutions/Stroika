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
        fRep_->SetAttribute (attrName, v);
    }
    inline bool Node::Ptr::HasAttribute (const String& attrName) const
    {
        RequireNotNull (fRep_);
        return fRep_->HasAttribute (attrName, nullptr);
    }
    inline bool Node::Ptr::HasAttribute (const String& attrName, const String& value) const
    {
        RequireNotNull (fRep_);
        return fRep_->HasAttribute (attrName, &value);
    }
    inline optional<String> Node::Ptr::GetAttribute (const String& attrName) const
    {
        RequireNotNull (fRep_);
        return fRep_->GetAttribute (attrName);
    }
    inline Node::Ptr Node::Ptr::GetFirstAncestorNodeWithAttribute (const String& attrName) const
    {
        RequireNotNull (fRep_);
        return fRep_->GetFirstAncestorNodeWithAttribute (attrName);
    }
    inline Node::Ptr Node::Ptr::InsertChild (const String& name, const Node::Ptr& afterNode)
    {
        RequireNotNull (fRep_);
        return fRep_->InsertChild (name, nullopt, afterNode);
    }
    inline Node::Ptr Node::Ptr::InsertChild (const String& name, const optional<URI>& ns, const Node::Ptr& afterNode)
    {
        RequireNotNull (fRep_);
        return fRep_->InsertChild (name, ns, afterNode);
    }
    inline Node::Ptr Node::Ptr::AppendChild (const String& name, const optional<URI>& ns)
    {
        RequireNotNull (fRep_);
        return fRep_->AppendChild (name, ns);
    }
    inline void Node::Ptr::AppendChild (const String& name, const optional<URI>& ns, const String& v)
    {
        AppendChild (name, ns).SetValue (v);
    }
    inline void Node::Ptr::AppendChildIfNotEmpty (const String& name, const optional<URI>& ns, const String& v)
    {
        RequireNotNull (fRep_);
        if (not v.empty ()) {
            fRep_->AppendChild (name, ns).SetValue (v);
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
        return fRep_->GetChildNodeByID (id);
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
    inline void Document::Ptr::Write (const Streams::OutputStream<byte>::Ptr& to, const SerializationOptions& options) const
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
    inline void Document::Ptr::Validate (const Schema::Ptr& schema) const
    {
        RequireNotNull (schema);
        RequireNotNull (fRep_);
        fRep_->Validate (schema);
    }

}
#endif

#endif
