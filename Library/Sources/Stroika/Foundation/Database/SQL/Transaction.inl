
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Database_SQL_Transaction_inl_
#define _Stroika_Foundation_Database_SQL_Transaction_inl_ 1

#include "../../Characters/StringBuilder.h"
#include "../../Characters/ToString.h"

namespace Stroika::Foundation::Database::SQL {

    /*
     ********************************************************************************
     **************************** SQL::Transaction **********************************
     ********************************************************************************
     */
    inline Transaction::Transaction (unique_ptr<IRep>&& rep)
        : _fRep{move (rep)}
    {
#if qDebug && 0
        // @todo debug why not compiles
        _fSharedContext = _fRep->_fSharedContext;
#endif
    }
    inline Transaction::~Transaction ()
    {
        if (_fRep->GetDisposition () == IRep::Disposition::eNone) {
            try {
                Rollback ();
            }
            catch (...) {
                DbgTrace (L"Suppress rollback failure exception in SQL transaction: %s", Characters::ToString (current_exception ()).c_str ());
                // intentially fall-thru
            }
        }
    }
    inline void Transaction::Rollback ()
    {
        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        _fRep->Rollback ();
    }
    inline void Transaction::Commit ()
    {
        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        _fRep->Commit ();
    }
    inline String Transaction::ToString () const
    {
        shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        Characters::StringBuilder                                  sb;
        sb += L"{";
        sb += L" disposition: " + Characters::ToString (_fRep->GetDisposition ());
        sb += L"}";
        return sb.str ();
    }

}
namespace Stroika::Foundation::Configuration {
    template <>
    constexpr EnumNames<Stroika::Foundation::Database::SQL::Transaction::IRep::Disposition> DefaultNames<Stroika::Foundation::Database::SQL::Transaction::IRep::Disposition>::k{
        EnumNames<Stroika::Foundation::Database::SQL::Transaction::IRep::Disposition>::BasicArrayInitializer{{
            {Stroika::Foundation::Database::SQL::Transaction::IRep::Disposition::eNone, L"None"},
            {Stroika::Foundation::Database::SQL::Transaction::IRep::Disposition::eRolledBack, L"Rolled-Back"},
            {Stroika::Foundation::Database::SQL::Transaction::IRep::Disposition::eCompleted, L"Completed"},
            {Stroika::Foundation::Database::SQL::Transaction::IRep::Disposition::eFailed, L"Failed"},
        }}};
}

#endif /*_Stroika_Foundation_Database_SQL_Transaction_inl_*/
