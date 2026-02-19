/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_Document_LocalDocumentDB_h_
#define _Stroika_Foundation_Database_Document_LocalDocumentDB_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <filesystem>
#include <optional>

#include "Stroika/Foundation/Characters/String.h"
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
#include "Stroika/Foundation/Execution/Synchronized.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 *
 *  LocalDocumentDB is a (typically filesystem, but can be RAM based) simple implementation of the DocumentDB
 *  API. You can use this to debug/test, and possibly for limited, or embedded, small scale uses.
 * 
 *  Advantages:
 *      -   Small, simple, easy to review code, and understand API
 *      -   Very small dependency footprint
 *      -   Lets you pick data format to store (when storing to disk), JSON, or BSON, or whatever you have
 *          serializers/de-serializers for.
 * 
 *  Disadvantages
 *      -   Performance on larger scale
 *      -   Transactions NYI
 */

namespace Stroika::Foundation::Database::Document::LocalDocumentDB {

    using namespace Database::Document::Connection;

    using Database::Document::Connection::IRep;

    /**
     *  These are options used to create a database Connection::Ptr object (with Connection::New).
     *
     *  Since this is also how you create a database, in a sense, its those options too.
     */
    struct Options final : Database::Document::Connection::Options {

        /**
         * \brief use eInternallySynchronized to make envelope internally synchronized
         * 
         *      \note - as of 2026-01-28, all the implementations are actually eInternallySynchronized, but
         *              easy to fix so they are not (so they will be more performant in that case).
         * 
         *      \note this refers to in-process syncrhonization. Future flags/fields/options will be needed
         *            in other impls to assure cross-process syncrhonization (not sure if even appropriate for this impl but maybe something simple with flock).
         */
        Execution::InternallySynchronized fInternallySynchronizedLetter{Execution::eNotKnownInternallySynchronized};

        /**
         *  @todo add options like max ram, max # objects?
         */
        struct MemoryStorage final {};

        /**
         * @todo add options like caching (support external process sync/flock)
         */
        struct SingleFileStorage final {
            /**
             * Where the file is stored.
             */
            filesystem::path fFile;

            /**
             * If true (the default), then the initial data in fFile - if found - is read. If false, any existing data in the file (if it exists at all) - is ignored.
             */
            bool fReadInitialData{true}

#if qStroika_Foundation_Common_Platform_Windows
            /**
             * \see IO::FileSystem::ThroughTmpFileWriter::fRetryOnSharingViolationFor
             */
            optional<Time::DurationSeconds> fRetryOnSharingViolationFor;
#endif

            /**
             *  Extension point so we can switch to writing files as BSON, msgpack, or some such...
             */
            tuple<DataExchange::Variant::Reader, DataExchange::Variant::Writer> fSerialization{DataExchange::Variant::JSON::Reader{},
                                                                                               DataExchange::Variant::JSON::Writer{}};
        };

        /**
         * @todo add options like caching (support external process sync/flock)
         */
        struct DirectoryFileStorage final {
            /**
             * The directory where the files are stored.
             */
            filesystem::path fRoot;

#if qStroika_Foundation_Common_Platform_Windows
            /**
             * \see IO::FileSystem::ThroughTmpFileWriter::fRetryOnSharingViolationFor
             */
            optional<Time::DurationSeconds> fRetryOnSharingViolationFor;
#endif

            /**
             *  Extension point so we can switch to writing files as BSON, msgpack, or some such...
             */
            tuple<DataExchange::Variant::Reader, DataExchange::Variant::Writer> fSerialization{DataExchange::Variant::JSON::Reader{},
                                                                                               DataExchange::Variant::JSON::Writer{}};
        };

        /**
         * 
         */
        variant<MemoryStorage, SingleFileStorage, DirectoryFileStorage> fStorage;
    };

    /**
     */
    using Database::Document::Connection::Ptr;

    /**
     *  \brief create an LocalDocumentDB database (and connection) object, guided by argument Options.
     * 
     *  \note 
     * 
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
     *          the internal synchronization of the resulting letter object is controlled by Options::fInternallySynchronizedLetter
     *
     */
    Ptr New (const Options& options);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LocalDocumentDB.inl"

#endif /*_Stroika_Foundation_Database_Document_LocalDocumentDB_h_*/
