/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_JSON_Reader_h_
#define _Stroika_Foundation_DataExchangeFormat_JSON_Reader_h_   1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Memory/VariantValue.h"
#include    "../../Streams/BinaryInputStream.h"
#include    "../../Streams/TextInputStream.h"

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
        namespace   DataExchangeFormat {
            namespace   JSON {

                Memory::VariantValue    Reader (const Streams::BinaryInputStream& in);
                Memory::VariantValue    Reader (const Streams::TextInputStream& in);

            }
        }
    }
}
#endif  /*_Stroika_Foundation_DataExchangeFormat_JSON_Reader_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
