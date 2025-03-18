/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_Document_Projection_h_
#define _Stroika_Foundation_Database_Document_Projection_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Database/Document/Document.h"

/**
 *  \file
 * 
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Database::Document {

    using Characters::String;
    using Containers::Set;

    /**
     *  \brief define a projection on a document, subsetting the fields of that document.
     * 
     *  The projection can either be specified as a set of fields to include, or a set of fields to exclude.
     * 
     *  \note As of Stroika v3.0d18, these can only be simple top level field names, but this might be someday extended like
     *        in mongodb, and perhaps using json pointer syntax?
     */
    class Projection {
    public:
        enum Flag {
            eOmit,
            eInclude
        };

    private:
        using MYV_ = variant</*monostate,*/ Set<String>, Set<String>>;

    public:
        Projection () = delete;
        inline Projection (Flag f, Set<String> fields)
        {
            if (f == eOmit) {
                fFields = MYV_{in_place_index<0>, fields};
            }
            else {
                fFields = MYV_{in_place_index<1>, fields};
            }
        }

    public:
        tuple<Flag, Set<String>> GetFields () const
        {
            if (auto i0 = get_if<0> (&fFields)) {
                return make_tuple (eOmit, *i0);
            }
            if (auto i1 = get_if<1> (&fFields)) {
                return make_tuple (eInclude, *i1);
            }
        }

    private:
        /**
         *  get<0> are fields that are used, and get<1> are fields that are omitted.
         *  ONLY specify one or the other.
         */
        variant</*monostate,*/ Set<String>, Set<String>> fFields;

    public:
        /**
         *  Apply this projection to the argument document, and return the updated document.
         */
        nonvirtual Document Apply (const Document& d) const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Projection.inl"

#endif /*_Stroika_Foundation_Database_Document_Collection_h_*/
