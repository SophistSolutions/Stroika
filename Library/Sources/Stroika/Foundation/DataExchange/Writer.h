/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Writer_h_
#define _Stroika_Foundation_DataExchange_Writer_h_    1

#include    "../StroikaPreComp.h"

#include    <ostream>

#include    "../Streams/BinaryOutputStream.h"
#include    "../Streams/TextOutputStream.h"

#include    "VariantValue.h"


/**
 *  \file
 *
 * TODO:
 *      @todo   UNDOCUMENTED - PRELIMINARY
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
            class   Writer {
            protected:
                class   _IRep;

            protected:
                explicit Writer (shared_ptr<_IRep> rep);
                Writer () = delete; // @todo may want to allow?

            public:
                /**
                 */
                nonvirtual String           GetDefaultFileSuffix () const;

            public:
                /**
                 *  Note - to use std::ostream - use
                 *      Read (BinaryOutputStreamFromOStreamAdapter (some_ostream));
                 */
                nonvirtual  void    Write (const VariantValue& v, const Streams::BinaryOutputStream& out);
                nonvirtual  void    Write (const VariantValue& v, const Streams::TextOutputStream& out);
                nonvirtual  void    Write (const VariantValue& v, ostream& out);
                nonvirtual  void    Write (const VariantValue& v, wostream& out);

            protected:
                nonvirtual  _IRep&          _GetRep ();
                nonvirtual  const _IRep&    _GetRep () const;

            protected:
                using   _SharedPtrIRep      =   shared_ptr<_IRep>;

            private:
                struct  _Rep_Cloner {
                    inline  static  _SharedPtrIRep   Copy (const _IRep& t);
                };
                using   SharedRepByValuePtr_        =   Memory::SharedByValue<Memory::SharedByValue_Traits<_IRep, _SharedPtrIRep, _Rep_Cloner>>;

            private:
                SharedRepByValuePtr_   fRep_;
            };


            class   Writer::_IRep {
            public:
                virtual _SharedPtrIRep  Clone () const                                                          =   0;
                virtual String          GetDefaultFileSuffix () const                                           =   0;
                virtual void            Write (const VariantValue& v, const Streams::BinaryOutputStream& out)   =   0;
                virtual void            Write (const VariantValue& v, const Streams::TextOutputStream& out)     =   0;
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

#endif  /*_Stroika_Foundation_DataExchange_Writer_h_*/
