/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_DOM_h_
#define _Stroika_Foundation_DataExchange_XML_DOM_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Execution/Exceptions.h"
#include "../../Streams/InputStream.h"
#include "../../Streams/OutputStream.h"

#include "../BadFormatException.h"

#include "Namespace.h"

namespace Stroika::Foundation::DataExchange::XML::Schema {
    class Ptr;
}
namespace Stroika::Foundation::DataExchange::XML::Providers {
    struct IDOMProvider;
};

namespace Stroika::Foundation::DataExchange::XML::DOM {

    using DataExchange::BadFormatException;
    using Traversal::Iterable;

    /**
     *  \note - some serializers (xerces) may ignore fIndent
     */
    struct SerializationOptions {
        bool                   fPrettyPrint{false};
        optional<unsigned int> fIndent{};
    };

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
         *  Note name argument slightly more flexible than just String so double conversion works ("" can be assigned to NameWithNamespace)
         */
        struct NameWithNamespace {
            String        fName;
            optional<URI> fNamespace;

            template <Characters::IConvertibleToString NAME_TYPE>
            NameWithNamespace (NAME_TYPE&& name);
            NameWithNamespace (const optional<URI>& ns, const String& name);
        };

        /**
         *  \brief Node::Ptr is a smart pointer to a Node::IRep
         * 
         *  \note Before Stroika v3.0d5 this was simply called "Node", and now Node::Ptr
         * 
         *   Nodes are not created directly, but either via Node::Ptr methods, or Document::Ptr methods (because nodes are always associated with some document).
         * 
         *  Node::Ptr maybe nullptr (default constructed, assigned nullptr, or rendered null by a call to DeleteNode). Calling other methods (like GetName () etc)
         *  while nullptr is a requires failure.
         * 
         *  \note Older XMLDB InsertNode/AppendNode APIs - we had some APIs which more genreally operated on Nodes andding them and probably allowed moving them.
         *        I couldnt think of any cases where I needed that, and it made it harder to port to other libraries, so I removed those APIs (til I see there utility again).
         *        And then - need to better document just what they do/are for (so can do portable).
         * 
         *          SImilarly for Document::SetRootElement. (thats the only place this AppendNode code used). If we need SetRootElement, we need AppendNode.
         *          But that seems to create lots of magic about being careful about tranplating nodes from one document to another. Not sure that was ever done properly and want to avoid
         *          learnign how todo with each librariy. For now hope note needed.--LGP 2023-12-16
         */
        class Ptr {
        public:
            /**
             */
            Ptr (nullptr_t);
            Ptr (const shared_ptr<IRep>& from);

        public:
            /**
             *  \note a bit queer, but very helpful - two Node::Ptr's are equal not by comparing their pointers (shared_ptr rep) but
             *        if they refer to the same underlying (in providers representation) object. Meaning if they are created by two
             *        different APIs (say an interator and some other api) - you can check to see if they refer to the same underlying node.
             * 
             *  \req the two Ptrs are either nullptr or come from the same XML Provider object.
             */
            nonvirtual bool operator== (const Ptr& rhs) const;

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
             *  This is the value between the brackets <a>text</a>. Note that <a></a> is the same as <a/> - the empty string.
             * 
             *      \note before Stroika v3.0d5, the value API was VariantValue but that was always meaningless, and just treated as a String.
             *
             *      \note the 'String' value maybe implemented in XML in a variety of ways (entities, CDATA, etc).
             */
            nonvirtual String GetValue () const;

        public:
            /**
             */
            nonvirtual void SetValue (const String& v);

        public:
            /**
             *  \req GetNodeType () == Node::eElementNT
             */
            nonvirtual void SetAttribute (const String& attrName, const optional<String>& v);

        public:
            /**
             *  \req GetNodeType () == Node::eElementNT
            // return true iff attribute exists on this node
            // return true iff attribute exists on this node and equals (case sensative) value
             */
            nonvirtual bool HasAttribute (const String& attrName) const;
            nonvirtual bool HasAttribute (const String& attrName, const String& value) const;

        public:
            /**
             *  \req GetNodeType () == Node::eElementNT
            // returns string value of attribute, and nullopt if doesn't exist
             */
            nonvirtual optional<String> GetAttribute (const String& attrName) const;

        public:
            /**
            * seems very special purpose. See if actually used any maybe lose this... --LGP 2023-12-16
             */
            nonvirtual Ptr GetFirstAncestorNodeWithAttribute (const String& attrName) const;

        public:
            /**
             *  \brief Insert Element (after argument node) inside of this 'Element'
             * 
             *  \req GetNodeType () == Node::eElementNT
             * 
             *  if afterNode is nullptr - then this is PREPEND, else require afterNode is a member of 'GetChildren()'
             */
            nonvirtual Ptr InsertElement (const String& name, const Ptr& afterNode);
            nonvirtual Ptr InsertElement (const optional<URI>& ns, const String& name, const Ptr& afterNode);

        public:
            /**
             *  \req GetNodeType () == Node::eElementNT
             * 
             *  value overload just creates the node and calls SetValue() before returning it (so simple shorthand).
             */
            nonvirtual Ptr AppendElement (const String& name);
            nonvirtual Ptr AppendElement (const optional<URI>& ns, const String& name);
            nonvirtual Ptr AppendElement (const optional<URI>& ns, const String& name, const String& value);

        public:
            /**
             *  \brief Trivial wrapper on AppendElement, but if v is missing or "", then this is a no-op.
             * 
             *  \req GetNodeType () == Node::eElementNT
             * 
             *  Trivial, but I've found helpful in making certain uses more terse.
             */
            nonvirtual void AppendElementIfNotEmpty (const String& name, const optional<String>& v);
            nonvirtual void AppendElementIfNotEmpty (const optional<URI>& ns, const String& name, const optional<String>& v);

        public:
            /**
             *  \req *this != nullptr
             *  \ens *this == nullptr
             */
            nonvirtual void DeleteNode ();

        public:
            /**
            * 
            * 
            *   @todo FIX so works with root elemeent too - not hard to detect and not hard to fix - delet the old and just do appendNode
            * 
            * 
             * creates a new (empty) node with same name (and namespace) as orig, and in the same position (but not root of document)
             * 
             *  \req Require (GetNodeType () == Node::eElementNT)
             *  \req GetParent () != nullptr
             * 
             *  To replace the root element, just create a new Document;
             * 
             *  This can be used to delete all the children/content under a given node and is equivilent, except that it returns
             *  a new NodePtr, and invalidates this.
             */
            nonvirtual Ptr ReplaceElement ();

        public:
            /**
             *  Can return nullptr
             */
            nonvirtual Ptr GetParentNode () const;

        public:
            /**
             */
            nonvirtual Iterable<Ptr> GetChildren () const;

        public:
            /**
             *  \req GetNodeType () == Node::eElementNT
             * can return a NULL Node Ptr if not found. Only examines this node's (direct) children
             * 
             *  \ens result == nullptr or result.GetNodeType () == Element
             */
            nonvirtual Ptr GetChildElementByID (const String& id) const;

        public:
            /**
             */
            nonvirtual shared_ptr<IRep> GetRep () const;

        public:
            [[deprecated ("Since Stroika v3.0d5 use InsertElement")]] inline Ptr InsertChild (const String& name, const Ptr& afterNode)
            {
                return InsertElement (name, afterNode);
            }
            [[deprecated ("Since Stroika v3.0d5 use InsertElement")]] inline Ptr InsertChild (const String& name, const optional<URI>& ns,
                                                                                              const Ptr& afterNode)
            {
                return InsertElement (ns, name, afterNode);
            }
            [[deprecated ("Since Stroika v3.0d5 use AppendElement")]] inline Ptr AppendChild (const String& name, const optional<URI>& ns = nullopt)
            {
                return AppendElement (ns, name);
            }
            [[deprecated ("Since Stroika v3.0d5 use AppendElement")]] inline Ptr AppendChild (const String& name, const optional<URI>& ns, const String& v)
            {
                return AppendElement (ns, name, v);
            }
            [[deprecated ("Since Stroika v3.0d5 use AppendElementIfNotEmpty")]] inline void
            AppendChildIfNotEmpty (const String& name, const optional<URI>& ns, const String& v)
            {
                return AppendElementIfNotEmpty (ns, name, v);
            }
            [[deprecated ("Since Stroika v3.0d5 use ReplaceElement or create new Document")]] Ptr ReplaceNode ()
            {
                return ReplaceElement ();
            }
            [[deprecated ("Since Stroika v3.0d5 use GetChildElementByID")]] Ptr GetChildNodeByID (const String& id) const
            {
                return GetChildElementByID (id);
            }

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
            virtual bool             Equals (const IRep* rhs) const                                                            = 0;
            virtual Type             GetNodeType () const                                                                      = 0;
            virtual optional<URI>    GetNamespace () const                                                                     = 0;
            virtual String           GetName () const                                                                          = 0;
            virtual void             SetName (const String& name)                                                              = 0;
            virtual String           GetValue () const                                                                         = 0;
            virtual void             SetValue (const String& v)                                                                = 0;
            virtual optional<String> GetAttribute (const optional<URI>& ns, const String& attrName) const                      = 0;
            virtual void             SetAttribute (const optional<URI>& ns, const String& attrName, const optional<String>& v) = 0;
            /**
             *  if afterNode is nullptr - then this is PREPEND, else require afterNode is a member of 'GetChildren()'
             */
            virtual Ptr           InsertElement (const optional<URI>& ns, const String& name, const Ptr& afterNode) = 0;
            virtual Ptr           AppendElement (const optional<URI>& ns, const String& name)                       = 0;
            virtual void          DeleteNode ()                                                                     = 0;
            virtual Ptr           GetParentNode () const                                                            = 0;
            virtual Iterable<Ptr> GetChildren () const                                                              = 0;
            // Redundant API, but provided since commonly used and can be optimized
            virtual Ptr GetChildElementByID (const String& id) const;
        };
    }

    namespace Element {

        using Node::NameWithNamespace;

        // Todo - maybe introduce ElementPtr, wtih the extra methods that require you are an Element?
        // @todo - and can simply Ptr api (docs at least) above. Maybe cast from Ptr to ElementPtr automatically maps non-elements to nullptr?? Simplifiication
        // // NYI...
        //???? --LGP 2024-01-06

        /**
         *  Simple wrapper on Node::Ptr API, but with the Ptr objects refer to Elements.
         * 
         *  If you assign a non-Element to an Element::Ptr, it automatically, silently becomes nullptr (like dynamic_cast).
         */
        struct Ptr : public Node::Ptr {
        public:
            /**
             */
            Ptr (const Node::Ptr& p);

        public:
            /**
             */
            nonvirtual NameWithNamespace GetName () const;

        public:
            /**
             */
            nonvirtual void SetName (const NameWithNamespace& name);

        public:
            /**
             *  This is the value between the brackets <a>text</a>. Note that <a></a> is the same as <a/> - the empty string.
             * 
             *      \note before Stroika v3.0d5, the value API was VariantValue but that was always meaningless, and just treated as a String.
             *
             *      \note the 'String' value maybe implemented in XML in a variety of ways (entities, CDATA, etc).
             */
            nonvirtual String GetValue () const;

        public:
            /**
             */
            nonvirtual void SetValue (const String& v);

        public:
            /**
             *  \req GetNodeType () == Node::eElementNT
            // returns string value of attribute, and nullopt if doesn't exist
             */
            nonvirtual optional<String> GetAttribute (const NameWithNamespace& attrName) const;

        public:
            /**
            // return true iff attribute exists on this node
            // return true iff attribute exists on this node and equals (case sensative) value
             */
            nonvirtual bool HasAttribute (const NameWithNamespace& attrName) const;
            nonvirtual bool HasAttribute (const NameWithNamespace& attrName, const String& value) const;

        public:
            /**
             */
            nonvirtual void SetAttribute (const NameWithNamespace& attrName, const optional<String>& v);

        public:
            /**
             *  \brief Insert Element (after argument node) inside of this 'Element'
             * 
             *  if afterNode is nullptr - then this is PREPEND, else require afterNode is a member of 'GetChildren()'
             */
            nonvirtual Ptr Insert (const NameWithNamespace& eltName, const Node::Ptr& afterNode);

        public:
            /**
             *  value overload just creates the node and calls SetValue() before returning it (so simple shorthand).
             */
            nonvirtual Ptr Append (const NameWithNamespace& eltName);
            nonvirtual Ptr Append (const NameWithNamespace& eltName, const String& value);

        public:
            /**
             *  \brief Trivial wrapper on AppendElement, but if v is missing or "", then this is a no-op.
             * 
             *  Trivial, but I've found helpful in making certain uses more terse.
             */
            nonvirtual Ptr AppendIfNotEmpty (const NameWithNamespace& eltName, const optional<String>& v);

        public:
            /**
             * creates a new (empty) node with same name (and namespace) as orig, and in the same position (but not root of document)
             * 
             *  \req GetParent () != nullptr
             * 
             *  To replace the root element, just create a new Document;
             * 
             *  This can be used to delete all the children/content under a given node and is equivilent, except that it returns
             *  a new NodePtr, and invalidates this.
             */
            nonvirtual Ptr ReplaceElement ();

        public:
            /**
             *  Can return nullptr
             */
            nonvirtual Ptr GetParent () const;

        public:
            /**
             *  note only returns sub-elements, so use Node::Ptr (inherited) GetChildren to get them all
             */
            nonvirtual Iterable<Ptr> GetChildren () const;

        public:
            /**
             * can return a NULL Node Ptr if not found. Only examines this node's (direct) children
             */
            nonvirtual Ptr GetChildElementByID (const String& id) const;
        };
    }

    namespace Document {
        struct IRep;

        /**
         *  \brief Document::Ptr is create with Document::New, and is a smart pointer to a DOM document object.
         * 
         *  \note nullptr is allowed for Ptr
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
            Ptr (nullptr_t);
            Ptr (const shared_ptr<IRep>& rep);
            Ptr (const Ptr& from) = default;

        public:
            Ptr& operator= (const Ptr& rhs) = default;

        public:
            ~Ptr () = default;

        public:
            /**
             */
            bool operator== (const Ptr&) const = default;

        public:
            /**
             */
            nonvirtual void   Write (const Streams::OutputStream::Ptr<byte>& to, const SerializationOptions& options = {}) const;
            nonvirtual String Write (const SerializationOptions& options = {}) const;

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
             * \brief always returns Node of eElement or return nullptr if none
             */
            nonvirtual Node::Ptr GetRootElement () const;

        public:
            /**
            * @@todo  - what happens if we rename this operator-> - IO think chaining works nicely then. maybe use that trick thoguhotu Stroika if it works here.
             */
            nonvirtual shared_ptr<IRep> GetRep () const;

        private:
            shared_ptr<IRep> fRep_;
        };

        /**
         *  Create a Document object and return a smart pointer (Ptr) to it.
         * 
         *  Use the optionally provided stream to deserialize the document from (or create an empty one with a single root documentElement given by name/ns args).
         * 
         *  \note String in overload is trivial wrapper on Streams::TextToByteReader{}, in case you want adjust paramters.
         * 
         * @todo add overload taking String 'in' and parse using Streams::TextToByteReader
         */
        Ptr New (const Providers::IDOMProvider& p, const String& documentElementName, const optional<URI>& ns);
        Ptr New (const Providers::IDOMProvider& p, const Streams::InputStream::Ptr<byte>& in);
        Ptr New (const Providers::IDOMProvider& p, const Streams::InputStream::Ptr<byte>& in, const Schema::Ptr& schemaToValidateAgainstWhileReadingr);
        Ptr New (const Providers::IDOMProvider& p, const String& in);
        Ptr New (const Providers::IDOMProvider& p, const String& in, const Schema::Ptr& schemaToValidateAgainstWhileReading);
#if qStroika_Foundation_DataExchange_XML_SupportDOM
        Ptr New (const String& documentElementName, const optional<URI>& ns);
        Ptr New (const Streams::InputStream::Ptr<byte>& in);
        Ptr New (const Streams::InputStream::Ptr<byte>& in, const Schema::Ptr& schemaToValidateAgainstWhileReadingr);
        Ptr New (const String& in);
        Ptr New (const String& in, const Schema::Ptr& schemaToValidateAgainstWhileReading);
#endif

        /**
         */
        struct IRep {
            virtual ~IRep () = default;
            virtual void                Write (const Streams::OutputStream::Ptr<byte>& to, const SerializationOptions& options) const = 0;
            virtual Iterable<Node::Ptr> GetChildren () const                                                                          = 0;
            virtual void                Validate (const Schema::Ptr& schema) const                                                    = 0;
        };
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "DOM.inl"

#endif /*_Stroika_Foundation_DataExchange_XML_DOM_h_*/
