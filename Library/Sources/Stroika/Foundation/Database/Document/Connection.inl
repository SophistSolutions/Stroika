/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/Characters/Format.h"

namespace Stroika::Foundation::Database::Document::Connection {

    /*
     ********************************************************************************
     ************************* Document::Connection::Ptr ****************************
     ********************************************************************************
     */
    inline bool Ptr::operator== (nullptr_t) const noexcept
    {
        return this->get () == nullptr;
    }
    inline Set<String> Ptr::GetCollections ()
    {
        return this->get ()->GetCollections ();
    }
    inline void Ptr::CreateCollection (const String& name)
    {
        this->get ()->CreateCollection (name);
    }
    inline void Ptr::DropCollection (const String& name)
    {
        this->get ()->DropCollection (name);
    }
    inline Collection::Ptr Ptr::GetCollection (const String& name)
    {
        return this->get ()->GetCollection (name);
    }
    inline String Ptr::ToString () const
    {
        using namespace Characters;
        return Format ("{}"_f, static_cast<const void*> (this->get ()));
    }

}
