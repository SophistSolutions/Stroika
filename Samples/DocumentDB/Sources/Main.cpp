/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>
#include <iostream>

#include "Stroika/Foundation/Database/Document/MongoDBClient.h"
#include "Stroika/Foundation/Database/Document/SQLite.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"
#include "Stroika/Foundation/Memory/Optional.h"

#include "ComputerNetwork.h"
#include "EmployeesDB.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::Execution;

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    using namespace Database::Document;

    using namespace Stroika::Samples::Document;

#if qStroika_HasComponent_mongocxxdriver
    optional<String> mongoConnectionString;
#endif

    // handle command-line arguments
    {
        using namespace StandardCommandLineOptions;
        const CommandLine::Option kMongoConnectionStringOption_{
            .fLongName         = "mongoConnectionString"sv,
            .fSupportsArgument = true,
            .fHelpOptionText = "Connect to this mongo database for testing, eg: mongodb://admin:pass@localhost:27017; OR ENV VAR MONGO_CONNECTION_STRING"sv};

        const initializer_list<CommandLine::Option> kAllOptions_{kHelp,
#if qStroika_HasComponent_mongocxxdriver
                                                                 kMongoConnectionStringOption_
#endif
        };

        CommandLine cmdLine{argc, argv};
        if (cmdLine.Has (kHelp)) {
            cerr << CommandLine::GenerateUsage ("documentDB", kAllOptions_);
            return EXIT_SUCCESS;
        }
        if (auto error = cmdLine.ValidateQuietly (kAllOptions_)) {
            cerr << "{}"_f(*error) << endl;
            cerr << CommandLine::GenerateUsage ("myApp", kAllOptions_) << endl;
        }
#if qStroika_HasComponent_mongocxxdriver
        mongoConnectionString = Memory::Or_Else (cmdLine.GetArgument (kMongoConnectionStringOption_),
                                                 [] () { return kEnvironment->Lookup ("MONGO_CONNECTION_STRING"sv); });
#endif
    }

#if qStroika_HasComponent_mongocxxdriver
    Database::Document::MongoDBClient::Activator activator{}; // must exist while using mongocxxclient library
#endif

#if qStroika_HasComponent_mongocxxdriver
    if (not mongoConnectionString) {
        cerr << "Warning: skipping mongodb test because no connection string specified on command-line nor environment variable" << endl;
    }
    if (mongoConnectionString) {
        using namespace Stroika::Foundation::Database::Document::MongoDBClient;
        const String kTestDBName_ = "DocumentDB-Sample-Networks"sv;
        auto         adminDB      = AdminConnection::New (AdminConnection::Options{.fConnectionTarget = *mongoConnectionString});
        IgnoreExceptionsForCall (adminDB.DropDatabase (kTestDBName_));
        adminDB.CreateDatabase (kTestDBName_);
        Database::Document::Connection::Ptr p = MongoDBClient::Connection::New (
            MongoDBClient::Connection::Options{.fConnectionTarget = *mongoConnectionString, .fDatabase = kTestDBName_});
        cerr << "Starting mongodb networks sample:" << endl;
        ComputerNetworksModel ([=] () {
            cerr << "\tConnecting to {} database {}"_f(*mongoConnectionString, kTestDBName_) << endl;
            return MongoDBClient::Connection::New (MongoDBClient::Connection::Options{.fConnectionTarget = *mongoConnectionString, .fDatabase = kTestDBName_});
        });
        cerr << "done." << endl;
    }
    if (mongoConnectionString) {
        using namespace Stroika::Foundation::Database::Document::MongoDBClient;
        const String kTestDBName_ = "DocumentDB-Sample-Employees"sv;
        auto         adminDB      = AdminConnection::New (AdminConnection::Options{.fConnectionTarget = *mongoConnectionString});
        IgnoreExceptionsForCall (adminDB.DropDatabase (kTestDBName_));
        adminDB.CreateDatabase (kTestDBName_);
        Database::Document::Connection::Ptr p = MongoDBClient::Connection::New (
            MongoDBClient::Connection::Options{.fConnectionTarget = *mongoConnectionString, .fDatabase = kTestDBName_});
        cerr << "Starting mongodb employees sample:" << endl;
        EmployeesDB ([=] () {
            cerr << "\tConnecting to {} database {}"_f(*mongoConnectionString, kTestDBName_) << endl;
            return MongoDBClient::Connection::New (MongoDBClient::Connection::Options{.fConnectionTarget = *mongoConnectionString, .fDatabase = kTestDBName_});
        });
        cerr << "done." << endl;
    }
#endif

#if qStroika_HasComponent_sqlite
    // quick tests with in-memory DB
    {
        const String kTestDBName_ = "DocumentDB-Sample-Networks"sv;
        cerr << "Starting sqlite documentdb networks sample:" << endl;
        ComputerNetworksModel ([=] () {
            cerr << "\tConnecting to sqlite memory db: {}"_f(kTestDBName_) << endl;
            return SQLite::Connection::New (SQLite::Connection::Options{.fInMemoryDB = kTestDBName_});
        });
        cerr << "done." << endl;
    }
    {
        const String kTestDBName_ = "DocumentDB-Sample-Employees"sv;
        cerr << "Starting sqlite documentdb employees sample:" << endl;
        EmployeesDB ([=] () {
            cerr << "\tConnecting to sqlite memory db: {}"_f(kTestDBName_) << endl;
            return SQLite::Connection::New (SQLite::Connection::Options{.fInMemoryDB = kTestDBName_, .fBusyTimeout = 100ms});
        });
        cerr << "done." << endl;
    }
    // or run same test on filesystem
    {
        auto dbPath = IO::FileSystem::WellKnownLocations::GetTemporary () / "networks-test.db";
        remove (dbPath); // test assumes empty db
        cerr << "Starting sqlite documentdb networks sample:" << endl;
        ComputerNetworksModel ([=] () {
            cerr << "\tConnecting to sqlite  db: {}"_f(dbPath) << endl;
            return SQLite::Connection::New (SQLite::Connection::Options{.fDBPath = dbPath});
        });
        cerr << "done." << endl;
    }
    {
        auto dbPath = IO::FileSystem::WellKnownLocations::GetTemporary () / "employees-test.db";
        remove (dbPath); // test assumes empty db
        cerr << "Starting sqlite documentdb employees sample:" << endl;
        EmployeesDB ([=] () {
            cerr << "\tConnecting to sqlite  db: {}"_f(dbPath) << endl;
            // works poorly with 100ms busyTimeout, but better than any other value - what am I missing!
            auto c = SQLite::Connection::New (SQLite::Connection::Options{.fDBPath = dbPath, .fBusyTimeout = 100ms});
            return c;
        });
        cerr << "done." << endl;
    }
#endif

    return EXIT_SUCCESS;
}
