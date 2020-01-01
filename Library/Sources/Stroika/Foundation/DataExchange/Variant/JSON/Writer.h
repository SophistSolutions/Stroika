/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_JSON_Writer_h_
#define _Stroika_Foundation_DataExchange_Variant_JSON_Writer_h_ 1

#include "../../../StroikaPreComp.h"

#include <optional>

#include "../../../Characters/FloatConversion.h"
#include "../../../Configuration/Common.h"
#include "../../../Streams/OutputStream.h"

#include "../../VariantValue.h"

#include "../Writer.h"

/**
 *  \file
 */

namespace Stroika::Foundation::DataExchange::Variant::JSON {

    /**
     *  \note   Our definition of the JSON format comes from:
     *          http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf
     *
     *          There are several variations. Please also note that http://jsonlint.com/ disallows
     *          json top level values other than array or object, but that doesn't appear to follow
     *          the standard as specified in ECMA.
     *
     *          This implementation allows any top-level value (as ECMA-404 calls for).
     *
     *  \note   Though ANY VariantValue can be mapped to a JSON object, the return trip
     *          will NOT necessarily be the same as the start object, because VariantValue supports more builtin types
     *          than JSON (e.g. Date). If you map a date to JSON, and back, you will get a String.
     *
     *  \note   Writing NAN/INF values (if fAllowNANInf set - default) - transoforms those values to strings, but on roundtrip
     *          automatically transforms them back to the correct IEEE floating point type (if you coence the VariantValue
     *          return from parsing to a floating point type).
     *
     *  \par Example Usage
     *      \code
     *          VariantValue v (3);
     *          JSON::Writer ().Write (v, IO::FileSystem::FileOutputStream ("fred.json"));
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          Mapping<String, VariantValue> m;
     *          m.Add (L"max-files", 10);
     *          m.Add (L"max-processes", 3);
     *          stringstream stringBuf;
     *          JSON::Writer ().Write (VariantValue (m), Streams::iostream::OutputStreamFromStdOStream (stringBuf));
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          VariantValue v (3); // or any other variant value - like a Mapping<String,VariantValue>
     *          String x = JSON::Writer {}.WriteAsString (v);
     *      \endcode
     *
     */
    class Writer : public Variant::Writer {
    private:
        using inherited = Variant::Writer;

    private:
        class Rep_;

    public:
        /**
         */
        struct Options {
            /**
             *  This defaults to true.
             *
             *  If false, more compact, and no leading/trailing spaces.
             */
            optional<bool> fJSONPrettyPrint;

            /**
             */
            optional<Characters::Float2StringOptions> fFloatOptions;

            /**
             *  This defaults to 4. If fJSONPrettyPrint evaluates false, then this is ignored.
             */
            optional<unsigned int> fSpacesPerIndent;

            /**
             *  This defaults to true.
             *
             *  \note   Note - NAN values (if floating point) - are also illegal, and generate a 'Require' failure.
             *          From http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf:
             *              Numeric values that cannot be represented as sequences of digits (such as Infinity and NaN) are not
             *              permitted.
             *
             *  And yet, there is a natural way to represent the numbers.
             *  We DEFAULT to allowing this. If not allowed, then preseence of NANs and INF in input, is treated as an assertion error.
             *
             *  \note - because these would be ILLEGAL JSON, - they are written as strings. So when written, they REMAIN LEGAL JSON. And when you
             *          re-read the JSON, it will be read as a string. But when you coerce that string to a floating point type, the 'INF' or whever,
             *          will be coereced to the proper IEEE floating point type.
             */
            optional<bool> fAllowNANInf;
        };

    public:
        Writer (const Options& options = Options{});

    private:
        nonvirtual shared_ptr<Rep_> GetRep_ () const;
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_DataExchange_Variant_JSON_Writer_h_*/
