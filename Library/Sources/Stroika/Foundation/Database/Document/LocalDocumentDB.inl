/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::Database::Document::LocalDocumentDB {

    /*
     ********************************************************************************
     ****************************** LocalDocumentDB::Ptr ****************************
     ********************************************************************************
     */
    inline LocalDocumentDB::Ptr::Ptr (const Ptr& src)
        : Ptr{Debug::UncheckedDynamicPointerCast<IRep> (src)}
    {
    }
    inline LocalDocumentDB::Ptr::Ptr (const shared_ptr<IRep>& src)
        : inherited{src}
    {
    }
    inline LocalDocumentDB::Ptr::Ptr (nullptr_t) noexcept
        : inherited{static_cast<shared_ptr<IRep>> (nullptr)}
    {
    }
    inline LocalDocumentDB::Ptr& LocalDocumentDB::Ptr::operator= (const Ptr& src)
    {
        inherited::operator= (src);
        return *this;
    }
    inline LocalDocumentDB::Ptr& LocalDocumentDB::Ptr::operator= (Ptr&& src) noexcept
    {
        inherited::operator= (move (src));
        return *this;
    }
    inline LocalDocumentDB::IRep* LocalDocumentDB::Ptr::operator->() const noexcept
    {
        return Debug::UncheckedDynamicPointerCast<IRep> (*this).get ();
    }
    inline void LocalDocumentDB::Ptr::Flush () const
    {
        Debug::UncheckedDynamicPointerCast<IRep> (*this)->Flush ();
    }

}
