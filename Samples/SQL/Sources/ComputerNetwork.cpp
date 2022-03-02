/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>
#include <random>
#include <tuple>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/Database/SQL/Connection.h"
#include "Stroika/Foundation/Database/SQL/ORM/Schema.h"
#include "Stroika/Foundation/Database/SQL/ORM/Versioning.h"
#include "Stroika/Foundation/Database/SQL/Statement.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Time/DateTime.h"

#include "ComputerNetwork.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::Database::SQL;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Time;

using Common::GUID;

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

            nonvirtual bool operator== (const Device& rhs) const = default;
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
            {.fName = L"ID", .fVariantValueName = L"id"sv, .fRequired = true, .fVariantValueType = VariantValue::eBLOB, .fIsKeyField = true, .fDefaultExpression = L"randomblob(16)"sv},
            {.fName = L"name", .fRequired = true, .fVariantValueType = VariantValue::eString}
#else
            {L"ID", L"id"sv, true, VariantValue::eBLOB, nullopt, true, nullopt, L"randomblob(16)"sv},
            {L"name", nullopt, true, VariantValue::eString}
#endif
        },
        SQL::ORM::Schema::CatchAllField{}};
}

void Stroika::Samples::SQL::ComputerNetworksModel (const std::function<Connection::Ptr ()>& connectionFactory)
{
    /*
     */
    Connection::Ptr conn = connectionFactory ();

    constexpr Configuration::Version kCurrentVersion_ = Configuration::Version{1, 0, Configuration::VersionStage::Alpha, 0};
    ORM::ProvisionForVersion (conn,
                              kCurrentVersion_,
                              Traversal::Iterable<ORM::Schema::Table>{kDeviceTableSchema_});

    using Model::Device;
    auto addDevice = [&] (const Device& d) {
        Statement addDeviceStatement = conn.mkStatement (StandardSQLStatements{kDeviceTableSchema_}.Insert ());
        addDeviceStatement.Execute (kDeviceTableSchema_.MapToDB (Device::kMapper.FromObject (d).As<Mapping<String, VariantValue>> ()));
    };
    auto getAllDevices = [&] () -> Sequence<Device> {
        Statement getAllDevicesStatement = conn.mkStatement (StandardSQLStatements{kDeviceTableSchema_}.GetAllElements ());
        return getAllDevicesStatement.GetAllRows ().Select<Device> ([] (const Statement::Row& r) {
                                                       return Device::kMapper.ToObject<Device> (VariantValue{kDeviceTableSchema_.MapFromDB (r)});
                                                   })
            .As<Sequence<Device>> ();
    };
    auto removeDevice = [&] (const GUID& id) {
        Statement deleteDeviceStatement = conn.mkStatement (StandardSQLStatements{kDeviceTableSchema_}.DeleteByID ());
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
