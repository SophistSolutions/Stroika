/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_INI_Profile_h_
#define _Stroika_Foundation_DataExchange_INI_Profile_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/String.h"
#include "../../../Configuration/Common.h"
#include "../../../Containers/Collection.h"
#include "../../../Containers/Mapping.h"

#include "../../VariantValue.h"

/**
 *  \file
 */

namespace Stroika::Foundation::DataExchange::Variant::INI {

    using Containers::Collection;
    using Containers::Mapping;

    /**
     *  Each 'section' of an INI file constists of name/value pairs.
     */
    struct Section {
        Mapping<String, String> fProperties;

        bool operator== (const Section& rhs) const = default;
    };

    /**
     *  An INI file is a series of sections - one of which maybe unnamed, and the remainered are named.
     */
    struct Profile {
        Section                  fUnnamedSection;
        Mapping<String, Section> fNamedSections;

        bool operator== (const Profile& rhs) const = default;
    };

    Profile      Convert (VariantValue v);
    VariantValue Convert (Profile p);
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Profile.inl"

#endif /*_Stroika_Foundation_DataExchange_INI_Profile_h_*/
