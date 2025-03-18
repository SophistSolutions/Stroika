/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_Document_Connection_h_
#define _Stroika_Foundation_Database_Document_Connection_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Database/Document/Collection.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"

/**
 *  \file
 * 
 *  The point of this module is to define a Connection abstraction that can be used for different kinds
 *  of connections (e.g. SQLite local database object, and ODBC remote database object). This generic API
 *  can then be used in places where either backend database might be in use.
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Database::Document {

    using Characters::String;
    using Containers::Set;

    class Transaction;
    class EngineProperties;

}
namespace Stroika::Foundation::Database::Document::Connection {

    class IRep;

    /**
         *  Connection::Ptr provides an API for accessing a document database.
         * 
         *  A new Connection::Ptr is typically created with SOME_SERVICE::Connection::New () (e.g. SQLite::Connection::New() or MongoDBClient::Connection::New ())
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
         *          But though each connection can only be accessed from a single thread at a time, the underlying database may be
         *          threadsafe (even if accessed across processes).
         *
         *          The Connection::Ptr itself is standardC++ thread safety. The thread-safety of the underlying database depends on how the underlying
         *          shared_ptr<IRep> was created.
         */
    class Ptr {
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
             *  Transaction object factory
             * 
             *  The reason you might use this instead of SQLite::Transaction{} - is in writing generic code
             *  that doesn't depend on the particular kind of SQL database you are connected to (e.g. that
             *  might be used for ODBC or SQLite).
             */
        nonvirtual Transaction mkTransaction ();

    public:
        /**
             *  @see Characters::ToString ()
             */
        nonvirtual String ToString () const;

    public:
        nonvirtual auto operator== (const Ptr& rhs) const;
        nonvirtual bool operator== (nullptr_t) const noexcept;

    public:
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySynchronizedMutex;

    protected:
        shared_ptr<IRep> _fRep;
    };

    /**
         *  Connection::IRep provides an (abstract) API for accessing an SQL database.
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Thread-Safety-Rules-Depends-On-Subtype">Thread-Safety-Rules-Depends-On-Subtype</a>
         */
    class IRep : public enable_shared_from_this<IRep> {
    public:
        /**
             */
        virtual ~IRep () = default;

    public:
        /**
             *  Transaction object factory
             */
        virtual Transaction mkTransaction () = 0;

    public:
        /**
             */
        virtual shared_ptr<const EngineProperties> GetEngineProperties () const = 0;

    public:
        virtual Set<String> GetCollections () = 0;

    public:
        virtual void CreateCollection (const String& name) = 0;

    public:
        virtual void DropCollection (const String& name) = 0;

    public:
        virtual Collection::Ptr GetCollection (const String& name) = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Connection.inl"

#endif /*_Stroika_Foundation_Database_Document_Connection_h_*/
