
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Database_SQL_Connection_inl_
#define _Stroika_Foundation_Database_SQL_Connection_inl_ 1

#include "../../Characters/Format.h"

namespace Stroika::Foundation::Database::SQL::Connection {

    /*
     ********************************************************************************
     ****************************** SQL::Connection::Ptr ****************************
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
    inline shared_ptr<const EngineProperties> Ptr::GetEngineProperties () const
    {
        return _fRep->GetEngineProperties ();
    }
    inline void Ptr::Exec (const String& sql) const
    {
        _fRep->Exec (sql);
    }
    inline String Ptr::ToString () const
    {
        return Characters::Format (L"%p", _fRep.get ());
    }

}

#endif /*_Stroika_Foundation_Database_SQL_Connection_inl_*/
