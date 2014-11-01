/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_CharacterDelimitedLines_Writer_h_
#define _Stroika_Foundation_DataExchange_CharacterDelimitedLines_Writer_h_   1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Streams/BinaryOutputStream.h"

#include    "../VariantValue.h"
#include    "../Writer.h"


/**
 *  \file
 *
 * TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   CharacterDelimitedLines {


                /**
                 *
                 */
                class Writer : public DataExchange::Writer {
                private:
                    using   inherited   =   DataExchange::Writer;

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

#endif  /*_Stroika_Foundation_DataExchange_CharacterDelimitedLines_Writer_h_*/
