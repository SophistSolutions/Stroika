/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_JSON_Reader_h_
#define _Stroika_Foundation_DataExchange_Variant_JSON_Reader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Streams/InputStream.h"

#include "../../VariantValue.h"

#include "../Reader.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   Review \u style Unicode characters (such as "\uFDD0") parsing. Its mostly right, but
 *              some sloppiness about surrogates, versus 4-byte wchar_t, versus char16_, char32_t, etc.
 *
 *      @todo   Should be able to REDO code which curerntly uses wstring::iterator to use TextStream -
 *              its basically the same thing... (except for the issue of seekability)
 */

namespace Stroika::Foundation::DataExchange::Variant::JSON {

    /**
     */
    struct ReaderOptions {
        enum Algorithm {
            eStroikaNative,
#if __has_include("boost/json.hpp")
            eBoost,
#endif

#if __has_include("boost/json.hpp")
            eDEFAULT = eBoost,
            Stroika_Define_Enum_Bounds (eStroikaNative, eBoost)
#else
            eDEFAULT = eStroikaNative,
            Stroika_Define_Enum_Bounds (eStroikaNative, eStroikaNative)
#endif
        };

        optional<Algorithm> fPreferredAlgorithm;
    };
    using ReaderOptions::Algorithm::eStroikaNative;
#if __has_include("boost/json.hpp")
    using ReaderOptions::Algorithm::eBoost;
#endif

    /**
     *  \note   Our definition of the JSON format comes from:
     *          http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf
     *
     *          There are several variations. Please also note that http://jsonlint.com/ disallows
     *          json top level values other than array or object, but that doesn't appear to follow
     *          the standard as specified in ECMA.
     *
     *  Another good summary is http://json.org/
     *
     *  \note   Any JSON object can be represented as as VariantValue object, and unambiguously mapped back to the same JSON.
     *
     *  \note   Aliases: JSONReader, JSON Reader, JSON-Reader.
     *
     *  \note   req inputStream.IsSeekable () - when reading an input stream
     *
     *  \par Example Usage
     *      \code
     *          string  srcJSON  = "[101]"; // this srcJSON can be a stream, String, string or any of a number of sources (@see DataExchange::Variant::Read)
     *          VariantValue v1 = DataExchange::Variant::JSON::Reader ().Read (srcJSON);
     *          // now unpack the result of the variant-value, with v1.GetType, v1.As<Sequence<VariantValue>> () etc...
     *      \endcode
     */
    class Reader : public Variant::Reader {
    private:
        using inherited = Variant::Reader;

    public:
        Reader (const ReaderOptions& options = {});

    private:
        class NativeRep_;
#if __has_include("boost/json.hpp")
        class BoostRep_;
#endif
        static shared_ptr<_IRep> mk_ (const ReaderOptions& options);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_DataExchange_Variant_JSON_Reader_h_*/
