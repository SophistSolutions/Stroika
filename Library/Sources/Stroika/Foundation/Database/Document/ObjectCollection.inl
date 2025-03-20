/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

namespace Stroika::Foundation::Database::Document::ObjectCollection {

    /*
     ********************************************************************************
     ********************* Document::ObjectCollection::Ptr **************************
     ********************************************************************************
     */
    inline Ptr::Ptr (const Database::Document::ObjectCollection::Ptr& underlyingCollection, const ObjectVariantMapper& mapper)
        : inherited{underlyingCollection}
        , fMapper_{mapper}
    {
    }
    template <typename T>
    inline String Ptr::AddDocument (const T& v)
    {
        return AddDocument (fMapper_.FromObject (v));
    }
    template <typename T>
    inline optional<T> Ptr::GetDocument (const String& id, const optional<Projection>& projection)
    {
        if (auto o = inherited::GetDocument (id, projection)) {
            return fMapper_.ToObject (*o);
        }
        else {
            return nullopt;
        }
    }
    template <typename T>
    inline T Ptr::GetDocumentOrThrow (const String& id, const optional<Projection>& projection)
    {
        return fMapper_.ToObject (inherited::GetDocument (id, projection));
    }
    template <typename T>
    Sequence<T> Ptr::GetDocuments (const optional<Filter>& filter, const optional<Projection>& projection)
    {
        return inherited::GetDocuments (filter, projection).Map<Sequence<T>> ([this] (const Document& d) { return fMapper_.ToObject (d); });
    }
    template <typename T>
    inline void Ptr::ReplaceDocument (const String& id, const T& newV)
    {
        ReplaceDocument (id, fMapper_.FromObject (newV));
    }
    template <typename T>
    inline void Ptr::UpdateDocument (const String& id, const T& newV, const Set<String>& onlyTheseFields)
    {
        UpdateDocument (id, fMapper_.FromObject (newV), onlyTheseFields);
    }

    /*
     ********************************************************************************
     ********************* Document::ObjectCollection::New **************************
     ********************************************************************************
     */
    inline Ptr New (const Database::Document::Collection::Ptr& underlyingCollection, const ObjectVariantMapper& mapper)
    {
        return Ptr{underlyingCollection, mapper};
    }

}
