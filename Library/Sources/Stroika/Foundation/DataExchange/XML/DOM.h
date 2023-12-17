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
     * NB: A Node can be EITHER an ELEMENT or an ATTRIBUTE (mostly). Nodes are 'internal' to a Document, and are always somehow contained in some document).
     *  Nodes have an abstract IRep, the the Ptr object is just a 'smart pointer' to that IRep.
     * 
     *  Note - the IRep can be implemented by a variety of backend libraries (dubbed 'Providers').
     * 
     *  Principally, users will interact with a Document::Ptr (initialized from Document::New()), and only rarely deal with Nodes.
     *  But Nodes can be accessed, created, update etc, once you start with a Document::Ptr.
     */
    namespace Node {
        class IRep;

        /**
         *  Prior to Stroika v3.0d5, this was Node::NodeType
         */
        enum Type {
            eAttributeNT,
            eElementNT,
            eTextNT,
            eCommentNT,
            eOtherNT
        };

        /**
         *  \brief Node::Ptr is a (never null) smart pointer to a Node::IRep
         * 
         *  \note Before Stroika v3.0d5 this was simply called "Node", and now Node::Ptr
         * 
         *   Note - Nodes are not created directly, but either via Node::Ptr methods, or Document::Ptr methods (because nodes are always associated with some document).
         */
        class Ptr {
        public:
            /**
             *  \req not nullptr
             */
            Ptr (const shared_ptr<IRep>& from);

        public:
            /**
             */
            nonvirtual Type GetNodeType () const;

        public:
            /**
             */
            nonvirtual String GetNamespace () const;

        public:
            /**
             */
            nonvirtual String GetName () const;

        public:
            /**
             */
            nonvirtual void SetName (const String& name);

        public:
            /**
             */
            nonvirtual VariantValue GetValue () const;

        public:
            /**
             *  Note a nullptr value is rendered as an empty DOM node, but then read back (GetValue) as an empty string.
             *      @todo CONSIDER IF THIS MAKES SENSE
             */
            nonvirtual void SetValue (const VariantValue& v);

        public:
            /**
             */
            nonvirtual void SetAttribute (const String& attrName, const String& v);

        public:
            /**
            // return true iff attribute exists on this node
            // return true iff attribute exists on this node and equals (case sensative) value
             */
            nonvirtual bool HasAttribute (const String& attrName) const;
            nonvirtual bool HasAttribute (const String& attrName, const String& value) const;

        public:
            /**
            // returns string value of attribute, and "" if doesn't exist (or empty - so not distinguishable this way)

            &&& todo redo this API so returns OPTIONAL and nullopt if missing
             */
            nonvirtual String GetAttribute (const String& attrName) const;

        public:
            /**
             */
            nonvirtual optional<Ptr> GetFirstAncestorNodeWithAttribute (const String& attrName) const;

        public:
            /**
            // if afterNode is nullptr - then this is PREPEND, else require afterNode is a member of 'GetChildren()'
             */
            nonvirtual Ptr InsertChild (const String& name, optional<Ptr> afterNode);
            nonvirtual Ptr InsertChild (const String& name, const String& ns, optional<Ptr> afterNode);

        public:
            /**
             */
            nonvirtual Ptr  AppendChild (const String& name);
            nonvirtual void AppendChild (const String& name, const VariantValue& v);
            nonvirtual void AppendChild (const String& name, const String& ns, const VariantValue& v);

        public:
            /**
             */
            // Node 'n' COULD come from other XMLDoc - so be careful how its inserted
            // NB: NODE CLONED/COPIED - NOT INSERTED BY REFERENCE!
            // if afterNode == nullopt, then PREPEND!
            nonvirtual Ptr InsertNode (const Ptr& n, const optional<Ptr>& afterNode, bool inheritNamespaceFromInsertionPoint = true);
            // Node 'n' COULD come from other XMLDoc - so be careful how its appended
            // NB: NODE CLONED/COPIED - NOT INSERTED BY REFERENCE!
        public:
            /**
             */
            nonvirtual Ptr AppendNode (const Ptr& n, bool inheritNamespaceFromInsertionPoint = true);

        public:
            /**
             */
            nonvirtual void AppendChildIfNotEmpty (const String& name, const VariantValue& v);
            nonvirtual void AppendChildIfNotEmpty (const String& name, const String& ns, const VariantValue& v);

        public:
            /**
            * @todo CLARIFY - THINK DELETES UNDERLYINHG NODE - LEAVES THIS PTR == nullptr (or should)
             */
            nonvirtual void DeleteNode ();

        public:
            /**
             */
            nonvirtual Ptr ReplaceNode (); // creates a new (empty) node with same name (and namespace) as orig

        public:
            /**
             */
            nonvirtual optional<Ptr> GetParentNode () const;

        public:
            /**
             */
            nonvirtual Iterable<Ptr> GetChildren () const;

        public:
            /**
            // can return a NULL Node. Only examines this node's children
             */
            nonvirtual optional<Ptr> GetChildNodeByID (const String& id) const;

        private:
            shared_ptr<IRep> fRep_;

        private:
            friend class IRep;
        };

        /**
         */
        class IRep {
        public:
            IRep ()          = default;
            virtual ~IRep () = default;

        public:
            virtual Type                GetNodeType () const                                                                         = 0;
            virtual String              GetNamespace () const                                                                        = 0;
            virtual String              GetName () const                                                                             = 0;
            virtual void                SetName (const String& name)                                                                 = 0;
            virtual VariantValue        GetValue () const                                                                            = 0;
            virtual void                SetValue (const VariantValue& v)                                                             = 0;
            virtual void                SetAttribute (const String& attrName, const String& v)                                       = 0;
            virtual bool                HasAttribute (const String& attrName, const String* value) const                             = 0;
            virtual String              GetAttribute (const String& attrName) const                                                  = 0;
            virtual optional<Node::Ptr> GetFirstAncestorNodeWithAttribute (const String& attrName) const                             = 0;
            virtual Ptr                 InsertChild (const String& name, const String* ns, optional<Ptr> afterNode)                  = 0;
            virtual Ptr                 AppendChild (const String& name)                                                             = 0;
            virtual void                AppendChild (const String& name, const String* ns, const VariantValue& v)                    = 0;
            virtual void                AppendChildIfNotEmpty (const String& name, const String* ns, const VariantValue& v)          = 0;
            virtual Ptr           InsertNode (const Ptr& n, const optional<Ptr>& afterNode, bool inheritNamespaceFromInsertionPoint) = 0;
            virtual Ptr           AppendNode (const Ptr& n, bool inheritNamespaceFromInsertionPoint)                                 = 0;
            virtual void          DeleteNode ()                                                                                      = 0;
            virtual Ptr           ReplaceNode ()                                                                                     = 0;
            virtual optional<Ptr> GetParentNode () const                                                                             = 0;
            virtual Iterable<Ptr> GetChildren () const                                                                               = 0;
            virtual optional<Ptr> GetChildNodeByID (const String& id) const                                                          = 0;

            // @todo see if I can lose GetInteralRep, and do with dynamic_cast - better/more portable to diff impls...
            virtual void* GetInternalTRep () = 0;

        protected:
            inline static shared_ptr<IRep> GetRep4Node (Ptr n)
            {
                // @todo - see if this needed/why
                return n.fRep_;
            }
        };
    }

    namespace Document {
        struct IRep;

        /**
        * 
        * todo consider how schema associated with doc means?? one meaning:
        *       Allow READ or WRITE to use ANY schema (or none) to validate, and just has assocaited schema be default for thiese?)
        *   tthats not well thoguth out but a gbeeingg.
        * 
        *   idea is - might have quick and full schemas and use one sometimes andother other sometimes.
         */
        class Ptr {
        public:
            /**
         */
            Ptr ();
            Ptr (const shared_ptr<IRep>& rep);
            ~Ptr () = default;

            // No copying. (maybe create a RWDocument for that - but doesnt appear needed externally)
        private:
            Ptr (const Ptr& from)           = delete;
            Ptr& operator= (const Ptr& rhs) = delete;

        public:
            /**
         */
            nonvirtual Schema::Ptr GetSchema () const;

        public:
            /**
            // IO routines - Serialize the document DOM
         */
            nonvirtual void WritePrettyPrinted (ostream& out) const;
            /**
         */
            nonvirtual void WriteAsIs (ostream& out) const;

        public:
            /**
         */
            nonvirtual void Validate () const; // throws BadFormatException exception on failure; uses current associated schema
                                               // nonvirtual void Validate (const Schema* schema) const; // ''; but uses PROVIDED schema

        public:
            /**
         */
            nonvirtual Iterable<Node::Ptr> GetChildren () const;

        public:
            /**
         * \brief always returns Node of eElement type, or throws on failure
         */
            nonvirtual Node::Ptr GetRootElement () const;

        public:
            /**
         */
            nonvirtual Node::Ptr CreateDocumentElement (const String& name);

        public:
            /**
         */
            nonvirtual void SetRootElement (const Node::Ptr& newRoot);

            // bad api - redo
        public:
            nonvirtual void Read (Streams::InputStream<byte>::Ptr in, Execution::ProgressMonitor::Updater progressCallback = nullptr);

            // bad api - redo
        public:
            nonvirtual void LoadXML (const String& xml); // 'xml' contains data to be parsed and to replace the current XML document

        public:
            /**
         */
            nonvirtual shared_ptr<IRep> GetRep () const;

        private:
            shared_ptr<IRep> fRep_;
        };

        // todo add overloads for input stream...
        Ptr New ();
        Ptr New (const Schema::Ptr& schema);

        /**
         */
        struct IRep {

            virtual ~IRep () = default;

            virtual const Schema::Ptr GetSchema () const = 0;

            //
            // If this function is passed a nullptr exceptionResult - it will throw on bad validation.
            // If it is passed a non-nullptr exceptionResult - then it will map BadFormatException to being ignored, but filling in this
            // parameter with the exception details. This is used to allow 'advisory' read xsd validation failure, without actually fully
            // failing the read (for http://bugzilla/show_bug.cgi?id=513).
            //
            virtual void Read (Streams::InputStream<byte>::Ptr& in, shared_ptr<BadFormatException>* exceptionResult,
                               Execution::ProgressMonitor::Updater progressCallback) = 0;

            virtual void SetRootElement (const Node::Ptr& newRoot) = 0;

            virtual Node::Ptr CreateDocumentElement (const String& name) = 0;

            virtual void LoadXML (const String& xml)             = 0;
            virtual void WritePrettyPrinted (ostream& out) const = 0;

            virtual void WriteAsIs (ostream& out) const = 0;

            virtual Iterable<Node::Ptr> GetChildren () const = 0;

            virtual void                     Validate () const                = 0;
            virtual NamespaceDefinitionsList GetNamespaceDefinitions () const = 0;
        };

    }

};
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "DOM.inl"

#endif /*_Stroika_Foundation_DataExchange_XML_DOM_h_*/
