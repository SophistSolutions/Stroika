/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/Characters/Format.h"

namespace Stroika::Foundation::Database::Document {

    /*
     ********************************************************************************
     ***************************** Document::Projection *****************************
     ********************************************************************************
     */
    inline Projection::Projection (Flag f, const Set<String>& fields)
    {
        if (f == eOmit) {
            fFields = MyVariant_{in_place_index<0>, fields};
        }
        else {
            fFields = MyVariant_{in_place_index<1>, fields};
        }
    }
    inline auto Projection::GetFields () const -> tuple<Flag, Set<String>>
    {
        if (auto i0 = get_if<0> (&fFields)) {
            return make_tuple (eOmit, *i0);
        }
        if (auto i1 = get_if<1> (&fFields)) {
            return make_tuple (eInclude, *i1);
        }
    }

}
