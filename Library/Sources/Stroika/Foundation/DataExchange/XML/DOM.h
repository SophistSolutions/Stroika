/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_DOM_h_
#define _Stroika_Foundation_DataExchange_XML_DOM_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Execution/Exceptions.h"
#include "../../Streams/InputStream.h"

#include "../BadFormatException.h"
#include "../VariantValue.h"

#include "Namespace.h"
#include "Schema.h"

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
         *  \brief Node::Ptr is a smart pointer to a Node::IRep
         * 
         *  \note Before Stroika v3.0d5 this was simply called "Node", and now Node::Ptr
         * 
         *   Note - Nodes are not created directly, but either via Node::Ptr methods, or Document::Ptr methods (because nodes are always associated with some document).
         * 
         * 
         * 
         * &&& todo add == nullptr_t and to bool conversion to check for null and then note REQ cannog call any mthods like GetName() if == nullptr
         */
        class Ptr {
        public:
            /**
             */
            Ptr (const shared_ptr<IRep>& from);

        public:
            /**
             */
            nonvirtual Type GetNodeType () const;

        public:
            /**
             */
            nonvirtual optional<URI> GetNamespace () const;

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
            virtual optional<URI>       GetNamespace () const                                                                        = 0;
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
            // todo see if I can lose this or better doucment the point
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
         *   \note the document object does NOT have an intrinsically associated schema object. We considered/started with such a design.
         *         but then the trouble was, what if you wanted two (say a short and long format - schema). Or what if sometimes you wanted to read
         *         with no schema (validation) and sometimes with validation? It just added alot of ambiguity (in how to interpret optional/nullptr as meaning
         *         default or none).
         * 
         *        These matters could have been resolved, but in the end it seems maximally flexible, and super clear, to just not associate the schema
         *       with the document, and just specify in as an argument to read (New) or Write (serialize) etc.
         * 
         * 
         * todo add better nullptr_t interop - static bool convert etc...
         */
        class Ptr {
        public:
            /**
             */
            Ptr ();
            Ptr (const shared_ptr<IRep>& rep);
            Ptr (const Ptr& from)           = default;
            Ptr& operator= (const Ptr& rhs) = default;
            ~Ptr ()                         = default;

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
             *  throws BadFormatException exception on failure
             */
            nonvirtual void Validate (const Schema::Ptr& schema) const;

        public:
            /**
             *  Return (and iterable) of nodes, which represent the top level children of the document. Typically there will be one interesting  - the
             *  root element, so calling GetRootElement() will typically be easier/more useful (so as to not see/get confused with things like comments, or processing directives).
             */
            nonvirtual Iterable<Node::Ptr> GetChildren () const;

        public:
            /**
             * \brief always returns Node of eElement type, or throws on failure
             */
            nonvirtual Node::Ptr GetRootElement () const;

        public:
            /**
            *   @todo document - invistigate what this does? If there already is one (replace?) or erorr?
             */
            nonvirtual Node::Ptr CreateDocumentElement (const String& name, const optional<URI>& ns = nullopt);

        public:
            /**
            *   @todo document - invistigate what this does? look at impl - maybe delete API? What if goes cross documents????
         */
            nonvirtual void SetRootElement (const Node::Ptr& newRoot);

        public:
            /**
             */
            nonvirtual shared_ptr<IRep> GetRep () const;

        private:
            shared_ptr<IRep> fRep_;
        };

        /**
         *  Create a Document object and return a smart pointer (Ptr) to it.
         * 
         *  Use the optionally provided stream to deserialize the document from (or create an empty one).
         * 
         *  \note String in overload is trivial wrapper on Streams::TextToByteReader{}, in case you want adjust paramters.
         * 
         * @todo add overload takign String 'in' and parse using Streams::TextToByteReader
         */
        Ptr New ();
        Ptr New (const Streams::InputStream<byte>::Ptr& in, const Schema::Ptr& schemaToValidateAgainstWhileReading = nullptr);
        Ptr New (const String& in, const Schema::Ptr& schemaToValidateAgainstWhileReading = nullptr);

        /**
         */
        struct IRep {
            virtual ~IRep ()                                                                                        = default;
            virtual void                Read (const Streams::InputStream<byte>::Ptr& in, const Schema::Ptr& schema) = 0;
            virtual void                SetRootElement (const Node::Ptr& newRoot)                                   = 0;
            virtual Node::Ptr           CreateDocumentElement (const String& name, const optional<URI>& ns)         = 0;
            virtual void                WritePrettyPrinted (ostream& out) const                                     = 0;
            virtual void                WriteAsIs (ostream& out) const                                              = 0;
            virtual Iterable<Node::Ptr> GetChildren () const                                                        = 0;
            virtual void                Validate (const Schema::Ptr& schema) const                                  = 0;
            //    virtual NamespaceDefinitionsList GetNamespaceDefinitions () const                       = 0;
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
