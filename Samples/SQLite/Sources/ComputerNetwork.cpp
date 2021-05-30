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
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Time;

using Common::GUID;

#if qHasFeature_sqlite
using namespace Database::SQLite;

namespace {
    /// TWO ways to handle sets:
    ///         (1) store as BLOB
    ///         (2) Store in Cross-table

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

        mapper.AddClass<Device> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {L"id", StructFieldMetaInfo{&Device::id}},
            {L"name", StructFieldMetaInfo{&Device::name}},
            {L"openPorts", StructFieldMetaInfo{&Device::openPorts}},
            {L"hardwareAddresses", StructFieldMetaInfo{&Device::hardwareAddresses}},
        });

        return mapper;
    }();

    const ORM::Schema::Table kDeviceTableSchema_{
        L"Devices",
        /*
         *  use the same names as the ObjectVariantMapper for simpler mapping, or specify an alternate name
         *  for ID, just as an example.
         */
        Collection<ORM::Schema::Field>{
#if __cpp_designated_initializers
            /**
             *  For ID, generate random GUID (BLOB) automatically in database
             */
            {.fName = L"ID", .fVariantValueFieldName = L"id"sv, .fVariantType = VariantValue::eBLOB, .fIsKeyField = true, .fDefaultExpression = L"randomblob(16)"sv, .fNotNull = true},
            {.fName = L"name", .fVariantType = VariantValue::eString, .fNotNull = true}
#else
            {L"ID", L"id"sv, false, VariantValue::eBLOB, nullopt, true, nullopt, L"randomblob(16)"sv, true},
            {L"name", nullopt, false, VariantValue::eString, nullopt, nullopt, nullopt, nullopt, true}
#endif
        },
        ORM::Schema::CatchAllField{}};

    Connection::Ptr SetupDB_ (const Options& options)
    {
        auto initializeDB = [] (const Connection::Ptr& c) {
            c.Exec (kDeviceTableSchema_.GetSQLToCreateTable ());
        };
        return Connection::New (options, initializeDB);
    }

    void AddDevice_ (Connection::Ptr conn, const Model::Device& d)
    {
        Statement addDeviceStatement{conn, kDeviceTableSchema_.GetSQLToInsert ()};
        addDeviceStatement.Execute (kDeviceTableSchema_.MapToDB (Model::Device::kMapper.FromObject (d).As<Mapping<String, VariantValue>> ()));
    }

    void AddDevices_OldWay_ (Connection::Ptr conn)
    {
        Statement addDeviceStatement{conn, L"INSERT INTO DEVICES (name) values (:NAME);"};
        addDeviceStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":NAME", L"PLATO"},
        });
        addDeviceStatement.Execute (initializer_list<Statement::ParameterDescription>{
            {L":NAME", L"ROUTER"},
        });
    }

    void AddDevices_ (Connection::Ptr conn)
    {
        AddDevices_OldWay_ (conn);

        AddDevice_ (conn, Model::Device{GUID::GenerateNew (), Set<int>{33}, L"myLaptop"sv, Set<String>{L"ff:33:aa:da:ff:33"}});
        AddDevice_ (conn, Model::Device{GUID::GenerateNew (), Set<int>{123, 145}, L"some machine"sv, Set<String>{L"33:aa:dd:ad:af:11"}});
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
