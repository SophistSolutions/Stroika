/*
 * Copyright(c) Records For Living, Inc. 2004-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Schema_h_
#define _Stroika_Foundation_DataExchange_XML_Schema_h_ 1

#include "../../StroikaPreComp.h"

#include <string>

#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Streams/InputStream.h"

#include "DOM.h"

/*
 *  @todo needs alot of cleanups - use shared_ptr not bare pointers!!!! 
 */

#if qStroika_Foundation_DataExchange_XML_SupportSchema
namespace Stroika::Foundation::DataExchange::XML {

    using Containers::Sequence;
    using IO::Network::URI;

    namespace Schema {

        // This is a named BLOB which is used to define a Schema. The BLOB can be named by a variety of
        // types of names (depending on the type of BLOB).
        class SourceComponent {
        public:
            Memory::BLOB     fBLOB;
            optional<URI>    fNamespace;
            optional<String> fPublicID;
            optional<String> fSystemID;
        };

        struct IRep {
            virtual optional<URI>             GetTargetNamespace () const      = 0;
            virtual NamespaceDefinitionsList  GetNamespaceDefinitions () const = 0;
            virtual Sequence<SourceComponent> GetSourceComponents ()           = 0;
        };

        class Ptr {
        public:
            Ptr (nullptr_t)
            {
            }
            Ptr (shared_ptr<IRep> s)
                : fRep_{s}
            {
            }
            Ptr (const Ptr&) = default;

            bool operator== (const Ptr& p) const = default;

        public:
            nonvirtual Sequence<SourceComponent> GetSourceComponents () const
            {
                return fRep_->GetSourceComponents ();
            }

        public:
            nonvirtual optional<URI> GetTargetNamespace () const
            {
                return fRep_->GetTargetNamespace ();
            }

        public:
            nonvirtual NamespaceDefinitionsList GetNamespaceDefinitions () const
            {
                return fRep_->GetNamespaceDefinitions ();
            }

        public:
            shared_ptr<IRep> GetRep () const
            {
                return fRep_;
            }

        public:
            //   nonvirtual Memory::BLOB GetSchemaData () const;    // get as date or DOM maybe better

        private:
            shared_ptr<IRep> fRep_;
        };

        enum class Provider {

            eXerces,
            eDefault = eXerces
        };
        Ptr        New (Provider p, const optional<URI>& targetNamespace, const Memory::BLOB& targetNamespaceData,
                        const Sequence<SourceComponent>& sourceComponents, const NamespaceDefinitionsList& namespaceDefinitions);
        inline Ptr New (const optional<URI>& targetNamespace, const Memory::BLOB& targetNamespaceData,
                        const Sequence<SourceComponent>& sourceComponents = {}, const NamespaceDefinitionsList& namespaceDefinitions = {})
        {
            return New (Provider::eDefault, targetNamespace, targetNamespaceData, sourceComponents, namespaceDefinitions);
        }

    }

    /**
     */
    void ValidateExternalFile (const filesystem::path& externalFileName, const Schema::Ptr& schema); // throws BadFormatException exception on error

};
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "Schema.inl"

#endif /*_Stroika_Foundation_DataExchange_XML_Schema_h_*/
