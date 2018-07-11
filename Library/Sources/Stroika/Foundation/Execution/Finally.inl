/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Finally_inl_
#define _Stroika_Foundation_Execution_Finally_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "Exceptions.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     **************************** Execution::FinallySentry **************************
     ********************************************************************************
     */
    template <typename FUNCTION>
    inline FinallySentry<FUNCTION>::FinallySentry (FUNCTION f)
        : fCleanupCodeBlock_ (move (f))
    {
    }
    template <typename FUNCTION>
    inline FinallySentry<FUNCTION>::~FinallySentry ()
    {
        // consider checking noexcept(f) and not doing the ignore, but that being helpful depends on compiler
        // analysis, which if done, probably always optimizes this try/catch anyhow
        IgnoreExceptionsForCall (fCleanupCodeBlock_ ());
    }

    /*
     ********************************************************************************
     ******************************* Execution::Finally *****************************
     ********************************************************************************
     */
    template <typename FUNCTION>
    inline auto Finally (FUNCTION f) -> FinallySentry<FUNCTION>
    {
        return {move (f)};
    }

}
#endif /*_Stroika_Foundation_Execution_Finally_inl_*/
