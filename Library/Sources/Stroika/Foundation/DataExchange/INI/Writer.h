/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_INI_Writer_h_
#define _Stroika_Foundation_DataExchange_INI_Writer_h_   1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Streams/BinaryOutputStream.h"

#include    "../VariantValue.h"
#include    "../Writer.h"


/**
 *  \file
 *
 * TODO:
 *      @todo ROUGH DRAFT
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   INI {


                /**
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
                 * Note that Write () writes in UTF-8 format to the output stream.??? UNCLEAR - DOCUMENT BETTER @todo
                 *
                 *
                 *  Example usage:
                 *      VariantValue v (3);
                 *      JSON::Writer ().Write (v, IO::FileSystem::BinaryFileOutputStream ("fred.json"));
                 *
                 *  Example usage:
                 *      Mapping<String, VariantValue> m;
                 *      m.Add (L"max-files", 10);
                 *      m.Add (L"max-processes", 3);
                 *      stringstream stringBuf;
                 *      JSON::Writer ().Write (VariantValue (m), Streams::iostream::BinaryOutputStreamFromOStreamAdapter (stringBuf));
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

#endif  /*_Stroika_Foundation_DataExchange_INI_Writer_h_*/
