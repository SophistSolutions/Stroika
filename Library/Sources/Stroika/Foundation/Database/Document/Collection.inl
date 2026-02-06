/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Execution/Activity.h"
#include "Stroika/Foundation/Memory/Optional.h"

namespace Stroika::Foundation::Database::Document::Collection {

    /*
     ********************************************************************************
     ************************* Document::Connection::Ptr ****************************
     ********************************************************************************
     */
    inline Ptr::Ptr (const shared_ptr<IRep>& src)
        : inherited{src}
    {
    }
    inline Ptr& Ptr::operator= (const Ptr& src)
    {
        inherited::operator= (src);
        return *this;
    }
    inline Ptr& Ptr::operator= (Ptr&& src) noexcept
    {
        inherited::operator= (src);
        return *this;
    }
    inline auto Ptr::operator== (const Ptr& rhs) const
    {
        return get () == rhs.get ();
    }
    inline bool Ptr::operator== (nullptr_t) const noexcept
    {
        return this->get () == nullptr;
    }
    inline String Ptr::ToString () const
    {
        using namespace Characters;
        return Format ("{}"_f, static_cast<const void*> (this->get ()));
    }
    inline IDType Ptr::Add (const Document& v) const
    {
        return this->get ()->Add (v);
    }
    inline optional<Document> Ptr::Get (const IDType& id, const optional<Projection>& projection) const
    {
        auto                       activity = Execution::LazyEvalActivity{[&] () -> String { return "Getting document from atabase"sv; }};
        Execution::DeclareActivity da{&activity};
        return this->get ()->Get (id, projection);
    }
    inline optional<Document> Ptr::Get (const Filter& filter, const optional<Projection>& projection) const
    {
        auto r = this->GetAll (filter, projection);
        if (r.size () > 1) {
            static auto kTooManyResultsException_ = Execution::RuntimeErrorException{"too many results from DocumentDB Get() call"sv};
            Execution::Throw (kTooManyResultsException_);
        }
        if (r.size () == 0) {
            return nullopt;
        }
        return r[0];
    }
    inline Document Ptr::GetOrThrow (const IDType& id, const optional<Projection>& projection) const
    {
        static const auto kExcept_ = Execution::RuntimeErrorException{"no such id"sv};
        return Memory::ValueOfOrThrow (Get (id, projection), kExcept_);
    }
    inline Document Ptr::GetOrThrow (const Filter& filter, const optional<Projection>& projection) const
    {
        static const auto kExcept_ = Execution::RuntimeErrorException{"no such document"sv};
        return Memory::ValueOfOrThrow (Get (filter, projection), kExcept_);
    }
    inline Sequence<Document> Ptr::GetAll (const optional<Filter>& filter, const optional<Projection>& projection) const
    {
        auto activity = Execution::LazyEvalActivity{[&] () -> String { return "Getting all matching documents from atabase"sv; }};
        Execution::DeclareActivity da{&activity};
        return this->get ()->GetAll (filter, projection);
    }
    inline Sequence<IDType> Ptr::GetAllIDs (const optional<Filter>& filter) const
    {
        return this->GetAll (filter, kOnlyIDs).Map<Sequence<IDType>> ([] (const Document& d) -> IDType {
            static const auto kExcept_ = Execution::RuntimeErrorException{"no such id"sv};
            return d.LookupChecked (kID, kExcept_).As<String> ();
        });
    }
    inline void Ptr::Replace (const Document& newV) const
    {
        Replace (Memory::ValueOf (newV.Lookup (kID)).As<String> (), newV);
    }
    inline void Ptr::Replace (const IDType& id, const Document& newV) const
    {
        auto                       activity = Execution::LazyEvalActivity{[&] () -> String { return "Updating document in database"sv; }};
        Execution::DeclareActivity da{&activity};
        this->get ()->Update (id, newV, nullopt);
    }
    inline void Ptr::Update (const Document& newV) const
    {
        Update (Memory::ValueOf (newV.Lookup (kID)).As<String> (), newV, Set<String>{newV.Keys ()});
    }
    inline void Ptr::Update (const Document& newV, const Set<String>& onlyTheseFields) const
    {
        Update (Memory::ValueOf (newV.Lookup (kID)).As<String> (), newV, onlyTheseFields);
    }
    inline void Ptr::Update (const IDType& id, const Document& newV) const
    {
        auto                       activity = Execution::LazyEvalActivity{[&] () -> String { return "Updating document in database"sv; }};
        Execution::DeclareActivity da{&activity};
        this->get ()->Update (id, newV, Set<String>{newV.Keys ()});
    }
    inline void Ptr::Update (const IDType& id, const Document& newV, const Set<String>& onlyTheseFields) const
    {
        auto                       activity = Execution::LazyEvalActivity{[&] () -> String { return "Updating document in database"sv; }};
        Execution::DeclareActivity da{&activity};
        this->get ()->Update (id, newV, onlyTheseFields);
    }
    inline void Ptr::Remove (const IDType& id) const
    {
        auto                       activity = Execution::LazyEvalActivity{[&] () -> String { return "Removing document from database"sv; }};
        Execution::DeclareActivity da{&activity};
        this->get ()->Remove (id);
    }
    inline IDType Ptr::AddOrUpdate (const Document& v) const
    {
        if (auto oid = v.Lookup (kID)) {
            if (Get (oid->As<String> ())) {
                Replace (v);
                return oid->As<String> ();
            }
        }
        return Add (v);
    }

}
