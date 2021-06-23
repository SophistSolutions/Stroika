/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_ORM_Schema_h_
#define _Stroika_Foundation_Database_SQL_ORM_Schema_h_ 1

#include "../../../StroikaPreComp.h"

#include <optional>

#include "../../../Containers/Collection.h"
#include "../../../Containers/Mapping.h"
#include "../../../DataExchange/VariantValue.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
 *
 *  TODO
 *      @todo   NOTE - this 'schema appropach of picking out important attributes and rolling rest into a
 *              BLOB might be used to turn a keystore DB into a JSON Document DB (not completely but a big
 *              step).
 * 
 *      @todo redo in light of JSON1 extension to sqlite
 */

namespace Stroika::Foundation::Database::SQL::ORM {

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
            String fName;

            /*
             *  Only used in mapping to/from DB representation
             */
            optional<String> fVariantValueFieldName;

            bool                         fRequired{false};
            optional<VariantValue::Type> fVariantType;
            optional<String>             fTypeName;
            optional<bool>               fIsKeyField;
            optional<String>             fForeignKeyToTable;
            optional<String>             fDefaultExpression;
            optional<bool>               fNotNull;

            nonvirtual String GetVariantValueFieldName () const;
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

            function<VariantValue (const Mapping<String, VariantValue>& fields2Map)> GetEffectiveRawToCombined () const;
            function<Mapping<String, VariantValue> (const VariantValue& map2Fields)> GetEffectiveCombinedToRaw () const;

            static VariantValue                  kDefaultMapper_RawToCombined_BLOB (const Mapping<String, VariantValue>& fields2Map);
            static VariantValue                  kDefaultMapper_RawToCombined_String (const Mapping<String, VariantValue>& fields2Map);
            static Mapping<String, VariantValue> kDefaultMapper_CombinedToRaw_BLOB (const VariantValue& fields2Map);
            static Mapping<String, VariantValue> kDefaultMapper_CombinedToRaw_String (const VariantValue& fields2Map);
        };

        /**
         */
        struct Table {
            /**
             *   table name
             */
            String                  fName;
            Collection<Field>       fNamedFields;
            optional<CatchAllField> fSpecialCatchAll;

            /**
             *  Note - this looks for fieldnames as provided by Field::GetVariantValueFieldName () and 
             *  returns field-names as provided by Field::fName.
             */
            nonvirtual Mapping<String, VariantValue> MapToDB (const Mapping<String, VariantValue>& fields) const;

            /**
             *  Note - this looks for fieldnames as provided by Field::fName and 
             *  returns field-names as provided by Field::GetVariantValueFieldName().
             */
            nonvirtual Mapping<String, VariantValue> MapFromDB (const Mapping<String, VariantValue>& fields) const;

            /**
             *  For now only support a single-field ID, and make it optional.
             */
            nonvirtual optional<Field> GetIDField () const;
        };

        /**
         */
        struct StandardSQLStatements {
            Table fTable;

            /**
             * A future version will take parameter to capture differences between
             *  differnet SQL implementations so can generate differnt types etc.
             */
            nonvirtual String CreateTable () const;

            /**
             */
            nonvirtual String Insert () const;

            /**
             *  Return SQL to delete record by ID parameter.
             */
            nonvirtual String DeleteByID () const;

            /**
             *  Return SQL to get record by ID parameter.
             */
            nonvirtual String GetByID () const;

            /**
             *  Return SQL to update record by ID parameter (and set all the rest - same params as Insert).
             */
            nonvirtual String UpdateByID () const;

            /**
             *  @todo add overload taking restrict to list of args projection and maybe arg for filterby
             */
            nonvirtual String GetAllElements () const;
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

#endif /*_Stroika_Foundation_Database_SQL_ORM_Schema_h_*/
