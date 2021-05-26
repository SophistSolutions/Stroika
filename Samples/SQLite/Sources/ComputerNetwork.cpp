/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>
#include <random>
#include <tuple>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/Database/ORM/Schema.h"
#include "Stroika/Foundation/Database/SQLite.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Time/DateTime.h"

#include "ComputerNetwork.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Time;

using Common::GUID;
using DataExchange::ObjectVariantMapper;
using DataExchange::VariantValue;

#if qHasFeature_sqlite
using namespace Database::SQLite;

namespace {
    /// TWO ways to handle sets:
    ///         (1) store as BLOB
    ///         (2) Store in Cross-table

    // see if can mark ID as random (lower(hex(randomblob(16))))
    namespace Model {
        struct Device {
            GUID        id;
            Set<int>    openPorts;
            String      name;
            Set<String> hardwareAddresses;

            static const ObjectVariantMapper kMapper;
        };
    }

    const ObjectVariantMapper Model::Device::kMapper = [] () {
        ObjectVariantMapper mapper;

        mapper.AddCommonType<Set<int>> ();
        mapper.AddCommonType<Set<String>> ();

        DISABLE_COMPILER_MSC_WARNING_START (4573);
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        mapper.AddClass<Device> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {L"id", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Device, id)},
            {L"name", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Device, name)},
            {L"openPorts", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Device, openPorts)},
            {L"hardwareAddresses", Stroika_Foundation_DataExchange_StructFieldMetaInfo (Device, hardwareAddresses)},
        });
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        DISABLE_COMPILER_MSC_WARNING_END (4573);

        return mapper;
    }();

    const ORM::Schema::Table kDeviceTableSchema_{
        L"Devices", Collection<ORM::Schema::Field>{
    /*
         *  use the same names as the ObjectVariantMapper for simpler mapping, or specify an alternate name
         *  for ID, just as an example.
         */
#if __cpp_designated_initializers
                        {.fName = L"ID", .fVariantValueFieldName = L"id"sv, .fVariantType = VariantValue::eBLOB, .fIsKeyField = true, .fDefaultExpression = L"randomblob(16)"sv, .fNotNull = true}, {.fName = L"name", .fVariantType = VariantValue::eString, .fNotNull = true}
#else
                        {L"ID", L"id"sv, false, VariantValue::eBLOB, nullopt, true, nullopt, L"randomblob(16)"sv, true}, {L"name", nullopt, false, VariantValue::eString, nullopt, nullopt, nullopt, nullopt, true}
#endif
                    },
        ORM::Schema::CatchAllField{}};

    Connection::Ptr SetupDB_ (const Options& options)
    {
        auto initializeDB = [] (const Connection::Ptr& c) {
            // Use Connection::Ptr::Exec because no parameter bindings needed
            #if 1
            c.Exec (kDeviceTableSchema_.GetSQLToCreateTable ());
            #else
            c.Exec (
                L"CREATE TABLE Devices("
                L"ID BLOB PRIMARY KEY DEFAULT(randomblob(16)),"
                L"name           TEXT    NOT NULL"
                L");");
            #endif
        };
        Options o      = options;
        o.fBusyTimeout = o.fBusyTimeout.value_or (1s); // default to 1 second busy timeout for these tests
        return Connection::New (o, initializeDB);
    }

    void AddDevices_ (Connection::Ptr conn)
    {
        Statement addDeviceStatement{conn, L"INSERT INTO DEVICES (name) values (:NAME);"};
        addDeviceStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":NAME", L"PLATO"},
        });
        addDeviceStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":NAME", L"ROUTER"},
        });
    }
}

void Stroika::Samples::SQLite::ComputerNetworksModel (const Options& options)
{
    /*
     */
    Connection::Ptr conn = SetupDB_ (options);
    AddDevices_ (conn);
}
#endif
