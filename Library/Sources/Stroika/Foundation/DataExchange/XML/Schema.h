/*
 * Copyright(c) Records For Living, Inc. 2004-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Schema_h_
#define _Stroika_Foundation_DataExchange_XML_Schema_h_ 1

#include "../../StroikaPreComp.h"

#include <string>

#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Streams/InputStream.h"

#include "DOM.h"

#if qHasFeature_Xerces
namespace xercesc_3_2 {
    class XMLGrammarPool;
}

namespace Stroika::Foundation::DataExchange::XML {

    // Schema objects can be shared among XMLDB Document objects.
    //
    // This class is used BOTH for a schema, and for a namespace. Really - we should separate the
    // concepts, but for simplicity of implementation - for now - leave things that way (--LGP 2008-08-09)
    //
    class Schema {
    public:
        // This is a named BLOB which is used to define a Schema. The BLOB can be named by a variety of
        // types of names (depending on the type of BLOB).
        class SourceComponent {
        public:
            Memory::BLOB fBLOB;
            wstring      fNamespace;
            wstring      fPublicID;
            wstring      fSystemID;
        };

    public:
        Schema (const Schema& from);
        // The targetNamespace argument is optional (we can have a schema with a blank target namespace).
        // The referencedSchema to support load/define the given schema (e.g. for xsd:import directives).
        Schema (const wstring& targetNamespace = wstring (), const vector<SourceComponent>& sources = vector<SourceComponent> ());
        Schema (const wstring& targetNamespace, const vector<SourceComponent>& sources, const vector<NamespaceDefinition>& namespaces);
        ~Schema ();
        Schema& operator= (const Schema& rhs);

    public:
        nonvirtual bool HasSchema () const;

    public:
        nonvirtual vector<SourceComponent> GetSourceComponents () const;
        nonvirtual void                    SetSourceComponents (const vector<SourceComponent>& sources);
        // assumes a single BLOB (with this namespace as target)
        nonvirtual void SetSchemaData (const Memory::BLOB& source);

    public:
        nonvirtual String GetTargetNamespace () const;
        nonvirtual void   SetTargetNamespace (const String& targetNamespace);

    public:
        nonvirtual NamespaceDefinitionsList GetNamespaceDefinitions () const;
        nonvirtual void                     SetNamespaceDefinitions (const NamespaceDefinitionsList& namespaceDefinitions);

    public:
        typedef xercesc_3_2::XMLGrammarPool* T_CompiledXSDRep;

    public:
        class AccessCompiledXSD {
        public:
            AccessCompiledXSD (const Schema& schema2Access);

        public:
            nonvirtual T_CompiledXSDRep GetCachedTRep () const;

        private:
            const Schema&               fSchema2Access;
            lock_guard<recursive_mutex> fEnterCritSection;
        };

    private:
        nonvirtual T_CompiledXSDRep GetCachedTRep_ () const;

    private:
        nonvirtual Memory::BLOB GetSchemaData_ () const;

    private:
        struct SchemaRep;

    private:
        shared_ptr<SchemaRep> fRep;

    private:
        friend class AccessCompiledXSD;
    };

    namespace DOM {
        class Document;
    }

    // helpful utility class to assure a particular schema is set into an XMLDocument for a particular
    // stack-based context
    class TempSchemaSetter {
    public:
        TempSchemaSetter (DOM::Document& doc, const Schema* s);
        ~TempSchemaSetter ();

    private:
        DOM::Document* fDoc;
        const Schema*  fOldSchema;
    };

    void ValidateExternalFile (const filesystem::path& externalFileName, const Schema* schema); // throws BadFormatException exception on error

};
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "Schema.inl"

#endif /*_Stroika_Foundation_DataExchange_XML_Schema_h_*/
