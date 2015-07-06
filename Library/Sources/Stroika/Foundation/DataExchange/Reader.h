/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Reader_h_
#define _Stroika_Foundation_DataExchange_Reader_h_    1

#include    "../StroikaPreComp.h"

#include    <istream>

#include    "../Memory/SharedByValue.h"
#include    "../Streams/InputStream.h"

#include    "VariantValue.h"


/**
 *  \file
 *
 * TODO:
 *      @todo   UNDOCUMENTED - PRELIMINARY
 *
 *
 *  \em Design Note:
 *      One question was whether or not to natively include support for istream sources or not.
 *      Its easy todo if not supported, by just using BinaryInputStreamFromIStreamAdapter. However,
 *      I decided it would be best to directly support it so typical users (who may not want to
 *      lookup those mapper classes) will just get the right results automagically.
 *
 *      Also note - since there are no virtual functions involved in the call, the linker/optimizer
 *      can eliminate the code if this feature isn't used.
 *
 *      This comports with a similar choice made in the String and Container classes (direct builtin
 *      first-class support for native STL objects where appropriate).
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


            /**
             */
            class   Reader {
            protected:
                class   _IRep;

            protected:
                Reader () = delete; // @todo may want to allow?

            protected:
                explicit Reader (shared_ptr<_IRep> rep);

            public:
                /**
                 */
                nonvirtual String           GetDefaultFileSuffix () const;

            public:
                /**
                 */
                nonvirtual  VariantValue    Read (const Streams::InputStream<Memory::Byte>& in);
                nonvirtual  VariantValue    Read (const Streams::InputStream<Characters::Character>& in);
                nonvirtual  VariantValue    Read (istream& in);
                nonvirtual  VariantValue    Read (wistream& in);

            protected:
                nonvirtual  _IRep&          _GetRep ();
                nonvirtual  const _IRep&    _GetRep () const;

            protected:
                using   _SharedPtrIRep  =   shared_ptr<_IRep>;

            private:
                struct  _Rep_Cloner {
                    inline  static  _SharedPtrIRep   Copy (const _IRep& t);
                };
                using   SharedRepByValuePtr_    =   Memory::SharedByValue<Memory::SharedByValue_Traits<_IRep, _SharedPtrIRep, _Rep_Cloner>>;

            private:
                SharedRepByValuePtr_   fRep_;
            };


            class   Reader::_IRep {
            public:
                virtual _SharedPtrIRep  Clone () const                                                  =   0;
                virtual String          GetDefaultFileSuffix () const                                   =   0;
                virtual VariantValue    Read (const Streams::InputStream<Memory::Byte>& in)             =   0;
                virtual VariantValue    Read (const Streams::InputStream<Characters::Character>& in)    =   0;
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

#endif  /*_Stroika_Foundation_DataExchange_Reader_h_*/
