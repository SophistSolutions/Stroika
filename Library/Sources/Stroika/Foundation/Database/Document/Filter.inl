/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

namespace Stroika::Foundation::Database::Document {

    /*
     ********************************************************************************
     ******************************* Document::Filter *******************************
     ********************************************************************************
     */
    inline Filter::Filter (const Sequence<FilterElements::Operation>& andedOperations)
        : fAndedOperations_{andedOperations}
    {
    }
    inline Sequence<FilterElements::Operation> Filter::GetConjunctionOperations () const
    {
        return fAndedOperations_;
    }

}