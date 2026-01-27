/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_Document_ObjectCollection_h_
#define _Stroika_Foundation_Database_Document_ObjectCollection_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/Database/Document/Collection.h"

/**
 *  \file
 * 
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Database::Document::ObjectCollection {

    using namespace Database::Document::Collection;
    using DataExchange::ObjectVariantMapper;

    /**
     * Provide an API similar to Database::Document::Collection::Ptr, but using an ObjectVariantMapper
     * to do IO with the higher level objects directly.
     */
    template <typename T>
    class Ptr : public Database::Document::Collection::Ptr {
    private:
        using inherited = Database::Document::Collection::Ptr;

    public:
        /**
         */
        Ptr (const Ptr& src)     = default;
        Ptr (Ptr&& src) noexcept = default;
        Ptr () noexcept          = default;
        Ptr (const Database::Document::Collection::Ptr& underlyingCollection, const ObjectVariantMapper& mapper);
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
         * returns ID
         */
        using inherited::Add;
        nonvirtual IDType Add (const T& v) const;

    public:
        /**
         */
        nonvirtual optional<T> GetOne (const IDType& id, const optional<Projection>& projection = {}) const;

    public:
        /**
         */
        nonvirtual T GetOneOrThrow (const IDType& id, const optional<Projection>& projection = {}) const;

    public:
        /**
         */
        nonvirtual Sequence<T> GetAll (const optional<Filter>& filter = {}, const optional<Projection>& projection = {}) const;

    public:
        /**
         */
        using inherited::Replace;
        nonvirtual void Replace (const T& newV) const;
        nonvirtual void Replace (const IDType& id, const T& newV) const;

    public:
        /**
         */
        using inherited::Update;
        nonvirtual void Update (const T& newV, const Set<String>& onlyTheseFields) const;
        nonvirtual void Update (const IDType& id, const T& newV, const Set<String>& onlyTheseFields) const;

    public:
        /**
         * @brief if v contains and ID, look it up, and if found, Replace. If not found, then synonym for Add ()
         */
        using inherited::AddOrUpdate;
        nonvirtual IDType AddOrUpdate (const T& newV) const;

    private:
        ObjectVariantMapper fMapper_;
    };

    /**
     */
    template <typename T>
    Ptr<T> New (const Database::Document::Collection::Ptr& underlyingCollection, const ObjectVariantMapper& mapper);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ObjectCollection.inl"

#endif /*_Stroika_Foundation_Database_Document_ObjectCollection_h_*/
