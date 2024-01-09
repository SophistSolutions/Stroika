/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
#include "Resource.h"

/**
 *  \file
 */

namespace Stroika::Foundation::DataExchange::XML {
    using Containers::Sequence;
    using IO::Network::URI;
    using Memory::BLOB;
}
namespace Stroika::Foundation::DataExchange::XML::DOM::Document {
    class Ptr;
}
namespace Stroika::Foundation::DataExchange::XML::Providers {
    struct IDOMProvider;
    struct ISchemaProvider;
};

namespace Stroika::Foundation::DataExchange::XML::Schema {

    /**
     *  There is more internally to a SchemaRep (perhaps should add method to extract it as a DOM)?
     * 
     *  But mostly, used for internal private data, and that is captured with dynamic_cast, but privately internally
     */
    struct IRep {
        virtual const Providers::ISchemaProvider* GetProvider () const        = 0;
        virtual optional<URI>                     GetTargetNamespace () const = 0;
        //    virtual NamespaceDefinitionsList          GetNamespaceDefinitions () const = 0;
        // not super useful, except if you want to clone
        virtual Memory::BLOB GetData () = 0;
        // not super useful, except if you want to clone
        virtual Resource::ResolverPtr GetResolver () = 0;
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
         *  A schema may target either no namespace (to validate a document with no namespace), or a specific namespace.
         */
        nonvirtual optional<URI> GetTargetNamespace () const;

    public:
        /**
         */
        nonvirtual shared_ptr<IRep> GetRep () const;

    public:
        /**
         *  The schema can be best thought of as a set of rules (for validating) described by a text file (the .xsd file).
         *  This method can return that as a BLOB (for now default encoded), a String (for easy viewing/display), or as a DOM object.
         * 
         *  This can be used to extract the schema as a DOM object (with no connection then to the original schema, its a copy). Or it can
         *  be used to extract the text for the Schema (or BLOB used to construct it), or a Schema using a different backend provider.
         * 
         *  None of the resulting objects retain any tie (except possibly reference to the same IProvider) from the original schema Ptr object.
         */
        template <typename AS_T>
        nonvirtual AS_T As ()
            requires (same_as<AS_T, String> or same_as<AS_T, XML::DOM::Document::Ptr> or same_as<AS_T, Memory::BLOB>);
        template <typename AS_T>
        nonvirtual AS_T As (const Providers::ISchemaProvider& p)
            requires (same_as<AS_T, XML::Schema::Ptr>);
        template <typename AS_T>
        nonvirtual AS_T As (const Providers::IDOMProvider& p)
            requires (same_as<AS_T, XML::DOM::Document::Ptr>);

    private:
        shared_ptr<IRep> fRep_;
    };

    /**
     */
    Ptr New (const Providers::ISchemaProvider& p, const BLOB& schemaData, const Resource::ResolverPtr& resolver = nullptr);
#if qStroika_Foundation_DataExchange_XML_SupportSchema
    Ptr New (const BLOB& schemaData, const Resource::ResolverPtr& resolver = nullptr);
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
