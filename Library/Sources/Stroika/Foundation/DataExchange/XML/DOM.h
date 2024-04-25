/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_DOM_h_
#define _Stroika_Foundation_DataExchange_XML_DOM_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <variant>

#include "Stroika/Foundation/Common/TemplateUtilities.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Streams/InputStream.h"
#include "Stroika/Foundation/Streams/OutputStream.h"

#include "Namespace.h"

namespace Stroika::Foundation::DataExchange::XML::Schema {
    class Ptr;
}
namespace Stroika::Foundation::DataExchange::XML::Providers {
    struct IDOMProvider;
};

namespace Stroika::Foundation::DataExchange::XML::DOM {

    using Traversal::Iterable;

    /*
     *  Special Note about namespaces:
     * 
     *      o   Default namespace does is inherited by enclosed elements, but **not** by enclosed attributes.
     * 
     *  The reason this basic fact is worthy of note is:
     *      o   IMHO, it is not intuitive
     *      o   https://learn.microsoft.com/en-us/previous-versions/windows/desktop/ms754539(v=vs.85) says
     *              "All elements and attributes in the document that do not have a prefix will then belong to the default namespace"
     *          THIS IS WRONG - and the top google search hit when searching about default xml namespaces.
     *
     *  The reason I believe my counter-claim above is:
     *      o   https://www.w3.org/TR/xml-names/#defaulting
     *          "The namespace name for an unprefixed attribute name always has no value" (after saying the reverse for element names)
     */

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
         *  Notes about XPath support:
         * 
         *  Tutorial/Syntax:
         *      https://www.w3schools.com/xml/xpath_syntax.asp
         * 
         *  Xerces Limitations (as of Xerces 3.2 - 2024-01)
         *      o   No [] support (so p/n[@f='3'] not supported, for example)
         */

        /**
         *  For example, Xerces 3.2 doesn't support [] in expressions
         */
        struct XPathExpressionNotSupported : Execution::RuntimeErrorException<> {
            XPathExpressionNotSupported ();
            static const XPathExpressionNotSupported kThe;
        };
        inline const XPathExpressionNotSupported XPathExpressionNotSupported::kThe;

        /**
         *  \note the default value is an empty (nullptr) Node::Ptr
         */
        using Result = variant<Node::Ptr, bool, int, double, String>;

        /**
         *  Some APIs (like Expression) need to know the index - not just the type itself, and this maps.
         */
        template <typename T>
        constexpr uint8_t ResultTypeIndex_v = static_cast<uint8_t> (Common::VariantIndex<Result, T>);

        /**
         *  Expression is provider independent, but since it is implemented with a shared_ptr and immutable,
         *  the providers can maintain a cache of mappings of expressions to internal compiled version of expression.
         * 
         *  So - it behaves rather obviously - except that if you copy an expression, you KNOW the underlying data wont
         *  change so can cache derived values (like a compiled expression).
         * 
         *  \note As of 2024-01-15, LibXML2 XPath doesn't support default namespace for XPath, so use explicit namespace prefixes.
         */
        class Expression {
        public:
            /**
             */
            struct Options {
                /**
                 * prefixes available to use in expression
                 */
                NamespaceDefinitions fNamespaces;

                /**
                 * index into 'Result' variant expected. REJECT other values - required by Xerces XPATH API, defaults to Node::Ptr
                 */
                optional<uint8_t> fResultTypeIndex{ResultTypeIndex_v<Node::Ptr>};

                /**
                 *  Caller cares (or does not) about order of elements returned (not sure what ordering is defined - depth first or breadth first?)
                 */
                bool fOrdered{false};

                /**
                 *  Snapshot less efficient, but safer in light of modifications to the node tree (but still not clearly defined what modifications allowed and what not?).
                 */
                bool fSnapshot{false};
            };
            static inline const Options kDefaultOptions{
                .fNamespaces = {}, .fResultTypeIndex = ResultTypeIndex_v<Node::Ptr>, .fOrdered = false, .fSnapshot = false};

        public:
            Expression (String&& e, const Options& o);
            template <Characters::IConvertibleToString ST>
            Expression (ST&& e, const Options& o = kDefaultOptions);

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
            /**
             */
            nonvirtual String ToString () const;

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
         *  Nodes are not created directly, but either via Node::Ptr methods, or Document::Ptr methods (because nodes are always associated with some document).
         * 
         *  Node::Ptr maybe nullptr (default constructed, assigned nullptr, or rendered null by a call to DeleteNode). Calling other methods (like GetName () etc)
         *  while nullptr is a requires failure.
         * 
         *  \note Older XMLDB InsertNode/AppendNode APIs - we had some APIs which more generally operated on Nodes adding them and probably allowed moving them.
         *        I couldn't think of any cases where I needed that, and it made it harder to port to other libraries, so I removed those APIs (til I see there utility again).
         *        And then - need to better document just what they do/are for (so can do portable).
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
            nonvirtual bool operator== (nullptr_t) const;

        public:
            /*
            * Like shared_ptr::operator bool - return true if non-null ptr
            */
            explicit operator bool () const;

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
             *  The /0 overload looks at 'this node'. The overload taking an XPath::Expression looks at the first selected node (using this node as context)
             *  Either way, the result is a node, and its text is returned. For XPath case, if no node is returned, the return value is nullopt.
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
             *  \brief return the associated shared_ptr (cannot be nullptr)
             * 
             *  \ens result != nullptr
             */
            nonvirtual shared_ptr<IRep> GetRep () const;

        public:
            /**
             *  \brief return the associated shared_ptr (can be nullptr)
             */
            nonvirtual shared_ptr<IRep> PeekRep () const;

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
         *  \note Giving a namespace to an element is done by giving an xmlns=XXX attribute, so the namespace
         *        is inherited by all children
         * 
         *  TODO:
         *      \todo   Maybe add GetNamespaceDefinitions () to return a NamespaceDefinitionsList object with the prefixes
         *              default, and namespace URIs for this particular element. But so far no need.
         */
        class Ptr : public Node::Ptr {
        public:
            /**
             *  for XPath::Result overload, require XPath produced right type (not string etc) - but allow for nullptr
             */
            Ptr () = default;
            Ptr (nullptr_t);
            Ptr (const Node::Ptr& p);
            Ptr (const XPath::Result& p);
            Ptr (const shared_ptr<IRep>& p);

        public:
            /**
             * returns string value of attribute, and nullopt if doesn't exist
             * 
             *  \req *this != nullptr
             */
            nonvirtual optional<String> GetAttribute (const NameWithNamespace& attrName) const;

        public:
            /**
             * return true iff attribute exists on this node
             * return true iff attribute exists on this node and equals (case sensitive) value
             * 
             *  \req *this != nullptr
             */
            nonvirtual bool HasAttribute (const NameWithNamespace& attrName) const;
            nonvirtual bool HasAttribute (const NameWithNamespace& attrName, const String& value) const;

        public:
            /**
             *  Note - setting the attribute to nullopt is equivalent to deleting the attribute
             * 
             *  The 'VariantValue' overload maps null to missing attribute, and any other value to a String (v.As<String>()).
             * 
             *  \req *this != nullptr
             */
            nonvirtual void SetAttribute (const NameWithNamespace& attrName, const optional<String>& v);
            template <same_as<VariantValue> VV>
            nonvirtual void SetAttribute (const NameWithNamespace& attrName, const VV& v);

        public:
            /**
             *  \req *this != nullptr
             */
            nonvirtual optional<String> GetID () const;

        public:
            /**
             *  Gets the XML xmlns= attribute associated with THIS element. This is the default namespace used for this element
             *  and all children. If its missing (even if a parent element has a default namespace) - this will return nullopt.
             */
            nonvirtual optional<URI> GetDefaultNamespace () const;

        public:
            /**
             *  \brief Sets the xmlns attribute of this element
             * 
             *  \see GetDefaultNamespace
             */
            nonvirtual void SetDefaultNamespace (const optional<URI> defaultNS = nullopt);

        public:
            /**
             *  This is the value between the brackets <a>text</a>. Note that <a></a> is the same as <a/> - the empty string.
             * 
             *      \note before Stroika v3.0d5, the value API was VariantValue but that was always meaningless, and just treated as a String.
             *
             *      \note the 'String' value maybe implemented in XML in a variety of ways (entities, CDATA, etc).
             * 
             *  The /0 overload looks at 'this node'. The overload taking an XPath::Expression looks at the first selected node (using this node as context)
             *  Either way, the resulting eAttributeNT or eElementNT nodes value.
             * 
             *  \req *this != nullptr
             */
            using Node::Ptr::GetValue;
            nonvirtual optional<String> GetValue (const XPath::Expression& e) const;

        public:
            /**
             *  \req e is an expression resulting in DOM nodes (could be attributes), and then get its text, and add that to an Iterable. Ignores nodes in result set other than attribute/element (like comment nodes)
             * 
             *  \note COULD have usefully made this return a Set<> since that's the most common case, but iterable is about as good, and you might
             *        plausibly want to track dup results, or other???
             * 
             *  \req *this != nullptr
             */
            nonvirtual Traversal::Iterable<String> GetValues (const XPath::Expression& e) const;

        public:
            /**
             *  For now, SetValue(e,v) - must  finds an Element (or Attribute) node at 'e' - otherwise throws runtime exception,
             *  cuz no way to know in general where to add what element (could do in some specific cases maybe - like simple QName or @QName expression).
             * 
             *  The overloads with VariantValue are just a short-hand for v.As<String> () on the variant value (with no checking).
             * 
             *  \req *this != nullptr
             */
            using Node::Ptr::SetValue;
            nonvirtual void SetValue (const XPath::Expression& e, const String& v);
            template <same_as<VariantValue> VV>
            nonvirtual void SetValue (const VV& v);
            template <same_as<VariantValue> VV>
            nonvirtual void SetValue (const XPath::Expression& e, const VV& v);

        public:
            /**
             *  \brief Insert Element (after argument node) inside of this 'Element'
             * 
             *  if afterNode is nullptr - then this is PREPEND, else require afterNode is a member of 'Node::Ptr::GetChildren()' - need not be an element
             * 
             *  \req *this != nullptr
             */
            nonvirtual Ptr Insert (const NameWithNamespace& eltName, const Node::Ptr& afterNode);

        public:
            /**
             *  value overload just creates the node and calls SetValue() before returning it (so simple shorthand).
             * 
             *  value=VariantValue overload is just a short-hand for value.As<String>() - so will fail if value cannot be converted to a String
             * 
             *  \req *this != nullptr
             */
            nonvirtual Ptr Append (const NameWithNamespace& eltName);
            nonvirtual Ptr Append (const NameWithNamespace& eltName, const String& value);
            template <same_as<VariantValue> VV>
            nonvirtual Ptr Append (const NameWithNamespace& eltName, const VV& value);

        public:
            /**
             *  \brief Trivial wrapper on AppendElement, but if v is missing then this is a no-op.
             * 
             *  Trivial, but I've found helpful in making certain uses more terse.
             * 
             *  VariantValue overload maps nullptr/nullopt=v to 'missing' case above, but otherwise calls v.As<String>() which may fail depending on the type of v.
             * 
             *  \req *this != nullptr
             */
            nonvirtual Ptr AppendIf (const NameWithNamespace& eltName, const optional<String>& v);
            template <same_as<VariantValue> VV>
            nonvirtual Ptr AppendIf (const NameWithNamespace& eltName, const VV& v);

        public:
            [[deprecated]] Ptr AppendIfNotEmpty (const NameWithNamespace& eltName, const optional<String>& v)
            {
                return AppendIf (eltName, v == nullopt or v->empty () ? nullopt : v);
            }

        public:
            /**
             * creates a new (empty) node with same argument name (and namespace, and in the same position
             * 
             *  This can be used to delete all the children/content under a given node and is equivalent, except that it returns
             *  a new NodePtr, and invalidates this.
             * 
             *  Note - this CAN be used to replace the document root (same as Document::Ptr{}.ReplaceRootElement()).
             * 
             *  \req *this != nullptr
             */
            nonvirtual Ptr Replace (const NameWithNamespace& newEltName);

        public:
            /**
             *  Can return nullptr
             * 
             *  \req *this != nullptr
             */
            nonvirtual Ptr GetParent () const;

        public:
            /**
             *  note only returns sub-elements, so use Node::Ptr (inherited) GetChildren to get them all
             *
             *  \warning Any modification of the DOM may invalidate live iterators or iterables, so re-fetch after each change
             * 
             *  \req *this != nullptr
             */
            nonvirtual Iterable<Node::Ptr> GetChildNodes () const;

        public:
            /**
             *  note only returns sub-elements, so use Node::Ptr (inherited) GetChildren to get them all
             *
             *  \warning Any modification of the DOM may invalidate live iterators or iterables, so re-fetch after each change
             * 
             *  \req *this != nullptr
             */
            nonvirtual Iterable<Ptr> GetChildElements () const;

        public:
            /**
             * can return a NULL Node Ptr if not found. Only examines this node's (direct) children elements (not attributes)
             * 
             *  \req *this != nullptr
             */
            nonvirtual Ptr GetChild (const NameWithNamespace& eltName) const;

        public:
            /**
             * can return a NULL Node Ptr if not found. Only examines this node's (direct) children
             * 
             *  \req *this != nullptr
             */
            nonvirtual Ptr GetChildByID (const String& id) const;

        public:
            /**
             *  \note same as Lookup(), but returns 0 or 1 result, instead of iterable of all of them (so ignores e.GetOptions.fSnapshot).
             *  Often more performant and easier to use (if you know zero or one matching element).
             * 
             *  \req *this != nullptr
             */
            nonvirtual Element::Ptr LookupOneElement (const XPath::Expression& e) const;

        public:
            /**
             *  \note same as Lookup(), but returns 0 or 1 result, instead of iterable of all of them (so ignores e.GetOptions.fSnapshot).
             *  Often more performant and easier to use (if you know zero or one matching element).
             * 
             *  \req *this != nullptr
             */
            nonvirtual Node::Ptr LookupOneNode (const XPath::Expression& e) const;

        public:
            /**
             *  PROBABLY quite unsafe to modify DOM while holding this result etc... Needs work on whehn/what is allowed.
             *  NOTE MORE SAFE IF expression uses 'snapshot'
             * 
             *  \req *this != nullptr
             */
            nonvirtual Traversal::Iterable<XPath::Result> Lookup (const XPath::Expression& e) const;

        public:
            /**
             *  PROBABLY quite unsafe to modify DOM while holding this result etc... Needs work on whehn/what is allowed.
             *  NOTE MORE SAFE IF expression uses 'snapshot'
             * 
             *  \note Same as Lookup - but filters out all non-element nodes
             * 
             *  \req *this != nullptr
             */
            nonvirtual Traversal::Iterable<Element::Ptr> LookupElements (const XPath::Expression& e) const;

        public:
            /**
             *  \brief return the associated shared_ptr (cannot be nullptr)
             * 
             *  \ens result != nullptr
             */
            nonvirtual shared_ptr<IRep> GetRep () const;

        public:
            /**
             *  \brief return the associated shared_ptr (can be nullptr)
             */
            nonvirtual shared_ptr<IRep> PeekRep () const;
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
            // LookupOne returns zero or one results (nullopt or XPath::Result)
            virtual optional<XPath::Result> LookupOne (const XPath::Expression& e) = 0;
            // Lookup returns full iterator of all results (whether live or snapshot depends on params in e)
            virtual Traversal::Iterable<XPath::Result> Lookup (const XPath::Expression& e) = 0;
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
            Ptr () = default;
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
            bool operator== (nullptr_t) const;

        public:
            nonvirtual bool GetStandalone () const;

        public:
            nonvirtual void SetStandalone (bool standalone);

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
             *  If there exists a root element, this is the same as GetRootElement ().Replace (arg), except for the childrenInheritNS part)
             * 
             *  \note this creates an xmlns=NS attribute on the root element if a namespace is given in newEltName, so that the namespace
             *        is inherited by subelements.
             */
            nonvirtual Element::Ptr ReplaceRootElement (const NameWithNamespace& newEltName, bool childrenInheritNS = true) const;

        public:
            /**
             *  \brief shorthand for GetRootElement ().LookupOneElement (e) - so requires root element exists
             */
            nonvirtual Element::Ptr LookupOneElement (const XPath::Expression& e) const;

        public:
            /**
             *  \brief shorthand for GetRootElement ().Lookup (e) - so requires root element exists
             */
            nonvirtual Traversal::Iterable<XPath::Result> Lookup (const XPath::Expression& e) const;

        public:
            /**
             *  \brief shorthand for GetRootElement ().LookupElements (e) - so requires root element exists
             */
            nonvirtual Traversal::Iterable<Element::Ptr> LookupElements (const XPath::Expression& e) const;

        public:
            /**
             */
            nonvirtual Characters::String ToString () const;

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
         *  \note Document::Ptr overload is a way to 'clone' a Document (but even possibly across provider implementations)
         *  \note 'in' stream may be null to create a document with no root element (why is this useful?).
         * 
         * @todo add overload taking String 'in' and parse using Streams::TextToByteReader
         *  @todo consider adding 'Resolver' argument - so missing #includes get loaded.
         * 
         *  If not otherwise specified (e..g if not read from stream where specified otherwise), XML documents default to standalone (=yes)
         */
        Ptr New (const Providers::IDOMProvider& p);
        Ptr New (const Providers::IDOMProvider& p, const NameWithNamespace& documentElementName);
        Ptr New (const Providers::IDOMProvider& p, const Streams::InputStream::Ptr<byte>& in);
        Ptr New (const Providers::IDOMProvider& p, const Streams::InputStream::Ptr<byte>& in, const Schema::Ptr& schemaToValidateAgainstWhileReading);
        Ptr New (const Providers::IDOMProvider& p, const String& in);
        Ptr New (const Providers::IDOMProvider& p, const String& in, const Schema::Ptr& schemaToValidateAgainstWhileReading);
        Ptr New (const Providers::IDOMProvider& p, const Ptr& clone);
#if qStroika_Foundation_DataExchange_XML_SupportDOM
        Ptr New ();
        Ptr New (const NameWithNamespace& documentElementName);
        Ptr New (const Streams::InputStream::Ptr<byte>& in);
        Ptr New (const Streams::InputStream::Ptr<byte>& in, const Schema::Ptr& schemaToValidateAgainstWhileReading);
        Ptr New (const String& in);
        Ptr New (const String& in, const Schema::Ptr& schemaToValidateAgainstWhileReading);
        Ptr New (const Ptr& clone);
#endif

        /**
         */
        struct IRep {
            virtual ~IRep ()                                                       = default;
            virtual const Providers::IDOMProvider* GetProvider () const            = 0;
            virtual bool                           GetStandalone () const          = 0;
            virtual void                           SetStandalone (bool standalone) = 0;
            virtual Element::Ptr                   ReplaceRootElement (const NameWithNamespace& newEltName, bool childrenInheritNS)   = 0;
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
