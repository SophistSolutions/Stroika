/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_Document_TrivialDocumentDB_h_
#define _Stroika_Foundation_Database_Document_TrivialDocumentDB_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <filesystem>
#include <optional>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/Database/Document/Collection.h"
#include "Stroika/Foundation/Database/Document/Connection.h"
#include "Stroika/Foundation/Database/Document/EngineProperties.h"
#include "Stroika/Foundation/Database/Document/Transaction.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Time/Duration.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 *
 *  TODO
 */

namespace Stroika::Foundation::Database::Document::TrivialDocumentDB {

    using Characters::String;
    using Containers::Mapping;
    using Containers::Sequence;
    using DataExchange::VariantValue;
    using IO::Network::URI;
    using Time::Duration;

    /**
     *  \brief SQLite::Connection namespace contains SQL::Connection::Ptr subclass, specific to SQLite, and ::New function factory.
     */
    //namespace Connection {

    using namespace Database::Document::Connection;

    using Database::Document::Connection::IRep;

    /**
         *  These are options used to create a database Connection::Ptr object (with Connection::New).
         *
         *  Since this is also how you create a database, in a sense, its those options too.
         */
    struct Options final {

        /**
             *  @todo add options like max ram, max # objects?
             */
        struct MemoryStorage final {};

        /**
             * add options like caching (support external process sync/flock)
             */
        struct FilesystemStorage final {
            /**
                 * 
                 */
            filesystem::path fRoot;

            /**
                 *  Extension point so we can switch to writing files as BSON, msgpack, or some such...
                 */
            tuple<DataExchange::Variant::Reader, DataExchange::Variant::Writer> fSerialization{DataExchange::Variant::JSON::Reader{},
                                                                                               DataExchange::Variant::JSON::Writer{}};
        };

        /**
             * 
             */
        variant<MemoryStorage, FilesystemStorage> fStorage;
    };

    /**
         * @brief &&&REWRITE
         * 
         *
         *  Connection provides an API for accessing an SQLite database.
         *
         *  A new Connection::Ptr is typically created SQLite::Connection::New()
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
         *          But though each connection can only be accessed from a single thread at a time, the underlying database may be
         *          threadsafe (even if accessed across processes) - depending on its construction Options::ThreadSafety
         *
         *          The Connection itself is standardC++ thread safety. The thread-safety of the underlying database depends on the setting
         *          of Options::fThreadingMode when the database is constructed.
         * 
         *          @see https://www.sqlite.org/threadsafe.html
         *          We set SQLITE_OPEN_NOMUTEX on open (so mode Multi-thread, but not Serialized).
         * 
         *          NOTE - two Connection::Ptr objects referring to the same underlying REP is NOT (probably) safe with SQLITE. But referring
         *          to the same database is safe.
         *
         */
    using Database::Document::Connection::Ptr;

    /**
         *  \brief create an SQLite database connection object, guided by argument Options.
         */
    Ptr New (const Options& options);

    //}

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TrivialDocumentDB.inl"

#endif /*_Stroika_Foundation_Database_Document_TrivialDocumentDB_h_*/
