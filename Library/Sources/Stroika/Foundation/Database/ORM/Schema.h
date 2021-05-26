/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_ORM_Schema_h_
#define _Stroika_Foundation_Database_ORM_Schema_h_ 1

#include "../../StroikaPreComp.h"

#include <optional>

#include "../../Containers/Collection.h"
#include "../../Containers/Mapping.h"
#include "../../DataExchange/VariantValue.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
 *
 *  TODO
 *      @todo   NOTE - this 'scehma appropach of picking out important attributes and rolling rest into a
 *              BLOB might be used to turn a keystore DB into a JSON Document DB (not completely but a big
 *              step).
 */

namespace Stroika::Foundation::Database::ORM {

    using namespace Containers;
    using namespace DataExchange;

    namespace Schema {

        /**
         */
        struct Field {
            /*
             *  This is the database field name, and by default also the name of the field
             *  mapped to in the VariantValue
             */
            String                       fName;
            optional<String>             fVariantValueFieldName;
            bool                         fRequired{false};
            optional<VariantValue::Type> fVariantType;
            optional<String>             fTypeName;
            optional<bool>               fIsKeyField;
            optional<String>             fForeignKeyToTable;
            optional<String>             fDefaultExpression;
            optional<bool>               fNotNull;
        };

        /**
         */
        struct CatchAllField : Field {

            /**
             */
            CatchAllField ();

            /*
             *  add extra properties like transform method
             *  DEFAULT IS XXX - maps by serializing to JSON and storing as BLOB (or maybe text)
             */
            function<VariantValue (const Mapping<String, VariantValue>& fields2Map)> fMapRawFieldsToCombinedField;
            function<Mapping<String, VariantValue> (const VariantValue& map2Fields)> fMapCombinedFieldToRawFields;

            static VariantValue                  kDefaultMapper_RawToCombined (const Mapping<String, VariantValue>& fields2Map);
            static Mapping<String, VariantValue> kDefaultMapper_CombinedToRaw (const VariantValue& fields2Map);
        };

        /**
         */
        struct Table {
            String                  fName; // table name
            Collection<Field>       fNamedFields;
            optional<CatchAllField> fSpecialCatchAll;

            /**
             */
            nonvirtual VariantValue MapToDB (const VariantValue& vv) const;

            /**
             */
            nonvirtual VariantValue MapFromDB (const VariantValue& vv) const;

            /**
             * A future version will take parameter to capture differences between
             *  differnet SQL implemtnations so can generate differnt types etc.
             */
            nonvirtual String GetSQLToCreateTable () const;
        };

/**
        * &&& multiple table set
        * &&&& NOT SURE HOW TO HANDLE THIS - things like cross-tables etc...
         */
#if 0
        struct Tables {
            Collection<Table> fTables;

            VariantValue MapToDB (VariantValue vv);
            VariantValue MapFromDB (VariantValue vv);
        };
#endif
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Schema.inl"

#endif /*_Stroika_Foundation_Database_ORM_Schema_h_*/
