/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_INI_Writer_h_
#define _Stroika_Foundation_DataExchange_INI_Writer_h_   1

#include    "../../../StroikaPreComp.h"

#include    "../../../Configuration/Common.h"
#include    "../../../Streams/OutputStream.h"

#include    "../../VariantValue.h"

#include    "../Writer.h"


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
            namespace   Variant {
                namespace   INI {


                    /**
                     */
                    class Writer : public Variant::VariantWriter {
                    private:
                        using   inherited   =   Variant::VariantWriter;

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
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_DataExchange_INI_Writer_h_*/
