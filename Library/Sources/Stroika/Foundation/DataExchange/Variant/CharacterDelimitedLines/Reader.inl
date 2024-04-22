/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
    inline Iterable<Sequence<String>> Reader::ReadMatrix (istream& in)
    {
        return ReadMatrix (_ToByteReader (in));
    }
    inline Iterable<Sequence<String>> Reader::ReadMatrix (wistream& in)
    {
        return ReadMatrix (_ToCharacterReader (in));
    }

}
