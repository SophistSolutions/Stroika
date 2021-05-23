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
 */

namespace Stroika::Foundation::Database::ORM {

    using namespace Containers;
    using namespace DataExchange;

    namespace Schema {

        /**
         */
        struct Field {
            String                       fName;
            bool                         fRequired{false};
            optional<VariantValue::Type> fVariantType;
            optional<String>             fTypeName;
            optional<bool>               fIsKeyField;
            optional<String>             fForeignKeyToTable;
            optional<bool>               fNotNull;
        };

        /**
         */
        struct CatchAllField : Field {
            /*
             *  add extra properties like transform method
             *  DEFAULT IS XXX - maps by serializing to JSON and storing as BLOB (or maybe text)
             */
            function<VariantValue (const Mapping<String, VariantValue>& fields2Map)> fMapper;
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
