/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
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
     * In a document database, a document is a mapping of field names to values (slightly differnt than just a variantvalue)
     */
    using Document = Mapping<String, VariantValue>;

    /**
      * \note this special value kID is used to identify the ID field in a document. Its value is "_id" (in mongodb, but in this API, its "id")
      * ?????? dont want to hardwire queer choice of mongodb, but dont wnat to have todo alot of needless mapping/translation later. RETHINK!!!
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
