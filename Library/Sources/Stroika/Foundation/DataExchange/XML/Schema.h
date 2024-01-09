/*
 * Copyright(c) Records For Living, Inc. 2004-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Schema_h_
#define _Stroika_Foundation_DataExchange_XML_Schema_h_ 1

#include "../../StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Streams/InputStream.h"

#include "Common.h"
#include "Namespace.h"

/**
 *  \file
 */

namespace Stroika::Foundation::DataExchange::XML {
    using Containers::Sequence;
    using IO::Network::URI;
    using Memory::BLOB;
}
namespace Stroika::Foundation::DataExchange::XML::DOM {
    class Ptr;
}
namespace Stroika::Foundation::DataExchange::XML::Providers {
    struct ISchemaProvider;
};

namespace Stroika::Foundation::DataExchange::XML::Schema {

    /**
     * This is a named BLOB which is used to define a Schema. The BLOB can be named by a variety of
     * types of names (depending on the type of BLOB).
     * 
     *      // @todo CLEANUP DOCS ON SOURCE COMPNENT - FOR NOW I THINK ITS TREATED AS RAW TEXT TO BE PARSED AS XML as an XML FILE.
     */
    class SourceComponent {
    public:
        BLOB             fBLOB;
        optional<URI>    fNamespace;
        optional<String> fPublicID;
        optional<String> fSystemID;
    };

    /**
     *  There is more internally to a SchemaRep (perhaps should add method to extract it as a DOM)?
     * 
     *  But mostly, used for internal private data, and that is captured with dynamic_cast, but privately internally
     */
    struct IRep {
        virtual const Providers::ISchemaProvider* GetProvider () const             = 0;
        virtual optional<URI>                     GetTargetNamespace () const      = 0;
        virtual NamespaceDefinitionsList          GetNamespaceDefinitions () const = 0;
        virtual Sequence<SourceComponent>         GetSourceComponents ()           = 0;
    };

    /**
     *  This is the main way you work with Schema objects - through a smart pointer.
     */
    class Ptr {
    public:
        /**
         */
        Ptr (nullptr_t);
        Ptr (shared_ptr<IRep> s);
        Ptr (const Ptr&) = default;

    public:
        bool operator== (const Ptr& p) const = default;

    public:
        /**
         */
        nonvirtual Sequence<SourceComponent> GetSourceComponents () const;

    public:
        /**
         */
        nonvirtual optional<URI> GetTargetNamespace () const;

    public:
        /**
         */
        nonvirtual NamespaceDefinitionsList GetNamespaceDefinitions () const;

    public:
        /**
         */
        nonvirtual shared_ptr<IRep> GetRep () const;

    public:
        /**
         *  The schema can be best thoguht of as a set of rules (for validating) described by a text file (the .xsd file).
         *  This method can return that as a BLOB (for now default encoded), a String (for easy viewing/display), or as a DOM object.
         * 
         *  Each of these conversions may take specific parameters, not yet specified / allowed here. The reason for the template
         *  is to allow for the clean notation of refering to all of them as schema.As<waht-i-want> (...optional but usually omitted params);
         * 
         *  \note NYI - tricky - cuz must handle subsidiary documents (includes) - and probably define soemthing like SchemaSource or ResolvedSchemaSource
         *        and tools to map (do all the #includes) from a stream with a resolver to ResolvedScehmaSource and then use that to clone/deserialize?
         *        Or maybe just something like that?
         */
        template <typename AS_T>
        AS_T As ()
            requires (same_as<AS_T, String> or same_as<AS_T, XML::DOM::Ptr> or same_as<AS_T, Memory::BLOB>);

    private:
        shared_ptr<IRep> fRep_;
    };

    /**
     *  @todo add API to construct from DOM...
     * 
     *  @todo consider why we have targetNamespace as argument to New () -since it can be and should be parsted out of the document!
     */
    Ptr New (const Providers::ISchemaProvider& p, const BLOB& targetNamespaceData, const Sequence<SourceComponent>& sourceComponents = {},
             const NamespaceDefinitionsList& namespaceDefinitions = {});
#if qStroika_Foundation_DataExchange_XML_SupportSchema
    Ptr New (const BLOB& targetNamespaceData, const Sequence<SourceComponent>& sourceComponents = {},
             const NamespaceDefinitionsList& namespaceDefinitions = {});
#endif

#if qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportParsing
    /**
     */
    void ValidateFile (const filesystem::path& externalFileName, const Ptr& schema); // throws BadFormatException exception on error
#endif

};

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "Schema.inl"

#endif /*_Stroika_Foundation_DataExchange_XML_Schema_h_*/
