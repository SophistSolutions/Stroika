/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"

namespace Stroika::Foundation::Database::Document {

    /*
     ********************************************************************************
     **************************** Document::Transaction *****************************
     ********************************************************************************
     */
    inline Transaction::Transaction (unique_ptr<IRep>&& rep)
        : _fRep{move (rep)}
    {
#if qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled
        this->_fThisAssertExternallySynchronized.SetAssertExternallySynchronizedCheckerContext (
            _fRep->_fThisAssertExternallySynchronized.GetSharedContext ());
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
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{this->_fThisAssertExternallySynchronized};
        RequireNotNull (_fRep);
        _fRep->Rollback ();
    }
    inline void Transaction::Commit ()
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{this->_fThisAssertExternallySynchronized};
        RequireNotNull (_fRep);
        _fRep->Commit ();
    }
    inline String Transaction::ToString () const
    {
        Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
        RequireNotNull (_fRep);
        Characters::StringBuilder sb;
        sb << "{"sv;
        sb << " disposition: "sv << _fRep->GetDisposition ();
        sb << "}"sv;
        return sb;
    }

}
namespace Stroika::Foundation::Common {
    template <>
    constexpr EnumNames<Stroika::Foundation::Database::Document::Transaction::IRep::Disposition>
        DefaultNames<Stroika::Foundation::Database::Document::Transaction::IRep::Disposition>::k{{{
            {Stroika::Foundation::Database::Document::Transaction::IRep::Disposition::eNone, L"None"},
            {Stroika::Foundation::Database::Document::Transaction::IRep::Disposition::eRolledBack, L"Rolled-Back"},
            {Stroika::Foundation::Database::Document::Transaction::IRep::Disposition::eCompleted, L"Completed"},
            {Stroika::Foundation::Database::Document::Transaction::IRep::Disposition::eFailed, L"Failed"},
        }}};
}
