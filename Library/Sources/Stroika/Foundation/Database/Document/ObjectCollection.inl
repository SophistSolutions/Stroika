/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

namespace Stroika::Foundation::Database::Document::ObjectCollection {

    /*
     ********************************************************************************
     ********************* Document::ObjectCollection::Ptr **************************
     ********************************************************************************
     */
    template <typename T>
    inline Ptr<T>::Ptr (const Database::Document::Collection::Ptr& underlyingCollection, const ObjectVariantMapper& mapper)
        : inherited{underlyingCollection}
        , fMapper_{mapper}
    {
    }
    template <typename T>
    inline String Ptr<T>::AddDocument (const T& v)
    {
        return inherited::AddDocument (fMapper_.FromObject (v).template As<Mapping<String, VariantValue>> ());
    }
    template <typename T>
    inline optional<T> Ptr<T>::GetDocument (const IDType& id, const optional<Projection>& projection)
    {
        if (optional<Document> o = inherited::GetDocument (id, projection)) {
            return fMapper_.ToObject<T> (VariantValue{*o});
        }
        else {
            return nullopt;
        }
    }
    template <typename T>
    inline T Ptr<T>::GetDocumentOrThrow (const IDType& id, const optional<Projection>& projection)
    {
        return fMapper_.ToObject<T> (VariantValue{inherited::GetDocument (id, projection)});
    }
    template <typename T>
    Sequence<T> Ptr<T>::GetDocuments (const optional<Filter>& filter, const optional<Projection>& projection)
    {
        return inherited::GetDocuments (filter, projection).template Map<Sequence<T>> ([this] (const Document& d) {
            return fMapper_.ToObject<T> (VariantValue{d});
        });
    }
    template <typename T>
    inline void Ptr<T>::ReplaceDocument (const IDType& id, const T& newV)
    {
        inherited::ReplaceDocument (id, fMapper_.FromObject (newV));
    }
    template <typename T>
    inline void Ptr<T>::UpdateDocument (const IDType& id, const T& newV, const Set<String>& onlyTheseFields)
    {
        inherited::UpdateDocument (id, fMapper_.FromObject (newV).template As<Mapping<String, VariantValue>> (), onlyTheseFields);
    }

    /*
     ********************************************************************************
     ********************* Document::ObjectCollection::New **************************
     ********************************************************************************
     */
    template <typename T>
    inline Ptr<T> New (const Database::Document::Collection::Ptr& underlyingCollection, const ObjectVariantMapper& mapper)
    {
        return Ptr<T>{underlyingCollection, mapper};
    }

}
