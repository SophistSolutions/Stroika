/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
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

    using namespace Database::Document::ObjectCollection;
    using DataExchange::ObjectVariantMapper;

    /**
     * Provide an API similar to Database::Document::Collection::Ptr, but using an ObjectVariantMapper
     * to do IO with the higher level objects directly.
     */
    class Ptr : Database::Document::Collection::Ptr {
    private:
        using inherited = Database::Document::Collection::Ptr;

    public:
        /**
         */
        Ptr (const Ptr& src) = default;
        Ptr (Ptr&& src)      = default;
        Ptr (const Database::Document::Collection::Ptr& underlyingCollection, const ObjectVariantMapper& mapper);

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
        using inherited::AddDocument;
        template <typename T>
        nonvirtual String AddDocument (const T& v);

    public:
        /**
         */
        using inherited::GetDocument;
        template <typename T>
        nonvirtual optional<T> GetDocument (const String& id, const optional<Projection>& projection = {});

    public:
        /**
         */
        using inherited::GetDocumentOrThrow;
        template <typename T>
        nonvirtual T GetDocumentOrThrow (const String& id, const optional<Projection>& projection = {});

    public:
        /**
         */
        using inherited::GetDocuments;
        template <typename T>
        nonvirtual Sequence<T> GetDocuments (const optional<Filter>& filter = {}, const optional<Projection>& projection = {});

    public:
        /**
         */
        using inherited::ReplaceDocument;
        template <typename T>
        nonvirtual void ReplaceDocument (const String& id, const T& newV);

    public:
        /**
         */
        using inherited::UpdateDocument;
        template <typename T>
        nonvirtual void UpdateDocument (const String& id, const T& newV, const Set<String>& onlyTheseFields);

    private:
        ObjectVariantMapper fMapper_;
    };

    /**
     */
    Ptr New (const Database::Document::Collection::Ptr& underlyingCollection, const ObjectVariantMapper& mapper);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ObjectCollection.inl"

#endif /*_Stroika_Foundation_Database_Document_ObjectCollection_h_*/
