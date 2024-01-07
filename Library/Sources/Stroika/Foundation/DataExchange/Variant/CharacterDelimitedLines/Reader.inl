/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_CharacterDelimitedLines_Reader_inl_
#define _Stroika_Foundation_DataExchange_Variant_CharacterDelimitedLines_Reader_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::DataExchange::Variant::CharacterDelimitedLines {

    /*
     ********************************************************************************
     ******************** CharacterDelimitedLines::Reader ***************************
     ********************************************************************************
     */
    inline Iterable<Sequence<String>> Reader::ReadMatrix (const Traversal::Iterable<Characters::Character>& in)
    {
        return ReadMatrix (_ToCharacterReader (in));
    }
    inline Iterable<Sequence<String>> Reader::ReadMatrix (const Memory::BLOB& in)
    {
        return ReadMatrix (_ToByteReader (in));
    }
    inline Iterable<Sequence<String>> Reader::ReadMatrix (istream& in)
    {
        return ReadMatrix (_ToByteReader (in));
    }
    inline Iterable<Sequence<String>> Reader::ReadMatrix (wistream& in)
    {
        return ReadMatrix (_ToCharacterReader (in));
    }

}
#endif /*_Stroika_Foundation_DataExchange_Variant_CharacterDelimitedLines_Reader_inl_*/
