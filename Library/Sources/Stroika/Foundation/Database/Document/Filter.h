/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_Document_Filter_h_
#define _Stroika_Foundation_Database_Document_Filter_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/Database/Document/Document.h"

/**
 *  \file
 * 
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Database::Document {

    using Containers::Sequence;

    namespace FilterElements {

        /**
         * String like object, but explciitly different type
         */
        struct FieldName : String {
            using String::String;
        };

        /**
         * \note this special value kID is used to identify the ID field in a document. Its value is "_id" (in mongodb, but in this API, its "id")
         * ?????? dont want to hardwire queer choice of mongodb, but dont wnat to have todo alot of needless mapping/translation later. RETHINK!!!
         */
        static inline const FieldName kID = "id"sv;

        /**
          * VariantValue like object, but explciitly different type
          */
        struct Value : VariantValue {
            using VariantValue::VariantValue;
        };

        struct Equals { // equals operator
            FieldName                 fLHS;
            variant<FieldName, Value> fRHS;
            bool                      Matches (const Database::Document::Document& doc) const;
        };

        /**
         *  \todo support a bunch more operators, like Less, and maybe arbitrary function (which obviously runs client side)
         */
        using Operatation = variant<Equals>;

    }

    /**
     * VERY INCOMPLETE
     * 
     * But basic idea is conjunctive normal form.
     * 
     * And elements are the data needed to compute filter value.
     * 
     *  And KEY ones - are ones that can be identified at query time and MAPPED to mongodb query operators, like equal {fieldname, value} - that
     *  would be mapped to a mongo filter, and the remainder computed client side if needed.
     * 
     */
    struct Filter {
        // todo list of function objects - or operator, arglist, with predefined constant functions for equals, and a few others
        // hardwired ones can be passed to mongo, and others applied ex-post-facto

        // conjunctive normal form (CNF)
        Sequence<FilterElements::Operatation> fAndedOperations;

        /**
         */
        bool Matches (const Database::Document::Document& doc) const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Filter.inl"

#endif /*_Stroika_Foundation_Database_Document_Filter_h_*/
