/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_JSON_Reader_h_
#define _Stroika_Foundation_DataExchange_JSON_Reader_h_   1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Streams/InputStream.h"

#include    "../VariantReader.h"
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
                 *  \note   Our definition of the JSON format comes from:
                 *          http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf
                 *
                 *          There are several variations. Please also note that http://jsonlint.com/ disallows
                 *          json top level values other than array or object, but that doesnt appear to follow
                 *          the standard as specified in ECMA.
                 *
                 *  Another good summary is http://json.org/
                 *
                 *  @todo add example usage (and docs)
                 */
                class   Reader : public DataExchange::VariantReader {
                private:
                    using   inherited   =   DataExchange::VariantReader;

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
