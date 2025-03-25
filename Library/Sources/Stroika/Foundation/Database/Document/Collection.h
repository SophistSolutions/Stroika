/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
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
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"

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
         * returns ID
         * 
         *      @todo think out/document if v MAY contain ID (typically doesnt - but do we allow specifying? - probably NO)
         */
        nonvirtual IDType Add (const Document& v);

    public:
        /**
         * @todo add options to only get parts of the document (say provide optional arg list of only-these fields) or omit these fields.
         * overload with id returns exactly that one, or nullopt if missing.
         */
        nonvirtual optional<Document> GetOne (const IDType& id, const optional<Projection>& projection = {});

    public:
        /**
         */
        nonvirtual Document GetOneOrThrow (const IDType& id, const optional<Projection>& projection = {});

    public:
        /**
         *  Return all 'documents' in this collection. Optionally subset them with a filter; optionally project (subset of fields) with an argument projection.
         */
        nonvirtual Sequence<Document> GetAll (const optional<Filter>& filter = {}, const optional<Projection>& projection = {});

    public:
        /**
         *  Same as GetAll (filter, projection=kOnlyIDs) - except for mapping the return type to pick out the id and move it out of a variant object into just a string value
         */
        nonvirtual Sequence<IDType> GetAllIDs (const optional<Filter>& filter = {});

    public:
        /**
         *  \brief except for issues of timing, equivalent to remove, and then add newV using id
         * 
         *  \pre for /1 overload, newV.ContainsKey (kID)
         *  \see also Update
         */
        nonvirtual void Replace (const Document& newV);
        nonvirtual void Replace (const IDType& id, const Document& newV);

    public:
        /**
         *  \brief Update the document named by 'id' - just updating fields in newV (all or ones given in argument onlyTheseFields)
         * 
         *  This does NOT modify other fields of the object.
         * 
         *  \pre for overloads without 'id' argument, newV.ContainsKey (kID)
         *  \see also Replace to replace the entire object
         */
        nonvirtual void Update (const Document& newV);
        nonvirtual void Update (const Document& newV, const Set<String>& onlyTheseFields);
        nonvirtual void Update (const IDType& id, const Document& newV);
        nonvirtual void Update (const IDType& id, const Document& newV, const Set<String>& onlyTheseFields);

    public:
        /**
         * @todo consider if this should return an indicator if found (removeif)
         */
        nonvirtual void Remove (const IDType& id);

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
         * returns ID
         */
        virtual String Add (const Document& v) = 0;

    public:
        /**
         * overload with id returns exactly that one, or nullopt if missing.
         */
        virtual optional<Document> GetOne (const IDType& id, const optional<Projection>& projection) = 0;

    public:
        /**
         * @todo add options to only get parts of the document (say provide optional arg list of only-these fields) or omit these fields.
         * overload with no id returns all.
         */
        virtual Sequence<Document> GetAll (const optional<Filter>& filter, const optional<Projection>& projection) = 0;

    public:
        /**
         *  if onlyTheseFields, does a Replace (), and if present, does 'Retain()' on newV for the argument fields.
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
