/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_Document_Collection_h_
#define _Stroika_Foundation_Database_Document_Collection_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/Database/Document/Document.h"
#include "Stroika/Foundation/Database/Document/Filter.h"
#include "Stroika/Foundation/Database/Document/Projection.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedChecker.h"

/**
 *  \file
 * 
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Database::Document {

    using Characters::String;
    using Containers::Sequence;
    using Containers::Set;
    using DataExchange::VariantValue;
    using Traversal::Iterable;

}

namespace Stroika::Foundation::Database::Document::Collection {

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
     *          The Collection::Ptr itself is standardC++ thread safety. The thread-safety of the underlying database depends on how the underlying
     *          shared_ptr<IRep> was created.
     */
    class Ptr : public shared_ptr<IRep> {
    private:
        using inherited = shared_ptr<IRep>;

    public:
        /**
         */
        Ptr (const Ptr&)     = default;
        Ptr (Ptr&&) noexcept = default;
        Ptr (const shared_ptr<IRep>& src);
        Ptr () noexcept = default;
        using inherited::inherited;

    public:
        ~Ptr () = default;

    public:
        /**
         */
        nonvirtual Ptr& operator= (const Ptr& src);
        nonvirtual Ptr& operator= (Ptr&& src) noexcept;

    public:
        /**
         * returns the name of the collection
         */
        nonvirtual String GetName () const;

    public:
        /**
         * returns ID of object added.
         * 
         *  If connection.GetOptions ().fAddAllowsExternallySpecifiedIDs and v contains a key with kID, that id will
         *  be used, and otherwise a new ID will be automatically generated. If an object with that ID already exists
         *  the Add() will fail (error). Use Update () or Replace () to update a record.
         * 
         *  if not connection.GetOptions ().fAddAllowsExternallySpecifiedIDs, then
         *      \req v does not contain an ID field (it will be auto-added).
         */
        nonvirtual IDType Add (const Document& v) const;

    public:
        /**
         *  \brief Read the unique document given by id (or filter) or return nullopt if no such
         *
         *  \alias GetOne
         * 
         *  \note each document always contains an ID (kID) field (even if not supplied with Add) - though it may not be returned because
         *       of the argument projection
         * 
         *  \note Get (Filter) - is a short-hand for GetAll(filter) - and throw if result.size () > 1
         *        and just return the one found (or nullopt for none)
         */
        nonvirtual optional<Document> Get (const IDType& id, const optional<Projection>& projection = {}) const;
        nonvirtual optional<Document> Get (const Filter& filter, const optional<Projection>& projection = {}) const;

    public:
        /**
         */
        nonvirtual Document GetOrThrow (const IDType& id, const optional<Projection>& projection = {}) const;
        nonvirtual Document GetOrThrow (const Filter& filter, const optional<Projection>& projection = {}) const;

    public:
        /**
         *  Return all 'documents' in this collection. Optionally subset them with a filter; optionally project (subset of fields) with an argument projection.
         * \note each document always contains an ID (kID) field (even if not supplied with Add) - though it may not be returned because
         *       of the argument projection
         */
        nonvirtual Sequence<Document> GetAll (const optional<Filter>& filter = {}, const optional<Projection>& projection = {}) const;

    public:
        /**
         * @brief Return all document IDs matching the argument filter.
         * 
         *  Same as GetAll (filter, projection=kOnlyIDs) - except for mapping the return type
         *  to pick out the id and move it out of a variant object into just a string value
         */
        nonvirtual Sequence<IDType> GetAllIDs (const optional<Filter>& filter = {}) const;

    public:
        /**
         *  \brief except for issues of timing, equivalent to remove, and then add newV using id
         * 
         *  \pre for /1 overload, newV.ContainsKey (kID)
         *  \see also Update
         */
        nonvirtual void Replace (const Document& newV) const;
        nonvirtual void Replace (const IDType& id, const Document& newV) const;

    public:
        /**
         *  \brief Update the document named by 'id' - just updating fields in newV (all or ones given in argument onlyTheseFields)
         * 
         *  This does NOT modify other fields of the object.
         * 
         *  \pre for overloads without 'id' argument, newV.ContainsKey (kID)
         *  \see also Replace to replace the entire object
         */
        nonvirtual void Update (const Document& newV) const;
        nonvirtual void Update (const Document& newV, const Set<String>& onlyTheseFields) const;
        nonvirtual void Update (const IDType& id, const Document& newV) const;
        nonvirtual void Update (const IDType& id, const Document& newV, const Set<String>& onlyTheseFields) const;

    public:
        /**
         * @todo consider if this should return an indicator if found (removeif)
         */
        nonvirtual void Remove (const IDType& id) const;

    public:
        /**
         * @brief if v contains and ID, look it up, and if found, Replace. If not found, then synonym for Add ()
         */
        nonvirtual IDType AddOrUpdate (const Document& v) const;

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    public:
        nonvirtual auto operator== (const Ptr& rhs) const;
        nonvirtual bool operator== (nullptr_t) const noexcept;

    public:
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE Debug::AssertExternallySynchronizedChecker fAssertExternallySynchronizedChecker;
    };

    /**
     *  Collection::IRep provides an (abstract) API for accessing a collection (aka table) of a document database.
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
         * returns the name of the collection
         */
        virtual String GetName () const = 0;

    public:
        /**
         * returns ID of object added.
         * 
         *  \alias AddNew
         * 
         *  If connection.GetOptions ().fAddAllowsExternallySpecifiedIDs and v contains a key with kID, that id will
         *  be used, and otherwise a new ID will be automatically generated. If an object with that ID already exists
         *  the Add() will fail (error). Use Update to update a record.
         * 
         *  if not connection.GetOptions ().fAddAllowsExternallySpecifiedIDs, then
         *      \req v does not contain an ID field (it will be auto-added).
         */
        virtual IDType Add (const Document& v) = 0;

    public:
        /**
         * overload with id returns exactly that one, or nullopt if missing.
         * 
         *      \note each document always contains an ID (kID) field (even if not supplied with Add) - though it may not be returned because
         *            of the argument projection
         */
        virtual optional<Document> Get (const IDType& id, const optional<Projection>& projection) = 0;

    public:
        /**
         * @todo add options to only get parts of the document (say provide optional arg list of only-these fields) or omit these fields.
         * overload with no id returns all.
         */
        virtual Sequence<Document> GetAll (const optional<Filter>& filter, const optional<Projection>& projection) = 0;

    public:
        /**
         *  if onlyTheseFields ==nullopt does a Replace (existing document entirely replaced with newV), and if present, does 'Retain()' on newV for the argument fields and only updates those fields (if missing in newV they are removed from database)
         * 
         *  \note newV may or may not contain an 'id' field. It will be automatically implicitly added/replaced with the explicit
         *        argument 'id'
         */
        virtual void Update (const IDType& id, const Document& newV, const optional<Set<String>>& onlyTheseFields) = 0;

    public:
        virtual void Remove (const IDType& id) = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Collection.inl"

#endif /*_Stroika_Foundation_Database_Document_Collection_h_*/
