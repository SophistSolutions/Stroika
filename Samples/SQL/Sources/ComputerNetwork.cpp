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
#include "Stroika/Foundation/Database/SQL/ORM/Schema.h"
#include "Stroika/Foundation/Database/SQL/ORM/Versioning.h"
#include "Stroika/Foundation/Database/SQL/SQLite.h"
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
using namespace Database::SQL::SQLite;

using SQL::ORM::Schema::StandardSQLStatements;

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

#if __cpp_impl_three_way_comparison >= 201907
            nonvirtual bool operator== (const Device& rhs) const = default;
#else
            nonvirtual bool operator== (const Device& rhs) const
            {
                return id == rhs.id and openPorts == rhs.openPorts and name == rhs.name and hardwareAddresses == rhs.hardwareAddresses;
            }
#endif
        };
    }

    /**
     *  This defines the mapping from C++ to how we would display the object in our 'model' - like via a webservice
     *  or stored in external files, or for debugging.
     */
    const ObjectVariantMapper Model::Device::kMapper = [] () {
        ObjectVariantMapper mapper;

        mapper.AddCommonType<Set<int>> ();
        mapper.AddCommonType<Set<String>> ();

        // ONLY DO THIS FOR WHEN WRITING TO DB -- store GUIDs as BLOBs - at least for database interactions (cuz more efficient)
        mapper.AddCommonType<Common::GUID> (VariantValue::eBLOB);

        mapper.AddClass<Device> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {L"id", StructFieldMetaInfo{&Device::id}},
            {L"name", StructFieldMetaInfo{&Device::name}},
            {L"openPorts", StructFieldMetaInfo{&Device::openPorts}},
            {L"hardwareAddresses", StructFieldMetaInfo{&Device::hardwareAddresses}},
        });

        return mapper;
    }();

    /**
     *  This defines the mapping from our external data model (Device::kMapper) to the SQL data model.
     */
    const SQL::ORM::Schema::Table kDeviceTableSchema_{
        L"Devices",
        /*
         *  use the same names as the ObjectVariantMapper for simpler mapping, or specify an alternate name
         *  for ID, just as an example.
         */
        Collection<SQL::ORM::Schema::Field>{
#if __cpp_designated_initializers
            /**
             *  For ID, generate random GUID (BLOB) automatically in database
             */
            {.fName = L"ID", .fVariantValueFieldName = L"id"sv, .fVariantType = VariantValue::eBLOB, .fIsKeyField = true, .fDefaultExpression = L"randomblob(16)"sv, .fNotNull = true},
            {.fName = L"name", .fVariantType = VariantValue::eString, .fNotNull = true}
#else
            {L"ID", L"id"sv, false, VariantValue::eBLOB, nullopt, true, nullopt, L"randomblob(16)"sv, true},
            {L"name", nullopt, false, VariantValue::eString, nullopt, false, nullopt, nullopt, true}
#endif
        },
        SQL::ORM::Schema::CatchAllField{}};

    Connection::Ptr SetupDB_ (const Options& options)
    {
        auto                             conn             = Connection::New (options);
        constexpr Configuration::Version kCurrentVersion_ = Configuration::Version{1, 0, Configuration::VersionStage::Alpha, 0};
        SQL::ORM::ProvisionForVersion (conn,
                                       kCurrentVersion_,
                                       Traversal::Iterable<SQL::ORM::Schema::Table>{kDeviceTableSchema_});
        return conn;
    }
}

void Stroika::Samples::SQLite::ComputerNetworksModel (const Options& options)
{
    /*
     */
    Connection::Ptr conn = SetupDB_ (options);

    using Model::Device;
    auto addDevice = [&] (const Device& d) {
        Statement addDeviceStatement{conn, StandardSQLStatements{kDeviceTableSchema_}.Insert ()};
        addDeviceStatement.Execute (kDeviceTableSchema_.MapToDB (Device::kMapper.FromObject (d).As<Mapping<String, VariantValue>> ()));
    };
    auto getAllDevices = [&] () -> Sequence<Device> {
        Statement getAllDevicesStatement{conn, StandardSQLStatements{kDeviceTableSchema_}.GetAllElements ()};
        return getAllDevicesStatement.GetAllRows ().Select<Device> ([] (const Statement::Row& r) {
                                                       return Device::kMapper.ToObject<Device> (VariantValue{kDeviceTableSchema_.MapFromDB (r)});
                                                   })
            .As<Sequence<Device>> ();
    };
    auto removeDevice = [&] (const GUID& id) {
        Statement deleteDeviceStatement{conn, StandardSQLStatements{kDeviceTableSchema_}.DeleteByID ()};
        deleteDeviceStatement.Execute (initializer_list<Common::KeyValuePair<String, VariantValue>>{{kDeviceTableSchema_.GetIDField ()->fName, VariantValue{static_cast<Memory::BLOB> (id)}}});
    };

    const Device kDevice1_ = Device{GUID::GenerateNew (), Set<int>{33}, L"myLaptop"sv, Set<String>{L"ff:33:aa:da:ff:33"}};
    const Device kDevice2_ = Device{GUID::GenerateNew (), Set<int>{123, 145}, L"some machine"sv, Set<String>{L"33:aa:dd:ad:af:11"}};
    if (not getAllDevices ().empty ()) {
        Execution::Throw (Execution::RuntimeErrorException{L"database should start empty"});
    }
    addDevice (kDevice1_);
    addDevice (kDevice2_);
    {
        auto devices = getAllDevices ();
        if (devices.size () != 2) {
            Execution::Throw (Execution::RuntimeErrorException{L"we should have the ones we just added"});
        }
        if (not devices.Contains (kDevice1_)) {
            Execution::Throw (Execution::RuntimeErrorException{L"we should have the ones we just added{1}"});
        }
        if (not devices.Contains (kDevice2_)) {
            Execution::Throw (Execution::RuntimeErrorException{L"we should have the ones we just added{2}"});
        }
    }
    removeDevice (kDevice2_.id);
    {
        auto devices = getAllDevices ();
        if (devices.size () != 1) {
            Execution::Throw (Execution::RuntimeErrorException{L"we should have the ones we just added"});
        }
        if (not devices.Contains (kDevice1_)) {
            Execution::Throw (Execution::RuntimeErrorException{L"we should have kDevice1_"});
        }
    }
}
#endif
