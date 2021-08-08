
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Database_SQL_Connection_inl_
#define _Stroika_Foundation_Database_SQL_Connection_inl_ 1

#include "../../Characters/Format.h"

namespace Stroika::Foundation::Database::SQL {

    /*
     ********************************************************************************
     ****************************** SQL::Connection::Ptr ****************************
     ********************************************************************************
     */
    inline Connection::Ptr::Ptr (const Ptr& src)
        : Ptr{src._fRep}
    {
    }
    inline Connection::Ptr::Ptr (const shared_ptr<IRep>& src)
        : _fRep{src}
    {
    }
    inline Connection::Ptr& Connection::Ptr::operator= (const Ptr& src)
    {
        if (this != &src) {
            _fRep = src._fRep;
#if qDebug
            if (_fRep) {
                _fSharedContext = _fRep->_fSharedContext;
            }
#endif
        }
        return *this;
    }
    inline Connection::Ptr& Connection::Ptr::operator= (Ptr&& src) noexcept
    {
        if (this != &src) {
            _fRep = move (src._fRep);
#if qDebug
            if (_fRep) {
                _fSharedContext = _fRep->_fSharedContext;
            }
#endif
        }
        return *this;
    }
    inline Connection::IRep* Connection::Ptr::operator-> () const noexcept
    {
        return _fRep.get ();
    }
    inline auto Connection::Ptr::operator== (const Ptr& rhs) const
    {
        return _fRep == rhs._fRep;
    }
    inline bool Connection::Ptr::operator== (nullptr_t) const noexcept
    {
        return _fRep.get () == nullptr;
    }
#if __cpp_impl_three_way_comparison < 201907
    inline bool Connection::Ptr::operator!= (const Ptr& rhs) const
    {
        return _fRep == rhs._fRep;
    }
    inline bool Connection::Ptr::operator!= (nullptr_t) const
    {
        return _fRep != nullptr;
    }
#endif
    inline shared_ptr<const EngineProperties> Connection::Ptr::GetEngineProperties () const
    {
        return _fRep->GetEngineProperties ();
    }
    inline void Connection::Ptr::Exec (const String& sql) const
    {
        _fRep->Exec (sql);
    }
    inline String Connection::Ptr::ToString () const
    {
        return Characters::Format (L"%p", _fRep.get ());
    }
#if qDebug
    inline auto Connection::Ptr::GetSharedContext () const -> shared_ptr<SharedContext>
    {
        return this->_fSharedContext;
    }
#endif

}

#endif /*_Stroika_Foundation_Database_SQL_Connection_inl_*/
