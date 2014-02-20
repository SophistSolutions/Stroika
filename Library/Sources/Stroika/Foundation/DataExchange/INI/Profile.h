/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_INI_Profile_h_
#define _Stroika_Foundation_DataExchange_INI_Profile_h_   1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Characters/String.h"
#include    "../../Containers/Collection.h"
#include    "../../Containers/Mapping.h"
#include    "../../Memory/Optional.h"

#include    "../VariantValue.h"


/**
 *  \file
 *
 * TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   INI {


				using	Containers::Collection;
				using	Containers::Mapping;
				using	Memory::Optional;


				/**
                 */
				struct  Section {
					Mapping<String,String>	fProperties;

					//TEMPORARY HACK TIL WE FIX Mapping<> to not require operator==
					// -- LGP 2014-02-19
					bool operator==(const Section& rhs) const {
					return fProperties == rhs.fProperties;
					}
				};


				/**
				 *	@todo maybe have struct with fields - unnamedprofile, and collection of named sections?
                 */
				struct	Profile {
					Section					fUnnamedSection;
					Mapping<String,Section>	fNamedSections;

				};

				Profile			Convert (VariantValue v);
				VariantValue	Convert (Profile p);


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Profile.inl"

#endif  /*_Stroika_Foundation_DataExchange_INI_Profile_h_*/
