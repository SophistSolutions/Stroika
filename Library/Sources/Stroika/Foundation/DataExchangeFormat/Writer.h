/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_Writer_h_
#define _Stroika_Foundation_DataExchangeFormat_Writer_h_    1

#include    "../StroikaPreComp.h"

#include	<ostream>

#include    "../Memory/VariantValue.h"
#include    "../Streams/BinaryOutputStream.h"
#include    "../Streams/TextOutputStream.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   UNDOCUMENTED - PRELIMINARY
 *
 *	\em Design Note:
 *		One question was whether or not to natively include support for istream sources or not.
 *		Its easy todo if not supported, by just using BinaryInputStreamFromIStreamAdapter. However,
 *		I decided it would be best to directly support it so typical users (who may not want to
 *		lookup those mapper classes) will just get the right results automagically.
 *
 *		Also note - since there are no virtual functions involved in the call, the linker/optimizer
 *		can eliminate the code if this feature isn't used.
 *
 *		This comports with a similar choice made in the String and Container classes (direct builtin
 *		first-class support for native STL objects where appropriate).
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
                nonvirtual	void    Write (const Memory::VariantValue& v, const Streams::BinaryOutputStream& out);
                nonvirtual	void    Write (const Memory::VariantValue& v, const Streams::TextOutputStream& out);
                nonvirtual	void    Write (const Memory::VariantValue& v, ostream& out);


            protected:
                shared_ptr<_IRep>   _GetRep () const;

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
