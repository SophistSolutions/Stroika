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
    inline String Ptr<T>::Add (const T& v)
    {
        return inherited::Add (fMapper_.FromObject (v).template As<Mapping<String, VariantValue>> ());
    }
    template <typename T>
    inline optional<T> Ptr<T>::GetOne (const IDType& id, const optional<Projection>& projection)
    {
        if (optional<Document> o = inherited::GetOne (id, projection)) {
            return fMapper_.ToObject<T> (VariantValue{*o});
        }
        else {
            return nullopt;
        }
    }
    template <typename T>
    inline T Ptr<T>::GetOneOrThrow (const IDType& id, const optional<Projection>& projection)
    {
        return fMapper_.ToObject<T> (VariantValue{inherited::GetOneOrThrow (id, projection)});
    }
    template <typename T>
    Sequence<T> Ptr<T>::GetAll (const optional<Filter>& filter, const optional<Projection>& projection)
    {
        return inherited::GetAll (filter, projection).template Map<Sequence<T>> ([this] (const Document& d) {
            return fMapper_.ToObject<T> (VariantValue{d});
        });
    }
    template <typename T>
    inline void Ptr<T>::Replace (const T& newV)
    {
        inherited::Replace (fMapper_.FromObject (newV).template As<Mapping<String, VariantValue>> ());
    }
    template <typename T>
    inline void Ptr<T>::Replace (const IDType& id, const T& newV)
    {
        inherited::Replace (id, fMapper_.FromObject (newV).template As<Mapping<String, VariantValue>> ());
    }
    template <typename T>
    inline void Ptr<T>::Update (const T& newV, const Set<String>& onlyTheseFields)
    {
        inherited::Update (fMapper_.FromObject (newV).template As<Mapping<String, VariantValue>> (), onlyTheseFields);
    }
    template <typename T>
    inline void Ptr<T>::Update (const IDType& id, const T& newV, const Set<String>& onlyTheseFields)
    {
        inherited::Update (id, fMapper_.FromObject (newV).template As<Mapping<String, VariantValue>> (), onlyTheseFields);
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
