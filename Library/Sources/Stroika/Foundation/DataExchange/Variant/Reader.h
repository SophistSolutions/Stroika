/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_Reader_h_
#define _Stroika_Foundation_DataExchange_Variant_Reader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <istream>

#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/Streams/InputStream.h"

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

namespace Stroika::Foundation::Memory {
    class BLOB;
}

namespace Stroika::Foundation::DataExchange::Variant {

    /**
     *  \brief  abstract class specifying interface for readers that map a source like XML or JSON to a VariantValue objects
     *
     *  **A Reader is configured when it is constructed, and immutable after**: what the subclass's constructor was given (its
     *  Options, say) is how it reads, and every method is const. To read differently, construct another Reader - that is
     *  cheap. Copies share one representation, never modified, so a Reader is cheap to copy - to store, to pass by value, to
     *  hand out from a factory.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    class Reader {
    protected:
        class _IRep;

    protected:
        Reader () = delete; // @todo may want to allow?

    protected:
        /**
         *  \pre rep != nullptr
         */
        explicit Reader (const shared_ptr<const _IRep>& rep);

    public:
        /**
         */
        nonvirtual optional<filesystem::path> GetDefaultFileSuffix () const;

    public:
        /**
         */
        nonvirtual VariantValue Read (const Streams::InputStream::Ptr<byte>& in) const;
        nonvirtual VariantValue Read (const Streams::InputStream::Ptr<Characters::Character>& in) const;
        nonvirtual VariantValue Read (const Traversal::Iterable<Characters::Character>& in) const;
        nonvirtual VariantValue Read (istream& in) const;
        nonvirtual VariantValue Read (wistream& in) const;

    protected:
        nonvirtual const _IRep& _ConstGetRep () const;

    protected:
        /**
         * \brief simple helper so subclasses can more easily provide varied Read overloads
         */
        static Streams::InputStream::Ptr<byte> _ToByteReader (const Streams::InputStream::Ptr<byte>& in);
        static Streams::InputStream::Ptr<byte> _ToByteReader (const Streams::InputStream::Ptr<Characters::Character>& in);
        static Streams::InputStream::Ptr<byte> _ToByteReader (const Memory::BLOB& in);
        static Streams::InputStream::Ptr<byte> _ToByteReader (istream& in);

    protected:
        /**
         * \brief simple helper so subclasses can more easily provide varied Read overloads
         */
        static Streams::InputStream::Ptr<Characters::Character> _ToCharacterReader (const Streams::InputStream::Ptr<Characters::Character>& in);
        static Streams::InputStream::Ptr<Characters::Character> _ToCharacterReader (const Traversal::Iterable<Characters::Character>& in);
        static Streams::InputStream::Ptr<Characters::Character> _ToCharacterReader (wistream& in);

    private:
        shared_ptr<const _IRep> fRep_;
    };

    /**
     */
    class Reader::_IRep {
    public:
        virtual ~_IRep ()                                                                                          = default;
        virtual optional<filesystem::path> GetDefaultFileSuffix () const                                           = 0;
        virtual VariantValue               Read (const Streams::InputStream::Ptr<byte>& in) const                  = 0;
        virtual VariantValue               Read (const Streams::InputStream::Ptr<Characters::Character>& in) const = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Reader.inl"

#endif /*_Stroika_Foundation_DataExchange_Variant_Reader_h_*/
