/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_JSON_Writer_h_
#define _Stroika_Foundation_DataExchange_Variant_JSON_Writer_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Characters/FloatConversion.h"
#include "../../../Configuration/Common.h"
#include "../../../Streams/OutputStream.h"

#include "../../VariantValue.h"

#include "../Writer.h"

/**
 *  \file
 */

namespace Stroika {
    namespace Foundation {
        namespace DataExchange {
            namespace Variant {
                namespace JSON {

                    /**
                     *  \note   Our definition of the JSON format comes from:
                     *          http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf
                     *
                     *          There are several variations. Please also note that http://jsonlint.com/ disallows
                     *          json top level values other than array or object, but that doesn't appear to follow
                     *          the standard as specified in ECMA.
                     *
                     *  \em Note:
                     *
                     * The arguemnt VariantValue must be composed of any combination of these types:
                     *          o   VariantValue::eBoolean
                     *          o   VariantValue::eInteger
                     *          o   VariantValue::eFloat
                     *          o   VariantValue::eString
                     *          o   VariantValue::eMap
                     *          o   VariantValue::eArray
                     *  or it can be the type:
                     *          o   VariantValue::eNull
                     *
                     *  Other types are illegal an JSON and will trigger a 'Require' failure.
                     *
                     *  \note   Note - NAN values (if floating point) - are also illegal, and generate a 'Require' failure.
                     *
                     *  \note   Note that Write () writes in UTF-8 format to the output stream.??? UNCLEAR - DOCUMENT BETTER @todo
                     *
                     *  \par Example Usage
                     *      \code
                     *      VariantValue v (3);
                     *      JSON::Writer ().Write (v, IO::FileSystem::FileOutputStream ("fred.json"));
                     *      \endcode
                     *
                     *  \par Example Usage
                     *      \code
                     *      Mapping<String, VariantValue> m;
                     *      m.Add (L"max-files", 10);
                     *      m.Add (L"max-processes", 3);
                     *      stringstream stringBuf;
                     *      JSON::Writer ().Write (VariantValue (m), Streams::iostream::OutputStreamFromStdOStream (stringBuf));
                     *      \endcode
                     *
                     *  \par Example Usage
                     *      \code
                     *      VariantValue v (3); // or any other variant value - like a Mapping<String,VariantValue>
                     *      String x = JSON::Writer {}.WriteAsString (v);
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
                            Memory::Optional<bool> fJSONPrettyPrint;

                            /**
                             */
                            Memory::Optional<Characters::Float2StringOptions> fFloatOptions;

                            /**
                             *  This defaults to 4. If fJSONPrettyPrint evaluates false, then this is ignored.
                             */
                            Memory::Optional<unsigned int> fSpacesPerIndent;
                        };

                    public:
                        Writer (const Options& options = Options{});

                    private:
                        nonvirtual shared_ptr<Rep_> GetRep_ () const;
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

#endif /*_Stroika_Foundation_DataExchange_Variant_JSON_Writer_h_*/
