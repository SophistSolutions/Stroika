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

    using Execution::ThrowIfNull;

    class Node::Rep {
    public:
        Rep ()          = default;
        virtual ~Rep () = default;

    public:
        virtual NodeType        GetNodeType () const                                                                       = 0;
        virtual String          GetNamespace () const                                                                      = 0;
        virtual String          GetName () const                                                                           = 0;
        virtual void            SetName (const String& name)                                                               = 0;
        virtual VariantValue    GetValue () const                                                                          = 0;
        virtual void            SetValue (const VariantValue& v)                                                           = 0;
        virtual void            SetAttribute (const String& attrName, const String& v)                                     = 0;
        virtual bool            HasAttribute (const String& attrName, const String* value) const                           = 0;
        virtual String          GetAttribute (const String& attrName) const                                                = 0;
        virtual Node            GetFirstAncestorNodeWithAttribute (const String& attrName) const                           = 0;
        virtual Node            InsertChild (const String& name, const String* ns, Node afterNode)                         = 0;
        virtual Node            AppendChild (const String& name)                                                           = 0;
        virtual void            AppendChild (const String& name, const String* ns, const VariantValue& v)                  = 0;
        virtual void            AppendChildIfNotEmpty (const String& name, const String* ns, const VariantValue& v)        = 0;
        virtual Node            InsertNode (const Node& n, const Node& afterNode, bool inheritNamespaceFromInsertionPoint) = 0;
        virtual Node            AppendNode (const Node& n, bool inheritNamespaceFromInsertionPoint)                        = 0;
        virtual void            DeleteNode ()                                                                              = 0;
        virtual Node            ReplaceNode ()                                                                             = 0;
        virtual Node            GetParentNode () const                                                                     = 0;
        virtual SubNodeIterator GetChildren () const                                                                       = 0;
        virtual Node            GetChildNodeByID (const String& id) const                                                  = 0;
        virtual void*           GetInternalTRep ()                                                                         = 0;

    protected:
        inline static shared_ptr<Rep> GetRep4Node (Node n)
        {
            return n.fRep;
        } // propagate access to subclasses
    };

    class SubNodeIterator::Rep {
    public:
        Rep ()          = default;
        virtual ~Rep () = default;

    public:
        virtual bool   IsAtEnd () const   = 0;
        virtual void   Next ()            = 0;
        virtual Node   Current () const   = 0;
        virtual size_t GetLength () const = 0;
    };

    /*
     ********************************************************************************
     *********************************** XML::DOM::Node *****************************
     ********************************************************************************
     */
    inline Node::Node (const shared_ptr<Rep>& from)
        : fRep{from}
    {
    }
    inline bool Node::IsNull () const
    {
        return fRep.get () == nullptr;
    }
    inline Node::NodeType Node::GetNodeType () const
    {
        ThrowIfNull (fRep);
        return fRep->GetNodeType ();
    }
    inline String Node::GetNamespace () const
    {
        ThrowIfNull (fRep);
        return fRep->GetNamespace ();
    }
    inline String Node::GetName () const
    {
        ThrowIfNull (fRep);
        return fRep->GetName ();
    }
    inline void Node::SetName (const String& name)
    {
        ThrowIfNull (fRep);
        fRep->SetName (name);
    }
    inline VariantValue Node::GetValue () const
    {
        ThrowIfNull (fRep);
        return fRep->GetValue ();
    }
    inline void Node::SetValue (const VariantValue& v)
    {
        ThrowIfNull (fRep);
        fRep->SetValue (v);
    }
    inline void Node::SetAttribute (const String& attrName, const String& v)
    {
        ThrowIfNull (fRep);
        fRep->SetAttribute (attrName, v);
    }
    inline bool Node::HasAttribute (const String& attrName) const
    {
        ThrowIfNull (fRep);
        return fRep->HasAttribute (attrName, nullptr);
    }
    inline bool Node::HasAttribute (const String& attrName, const String& value) const
    {
        ThrowIfNull (fRep);
        return fRep->HasAttribute (attrName, &value);
    }
    inline String Node::GetAttribute (const String& attrName) const
    {
        ThrowIfNull (fRep);
        return fRep->GetAttribute (attrName);
    }
    inline Node Node::GetFirstAncestorNodeWithAttribute (const String& attrName) const
    {
        ThrowIfNull (fRep);
        return fRep->GetFirstAncestorNodeWithAttribute (attrName);
    }
    inline Node Node::InsertChild (const String& name, Node afterNode)
    {
        ThrowIfNull (fRep);
        return fRep->InsertChild (name, nullptr, afterNode);
    }
    inline Node Node::InsertChild (const String& name, const String& ns, Node afterNode)
    {
        ThrowIfNull (fRep);
        return fRep->InsertChild (name, &ns, afterNode);
    }
    inline Node Node::AppendChild (const String& name)
    {
        ThrowIfNull (fRep);
        return fRep->AppendChild (name);
    }
    inline void Node::AppendChild (const String& name, const VariantValue& v)
    {
        ThrowIfNull (fRep);
        fRep->AppendChild (name, nullptr, v);
    }
    inline void Node::AppendChild (const String& name, const String& ns, const VariantValue& v)
    {
        ThrowIfNull (fRep);
        fRep->AppendChild (name, &ns, v);
    }
    inline void Node::AppendChildIfNotEmpty (const String& name, const VariantValue& v)
    {
        ThrowIfNull (fRep);
        fRep->AppendChildIfNotEmpty (name, nullptr, v);
    }
    inline void Node::AppendChildIfNotEmpty (const String& name, const String& ns, const VariantValue& v)
    {
        ThrowIfNull (fRep);
        fRep->AppendChildIfNotEmpty (name, &ns, v);
    }
    inline Node Node::InsertNode (const Node& n, const Node& afterNode, bool inheritNamespaceFromInsertionPoint)
    {
        ThrowIfNull (fRep);
        return fRep->InsertNode (n, afterNode, inheritNamespaceFromInsertionPoint);
    }
    inline Node Node::AppendNode (const Node& n, bool inheritNamespaceFromInsertionPoint)
    {
        ThrowIfNull (fRep);
        return fRep->AppendNode (n, inheritNamespaceFromInsertionPoint);
    }
    inline void Node::DeleteNode ()
    {
        ThrowIfNull (fRep);
        fRep->DeleteNode ();
    }
    inline Node Node::ReplaceNode ()
    {
        ThrowIfNull (fRep);
        return fRep->ReplaceNode ();
    }
    inline Node Node::GetParentNode () const
    {
        ThrowIfNull (fRep);
        return fRep->GetParentNode ();
    }
    inline SubNodeIterator Node::GetChildren () const
    {
        ThrowIfNull (fRep);
        return fRep->GetChildren ();
    }
    inline Node Node::GetChildNodeByID (const String& id) const
    {
        return fRep->GetChildNodeByID (id);
    }

    /*
     ********************************************************************************
     **************************** XML::DOM::SubNodeIterator *************************
     ********************************************************************************
     */
    inline SubNodeIterator::SubNodeIterator (const shared_ptr<Rep>& from)
        : fRep{from}
    {
    }
    inline bool SubNodeIterator::NotDone () const
    {
        return not fRep->IsAtEnd ();
    }
    inline bool SubNodeIterator::IsAtEnd () const
    {
        return fRep->IsAtEnd ();
    }
    inline void SubNodeIterator::Next ()
    {
        fRep->Next ();
    }
    inline Node SubNodeIterator::Current () const
    {
        return fRep->Current ();
    }
    inline size_t SubNodeIterator::GetLength () const
    {
        return fRep->GetLength ();
    }

}
#endif

#endif
