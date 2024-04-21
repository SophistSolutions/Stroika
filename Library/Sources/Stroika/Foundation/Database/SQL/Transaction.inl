
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Database_SQL_Transaction_inl_
#define _Stroika_Foundation_Database_SQL_Transaction_inl_ 1

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"

namespace Stroika::Foundation::Database::SQL {

    /*
     ********************************************************************************
     **************************** SQL::Transaction **********************************
     ********************************************************************************
     */
    inline Transaction::Transaction (unique_ptr<IRep>&& rep)
        : _fRep{move (rep)}
    {
#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
        this->_fThisAssertExternallySynchronized.SetAssertExternallySynchronizedMutexContext (_fRep->_fThisAssertExternallySynchronized.GetSharedContext ());
#endif
    }
    inline Transaction::~Transaction ()
    {
        // Since we allow move, that nulls out _fRep
        if (_fRep != nullptr) {
            if (_fRep->GetDisposition () == IRep::Disposition::eNone) {
                try {
                    Rollback ();
                }
                catch (...) {
                    using namespace Characters::Literals;
                    DbgTrace ("Suppress rollback failure exception in SQL transaction: {}"_f, current_exception ());
                    // intentially fall-thru
                }
            }
        }
    }
    inline void Transaction::Rollback ()
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{this->_fThisAssertExternallySynchronized};
        RequireNotNull (_fRep);
        _fRep->Rollback ();
    }
    inline void Transaction::Commit ()
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{this->_fThisAssertExternallySynchronized};
        RequireNotNull (_fRep);
        _fRep->Commit ();
    }
    inline String Transaction::ToString () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        RequireNotNull (_fRep);
        Characters::StringBuilder sb;
        sb << "{"sv;
        sb << " disposition: "sv << _fRep->GetDisposition ();
        sb << "}"sv;
        return sb.str ();
    }

}
namespace Stroika::Foundation::Configuration {
    template <>
    constexpr EnumNames<Stroika::Foundation::Database::SQL::Transaction::IRep::Disposition>
        DefaultNames<Stroika::Foundation::Database::SQL::Transaction::IRep::Disposition>::k{{{
            {Stroika::Foundation::Database::SQL::Transaction::IRep::Disposition::eNone, L"None"},
            {Stroika::Foundation::Database::SQL::Transaction::IRep::Disposition::eRolledBack, L"Rolled-Back"},
            {Stroika::Foundation::Database::SQL::Transaction::IRep::Disposition::eCompleted, L"Completed"},
            {Stroika::Foundation::Database::SQL::Transaction::IRep::Disposition::eFailed, L"Failed"},
        }}};
}

#endif /*_Stroika_Foundation_Database_SQL_Transaction_inl_*/
