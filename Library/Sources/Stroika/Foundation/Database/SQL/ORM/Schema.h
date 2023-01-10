/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
 * 
 *      @todo   OLD todo note from ODBCClient code:
 *          o   review functionality of ADO, and make sure ORM layer does most of the same stuff (when coupled with ODBCClient)
 *          o   THEN - as a totally separate (layered on top) matter, provide a FRAMEWORK LAYER "ORM" - which provides mapping to C++ objects, in a
 *              maner vaguely similar to SQLAlchemy.
 *              >   Define MAPPERS - which specify a correspondence between C++ struct and database 'variant objects' from above.
 *
 *              >   Use virtual class with SharedByValue<> rep popinters to actual C++ objects, so C++-side objects all GC'd, and can be constructed by
 *                  mapping layer.
 *
 *              >   Avoid circular dependencies by having 'set' relationships managed by central object (or faked trough mtohod calls on smart object),
 *                  but not throgh direct pointers (just ids).
 *
 *              >   SharedByValue<> important, for update semantics - cuz core mapping layer can cache/store orig objects whcih can be copied, and then 'updated'
 *                  by save operation.
 *
 *              >   COULD autogenerate templates from SQL schema, but also can use user-provided ones to generate a schema, or 'hook up' and dynamcially gneerate
 *                  runtime error of C++ template specification of related types doesn't match that read back from DB dynamically.
 */

namespace Stroika::Foundation::Database::SQL::ORM {

    using namespace Containers;
    using namespace DataExchange;

    namespace Schema {

        /**
         * @brief Provide enough information about a field of a c++ VariantValue object to map it to a field in the SQL database.
         * 
         * This is used to automate provisioning (defining the schema) for an SQL database, and used to help automate mapping
         * of VariantValue objects to/from SQL database rows.
         */
        struct Field {
            /*
             *  This is the database field name, and by default also the name of the field
             *  mapped to in the VariantValue;
             * 
             *  \note - this is called fName (and not fSQLName) since it is required and generally the same
             *          for the SQL fields and the VariantValue fields.
             */
            String fName;

            /**
             *  \brief Variant Value field name (if missing, use fName)
             * 
             *  Only used in mapping to/from DB representation.
             */
            optional<String> fVariantValueName;

            /**
             * if required, implies NOT NULL in db schema.
             * synonym for not nullable;
             * 
             * NOTE a value can be Required and not provided if
             * 'default-value' is provided (for adding new objects, not for
             * what is returned from DB). Also allowed if fVariantType is integer, and fIsKeyField and then default is 'autocomputed'
             */
            bool fRequired{false};

            /**
             *  This is the type of the VariantValue object and is used as a hint for the SQL typename (if not provided).
             */
            optional<VariantValue::Type> fVariantValueType;

            /**
             *  SQL type name for the field. If not provided, inferred from fVariantType.
             */
            optional<String> fSQLTypeName;

            /**
             *  If fIsKeyField, then fRequired must be true.
             */
            bool fIsKeyField{false};

            /**
             *  Use to instruct the database to check referential integrity (so argument to foriegnKey statement)
             */
            optional<String> fForeignKeyToTable;

            /**
             *  Note, this can be the special (sentinal) value kDefaultExpression_AutoIncrement
             */
            optional<String> fDefaultExpression;

            /**
             *  Sentinal value for fDefaultExpression, meaning the default value is 1 + the previous max for the field.
             */
            static inline constexpr string_view kDefaultExpression_AutoIncrement = "<<auto-increment>>"sv;

            /**
             *  Each field has two names - the field name in the object map and the fieldname used in the database.
             *  This returns the one from the object representation (result of model->ToObject()).
             */
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
             * 
             *  This can return a different number of values (.size of arg maybe != .size of result), and it can change values of associated fields.
             */
            nonvirtual Mapping<String, VariantValue> MapToDB (const Mapping<String, VariantValue>& fields) const;

            /**
             *  Note - this looks for fieldnames as provided by Field::fName and 
             *  returns field-names as provided by Field::GetVariantValueFieldName().
             * 
             *  This can return a different number of values (.size of arg maybe != .size of result), and it can change values of associated fields.
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
