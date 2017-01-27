/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_Writer_h_
#define _Stroika_Foundation_DataExchange_Variant_Writer_h_    1

#include    "../../StroikaPreComp.h"

#include    <ostream>

#include    "../../Streams/OutputStream.h"

#include    "../VariantValue.h"


/**
 *  \file
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
            namespace   Variant {


                /**
                 *  \brief  abstract class specifying interface for writers VariantValue objects to serialized formats like JSON, CSV, XML, etc
                 */
                class   Writer {
                protected:
                    class   _IRep;

                protected:
                    explicit Writer (const shared_ptr<_IRep>& rep);
                    Writer () = delete; // @todo may want to allow?

                public:
                    /**
                     */
                    nonvirtual String           GetDefaultFileSuffix () const;

                public:
                    /**
                     */
                    nonvirtual  void    Write (const VariantValue& v, const Streams::OutputStream<Memory::Byte>& out);
                    nonvirtual  void    Write (const VariantValue& v, const Streams::OutputStream<Characters::Character>& out);
                    nonvirtual  void    Write (const VariantValue& v, ostream& out);
                    nonvirtual  void    Write (const VariantValue& v, wostream& out);

                public:
                    /**
                     *  Take the given variant value, and convert it to JSON, and return that JSON as a BLOB.
                     */
                    nonvirtual  Memory::BLOB    WriteAsBLOB (const VariantValue& v);

                public:
                    /**
                     *  Take the given variant value, and convert it to JSON, and return that JSON as a String.
                     */
                    nonvirtual  String  WriteAsString (const VariantValue& v);

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
                    virtual _SharedPtrIRep  Clone () const                                                                          =   0;
                    virtual String          GetDefaultFileSuffix () const                                                           =   0;
                    virtual void            Write (const VariantValue& v, const Streams::OutputStream<Memory::Byte>& out)           =   0;
                    virtual void            Write (const VariantValue& v, const Streams::OutputStream<Characters::Character>& out)  =   0;
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
#include    "Writer.inl"

#endif  /*_Stroika_Foundation_DataExchange_Variant_Writer_h_*/
