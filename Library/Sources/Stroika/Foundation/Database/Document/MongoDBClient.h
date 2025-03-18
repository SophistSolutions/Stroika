/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_Document_MongoDBClient_h_
#define _Stroika_Foundation_Database_Document_MongoDBClient_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_mongocxxdriver
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#endif

#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/Database/Document/Connection.h"
#include "Stroika/Foundation/Database/Document/EngineProperties.h"
#include "Stroika/Foundation/Database/Document/Transaction.h"

/*
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 *
 * TODO:
 *
 */

/*
@CONFIGVAR:     DqStroika_HasComponent_mongocxxdriver
@DESCRIPTION:   <p>Defines if Stroika is built supporting MongoDBClient (only do if ODBC headers in -I path)/p>
*/
#ifndef qStroika_HasComponent_mongocxxdriver
#error "qStroika_HasComponent_mongocxxdriver should normally be defined indirectly by StroikaConfig.h"
#endif

namespace Stroika::Foundation::Database::Document::MongoDBClient {

#if qStroika_HasComponent_mongocxxdriver

    /**
     *  \brief must be instantiated after main, but before any use of MongoCXX library methods, and destroyed after all such methods
     * 
     *  \pre Debug::AppearsDuringMainLifetime ()
     */
    struct Activator {
#if qStroika_Foundation_Debug_AssertionsChecked
        Activator ();
        ~Activator ();
#else
        Activator ()  = default;
        ~Activator () = default;
#endif

    private:
        mongocxx::instance fMongoInstance_;
    };

    namespace AdminConnection {
        class IRep;

        /**
         */
        class Ptr : public shared_ptr<IRep> {
        private:
            using inherited = shared_ptr<IRep>;

        public:
            /**
             */
            Ptr (const Ptr& src) noexcept = default;
            Ptr (const shared_ptr<IRep>& src = nullptr) noexcept;

        public:
            ~Ptr () noexcept = default;
        };

        /**
         */
        class IRep {
        public:
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySynchronizedMutex;

        public:
            virtual Document run_command (const Document& v) = 0;

        public:
            virtual mongocxx::client* get_client () = 0;

        public:
            virtual Set<String> GetDatabases () = 0;

        public:
            virtual void DropDatabase (const String& dbName) = 0;

        public:
            virtual void CreateDatabase (const String& dbName) = 0;

        private:
            friend class Ptr;
        };

        /**
         *  These are options used to create a database Connection::Ptr object (with Connection::New).
         *
         *  Since this is also how you create a database, in a sense, its those options too.
         */
        struct Options final {
            /**
             * @brief see https://www.mongodb.com/docs/manual/reference/connection-string/
             */
            String fConnectionString;
        };

        /**
         */
        Ptr New (const Options& options);
    }

    namespace Connection {

        using namespace Database::Document::Connection;

        class IRep;

        /**
         *  These are options used to create a database Connection::Ptr object (with Connection::New).
         *
         *  Since this is also how you create a database, in a sense, its those options too.
         */
        struct Options final {
            /**
             * @brief see https://www.mongodb.com/docs/manual/reference/connection-string/
             */
            String fConnectionString;

            /**
             *  Connection string does not contain database name. Different from Mongo API, we choose to require a database name
             *  in the connection options. Use AdminConnection to operate on server itself, not a particular database.
             */
            String fDatabase;
        };

        /**
         *  Connection provides an API for accessing a particular MongoDB database.
         *
         *  A new Connection::Ptr is typically created MongoDBClient::Connection::New()
         */
        class Ptr : public Database::Document::Connection::Ptr {
        private:
            using inherited = Database::Document::Connection::Ptr;

        public:
            /**
             */
            Ptr (const Ptr& src);
            Ptr (const shared_ptr<IRep>& src = nullptr);

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
        };

        /**
         */
        Ptr New (const Options& options);

        /**
         *  Connection provides an API for accessing an MongoDBClient database.
         * 
         *  Typically don't use this directly, but use Connection::Ptr, a smart ptr wrapper on this interface.
         */
        class IRep : public Database::Document::Connection::IRep {
        public:
            virtual mongocxx::client* get_client () = 0;

        public:
        private:
            friend class Ptr;
        };

    };

    /**
     */
    class Transaction : public Database::Document::Transaction {
    private:
        using inherited = Database::Document::Transaction;

    public:
        /**
         */
        Transaction () = delete;
        Transaction (const Connection::Ptr& db);
        Transaction (const Transaction&) = delete;

    private:
        struct MyRep_;
    };
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MongoDBClient.inl"

#endif /*_Stroika_Foundation_Database_Document_MongoDBClient_h_*/
