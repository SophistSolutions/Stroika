/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::Database::Document {

    /*
     ********************************************************************************
     ******************************* Document::Filter *******************************
     ********************************************************************************
     */
    inline Filter::Filter (const Sequence<FilterElements::Operation>& conjunction)
        : fConjunction_{conjunction}
    {
    }
    inline Sequence<FilterElements::Operation> Filter::GetConjunctionOperations () const
    {
        return fConjunction_;
    }

}