/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_Document_Connection_h_
#define _Stroika_Foundation_Database_Document_Connection_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Database/Document/Collection.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"

/**
 *  \file
 * 
 *  The point of this module is to define a Connection abstraction that can be used for different kinds
 *  of connections (e.g. SQLite local database object, and mongoDB remote database object). This generic API
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
    class Ptr;

    /**
     * Optionally passed to OpertionCallbackPtr for the purpose of logging
     */
    enum Operation {
        eStartingRead,
        eCompletedRead,
        eStartingWrite,
        eCompletedWrite,
        eNotifyError
    };

    /**
     * Optionally passed to Connection::New (its Options argument) for the purpose of logging; 
     * note exception_ptr is only provided for eNotifyError, and is typically current_exception () but can be nullptr
     * 
     * Note - callback must be internally synchonized, and maybe called in nested fasion.
     * 
     * Note - OpertionCallbackPtr must be no-throw (sadly not capturable with std::function).
     */
    using OpertionCallbackPtr =
        function<void (Operation op, const Ptr& documentDBConnection, const optional<String>& collectionName, const exception_ptr& e)>;

    /**
     * 
     */
    struct Options {
        /**
         * @brief  If specified for a Connection object, then the Add () API allows externally specified IDs (optionally)
         * 
         * This defaults to true, the easiest to use and most permissive setting. Setting to false is more efficient.
         * 
         * \note - for some database backends, this affects the layout, so it it is critical it agrees on all database connections
         *         if you have multiple connections to the same database file/object.
         * 
         * \note some backends may impose special requirements on the format of externally provided IDs (e.g.
         *       assume they will be in the form of a guid, but such requirements will be documented/enforced on a
         *       backend-by-backend 'new' factory basis).
         */
        bool fAddAllowsExternallySpecifiedIDs{true};

        /**
         * Note - callback must be internally synchonized, and maybe called in nested fasion.
         * Callback can be used to calculate usage statistics, and note database errors. Presence of
         * the operation callback doesn't affect normal operation of the 
         */
        OpertionCallbackPtr fOperationLoggingCallback{nullptr};
    };

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
    class Ptr : public shared_ptr<IRep> {
    private:
        using inherited = shared_ptr<IRep>;

    public:
        /**
         */
        Ptr (const Ptr& src)     = default;
        Ptr (Ptr&& src) noexcept = default;
        using inherited::inherited;

    public:
        ~Ptr () = default;

    public:
        /**
         */
        nonvirtual Ptr& operator= (const Ptr& src)     = default;
        nonvirtual Ptr& operator= (Ptr&& src) noexcept = default;

    public:
        /**
         */
        nonvirtual shared_ptr<const EngineProperties> GetEngineProperties () const;

    public:
        /**
         */
        nonvirtual Options GetOptions () const;

    public:
        /**
         * returns roughly the disk (or RAM if not stored on disk) space consumed by the database referenced in the Connection.
         */
        nonvirtual uintmax_t GetSpaceConsumed () const;

    public:
        /**
         * @brief return the names of all collections
         */
        nonvirtual Set<String> GetCollections () const;

    public:
        /**
         *  Creates the (named) collection (aka table), and does nothing if the table/collection already exists.
         *
         *   \see also GetCollection - much the same but creates first if doesnt exist
         *  
         *   \note This returns the same thing as GetCollection (name) after creating it, and its result can be safely
         *         ignored and fetched later.
         */
        nonvirtual Collection::Ptr CreateCollection (const String& name) const;

    public:
        /**
         * \note not an error and ignored if the named collection does not exist.
         * \note but this can create errors if the underlying database has problems (e.g. file system errors, permission errors, etc).
         */
        nonvirtual void DropCollection (const String& name) const;

    public:
        /**
         *  Returns a (shared) pointer to the named collection. If the named collection does not exist, it is an error.
         * 
         *  \see also CreateCollection
         */
        nonvirtual Collection::Ptr GetCollection (const String& name) const;

    public:
        /**
          *  Transaction object factory
          * 
          *  The reason you might use this instead of SQLite::Transaction{} - is in writing generic code
          *  that doesn't depend on the particular kind of SQL database you are connected to (e.g. that
          *  might be used for ODBC or SQLite).
          */
        nonvirtual Transaction mkTransaction () const;

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    public:
        nonvirtual bool operator== (const Ptr& rhs) const noexcept;
        nonvirtual bool operator== (nullptr_t) const noexcept;

    protected:
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex _fAssertExternallySynchronizedMutex;
    };

    /**
     *  Connection::IRep provides an (abstract) API for accessing a Document database.
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
         */
        virtual shared_ptr<const EngineProperties> GetEngineProperties () const = 0;

    public:
        /**
         */
        virtual Options GetOptions () const = 0;

    public:
        /**
         * Note only provides an estimate of size used.
         */
        virtual uintmax_t GetSpaceConsumed () const = 0;

    public:
        /**
         */
        virtual Set<String> GetCollections () = 0;

    public:
        /**
         *  Creates the (named) collection (aka table), and does nothing if the table/collection already exists.
         */
        virtual Collection::Ptr CreateCollection (const String& name) = 0;

    public:
        /**
         */
        virtual void DropCollection (const String& name) = 0;

    public:
        /**
         */
        virtual Collection::Ptr GetCollection (const String& name) = 0;

    public:
        /**
         *  Transaction object factory
         * 
         *  \note operations that happen OUTSIDE of a transaction are IMPLICITLY auto-committed immediately.
         */
        virtual Transaction mkTransaction () = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Connection.inl"

#endif /*_Stroika_Foundation_Database_Document_Connection_h_*/
