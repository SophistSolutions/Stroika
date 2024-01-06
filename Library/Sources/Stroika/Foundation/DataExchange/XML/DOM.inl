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
     **************** XML::DOM::Node::NameWithNamespace *****************************
     ********************************************************************************
     */
    template <Characters::IConvertibleToString NAME_TYPE>
    inline Node::NameWithNamespace::NameWithNamespace (NAME_TYPE&& name)
        : fName{name}
    {
    }
    inline Node::NameWithNamespace::NameWithNamespace (const optional<URI>& ns, const String& name)
        : fNamespace{ns}
        , fName{name}
    {
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
        fRep_->SetName (nullopt, name);
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
    inline void Node::Ptr::SetAttribute (const String& attrName, const optional<String>& v)
    {
        RequireNotNull (fRep_);
        Require (GetNodeType () == Node::eElementNT);
        fRep_->SetAttribute (nullopt, attrName, v);
    }
    inline bool Node::Ptr::HasAttribute (const String& attrName) const
    {
        RequireNotNull (fRep_);
        Require (GetNodeType () == Node::eElementNT);
        return fRep_->GetAttribute (nullopt, attrName) != nullopt;
    }
    inline bool Node::Ptr::HasAttribute (const String& attrName, const String& value) const
    {
        RequireNotNull (fRep_);
        Require (GetNodeType () == Node::eElementNT);
        if (auto o = fRep_->GetAttribute (nullopt, attrName)) {
            return *o == value;
        }
        return false;
    }
    inline optional<String> Node::Ptr::GetAttribute (const String& attrName) const
    {
        RequireNotNull (fRep_);
        Require (GetNodeType () == Node::eElementNT);
        return fRep_->GetAttribute (nullopt, attrName);
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
        return InsertElement (nullopt, name, afterNode);
    }
    inline Node::Ptr Node::Ptr::InsertElement (const optional<URI>& ns, const String& name, const Node::Ptr& afterNode)
    {
        RequireNotNull (fRep_);
        Require (GetNodeType () == Node::eElementNT);
        return fRep_->InsertElement (ns, name, afterNode);
    }
    inline Node::Ptr Node::Ptr::AppendElement (const String& name)
    {
        return AppendElement (nullopt, name);
    }
    inline Node::Ptr Node::Ptr::AppendElement (const optional<URI>& ns, const String& name)
    {
        RequireNotNull (fRep_);
        Require (GetNodeType () == Node::eElementNT);
        return fRep_->AppendElement (ns, name);
    }
    inline Node::Ptr Node::Ptr::AppendElement (const optional<URI>& ns, const String& name, const String& v)
    {
        auto r = AppendElement (ns, name);
        r.SetValue (v);
        return r;
    }
    inline void Node::Ptr::AppendElementIfNotEmpty (const String& name, const optional<String>& v)
    {
        AppendElementIfNotEmpty (nullopt, name, v);
    }
    inline void Node::Ptr::AppendElementIfNotEmpty (const optional<URI>& ns, const String& name, const optional<String>& v)
    {
        RequireNotNull (fRep_);
        Require (GetNodeType () == Node::eElementNT);
        if (v != nullopt and not v->empty ()) {
            fRep_->AppendElement (ns, name).SetValue (*v);
        }
    }
    inline void Node::Ptr::DeleteNode ()
    {
        RequireNotNull (fRep_);
        fRep_->DeleteNode ();
        fRep_ = nullptr;
    }
    inline Node::Ptr Node::Ptr::ReplaceElement ()
    {
        Require (GetNodeType () == Node::eElementNT);
        AssertNotNull (fRep_);
        /**
         *  Disallow replacing the root element - create a new document instead.
         * 
         *  Basic algorithm:
         *      o   Save parent node
         *      o   Add new node just after this one (with same name/namespace)
         *      o   Delete this node (one just 'replaced')
         * 
         *  Note - to avoid issues with temporarily having two nodes of the same name in list of children - may need to modify this logic?
         */
        Node::Ptr parent = this->GetParentNode ();
        Require (parent != nullptr);                                        // create new Document for this case
        Node::Ptr newNode = parent.InsertElement (this->GetName (), *this); // @todo clone NS as well!!!
        this->DeleteNode ();
        Ensure (newNode.GetParentNode () == parent);
        return newNode;
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
    inline Node::Ptr Node::Ptr::GetChildElementByID (const String& id) const
    {
        RequireNotNull (fRep_);
        Require (GetNodeType () == Node::eElementNT);
        auto result = fRep_->GetChildElementByID (id);
        Ensure (result == nullptr or result.GetNodeType () == Node::eElementNT);
        return result;
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
    inline Element::Ptr::Ptr (const Node::Ptr& p)
        : Node::Ptr{p != nullptr and p.GetNodeType () == Node::eElementNT ? p : nullptr}
    {
    }
    inline auto Element::Ptr::GetName () const -> NameWithNamespace
    {
        Require (GetNodeType () == eElementNT); // cheaters never prosper
        return NameWithNamespace{GetRep ()->GetNamespace (), GetRep ()->GetName ()};
    }
    inline void Element::Ptr::SetName (const NameWithNamespace& name)
    {
        Require (GetNodeType () == eElementNT); // cheaters never prosper
        GetRep ()->SetName (name.fNamespace, name.fName);
    }
    inline String Element::Ptr::GetValue () const
    {
        Require (GetNodeType () == eElementNT); // cheaters never prosper
        return GetRep ()->GetValue ();
    }
    inline void Element::Ptr::SetValue (const String& v)
    {
        Require (GetNodeType () == eElementNT); // cheaters never prosper
        return GetRep ()->SetValue (v);
    }
    inline optional<String> Element::Ptr::GetAttribute (const NameWithNamespace& attrName) const
    {
        Require (GetNodeType () == eElementNT); // cheaters never prosper
        return GetRep ()->GetAttribute (attrName.fNamespace, attrName.fName);
    }
    inline bool Element::Ptr::HasAttribute (const NameWithNamespace& attrName) const
    {
        Require (GetNodeType () == eElementNT); // cheaters never prosper
        return GetRep ()->GetAttribute (attrName.fNamespace, attrName.fName) != nullopt;
    }
    inline bool Element::Ptr::HasAttribute (const NameWithNamespace& attrName, const String& value) const
    {
        Require (GetNodeType () == eElementNT); // cheaters never prosper
        if (auto o = GetRep ()->GetAttribute (attrName.fNamespace, attrName.fName)) {
            return *o == value;
        }
        return false;
    }
    inline void Element::Ptr::SetAttribute (const NameWithNamespace& attrName, const optional<String>& v)
    {
        Require (GetNodeType () == eElementNT); // cheaters never prosper
        GetRep ()->SetAttribute (attrName.fNamespace, attrName.fName, v);
    }
    inline auto Element::Ptr::Insert (const NameWithNamespace& eltName, const Node::Ptr& afterNode) -> Ptr
    {
        Require (GetNodeType () == eElementNT); // cheaters never prosper
        return GetRep ()->InsertElement (eltName.fNamespace, eltName.fName, afterNode);
    }
    inline Element::Ptr Element::Ptr::Append (const NameWithNamespace& eltName)
    {
        Require (GetNodeType () == eElementNT); // cheaters never prosper
        return GetRep ()->AppendElement (eltName.fNamespace, eltName.fName);
    }
    inline Element::Ptr Element::Ptr::Append (const NameWithNamespace& eltName, const String& v)
    {
        Require (GetNodeType () == eElementNT); // cheaters never prosper
        auto r = Append (eltName);
        r.SetValue (v);
        return r;
    }
    inline Element::Ptr Element::Ptr::AppendIfNotEmpty (const NameWithNamespace& eltName, const optional<String>& v)
    {
        Require (GetNodeType () == eElementNT); // cheaters never prosper
        if (v) {
            auto r = Append (eltName);
            r.SetValue (*v);
            return r;
        }
        return Element::Ptr{nullptr};
    }
    inline Element::Ptr Element::Ptr::ReplaceElement ()
    {
        Require (GetNodeType () == eElementNT); // cheaters never prosper
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
        Require (parent != nullptr);                                    // create new Document for this case
        Element::Ptr newNode = parent.Insert (this->GetName (), *this); // @todo clone NS as well!!!
        this->DeleteNode ();
        Ensure (newNode.GetParent () == parent);
        return newNode;
    }
    inline Element::Ptr Element::Ptr::GetParent () const
    {
        Require (GetNodeType () == eElementNT); // cheaters never prosper
        return Element::Ptr{GetRep ()->GetParentNode ()};
    }
    inline auto Element::Ptr::GetChildren () const -> Iterable<Ptr>
    {
        // return just the child elements - so simple filter
        return this->Node::Ptr::GetChildren ().Map<Iterable<Ptr>> ([] (Node::Ptr p) -> optional<Ptr> { return Ptr{p}; });
    }
    inline auto Element::Ptr::GetChildByID (const String& id) const -> Ptr
    {
        Require (GetNodeType () == eElementNT); // cheaters never prosper
        return Element::Ptr{GetRep ()->GetChildElementByID (id)};
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
