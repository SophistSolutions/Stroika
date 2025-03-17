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

}
