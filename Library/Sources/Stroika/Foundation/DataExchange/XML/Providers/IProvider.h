/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Providers_IProvider_h_
#define _Stroika_Foundation_DataExchange_XML_Providers_IProvider_h_ 1

#include "../../../StroikaPreComp.h"

#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Streams/InputStream.h"

#include "../Common.h"
#include "../DOM.h"
#include "../SAXReader.h"
#include "../Schema.h"

/**
 *  \file
 * 
 *  Generally ignore this file/module unless you need to control which XML provider you are using
 *  and/or to access internal representations from one of these providers.
 *
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::DataExchange::XML::Providers {

    /**
     *  Abstract interface generally unused EXCEPT to compare that the ISchemaProvider POINTER EQUALS the IDOMProvider pointer.
     */
    struct IProvider {
        virtual ~IProvider () = default;
    };

    struct ISchemaProvider : public virtual IProvider {
        virtual ~ISchemaProvider ()                                                                  = default;
        virtual shared_ptr<Schema::IRep> SchemaFactory (const Streams::InputStream::Ptr<byte>& schemaData,
                                                        const Resource::ResolverPtr&           resolver) const = 0;
    };
    struct IDOMProvider : public virtual IProvider {
        virtual ~IDOMProvider ()                                                                                               = default;
        virtual shared_ptr<DOM::Document::IRep> DocumentFactory (const NameWithNamespace& documentElementName) const           = 0;
        virtual shared_ptr<DOM::Document::IRep> DocumentFactory (const Streams::InputStream::Ptr<byte>& in,
                                                                 const Schema::Ptr& schemaToValidateAgainstWhileReading) const = 0;
    };
    struct ISAXProvider : public virtual IProvider {
        virtual ~ISAXProvider ()                                = default;
        virtual void SAXParse (const Streams::InputStream::Ptr<byte>& in, StructuredStreamEvents::IConsumer* callback,
                               const Schema::Ptr& schema) const = 0;
    };
    struct IXMLProvider : ISchemaProvider, IDOMProvider, ISAXProvider {};

#if qStroika_Foundation_DataExchange_XML_SupportParsing and qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportDOM
    namespace Private_ {
        const IXMLProvider* GetDefaultProvider_ ();
    }
#endif

    /**
     *  Provided as a property so can be ...??? so used before main - and still destroyed properly... without forcing include of libxml2/xerces stuff
     */
#if qStroika_Foundation_DataExchange_XML_SupportParsing and qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportDOM
    inline const Foundation::Common::ConstantProperty<const IXMLProvider*> kDefaultProvider{Private_::GetDefaultProvider_};
#endif

}

#endif /*_Stroika_Foundation_DataExchange_XML_Providers_IProvider_h_*/
