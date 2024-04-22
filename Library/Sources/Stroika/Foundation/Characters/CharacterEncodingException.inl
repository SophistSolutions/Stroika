/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
