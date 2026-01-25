/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>
#include <random>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/Database/Document/Connection.h"
#include "Stroika/Foundation/Database/Document/ObjectCollection.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"

#include "ComputerNetwork.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::Database::Document;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Time;

namespace {

    /**
     * Define the c++ objects which will be persisted.
     */
    namespace Model {
        struct Device {
            optional<IDType> id; // use DocumentDB::IDType - to generically represent IDs in a way that works across all document DBs (and web service apis)
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

        /*
         *  Add dependency mappers
         */
        mapper.AddCommonType<Set<int>> ();
        mapper.AddCommonType<Set<String>> ();

        /*
         *  Add mapper for our user defined type we will be storing.
         */
        mapper.AddClass<Device> ({
            {"id"sv, &Device::id},
            {"name"sv, &Device::name},
            {"openPorts"sv, &Device::openPorts},
            {"hardwareAddresses"sv, &Device::hardwareAddresses},
        });

        return mapper;
    }();
}

void Stroika::Samples::Document::ComputerNetworksModel (const function<Connection::Ptr ()>& connectionFactory)
{
    /*
     *  Create a database connection, and then a connection to a particular table (collection).
     */
    using Model::Device;
    auto dbConnection = connectionFactory ();
    ObjectCollection::Ptr<Device> deviceConnection = ObjectCollection::New<Device> (dbConnection.CreateCollection ("Networks"), Device::kMapper);

    if (not deviceConnection.GetAll ().empty ()) {
        Throw (RuntimeErrorException{"database should start empty"});
    }

    /*
     *  Create two c++ objects, perist them, delete 1, and check reading back we get the right results.
     */
    Device device1_ = Device{.openPorts = {33}, .name = "myLaptop"sv, .hardwareAddresses = {"ff:33:aa:da:ff:33"_k}};
    Device device2_ = Device{.openPorts = {123, 145}, .name = "some machine"sv, .hardwareAddresses = {"33:aa:dd:ad:af:11"_k}};
    device1_.id     = deviceConnection.Add (device1_);
    device2_.id     = deviceConnection.Add (device2_);
    {
        auto devices = deviceConnection.GetAll ();
        if (devices.size () != 2) {
            Throw (RuntimeErrorException{"we should have the ones we just added"sv});
        }
        if (not devices.Contains (device1_)) {
            Throw (RuntimeErrorException{"we should have the ones we just added{1}"sv});
        }
        if (not devices.Contains (device2_)) {
            Throw (RuntimeErrorException{"we should have the ones we just added{2}"sv});
        }
    }
    deviceConnection.Remove (Memory::ValueOf (device2_.id));
    {
        auto devices = deviceConnection.GetAll ();
        if (devices.size () != 1) {
            Throw (RuntimeErrorException{"we should have the ones we just added"});
        }
        if (not devices.Contains (device1_)) {
            Throw (RuntimeErrorException{"we should have kDevice1_"});
        }
    }
}
