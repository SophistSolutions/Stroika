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
namespace Stroika::Foundation::DataExchange::XML {
    class Schema;
}
#endif

#if qStroika_Foundation_DataExchange_XML_SupportDOM
namespace Stroika::Foundation::DataExchange::XML::DOM {

    using DataExchange::BadFormatException;

    class SubNodeIterator;

    /**
     * NB: A Node can be EITHER an ELEMENT or an ATTRIBUTE
     */
    class Node {
    public:
        class Rep;
        Node () = default;
        Node (const shared_ptr<Rep>& from);

    public:
        nonvirtual bool IsNull () const;

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
        nonvirtual String       GetNamespace () const;
        nonvirtual String       GetName () const;
        nonvirtual void         SetName (const String& name);
        nonvirtual VariantValue GetValue () const;
        nonvirtual void         SetValue (const VariantValue& v);
        nonvirtual void         SetAttribute (const String& attrName, const String& v);
        // return true iff attribute exists on this node
        nonvirtual bool HasAttribute (const String& attrName) const;
        // return true iff attribute exists on this node and equals value
        nonvirtual bool HasAttribute (const String& attrName, const String& value) const;
        // returns string value of attribute, and "" if doesn't exist (or empty - so not distinguishable this way)
        nonvirtual String GetAttribute (const String& attrName) const;
        nonvirtual Node   GetFirstAncestorNodeWithAttribute (const String& attrName) const;
        // if afterNode is nullptr - then this is PREPEND
        nonvirtual Node InsertChild (const String& name, Node afterNode);
        nonvirtual Node InsertChild (const String& name, const String& ns, Node afterNode);
        nonvirtual Node AppendChild (const String& name);
        nonvirtual void AppendChild (const String& name, const VariantValue& v);
        nonvirtual void AppendChild (const String& name, const String& ns, const VariantValue& v);
        // Node 'n' COULD come from other XMLDoc - so be careful how its inserted
        // NB: NODE CLONED/COPIED - NOT INSERTED BY REFERENCE!
        // if afterNode == nullptr, then PREPEND!
        nonvirtual Node InsertNode (const Node& n, const Node& afterNode, bool inheritNamespaceFromInsertionPoint = true);
        // Node 'n' COULD come from other XMLDoc - so be careful how its appended
        // NB: NODE CLONED/COPIED - NOT INSERTED BY REFERENCE!
        nonvirtual Node AppendNode (const Node& n, bool inheritNamespaceFromInsertionPoint = true);
        nonvirtual void AppendChildIfNotEmpty (const String& name, const VariantValue& v);
        nonvirtual void AppendChildIfNotEmpty (const String& name, const String& ns, const VariantValue& v);
        nonvirtual void DeleteNode ();
        nonvirtual Node ReplaceNode (); // creates a new (empty) node with same name (and namespace) as orig
        nonvirtual Node GetParentNode () const;

    public:
        nonvirtual SubNodeIterator GetChildren () const;

    public:
        // can return a NULL Node. Only examines this node's children
        nonvirtual Node GetChildNodeByID (const String& id) const;

    protected:
        shared_ptr<Rep> fRep;

    private:
        friend class Rep;
    };

    /*
     *  Short lifetime. Don't save these iterator objects. Just use them to enumerate a collection and then let them
     *  go. They (could) become invalid after a call to update the database.
     */
    class SubNodeIterator {
    public:
        class Rep;
        explicit SubNodeIterator (const shared_ptr<Rep>& from);

    public:
        nonvirtual bool   NotDone () const;
        nonvirtual bool   IsAtEnd () const;
        nonvirtual void   Next ();
        nonvirtual Node   Current () const;
        nonvirtual size_t GetLength () const;

    public:
        nonvirtual void operator++ ()
        {
            Next ();
        }
        nonvirtual void operator++ (int)
        {
            Next ();
        }
        nonvirtual Node operator* () const
        {
            return Current ();
        }

    protected:
        shared_ptr<Rep> fRep;
    };

    class RecordNotFoundException : public Execution::RuntimeErrorException<> {
    public:
        RecordNotFoundException ();
    };

    class Document {
    public:
        class Rep;

    public:
        Document (const Schema* schema = nullptr);
        ~Document ();

    protected:
        Document (Rep* rep);

        // No copying. (maybe create a RWDocument for that - but doesnt appear needed externally)
    private:
        Document (const Document& from)           = delete;
        Document& operator= (const Document& rhs) = delete;

    public:
        // this call returns the object you can use with lock_gaurd<>
        // The Document is already largely threadsafe, but if you want to do logically coordianted changes, you can
        // use this locker object to assure consistency
        nonvirtual recursive_mutex& GetLock () const;

    public:
        nonvirtual const Schema* GetSchema () const;
        nonvirtual void          SetSchema (const Schema* schema);

        // IO routines - Serialize the document DOM
    public:
        nonvirtual void WritePrettyPrinted (ostream& out) const;
        nonvirtual void WriteAsIs (ostream& out) const;

    public:
        nonvirtual void Validate () const; // throws BadFormatException exception on failure; uses current associated schema
        nonvirtual void Validate (const Schema* schema) const; // ''; but uses PROVIDED schema

    public:
        nonvirtual SubNodeIterator GetChildren () const;
        nonvirtual Node            GetRootElement () const;

    public:
        nonvirtual shared_ptr<Rep> GetRep () const;

    protected:
        shared_ptr<Rep> fRep;
    };

    class RWDocument : public Document {
    public:
        RWDocument (const Schema* schema = nullptr);
        RWDocument (const Document& from);
        RWDocument (const RWDocument& from);
        RWDocument& operator= (const Document& rhs);

    public:
        nonvirtual Node CreateDocumentElement (const String& name);

    public:
        nonvirtual void SetRootElement (const Node& newRoot);

    public:
        nonvirtual void Read (Streams::InputStream<byte>::Ptr& in, bool encrypted, Execution::ProgressMonitor::Updater progressCallback);
        /*
         * ReadAllowingInvalidSrc () does a normal Read () - but if there is a validation exception, instead of throwing,
         * it simply returns the result of that validation exception
         * in the exceptionResult argument (it must be non-null). Note - OTHER exceptions (such as out of memory etc) are still
         * thrown as normal. The point of this function is to accomodate MOSTLY valid & correct sources of HRs, but which
         * may not be fully schema compliant. Shockingly, many medical record sources produce data that doesn't XSD validate!
         *      (See http://bugzilla/show_bug.cgi?id=513)
         */
        nonvirtual void ReadAllowingInvalidSrc (Streams::InputStream<byte>::Ptr& in, bool encrypted, shared_ptr<BadFormatException>* exceptionResult,
                                                Execution::ProgressMonitor::Updater progressCallback = nullptr);

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
