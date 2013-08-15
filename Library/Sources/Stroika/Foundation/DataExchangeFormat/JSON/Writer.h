/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_JSON_Writer_h_
#define _Stroika_Foundation_DataExchangeFormat_JSON_Writer_h_   1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Memory/VariantValue.h"
#include    "../../Streams/BinaryOutputStream.h"

#include    "../Writer.h"


/**
 *  \file
 *
 * TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {
            namespace   JSON {


                /**
				 *
				 *	\em Note:
				 *
                 * The arguemnt VariantValue must be composed of any combination of these types:
                 *          o   Memory::VariantValue::eBoolean
                 *          o   Memory::VariantValue::eInteger
                 *          o   Memory::VariantValue::eFloat
                 *          o   Memory::VariantValue::eString
                 *          o   Memory::VariantValue::eMap
                 *          o   Memory::VariantValue::eArray
                 *  or it can be the type:
                 *          o   Memory::VariantValue::eNull
                 *
                 *  Other types are illegal an JSON and will trigger a 'Require' failure.
                 *
                 * Note that Write () writes in UTF-8 format to the output stream.??? UNCLEAR - DOCUMENT BETTER @todo
                 *
                 *
				 *	Example usage:
				 *		Memory::VariantValue v (3);
				 *		JSON::Writer ().Write (v, IO::FileSystem::BinaryFileOutputStream ("fred.json"));
				 *
				 *	Example usage:
				 *		Mapping<String, Memory::VariantValue> m;
				 *		m.Add (L"max-files", 10);
				 *		m.Add (L"max-processes", 3);
				 *		stringstream stringBuf;
				 *		JSON::Writer ().Write (Memory::VariantValue (m), Streams::iostream::BinaryOutputStreamFromOStreamAdapter (stringBuf));
                 */
                class Writer : public DataExchangeFormat::Writer {
                private:
                    typedef DataExchangeFormat::Writer  inherited;

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

#endif  /*_Stroika_Foundation_DataExchangeFormat_JSON_Writer_h_*/
