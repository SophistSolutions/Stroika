/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_Writer_h_
#define _Stroika_Foundation_DataExchange_Variant_Writer_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <ostream>

#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/Streams/OutputStream.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
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

namespace Stroika::Foundation::DataExchange::Variant {

    /**
     *  \brief  abstract class specifying interface for writers VariantValue objects to serialized formats like JSON, CSV, XML, etc
     *
     *  **A Writer is configured when it is constructed, and immutable after**: what the subclass's constructor was given (its
     *  Options, say) is how it writes, and every method is const. To write differently, construct another Writer - that is
     *  cheap. Copies share one representation, never modified, so a Writer is cheap to copy - to store, to pass by value, to
     *  hand out from a factory.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    class Writer {
    protected:
        class _IRep;

    protected:
        /**
         *  \pre rep != nullptr
         */
        explicit Writer (const shared_ptr<const _IRep>& rep);
        Writer () = delete;

    public:
        /**
         */
        nonvirtual optional<filesystem::path> GetDefaultFileSuffix () const;

    public:
        /**
         *  Serialize (according to the subtype of Writer constructed) the argument VariantValue object to the 
         *  argument output 'stream' accumulator, or in the /1 case, just return the result as a BLOB (same as WriteAsBLOB).
         *
         *  @see WriteAsBLOB
         *  @see WriteAsString
         * 
         *  \note const methods because these don't change the state of the writer, but rather the underlying output stream (argument)
         */
        nonvirtual void Write (const VariantValue& v, const Streams::OutputStream::Ptr<byte>& out) const;
        nonvirtual void Write (const VariantValue& v, const Streams::OutputStream::Ptr<Characters::Character>& out) const;
        nonvirtual void Write (const VariantValue& v, ostream& out) const;
        nonvirtual void Write (const VariantValue& v, wostream& out) const;

    public:
        /**
         *  Take the given variant value, and convert it to JSON, and return that JSON as a BLOB.
         */
        [[nodiscard]] nonvirtual Memory::BLOB WriteAsBLOB (const VariantValue& v) const;

    public:
        /**
         *  Take the given variant value, and Serialize (according to the subtype of Writer constructed), 
         *  and return that JSON as a String.
         */
        [[nodiscard]] nonvirtual String WriteAsString (const VariantValue& v) const;

    protected:
        nonvirtual const _IRep& _ConstGetRep () const;

    protected:
        /**
         *  Helper for subclasses to take various kinds of output targets, and convert them to Streams::OutputStream<byte>::
         *  used by the IRep_.
         */
        static Streams::OutputStream::Ptr<byte> _WrapBinaryOutput (const Streams::OutputStream::Ptr<byte>& out);
        static Streams::OutputStream::Ptr<byte> _WrapBinaryOutput (ostream& out);

    protected:
        /**
         *  Helper for subclasses to take various kinds of output targets, and convert them to Streams::OutputStream<byte>::
         *  used by the IRep_.
         */
        static Streams::OutputStream::Ptr<Characters::Character> _WrapTextOutput (const Streams::OutputStream::Ptr<Characters::Character>& out);
        static Streams::OutputStream::Ptr<Characters::Character> _WrapTextOutput (wostream& out);

    protected:
        /**
         *  Helper for subclasses to take binary-ostream writer and return a BLOB result.
         */
        static Memory::BLOB _WriteAsBLOBHelper (const function<void (Streams::OutputStream::Ptr<byte>)>& f);

    protected:
        /**
         *  Helper for subclasses to take binary-ostream writer and return a String result.
         */
        static String _WriteAsStringHelper (const function<void (Streams::OutputStream::Ptr<Characters::Character>)>& f);

    private:
        shared_ptr<const _IRep> fRep_;

    public:
        [[deprecated ("Since v3.0d1, use WriteAsBLOB or another Write overload)")]] Memory::BLOB Write (const VariantValue& v)
        {
            return WriteAsBLOB (v);
        }
    };

    class Writer::_IRep {
    public:
        virtual ~_IRep ()                                                                                                   = default;
        virtual optional<filesystem::path> GetDefaultFileSuffix () const                                                    = 0;
        virtual void                       Write (const VariantValue& v, const Streams::OutputStream::Ptr<byte>& out) const = 0;
        virtual void Write (const VariantValue& v, const Streams::OutputStream::Ptr<Characters::Character>& out) const      = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Writer.inl"

#endif /*_Stroika_Foundation_DataExchange_Variant_Writer_h_*/
