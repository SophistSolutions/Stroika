/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

#include "Stroika/Foundation/Debug/Cast.h"

namespace Stroika::Foundation::Database::Document::MongoDBClient {

#if qStroika_HasComponent_mongocxxdriver
    /*
     ********************************************************************************
     **************** Document::MongoDBClient::AdminConnection::Ptr *****************
     ********************************************************************************
     */
    inline AdminConnection::Ptr::Ptr (const shared_ptr<IRep>& src) noexcept
        : inherited{src}
    {
    }
    inline AdminConnection::Ptr::Ptr (nullptr_t) noexcept
        : inherited{nullptr}
    {
    }
    inline Document AdminConnection::Ptr::run_command (const Document& v)
    {
        return get ()->run_command (v);
    }
    inline mongocxx::client& AdminConnection::Ptr::GetClientRef ()
    {
        return get ()->GetClientRef ();
    }
    inline Set<String> AdminConnection::Ptr::GetDatabases ()
    {
        return get ()->GetDatabases ();
    }
    inline void AdminConnection::Ptr::DropDatabase (const String& dbName)
    {
        get ()->DropDatabase (dbName);
    }
    inline void AdminConnection::Ptr::CreateDatabase (const String& dbName)
    {
        get ()->CreateDatabase (dbName);
    }

    /*
     ********************************************************************************
     ******************* Document::MongoDBClient::Connection::Ptr *******************
     ********************************************************************************
     */
    inline Connection::Ptr::Ptr (const Ptr& src)
        : Ptr{Debug::UncheckedDynamicPointerCast<IRep> (src)}
    {
    }
    inline Connection::Ptr::Ptr (nullptr_t) noexcept
        : inherited{nullptr}
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
    inline Connection::IRep* Connection::Ptr::operator->() const noexcept
    {
        return Debug::UncheckedDynamicPointerCast<IRep> (*this).get ();
    }
#endif

}
