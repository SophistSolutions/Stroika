
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Database_SQL_ODBC_inl_
#define _Stroika_Foundation_Database_SQL_ODBC_inl_ 1

namespace Stroika::Foundation::Database::SQL::ODBC {

#if qHasLibrary_ODBC
    /*
     ********************************************************************************
     ****************************** ODBC::Connection::Ptr ***************************
     ********************************************************************************
     */
    inline Connection::Ptr::Ptr (const Ptr& src)
        : Ptr{dynamic_pointer_cast<IRep> (src._fRep)}
    {
    }
    inline Connection::Ptr& Connection::Ptr::operator= (const Ptr& src)
    {
        inherited::operator= (src);
        return *this;
    }
    inline Connection::Ptr& Connection::Ptr::operator= (Ptr&& src) noexcept
    {
        inherited::operator= (move (src));
        return *this;
    }
    inline Connection::IRep* Connection::Ptr::operator-> () const noexcept
    {
        return dynamic_pointer_cast<IRep> (_fRep).get ();
    }
#endif

}

#endif /*_Stroika_Foundation_Database_SQL_ODBC_inl_*/
