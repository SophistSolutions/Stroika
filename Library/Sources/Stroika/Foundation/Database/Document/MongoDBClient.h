/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_Document_MongoDBClient_h_
#define _Stroika_Foundation_Database_Document_MongoDBClient_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_mongocxxdriver
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#endif

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

    namespace Connection {

        using namespace Document::Connection;

        class IRep;

        /**
         *  These are options used to create a database Connection::Ptr object (with Connection::New).
         *
         *  Since this is also how you create a database, in a sense, its those options too.
         */
        struct Options final {
            /**
             */
            optional<String> fDSN;
        };

        /**
         *  Connection provides an API for accessing an ODBC database.
         *
         *  A new Connection::Ptr is typically created MongoDBClient::Connection::New()
         */
        class Ptr : public Document::Connection::Ptr {
        private:
            using inherited = Document::Connection::Ptr;

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
         *  The dbInitializer is called IFF the New () call results in a newly created database (@todo RECONSIDER).
         */
        Ptr New (const Options& options);

        /**
         *  Connection provides an API for accessing an MongoDBClient database.
         * 
         *  Typically don't use this directly, but use Connecion::Ptr, a smart ptr wrapper on this interface.
         */
        class IRep : public Document::Connection::IRep {
        public:
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySynchronizedMutex;

        private:
            friend class Ptr;
        };

    };

    /**
     */
    class Transaction : public Document::Transaction {
    private:
        using inherited = Document::Transaction;

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
