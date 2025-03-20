/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/Characters/Format.h"

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
    inline IRep* Ptr::operator->() const noexcept
    {
        return inherited::operator->();
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
    inline String Ptr::AddDocument (const Document& v)
    {
        return this->get ()->AddDocument (v);
    }
    inline optional<Document> Ptr::GetDocument (const String& id, const optional<Projection>& projection)
    {
        return this->get ()->GetDocument (id, projection);
    }
    inline Document Ptr::GetDocumentOrThrow (const String& id, const optional<Projection>& projection)
    {
        static const auto kExcept_ = Execution::RuntimeErrorException{"no such id"};
        return Memory::ValueOfOrThrow (GetDocument (id, projection), kExcept_);
    }
    inline Sequence<Document> Ptr::GetDocuments (const optional<Filter>& filter, const optional<Projection>& projection)
    {
        return this->get ()->GetDocuments (filter, projection);
    }
    inline void Ptr::ReplaceDocument (const String& id, const Document& newV)
    {
        this->get ()->UpdateDocument (id, newV, nullopt);
    }
    inline void Ptr::UpdateDocument (const String& id, const Document& newV)
    {
        this->get ()->UpdateDocument (id, newV, Set<String>{newV.Keys ()});
    }
    inline void Ptr::UpdateDocument (const String& id, const Document& newV, const optional<Set<String>>& onlyTheseFields)
    {
        this->get ()->UpdateDocument (id, newV, onlyTheseFields);
    }
    inline void Ptr::DeleteDocument (const String& id)
    {
        this->get ()->DeleteDocument (id);
    }

}
