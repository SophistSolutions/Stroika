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
        : fName{name}
        , fNamespace{ns}
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
    inline void Node::Ptr::DeleteNode ()
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
    inline Iterable<Node::Ptr> Node::Ptr::GetChildren () const
    {
        AssertNotNull (fRep_);
        return fRep_->GetChildren ();
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
        Element::Ptr newNode{nullptr};
        if (parent == nullptr) {
            auto n = this->GetName ();
            this->DeleteNode ();
            // technically 'Append' is not right and we really should grab the 'next sibling' and insert before it...but that seems unlikely to matter --LGP 2024-01-06
            newNode = parent.Append (n);
        }
        else {
            newNode = parent.Insert (this->GetName (), *this); // @todo clone NS as well!!!
            this->DeleteNode ();
        }
        Ensure (newNode.GetParent () == parent);
        *this = newNode;
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
