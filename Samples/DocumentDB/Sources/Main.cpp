/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cstdlib>
#include <iostream>

#include "Stroika/Foundation/Database/Document/LocalDocumentDB.h"
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
        try {
            static const Activity kActivity_{"performing MongoDBClient test on {}"_f(kTestDBName_)};
            DeclareActivity       da{&kActivity_};
            auto                  adminDB = AdminConnection::New (AdminConnection::Options{.fConnectionTarget = *mongoConnectionString});
            IgnoreExceptionsForCall (adminDB.DropDatabase (kTestDBName_));
            adminDB.CreateDatabase (kTestDBName_);
            cerr << "Starting MongoDBClient networks sample:" << endl;
            ComputerNetworksModel ([=] () {
                // can create a new connection each time
                cerr << "\tConnecting to {} database {}"_f(*mongoConnectionString, kTestDBName_) << endl;
                return MongoDBClient::Connection::New (
                    MongoDBClient::Connection::Options{.fConnectionTarget = *mongoConnectionString, .fDatabase = kTestDBName_});
            });
            cerr << "done." << endl;
        }
        catch (...) {
            cerr << "\t{}"_f(current_exception ()) << endl;
        }
    }
    if (mongoConnectionString) {
        using namespace Stroika::Foundation::Database::Document::MongoDBClient;
        const String kTestDBName_ = "DocumentDB-Sample-Employees"sv;
        try {
            static const Activity kActivity_{"performing MongoDBClient test on {}"_f(kTestDBName_)};
            DeclareActivity       da{&kActivity_};
            auto                  adminDB = AdminConnection::New (AdminConnection::Options{.fConnectionTarget = *mongoConnectionString});
            IgnoreExceptionsForCall (adminDB.DropDatabase (kTestDBName_));
            adminDB.CreateDatabase (kTestDBName_);
            Database::Document::Connection::Ptr internallySyncrhonizedConnection = MongoDBClient::Connection::New (MongoDBClient::Connection::Options{
                .fInternallySynchronizedLetter = eInternallySynchronized, .fConnectionTarget = *mongoConnectionString, .fDatabase = kTestDBName_});
            cerr << "Starting mongodb employees sample:" << endl;
            EmployeesDB ([=] () {
                cerr << "\tConnecting to {} database {}"_f(*mongoConnectionString, kTestDBName_) << endl;
                return internallySyncrhonizedConnection; // each thread using same internally syncrhonized connection
                    // or can create a new connection each time in factory, and make them unsynchronized
            });
            cerr << "done." << endl;
        }
        catch (...) {
            cerr << "\t{}"_f(current_exception ()) << endl;
        }
    }
#endif

#if qStroika_HasComponent_sqlite
    // quick tests with in-memory DB
    try {
        const String          kTestDBName_ = "DocumentDB-Sample-Networks"sv;
        static const Activity kActivity_{"performing sqlite document db networks sample on {}"_f(kTestDBName_)};
        DeclareActivity       da{&kActivity_};
        cerr << "Starting sqlite document db networks sample:" << endl;
        ComputerNetworksModel ([=] () {
            cerr << "\tConnecting to sqlite memory db: {}"_f(kTestDBName_) << endl;
            return SQLite::Connection::New (SQLite::Connection::Options{.fInMemoryDB = kTestDBName_});
        });
        cerr << "done." << endl;
    }
    catch (...) {
        cerr << "\t{}"_f(current_exception ()) << endl;
    }
    try {
        const String          kTestDBName_ = "DocumentDB-Sample-Employees"sv;
        static const Activity kActivity_{"performing sqlite document db employees sample on {}"_f(kTestDBName_)};
        DeclareActivity       da{&kActivity_};
        cerr << "Starting sqlite document db employees sample on memory db {}"_f(kTestDBName_) << endl;
        EmployeesDB ([=] () {
            cerr << "\tConnecting to sqlite memory db: {}"_f(kTestDBName_) << endl;
            return SQLite::Connection::New (SQLite::Connection::Options{.fInMemoryDB = kTestDBName_});
        });
        cerr << "done." << endl;
    }
    catch (...) {
        cerr << "\t{}"_f(current_exception ()) << endl;
    }
    // or run same test on filesystem
    try {
        auto                  dbPath = IO::FileSystem::WellKnownLocations::GetTemporary () / "networks-test.db";
        static const Activity kActivity_{"performing sqlite document db networks sample on {}"_f(dbPath)};
        DeclareActivity       da{&kActivity_};
        remove (dbPath); // test assumes empty db
        cerr << "Starting sqlite document db networks sample:" << endl;
        ComputerNetworksModel ([=] () {
            cerr << "\tConnecting to sqlite  db: {}"_f(dbPath) << endl;
            return SQLite::Connection::New (SQLite::Connection::Options{.fDBPath = dbPath});
        });
        cerr << "done." << endl;
    }
    catch (...) {
        cerr << "\t{}"_f(current_exception ()) << endl;
    }
    try {
        auto                  dbPath = IO::FileSystem::WellKnownLocations::GetTemporary () / "employees-test.db";
        static const Activity kActivity_{"performing Starting sqlite document db employees sample on {}"_f(dbPath)};
        DeclareActivity       da{&kActivity_};
        remove (dbPath); // test assumes empty db
        cerr << "Starting sqlite document db employees sample on {}"_f(dbPath) << endl;
        EmployeesDB ([=] () {
            cerr << "\tConnecting to sqlite  db: {}"_f(dbPath) << endl;
            auto c = SQLite::Connection::New (SQLite::Connection::Options{.fDBPath = dbPath});
            return c;
        });
        cerr << "done." << endl;
    }
    catch (...) {
        cerr << "\t{}"_f(current_exception ()) << endl;
    }
#endif
    // Try LocalDocumentDB
    try {
        static constexpr Activity kActivity_{"performing trivial document db networks sample"sv};
        DeclareActivity           da{&kActivity_};
        cerr << "Starting trivial document db networks sample:" << endl;
        auto internallySynchronizedDBConnection = LocalDocumentDB::New (LocalDocumentDB::Options{
            .fInternallySynchronizedLetter = eInternallySynchronized, .fStorage = LocalDocumentDB::Options::MemoryStorage{}});
        ComputerNetworksModel ([=] () {
            cerr << "\tConnecting to trivial document db: memory" << endl;
            return internallySynchronizedDBConnection; // re-used multiple times from different threads
        });
        cerr << "done." << endl;
    }
    catch (...) {
        cerr << "\t{}"_f(current_exception ()) << endl;
    }
    try {
        static constexpr Activity kActivity_{"performing trivial document db employees sample"sv};
        DeclareActivity           da{&kActivity_};
        cerr << "Starting trivial document db employees sample:" << endl;
        auto internallySynchronizedDBConnection = LocalDocumentDB::New (LocalDocumentDB::Options{
            .fInternallySynchronizedLetter = eInternallySynchronized, .fStorage = LocalDocumentDB::Options::MemoryStorage{}});
        EmployeesDB ([=] () {
            cerr << "\tConnecting to trivial document db: memory" << endl;
            return internallySynchronizedDBConnection; // re-used multiple times from different threads
        });
        cerr << "done." << endl;
    }
    catch (...) {
        cerr << "\t{}"_f(current_exception ()) << endl;
    }
    try {
        static constexpr Activity kActivity_{"performing trivial document db employees sample"sv};
        DeclareActivity           da{&kActivity_};
        cerr << "Starting trivial document db employees sample:" << endl;
        filesystem::path p = IO::FileSystem::WellKnownLocations::GetTemporary () / "employees-trivialdb-test.json";
        auto             internallySynchronizedDBConnection = LocalDocumentDB::New (
            LocalDocumentDB::Options{.fInternallySynchronizedLetter = eInternallySynchronized,
                                                 .fStorage = LocalDocumentDB::Options::SingleFileStorage{.fFile = p, .fForceCreateNew = true}});
        EmployeesDB ([=] () {
            cerr << "\tConnecting to trivial document db: {}"_f(p) << endl;
            return internallySynchronizedDBConnection; // re-used multiple times from different threads
        });
        cerr << "done." << endl;
    }
    catch (...) {
        cerr << "\t{}"_f(current_exception ()) << endl;
    }
    try {
        static constexpr Activity kActivity_{"performing trivial document db employees sample"sv};
        DeclareActivity           da{&kActivity_};
        cerr << "Starting trivial document db employees sample:" << endl;
        filesystem::path p = IO::FileSystem::WellKnownLocations::GetTemporary () / "employees-trivialdb-dir-test";
        auto             internallySynchronizedDBConnection = LocalDocumentDB::New (
            LocalDocumentDB::Options{.fInternallySynchronizedLetter = eInternallySynchronized,
                                                 .fStorage = LocalDocumentDB::Options::DirectoryFileStorage{.fRoot = p, .fForceCreateNew = true}});
        EmployeesDB ([=] () {
            cerr << "\tConnecting to trivial document db: {}"_f(p) << endl;
            return internallySynchronizedDBConnection; // re-used multiple times from different threads
        });
        cerr << "done." << endl;
    }
    catch (...) {
        cerr << "\t{}"_f(current_exception ()) << endl;
    }

    return EXIT_SUCCESS;
}
