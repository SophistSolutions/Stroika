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

    class IRep;

    /**
     *  These are options used to create a database Connection::Ptr object (with Connection::New).
     *
     *  Since this is also how you create a database, in a sense, its those options too.
     */
    struct Options final : Database::Document::Connection::Options {

        /**
         * \brief use eInternallySynchronized to make letter internally synchronized
         * 
         *      \note this refers to in-process synchronization. Future flags/fields/options will be needed
         *            in other impls to assure cross-process synchronization (not sure if even appropriate for this impl but maybe something simple with flock).
         * 
         *      \note if set eNotKnownInternallySynchronized (the default), in debug mode, the system uses AssertExternallySynchronizedMutex
         *            to check for unsafe thread usage.
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
             * If true, the database connection will ignore any existing file (not read it). Either way
             * a new file will be created (whether one was there before or not).
             */
            bool fForceCreateNew{false};

            /**
             * If true, the file will be read (unless fForceCreateNew is set, and it can), but will never be written to disk.
             * (just cached in memory).
             */
            bool fReadOnly{false};

            /**
            * If true, each modification causes a write. If false, the implmentation MAY buffer writes (or may write thruough).
            * The caller can always trigger a write by calling Flush(), or destorying the connection.
            */
            bool fFlushOnEachWrite{false};

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

            /**
             * If true, the database connection will ignore any existing file (not read it). Either way
             * a new file will be created (whether one was there before or not).
             */
            bool fForceCreateNew{false};

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
     *
     */
    class Ptr : public Database::Document::Connection::Ptr {
    private:
        using inherited = Database::Document::Connection::Ptr;

    public:
        /**
         */
        Ptr (const Ptr& src);
        Ptr (const shared_ptr<IRep>& src);
        Ptr (nullptr_t = nullptr) noexcept;

    public:
        ~Ptr () = default;

    public:
        /**
         */
        nonvirtual Ptr& operator= (const Ptr& src);
        nonvirtual Ptr& operator= (Ptr&& src) noexcept;

    public:
        /**
         */
        nonvirtual IRep* operator->() const noexcept;

    public:
        /**
         * If the database is configured to buffer writes, this forces a flush of all buffered writes to the underlying storage (e.g. disk).
         * It may do nothing.
         */
        nonvirtual void Flush () const;
    };

    /**
     *  \brief create an LocalDocumentDB database (and connection) object, guided by argument Options.
     * 
     *  \par Example Usage
     *      \code
     *          // In memory DB can be used by multiple threads
     *          Connection::Ptr internallySynchronizedMemoryDBConnection = LocalDocumentDB::New (LocalDocumentDB::Options{
     *              .fInternallySynchronizedLetter = eInternallySynchronized, .fStorage = LocalDocumentDB::Options::MemoryStorage{}});
     *      \endcode
     * 
     *  \par Example Usage
     *      \code
     *          // only json file read-only using DocumentDB API
     *          Connection::Ptr copyFrom = LocalDocumentDB::New (LocalDocumentDB::Options{
     *              .fStorage = LocalDocumentDB::Options::SingleFileStorage{.fFile = loadFromStartupFile, .fReadOnly = true}});
     *      \endcode
     * 
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
     *          the internal synchronization of the resulting letter object is controlled by Options::fInternallySynchronizedLetter
     *
     */
    Ptr New (const Options& options);

    /**
     */
    class IRep : public Database::Document::Connection::IRep {
    public:
        /**
         * If the database is configured to buffer writes, this forces a flush of all buffered writes to the underlying storage (e.g. disk).
         * It may do nothing.
         */
        virtual void Flush () = 0;

    private:
        friend class Ptr;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LocalDocumentDB.inl"

#endif /*_Stroika_Foundation_Database_Document_LocalDocumentDB_h_*/
