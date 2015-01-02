/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_JSON_Reader_h_
#define _Stroika_Foundation_DataExchange_JSON_Reader_h_   1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Streams/BinaryInputStream.h"
#include    "../../Streams/TextInputStream.h"

#include    "../Reader.h"
#include    "../VariantValue.h"


/**
 *  \file
 *
 * TODO:
 *      @todo   Review \u style Unicode characters (such as "\uFDD0") parsing. Its mostly right, but
 *              some sloppiness about surrogates, versus 4-byte wchar_t, versus char16_, char32_t, etc.
 *
 *      @todo   Should be able to REDO code which curerntly uses wstring::iterator to use TextStream -
 *              its basically the same thing... (except for the issue of seekability)
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   JSON {


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

#endif  /*_Stroika_Foundation_DataExchange_JSON_Reader_h_*/
