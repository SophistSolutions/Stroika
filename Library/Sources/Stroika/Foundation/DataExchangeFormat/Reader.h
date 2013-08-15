/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_Reader_h_
#define _Stroika_Foundation_DataExchangeFormat_Reader_h_    1

#include    "../StroikaPreComp.h"

#include	<istream>

#include    "../Memory/VariantValue.h"
#include    "../Streams/BinaryInputStream.h"
#include    "../Streams/TextInputStream.h"


/**
 *  \file
 *
 * TODO:
 *      @todo   UNDOCUMENTED - PRELIMINARY
 *
 *		@todo	Consider support for wistream reader as well..
 *
 *
 *	\em Design Note:
 *		One question was whether or not to natively include support for istream sources or not.
 *		Its easy todo if not supported, by just using BinaryInputStreamFromIStreamAdapter. However,
 *		I decided it would be best to directly support it so typical users (who may not want to
 *		lookup those mapper classes) will just get the right results automagically.
 *
 *		This comports with a similar choice made in the String and Container classes (direct builtin
 *		first-class support for native STL objects where appropriate).
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {


            /**
             */
            class   Reader {
            protected:
                class _IRep;

            protected:
                // we may want to lift these restrictions? But start out restricive...
                NO_DEFAULT_CONSTRUCTOR(Reader);
                NO_ASSIGNMENT_OPERATOR(Reader);
                NO_COPY_CONSTRUCTOR(Reader);

            protected:
                explicit Reader (shared_ptr<_IRep> rep);

            public:
                /**
                 */
                Memory::VariantValue    Read (const Streams::BinaryInputStream& in);
                Memory::VariantValue    Read (const Streams::TextInputStream& in);
                Memory::VariantValue    Read (istream& in);

            private:
                shared_ptr<_IRep>   fRep_;
            };


            class Reader::_IRep {
            public:
                virtual Memory::VariantValue    Read (const Streams::BinaryInputStream& in) =   0;
                virtual Memory::VariantValue    Read (const Streams::TextInputStream& in)   =   0;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Reader.inl"

#endif  /*_Stroika_Foundation_DataExchangeFormat_Reader_h_*/
