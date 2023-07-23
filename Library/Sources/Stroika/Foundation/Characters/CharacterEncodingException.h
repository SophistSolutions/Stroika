/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_CharacterEncodingException_h_
#define _Stroika_Foundation_Characters_CharacterEncodingExceptionm_h_ 1

#include "../StroikaPreComp.h"

#include <optional>

#include "../Execution/Exceptions.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 */

namespace Stroika::Foundation::Characters {

    /**
     *  \brief - An error occurred encoding or decoding a character
     * 
     *  TODO:
     *      @todo Do more to construct these with valid 'encoding' - so calls / failures can see what not only the character offset, but the
     *      encoding converting to/from where there is a problem (like turkish, or UTF-8, etc).
     */
    class CharacterEncodingException : public Execution::RuntimeErrorException<> {
    private:
        using inherited = Execution::RuntimeErrorException<>;

    public:
        enum EncodingOrDecoding {
            /**
              * CharactersToBytes
              */
            eEncoding,
            /**
              * BytesToCharacters
              */
            eDecoding
        };

    public:
        CharacterEncodingException (EncodingOrDecoding encodingOrDecoding, optional<size_t> atSourceOffset = nullopt, optional<String> encoding = nullopt);

    public:
        nonvirtual EncodingOrDecoding GetEncodingOrDecoding () const;

    public:
        nonvirtual optional<size_t> GetAtSourceOffset () const;

    public:
        /**
         *  Returns the (print name - not enumeration) of the encoding (if known) the character was being encoded into or from
         */
        nonvirtual optional<String> GetEncodiing () const;

    public:
        /**
         */
        static const CharacterEncodingException kTheEncoding;
        static const CharacterEncodingException kTheDecoding;

    private:
        EncodingOrDecoding fEncodingOrDecoding_{};
        optional<size_t>   fAtSourceOffset_{};
        optional<String>   fEncoding_;
    };
    inline const CharacterEncodingException CharacterEncodingException::kTheEncoding{CharacterEncodingException::eEncoding};
    inline const CharacterEncodingException CharacterEncodingException::kTheDecoding{CharacterEncodingException::eDecoding};

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CharacterEncodingException.inl"

#endif /*_Stroika_Foundation_Characters_CharacterEncodingException_h_*/
