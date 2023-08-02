/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_CharacterEncodingException_inl_
#define _Stroika_Foundation_Characters_CharacterEncodingException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ************************ Characters::CharacterEncodingException ****************
     ********************************************************************************
     */
    inline auto CharacterEncodingException::GetEncodingOrDecoding () const -> EncodingOrDecoding
    {
        return fEncodingOrDecoding_;
    }
    inline auto CharacterEncodingException::GetAtSourceOffset () const -> optional<size_t>
    {
        return fAtSourceOffset_;
    }
    inline auto CharacterEncodingException::GetEncodiing () const -> optional<String>
    {
        return fEncoding_;
    }

}

#endif /*_Stroika_Foundation_Characters_CharacterEncodingException_inl_*/
