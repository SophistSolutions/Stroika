/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::DataExchange::Variant::CharacterDelimitedLines {

    /*
     ********************************************************************************
     ******************** CharacterDelimitedLines::Writer ***************************
     ********************************************************************************
     */
    // A separate overload, rather than 'const Options& options = {}': g++ and clang (as of g++ 16, clang 22) will not use
    // a nested class's default member initializers inside its enclosing class's definition. The standard has not settled that
    // (https://cplusplus.github.io/CWG/issues/2335.html) and MSVC allows it, so this is the portable spelling, not a workaround.
    inline Writer::Writer ()
        : Writer{Options{}}
    {
    }
    inline void Writer::Write (const Traversal::Iterable<Sequence<String>>& m, ostream& out)
    {
        Write (m, _WrapBinaryOutput (out));
    }
    inline void Writer::Write (const Traversal::Iterable<Sequence<String>>& m, wostream& out)
    {
        Write (m, _WrapTextOutput (out));
    }

}
