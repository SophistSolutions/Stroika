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
     *  \brief define a (simple) projection on a document, subsetting the fields of that document.
     * 
     *  The projection can either be specified as a set of fields to include, or a set of fields to exclude.
     * 
     *  \note As of Stroika v3.0d18, these can only be simple top level field names, but this might be someday extended like
     *        in mongodb, and perhaps using json pointer syntax?
     */
    class Projection {
    private:
        // use optional<Projection> so no need for monostate/default-constructible Projection
        using MyVariant_ = variant<Set<String>, Set<String>>;

    public:
        enum class Flag {
            eOmit    = 0,
            eInclude = 1
        };
        using Flag::eInclude;
        using Flag::eOmit;

    public:
        /**
         */
        Projection () = delete;
        Projection (Flag f, const Set<String>& fields);

    public:
        /**
         */
        nonvirtual tuple<Flag, Set<String>> GetFields () const;

    public:
        /**
         *  Apply this projection to the argument document, and return the updated document.
         */
        nonvirtual Database::Document::Document Apply (const Database::Document::Document& d) const;

    public:
        /**
         *  See if fieldName included after Projection applied
         */
        nonvirtual bool Includes (const String& fieldName) const;

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    public:
        /**
         */
        nonvirtual bool operator== (const Projection&) const = default;

    public:
        /**
         */
        nonvirtual auto operator<=> (const Projection&) const = default;

    private:
        /**
         *  get<0> are fields that are omitted, and get<1> are fields that are omitted.
         *  ONLY specify one or the other.
         */
        MyVariant_ fFields_;
    };

    /**
     */
    inline const Projection kOmitIDs = Projection{Projection::eOmit, {"id"sv}};

    /**
     */
    inline const Projection kOnlyIDs = Projection{Projection::eInclude, get<Set<String>> (kOmitIDs.GetFields ())};

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Projection.inl"

#endif /*_Stroika_Foundation_Database_Document_Projection_h_*/
