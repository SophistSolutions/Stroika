/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::DataExchange::Variant::FormURLEncoded {

    /*
     ********************************************************************************
     **************************** FormURLEncoded::Reader ****************************
     ********************************************************************************
     */
    inline Association<String, String> Reader::ReadAssociation (const Traversal::Iterable<Character>& in)
    {
        return ReadAssociation (_ToCharacterReader (in));
    }
    inline Association<String, String> Reader::ReadAssociation (istream& in)
    {
        return ReadAssociation (_ToByteReader (in));
    }
    inline Association<String, String> Reader::ReadAssociation (wistream& in)
    {
        return ReadAssociation (_ToCharacterReader (in));
    }

}
