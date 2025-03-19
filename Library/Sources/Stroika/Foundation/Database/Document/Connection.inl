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
    inline Ptr::Ptr (const Ptr& src)
        : Ptr{src._fRep}
    {
    }
    inline Ptr::Ptr (const shared_ptr<IRep>& src)
        : _fRep{src}
    {
    }
    inline Ptr& Ptr::operator= (const Ptr& src)
    {
        if (this != &src) {
            _fRep = src._fRep;
        }
        return *this;
    }
    inline Ptr& Ptr::operator= (Ptr&& src) noexcept
    {
        if (this != &src) {
            _fRep = move (src._fRep);
        }
        return *this;
    }
    inline IRep* Ptr::operator->() const noexcept
    {
        return _fRep.get ();
    }
    inline auto Ptr::operator== (const Ptr& rhs) const
    {
        return _fRep == rhs._fRep;
    }
    inline bool Ptr::operator== (nullptr_t) const noexcept
    {
        return _fRep.get () == nullptr;
    }
    inline Set<String> Ptr::GetCollections ()
    {
        return _fRep->GetCollections ();
    }
    inline void Ptr::CreateCollection (const String& name)
    {
        _fRep.get ()->CreateCollection (name);
    }
    inline void Ptr::DropCollection (const String& name)
    {
        _fRep.get ()->DropCollection (name);
    }
    inline Collection::Ptr Ptr::GetCollection (const String& name)
    {
        return _fRep.get ()->GetCollection (name);
    }
    inline String Ptr::ToString () const
    {
        using namespace Characters;
        return Format ("{}"_f, static_cast<const void*> (_fRep.get ()));
    }

}
