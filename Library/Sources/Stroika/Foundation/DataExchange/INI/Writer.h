/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_INI_Writer_h_
#define _Stroika_Foundation_DataExchange_INI_Writer_h_   1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Streams/OutputStream.h"

#include    "../VariantValue.h"
#include    "../VariantWriter.h"


/**
 *  \file
 *
 * TODO:
 *      @todo   Write() NYI
 *
 *      @todo   See Reader () docs (thats largely implemented)
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   INI {


                /**
                 */
                class Writer : public DataExchange::VariantWriter {
                private:
                    using   inherited   =   DataExchange::VariantWriter;

                private:
                    class   Rep_;

                public:
                    Writer ();

                private:
                    nonvirtual  shared_ptr<Rep_>    GetRep_ () const;
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

#endif  /*_Stroika_Foundation_DataExchange_INI_Writer_h_*/
