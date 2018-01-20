/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_INI_Profile_h_
#define _Stroika_Foundation_DataExchange_INI_Profile_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/String.h"
#include "../../../Configuration/Common.h"
#include "../../../Containers/Collection.h"
#include "../../../Containers/Mapping.h"
#include "../../../Memory/Optional.h"

#include "../../VariantValue.h"

/**
 *  \file
 *
 * TODO:
 */

namespace Stroika {
    namespace Foundation {
        namespace DataExchange {
            namespace Variant {
                namespace INI {

                    using Containers::Collection;
                    using Containers::Mapping;
                    using Memory::Optional;

                    /**
                     */
                    struct Section {
                        Mapping<String, String> fProperties;
                    };

                    /**
                     *  @todo maybe have struct with fields - unnamedprofile, and collection of named sections?
                     */
                    struct Profile {
                        Section                  fUnnamedSection;
                        Mapping<String, Section> fNamedSections;
                    };

                    Profile      Convert (VariantValue v);
                    VariantValue Convert (Profile p);
                }
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Profile.inl"

#endif /*_Stroika_Foundation_DataExchange_INI_Profile_h_*/
