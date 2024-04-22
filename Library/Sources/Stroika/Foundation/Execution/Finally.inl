/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Throw.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ****************** Execution::Private_::FinallySentry **************************
     ********************************************************************************
     */
    template <Configuration::INoThrowInvocable FUNCTION>
    inline Private_::FinallySentry<FUNCTION>::FinallySentry (FUNCTION&& f)
        : fCleanupCodeBlock_{std::forward<FUNCTION> (f)}
    {
    }
    template <Configuration::INoThrowInvocable FUNCTION>
    inline Private_::FinallySentry<FUNCTION>::~FinallySentry ()
    {
        // No need for IgnoreExceptionsForCall, because we assure its no-throw invocable
        fCleanupCodeBlock_ ();
    }

    /*
     ********************************************************************************
     ******************************* Execution::Finally *****************************
     ********************************************************************************
     */
    template <Configuration::INoThrowInvocable FUNCTION>
    inline auto Finally (FUNCTION&& f) -> Private_::FinallySentry<FUNCTION>
    {
        return {std::forward<FUNCTION> (f)};
    }

}
