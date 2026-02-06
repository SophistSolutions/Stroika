/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Execution/Activity.h"
#include "Stroika/Foundation/Execution/Finally.h"

namespace Stroika::Foundation::Database::Document::Connection {

    /*
     ********************************************************************************
     ************************* Document::Connection::Ptr ****************************
     ********************************************************************************
     */
    inline Ptr::Ptr (const inherited& i) noexcept
        : inherited{i}
    {
    }
    inline bool Ptr::operator== (const Ptr& rhs) const noexcept
    {
        return this->get () == rhs.get ();
    }
    inline bool Ptr::operator== (nullptr_t) const noexcept
    {
        return this->get () == nullptr;
    }
    inline auto Ptr::GetEngineProperties () const -> shared_ptr<const EngineProperties>
    {
        return this->get ()->GetEngineProperties ();
    }
    inline auto Ptr::GetOptions () const -> Options
    {
        return this->get ()->GetOptions ();
    }
    inline uintmax_t Ptr::GetSpaceConsumed () const
    {
        auto                       activity = Execution::LazyEvalActivity{[&] () -> String { return "Getting database size"sv; }};
        Execution::DeclareActivity da{&activity};
        return this->get ()->GetSpaceConsumed ();
    }
    inline Set<String> Ptr::GetCollections () const
    {
        auto activity = Execution::LazyEvalActivity{[&] () -> String { return "Getting collections from document database"sv; }};
        Execution::DeclareActivity da{&activity};
        return this->get ()->GetCollections ();
    }
    inline Collection::Ptr Ptr::CreateCollection (const String& name) const
    {
        auto activity = Execution::LazyEvalActivity{[&] () -> String { return "Creating collection in document database"sv; }};
        Execution::DeclareActivity da{&activity};
        return this->get ()->CreateCollection (name);
    }
    inline void Ptr::DropCollection (const String& name) const
    {
        auto activity = Execution::LazyEvalActivity{[&] () -> String { return "Dropping collection from document database"sv; }};
        Execution::DeclareActivity da{&activity};
        this->get ()->DropCollection (name);
    }
    inline Collection::Ptr Ptr::GetCollection (const String& name) const
    {
        auto activity = Execution::LazyEvalActivity{[&] () -> String { return "Gett collection from document database"sv; }};
        Execution::DeclareActivity da{&activity};
        return this->get ()->GetCollection (name);
    }
    inline String Ptr::ToString () const
    {
        using namespace Characters;
        return Format ("{}"_f, static_cast<const void*> (this->get ()));
    }
}

namespace Stroika::Foundation::Database::Document::Connection::Private_ {
    /**
     * Private utility to faciliate logging and tracking of database reads/writes times
     */
    template <typename FUN>
    auto WrapLoggingExecuteHelper_ (FUN&& f, Connection::IRep* documentDBConnection, const Connection::Options& options,
                                    const optional<String>& collectionName, bool write)
    {
        if (options.fOperationLoggingCallback == nullptr) {
            return f ();
        }
        else {
            auto            callback = options.fOperationLoggingCallback;
            Connection::Ptr connPtr  = documentDBConnection->shared_from_this ();
            callback (write ? Operation::eStartingWrite : Operation::eStartingRead, connPtr, collectionName, nullptr);
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept {
                callback (write ? Operation::eCompletedWrite : Operation::eCompletedRead, connPtr, collectionName, nullptr);
            });
            try {
                return f ();
            }
            catch (...) {
                callback (Operation::eNotifyError, connPtr, collectionName, current_exception ());
                Execution::ReThrow ();
            }
        }
    }
}
