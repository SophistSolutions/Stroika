/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::DataExchange::Variant::CharacterDelimitedLines {

    /*
     ********************************************************************************
     ******************** CharacterDelimitedLines::Writer ***************************
     ********************************************************************************
     */
#if qCompilerAndStdLib_default_member_initializer_needed_within_enclosing_class_Buggy
    inline Writer::Writer ()
        : Writer{Options{}}
    {
    }
#endif
    inline void Writer::Write (const Traversal::Iterable<Sequence<String>>& m, ostream& out) const
    {
        Write (m, _WrapBinaryOutput (out));
    }
    inline void Writer::Write (const Traversal::Iterable<Sequence<String>>& m, wostream& out) const
    {
        Write (m, _WrapTextOutput (out));
    }

}
