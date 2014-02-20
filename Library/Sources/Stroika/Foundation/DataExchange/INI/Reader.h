/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_INI_Reader_h_
#define _Stroika_Foundation_DataExchange_INI_Reader_h_   1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Containers/Collection.h"

#include    "../Reader.h"
#include    "../VariantValue.h"

#include	"Profile.h"


/**
 *  \file
 *
 * TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   INI {


                /**
                 *  @todo add example usage (and docs)
                 */
                class Reader : public DataExchange::Reader {
                private:
                    using   inherited   =   DataExchange::Reader;

                private:
                    class   Rep_;

                public:
                    Reader ();

				public:
					/**
					*/
					nonvirtual	Profile	ReadProfile (const Streams::BinaryInputStream& in);
					nonvirtual	Profile	ReadProfile (const Streams::TextInputStream& in);
					nonvirtual	Profile	ReadProfile (istream& in);
					nonvirtual	Profile	ReadProfile (wistream& in);
                };


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Reader.inl"

#endif  /*_Stroika_Foundation_DataExchange_INI_Reader_h_*/
