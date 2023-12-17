/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_DOM_h_
#define _Stroika_Foundation_DataExchange_XML_DOM_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Execution/Exceptions.h"
#include "../../Execution/ProgressMonitor.h"
#include "../../Streams/InputStream.h"

#include "../BadFormatException.h"
#include "../VariantValue.h"

#include "Namespace.h"

#if qStroika_Foundation_DataExchange_XML_SupportSchema
namespace Stroika::Foundation::DataExchange::XML::Schema {
    class Ptr;
}
#endif

#if qStroika_Foundation_DataExchange_XML_SupportDOM
namespace Stroika::Foundation::DataExchange::XML::DOM {

    using DataExchange::BadFormatException;
    using Traversal::Iterable;

    /**
     * NB: A Node can be EITHER an ELEMENT or an ATTRIBUTE
     */

    // @todo name Node namespace with IRep and Ptr classes
    class Node {
    public:
        class Rep;
        Node () = delete; // @todo LOSE NULL NODE PTR OBJECTS  (two apis - change to return optional)
        Node (const shared_ptr<Rep>& from);

    public:
        //   nonvirtual bool IsNull () const;

    public:
        enum NodeType {
            eAttributeNT,
            eElementNT,
            eTextNT,
            eCommentNT,
            eOtherNT
        };
        nonvirtual NodeType GetNodeType () const;

    public:
        nonvirtual String GetNamespace () const;

    public:
        nonvirtual String GetName () const;

    public:
        nonvirtual void SetName (const String& name);

    public:
        nonvirtual VariantValue GetValue () const;

    public:
        /**
         *  Note a nullptr value is rendered as an empty DOM node, but then read back (GetValue) as an empty string.
         *      @todo CONSIDER IF THIS MAKES SENSE
         */
        nonvirtual void SetValue (const VariantValue& v);

    public:
        nonvirtual void SetAttribute (const String& attrName, const String& v);

    public:
        // return true iff attribute exists on this node
        nonvirtual bool HasAttribute (const String& attrName) const;
        // return true iff attribute exists on this node and equals value
        nonvirtual bool HasAttribute (const String& attrName, const String& value) const;
        // returns string value of attribute, and "" if doesn't exist (or empty - so not distinguishable this way)
    public:
        nonvirtual String GetAttribute (const String& attrName) const;

    public:
        nonvirtual optional<Node> GetFirstAncestorNodeWithAttribute (const String& attrName) const;
        // if afterNode is nullptr - then this is PREPEND
    public:
        nonvirtual Node InsertChild (const String& name, optional<Node> afterNode);
        nonvirtual Node InsertChild (const String& name, const String& ns, optional<Node> afterNode);

    public:
        nonvirtual Node AppendChild (const String& name);
        nonvirtual void AppendChild (const String& name, const VariantValue& v);
        nonvirtual void AppendChild (const String& name, const String& ns, const VariantValue& v);

    public:
        // Node 'n' COULD come from other XMLDoc - so be careful how its inserted
        // NB: NODE CLONED/COPIED - NOT INSERTED BY REFERENCE!
        // if afterNode == nullptr, then PREPEND!
        nonvirtual Node InsertNode (const Node& n, const optional<Node>& afterNode, bool inheritNamespaceFromInsertionPoint = true);
        // Node 'n' COULD come from other XMLDoc - so be careful how its appended
        // NB: NODE CLONED/COPIED - NOT INSERTED BY REFERENCE!
    public:
        nonvirtual Node AppendNode (const Node& n, bool inheritNamespaceFromInsertionPoint = true);

    public:
        nonvirtual void AppendChildIfNotEmpty (const String& name, const VariantValue& v);
        nonvirtual void AppendChildIfNotEmpty (const String& name, const String& ns, const VariantValue& v);

    public:
        nonvirtual void DeleteNode ();

    public:
        nonvirtual Node ReplaceNode (); // creates a new (empty) node with same name (and namespace) as orig
    public:
        nonvirtual optional<Node> GetParentNode () const;

    public:
        /**
         */
        nonvirtual Iterable<Node> GetChildren () const;

    public:
        // can return a NULL Node. Only examines this node's children
        nonvirtual optional<Node> GetChildNodeByID (const String& id) const;

    protected:
        shared_ptr<Rep> fRep;

    private:
        friend class Rep;
    };

    /**
    *   !@todo better docs/clearer design - but I think Document is immutable in current design? Else not sure what we have RWDocument).
    * 
    *   Also - consider IRep, and Ptr style names.
     */
    class Document {
    public:
        class Rep;

    public:
        Document ();
        Document (const Schema::Ptr& schema);
        ~Document () = default;

    protected:
        Document (const shared_ptr<Rep>& rep);

        // No copying. (maybe create a RWDocument for that - but doesnt appear needed externally)
    private:
        Document (const Document& from)           = delete;
        Document& operator= (const Document& rhs) = delete;

    public:
        nonvirtual Schema::Ptr GetSchema () const;

        // IO routines - Serialize the document DOM
    public:
        nonvirtual void WritePrettyPrinted (ostream& out) const;
        nonvirtual void WriteAsIs (ostream& out) const;

    public:
        nonvirtual void Validate () const; // throws BadFormatException exception on failure; uses current associated schema
                                           // nonvirtual void Validate (const Schema* schema) const; // ''; but uses PROVIDED schema

    public:
        /**
         */
        nonvirtual Iterable<Node> GetChildren () const;

    public:
        /**
         * \brief always returns Node of eElement type, or throws on failure
         */
        nonvirtual Node GetRootElement () const;

    public:
        nonvirtual shared_ptr<Rep> GetRep () const;

    protected:
        shared_ptr<Rep> fRep;
    };

    /**
     *  RWDocument is a modifiable DOM document.
     * 
     *      @todo LOSE THIS CONCEPT - Document is modifyable - even if no APIs for it, cuz you can modify the NODES.
     */
    class RWDocument : public Document {
    public:
        RWDocument ();
        RWDocument (const Schema::Ptr& schema);
        RWDocument (const Document& from);
        RWDocument (const RWDocument& from);
        RWDocument& operator= (const Document& rhs);

    public:
        nonvirtual Node CreateDocumentElement (const String& name);

    public:
        nonvirtual void SetRootElement (const Node& newRoot);

    public:
        nonvirtual void Read (Streams::InputStream<byte>::Ptr in, Execution::ProgressMonitor::Updater progressCallback = nullptr);

    public:
        nonvirtual void LoadXML (const String& xml); // 'xml' contains data to be parsed and to replace the current XML document
    };

};
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "DOM.inl"

#endif /*_Stroika_Foundation_DataExchange_XML_DOM_h_*/
