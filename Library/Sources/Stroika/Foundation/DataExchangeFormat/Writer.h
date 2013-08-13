/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_Writer_h_
#define _Stroika_Foundation_DataExchangeFormat_Writer_h_    1

#include    "../StroikaPreComp.h"

#include    "../Memory/VariantValue.h"
#include    "../Streams/BinaryOutputStream.h"
#include    "../Streams/TextOutputStream.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   UNDOCUMENTED - PRELIMINARY
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {


            /**
             */
            class   Writer {
                // we may want to lift these restrictions? But start out restricive...
                NO_DEFAULT_CONSTRUCTOR(Writer);
                NO_ASSIGNMENT_OPERATOR(Writer);
                NO_COPY_CONSTRUCTOR(Writer);

            protected:
                class   _IRep;

            protected:
                explicit Writer (shared_ptr<_IRep> rep);
            public:
                /**
                 *  Note - to use std::ostream - use
                 *      Read (BinaryOutputStreamFromOStreamAdapter (some_ostream));
                 */
                void    Write (const Memory::VariantValue& v, const Streams::BinaryOutputStream& out);
                void    Write (const Memory::VariantValue& v, const Streams::TextOutputStream& out);

            private:
                shared_ptr<_IRep>   fRep_;
            };


            class   Writer::_IRep {
            public:
                virtual void    Write (const Memory::VariantValue& v, const Streams::BinaryOutputStream& out) = 0;
                virtual void    Write (const Memory::VariantValue& v, const Streams::TextOutputStream& out) = 0;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Writer.inl"

#endif  /*_Stroika_Foundation_DataExchangeFormat_Writer_h_*/
