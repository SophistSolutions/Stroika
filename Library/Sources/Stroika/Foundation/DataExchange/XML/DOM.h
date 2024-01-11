/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_DOM_h_
#define _Stroika_Foundation_DataExchange_XML_DOM_h_ 1

#include "../../StroikaPreComp.h"

#include <variant>

#include "../../Common/TemplateUtilities.h"
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

    namespace Node {
        struct IRep;
        class Ptr;
    }

    namespace Element {
        class Ptr;
    }

    namespace XPath {

        /**
         * Probably incomplete - see https://xerces.apache.org/xerces-c/apiDocs-3/classDOMXPathResult.html#ab718aec450c5438e0cc3a6920044a0c1
         * 
         *  @todo unclear if we ever return Nodes other than Element::Ptr - so for now just say Element::Ptr, but maybe expand list later.
         */
        using Result = variant<bool, int, double, String, Element::Ptr>;

        /**
         *  Some APIs (like Expression) need to know the index - not just the type itself, and this maps.
         */
        template <typename T>
        constexpr uint8_t ResultTypeIndex_v = static_cast<uint8_t> (Common::variant_index<Result, T> ());

        /**
         *  This def is provider independent, but since implemetned with a shared_ptr and immutable, the providers can maintain a cache of mappings
         *  of expressions to internal compiled version of expression.
         * 
         *  So - it behaves rather obviously - except that if you copy an expression, you KNOW the underlying data wont change so can cache dervied values (like a compiled expression).
         */
        class Expression {
        public:
            /**
             *  Xerces seems to have a bunch more options controlling things like order of results returend. Look into this...
             */
            struct Options {
                /**
                 * prefixes available to use in expression
                 */
                NamespaceDefinitionsList fNamespaces;

                /**
                 * index into 'Result' variant expected. REJECT other values - required by Xerces XPATH API, and not a biggie (why wouldn't you know?)
                 */
                optional<uint8_t> fResultTypeIndex{ResultTypeIndex_v<Element::Ptr>};
            };
            static inline const Options kDefaultOptions{.fNamespaces = {}, .fResultTypeIndex = ResultTypeIndex_v<Element::Ptr>};

        public:
            Expression (const String& e, const Options& o = kDefaultOptions);

        public:
            struct IRep {
                virtual ~IRep ()                       = default;
                virtual String  GetExpression () const = 0;
                virtual Options GetOptions () const    = 0;
            };

        public:
            /**
             * The text of the expression
             */
            nonvirtual String GetExpression () const;

        public:
            /**
             * Get options (like prefix to namespace map, etc)
             */
            nonvirtual Options GetOptions () const;

        public:
            nonvirtual shared_ptr<const IRep> GetRep () const;

        private:
            shared_ptr<const IRep> fRep_;
        };

    }

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
            Ptr () = default;
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
             *  \req GetNodeType == eAttributeNT or eElementNT
             */
            nonvirtual NameWithNamespace GetName () const;

        public:
            /**
             *  \req GetNodeType == eAttributeNT or eElementNT
             */
            nonvirtual void SetName (const NameWithNamespace& name);

        public:
            /**
             *  This is the value between the brackets <a>text</a>. Note that <a></a> is the same as <a/> - the empty string.
             * 
             *      \note before Stroika v3.0d5, the value API was VariantValue but that was always meaningless, and just treated as a String.
             *
             *      \note the 'String' value maybe implemented in XML in a variety of ways (entities, CDATA, etc).
             *
             *  \req GetNodeType == eAttributeNT or eElementNT
             */
            nonvirtual String GetValue () const;

        public:
            /**
             *  \req GetNodeType == eAttributeNT or eElementNT
             */
            nonvirtual void SetValue (const String& v);

        public:
            /**
             *  \req *this != nullptr
             *  \ens *this == nullptr
             */
            nonvirtual void Delete ();

        public:
            /**
             *  Can return nullptr
             */
            nonvirtual Ptr GetParentNode () const;

        public:
            /**
             */
            nonvirtual shared_ptr<IRep> GetRep () const;

        public:
            /**
             */
            nonvirtual Characters::String ToString () const;

        private:
            shared_ptr<IRep> fRep_;
        };

        /**
         *  DOM Nodes are typically 'elements', but can also be 'text' nodes, or comments, or attributes, etc...
         */
        struct IRep {
            IRep ()          = default;
            virtual ~IRep () = default;

            virtual const Providers::IDOMProvider* GetProvider () const           = 0;
            virtual bool                           Equals (const IRep* rhs) const = 0;
            virtual Type                           GetNodeType () const           = 0;
            // GetName () only allowed on element/attribute
            virtual NameWithNamespace GetName () const = 0;
            // SetName () only allowed on element/attribute
            virtual void SetName (const NameWithNamespace& name) = 0;
            // GetValue () only allowed on element/attribute
            virtual String GetValue () const = 0;
            // SetValue () only allowed on element/attribute
            virtual void SetValue (const String& v)                                                                    = 0;
            virtual void DeleteNode ()                                                                                 = 0;
            virtual Ptr  GetParentNode () const                                                                        = 0;
            virtual void Write (const Streams::OutputStream::Ptr<byte>& to, const SerializationOptions& options) const = 0;
        };
    }

    namespace Element {

        using namespace Node;

        struct IRep;

        /**
         *  Simple wrapper on Node::Ptr API, but with the Ptr objects refer to Elements.
         * 
         *  If you assign a non-Element to an Element::Ptr, it automatically, silently becomes nullptr (like dynamic_cast).
         * 
         *  TODO:
         *      \todo   Maybe add GetNamespaceDefinitions () to return a NamespaceDefinitionsList object with the prefixes
         *              default, and namespace URIs for this particular element. But so far no need.
         */
        class Ptr : public Node::Ptr {
        public:
            /**
             */
            Ptr () = default;
            Ptr (nullptr_t);
            Ptr (const Node::Ptr& p);
            Ptr (const shared_ptr<IRep>& p);

        public:
            /**
             * returns string value of attribute, and nullopt if doesn't exist
             */
            nonvirtual optional<String> GetAttribute (const NameWithNamespace& attrName) const;

        public:
            /**
             * return true iff attribute exists on this node
             * return true iff attribute exists on this node and equals (case sensative) value
             */
            nonvirtual bool HasAttribute (const NameWithNamespace& attrName) const;
            nonvirtual bool HasAttribute (const NameWithNamespace& attrName, const String& value) const;

        public:
            /**
             *  Note - setting the attribute to nullopt is equivalent to deleting the attribute
             */
            nonvirtual void SetAttribute (const NameWithNamespace& attrName, const optional<String>& v);

        public:
            /**
             *  \brief Insert Element (after argument node) inside of this 'Element'
             * 
             *  if afterNode is nullptr - then this is PREPEND, else require afterNode is a member of 'Node::Ptr::GetChildren()' - need not be an element
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
             * creates a new (empty) node with same name (and namespace) as orig, and in the same position
             * 
             *  \req GetParent () != nullptr
             * 
             *  This can be used to delete all the children/content under a given node and is equivilent, except that it returns
             *  a new NodePtr, and invalidates this.
             * 
             *  Note - this CAN be used to replace the document root.
             */
            nonvirtual Ptr Replace ();

        public:
            /**
             *  Can return nullptr
             */
            nonvirtual Ptr GetParent () const;

        public:
            /**
             *  note only returns sub-elements, so use Node::Ptr (inherited) GetChildren to get them all
             *
             *  \warning Any modification of the DOM may invalidate live iterators or iterables, so refecth after each change
             */
            nonvirtual Iterable<Node::Ptr> GetChildNodes () const;

        public:
            /**
             *  note only returns sub-elements, so use Node::Ptr (inherited) GetChildren to get them all
             *
             *  \warning Any modification of the DOM may invalidate live iterators or iterables, so refecth after each change
             */
            nonvirtual Iterable<Ptr> GetChildElements () const;

        public:
            /**
             * can return a NULL Node Ptr if not found. Only examines this node's (direct) children elements (not attributes)
             */
            nonvirtual Ptr GetChild (const NameWithNamespace& eltName) const;

        public:
            /**
             * can return a NULL Node Ptr if not found. Only examines this node's (direct) children
             */
            nonvirtual Ptr GetChildByID (const String& id) const;

        public:
            nonvirtual optional<XPath::Result> LookupOne (const XPath::Expression& e) const;

        public:
            nonvirtual Traversal::Iterator<XPath::Result> Lookup (const XPath::Expression& e) const;

        public:
            /**
             */
            nonvirtual shared_ptr<IRep> GetRep () const;
        };

        /**
         *  Elements are special nodes, that may contain sub-nodes.
         */
        struct IRep : virtual Node::IRep {
            virtual optional<String> GetAttribute (const NameWithNamespace& attrName) const                      = 0;
            virtual void             SetAttribute (const NameWithNamespace& attrName, const optional<String>& v) = 0;
            /**
             *  if afterNode is nullptr - then this is PREPEND, else require afterNode is a member of 'GetChildren()'
             */
            virtual Ptr                 InsertElement (const NameWithNamespace& eltName, const Ptr& afterNode) = 0;
            virtual Ptr                 AppendElement (const NameWithNamespace& eltName)                       = 0;
            virtual Iterable<Node::Ptr> GetChildren () const                                                   = 0;
            // Redundant API, but provided since commonly used and can be optimized
            virtual Ptr GetChildElementByID (const String& id) const;
            virtual optional<XPath::Result> LookupOne (const XPath::Expression& e) = 0; /// maybe lose this and do LookupOne/LookupAll etc in Ptr wrapper?
            virtual Traversal::Iterator<XPath::Result> Lookup (const XPath::Expression& e) = 0;
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
            nonvirtual Element::Ptr GetRootElement () const;

        public:
            /**
            * @@todo  - what happens if we rename this operator-> - IO think chaining works nicely then. maybe use that trick throughout Stroika if it works here.
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
         *  \note String in overload is trivial wrapper on Streams::TextToByteReader{}, in case you want adjust parameters.
         * 
         * @todo add overload taking String 'in' and parse using Streams::TextToByteReader
         *  @todo consider adding 'Resolver' argument - so missing #includes get loaded.
         */
        Ptr New (const Providers::IDOMProvider& p, const NameWithNamespace& documentElementName);
        Ptr New (const Providers::IDOMProvider& p, const Streams::InputStream::Ptr<byte>& in);
        Ptr New (const Providers::IDOMProvider& p, const Streams::InputStream::Ptr<byte>& in, const Schema::Ptr& schemaToValidateAgainstWhileReadingr);
        Ptr New (const Providers::IDOMProvider& p, const String& in);
        Ptr New (const Providers::IDOMProvider& p, const String& in, const Schema::Ptr& schemaToValidateAgainstWhileReading);
#if qStroika_Foundation_DataExchange_XML_SupportDOM
        Ptr New (const NameWithNamespace& documentElementName);
        Ptr New (const Streams::InputStream::Ptr<byte>& in);
        Ptr New (const Streams::InputStream::Ptr<byte>& in, const Schema::Ptr& schemaToValidateAgainstWhileReadingr);
        Ptr New (const String& in);
        Ptr New (const String& in, const Schema::Ptr& schemaToValidateAgainstWhileReading);
#endif

        /**
         */
        struct IRep {
            virtual ~IRep ()                                            = default;
            virtual const Providers::IDOMProvider* GetProvider () const = 0;
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
