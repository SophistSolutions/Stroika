/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_Document_Document_h_
#define _Stroika_Foundation_Database_Document_Document_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"

/**
 *  \file
 * 
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Database::Document {

    using Characters::String;
    using Containers::Mapping;
    using DataExchange::VariantValue;

    /**
     * In a document database, a document is a mapping of field names to values (slightly different than just a VariantValue)
     */
    using Document = Mapping<String, VariantValue>;

    /**
     *  'Type' used for value of 'id' field.
     * 
     *  \note - we considered using GUID, but String is more flexible given that different systems (e.g. XML, MongoDB etc) all might
     *          have different requirements on how to format/size/rules for those IDs. String is lingua-franca.
     */
    using IDType = String;

    /**
     *  \note this special value kID is used to identify the ID field in a document (Mapping::KeyType).
     * 
     *  \note mongodb uses "_id", but in this API, uses "id", so the Stroika/mongodb layer must translate
     *  \note dont want to hardwire queer choice of mongodb, but dont want to have todo a lot of needless mapping/translation later.
     *  \note the VALUE associated with this key is of type IDType.
     */
    static inline const String kID = "id"sv;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Document.inl"

#endif /*_Stroika_Foundation_Database_Document_Document_h_*/
