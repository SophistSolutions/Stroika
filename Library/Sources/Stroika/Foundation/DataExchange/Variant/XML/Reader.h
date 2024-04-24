/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_XML_Reader_h_
#define _Stroika_Foundation_DataExchange_Variant_XML_Reader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/DataExchange/Variant/Reader.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/DataExchange/XML/SerializationConfiguration.h"
#include "Stroika/Foundation/Streams/InputStream.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   NotYetImplemented
 *
 *      @todo   fix thread-safety - cloning rep - lock for access config data
 */

namespace Stroika::Foundation::DataExchange::Variant::XML {

    using DataExchange::XML::SerializationConfiguration;

    /**
     *  @todo
     * add options for stuff like - special for xml - assumed-outer-doc, stuff abotu namespaces, and st
     *... todo namespaces - store in string (elt name a:b) and fill in namespace object accordingly.

        define common struct (shared module - in common.h of this I guess? - for serializeation package parmas
        like "use this tag for arrays" (where empty is an option but hard to read back)", and namespace list, and
        do or don't include xml processing instruction
        */
    class Reader : public Variant::Reader {
    private:
        using inherited = Variant::Reader;

    private:
        class Rep_;

    public:
        Reader (const SerializationConfiguration& config = {});

    public:
        nonvirtual SerializationConfiguration GetConfiguration () const;
        nonvirtual void                       SetConfiguration (const SerializationConfiguration& config);

    private:
        nonvirtual const Rep_& GetRep_ () const;
        nonvirtual Rep_&       GetRep_ ();
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_DataExchange_Variant_XML_Reader_h_*/
