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
@DESCRIPTION:   Defines if Stroika is built supporting mongo-cxx-driver
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
     * 
     *  From mongo-cxx-driver source code:
     *      Exactly one instance must be created in a given program. Not constructing an instance or
     *      constructing more than one instance in a program are errors, even if the multiple instances have
     *      non-overlapping lifetimes.
     * 
     *  To allow Stroika code to activate and deactivate (rarely but possibly useful) - an optional flag is provided, and
     *  if set allowReactivation=true, then the underlying mongocxx instance object not destroyed until application end,
     *  so that you can re-activate. JUST IGNORE this for hte most part and stick with the default!
     */
    struct Activator {
        enum AllowReactivateFlag {
            eAllowReactivateFlag
        };
        Activator ();
        Activator (AllowReactivateFlag);
        ~Activator ();

    private:
        bool                                       fAllowReactivation_;
        static inline optional<mongocxx::instance> sMongoInstance_;
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

        public:
            /**
             */
            nonvirtual Document run_command (const Document& v);

        public:
            /**
             */
            nonvirtual mongocxx::client& GetClientRef ();

        public:
            /**
             */
            nonvirtual Set<String> GetDatabases ();

        public:
            /**
             */
            nonvirtual void DropDatabase (const String& dbName);

        public:
            /**
             */
            nonvirtual void CreateDatabase (const String& dbName);
        };

        /**
         */
        class IRep {
        public:
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySynchronizedMutex;

        public:
            virtual Document run_command (const Document& v) = 0;

        public:
            virtual mongocxx::client& GetClientRef () = 0;

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

        public:
            /**
             */
            nonvirtual mongocxx::client& GetClientRef ();
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
            virtual mongocxx::client& GetClientRef () = 0;

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
