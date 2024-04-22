/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::DataExchange::Variant::CharacterDelimitedLines {

    /*
     ********************************************************************************
     ******************** CharacterDelimitedLines::Writer ***************************
     ********************************************************************************
     */
    inline void Writer::Write (const Traversal::Iterable<Sequence<String>>& m, ostream& out)
    {
        Write (m, _WrapBinaryOutput (out));
    }
    inline void Writer::Write (const Traversal::Iterable<Sequence<String>>& m, wostream& out)
    {
        Write (m, _WrapTextOutput (out));
    }

}
