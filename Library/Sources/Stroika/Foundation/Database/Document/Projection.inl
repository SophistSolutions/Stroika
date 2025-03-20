/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

namespace Stroika::Foundation::Database::Document {

    /*
     ********************************************************************************
     ***************************** Document::Projection *****************************
     ********************************************************************************
     */
    inline Projection::Projection (Flag f, const Set<String>& fields)
        : fFields_{f == eOmit ? MyVariant_{in_place_index<0>, fields} : MyVariant_{in_place_index<1>, fields}}
    {
    }
    inline auto Projection::GetFields () const -> tuple<Flag, Set<String>>
    {
        if (auto i0 = get_if<0> (&fFields_)) {
            return make_tuple (eOmit, *i0);
        }
        if (auto i1 = get_if<1> (&fFields_)) {
            return make_tuple (eInclude, *i1);
        }
        AssertNotReached ();
        return make_tuple (eOmit, Set<String>{});
    }

}
