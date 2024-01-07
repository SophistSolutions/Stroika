/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_CharacterDelimitedLines_Writer_inl_
#define _Stroika_Foundation_DataExchange_Variant_CharacterDelimitedLines_Writer_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
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
#endif /*_Stroika_Foundation_DataExchange_Variant_CharacterDelimitedLines_Writer_inl_*/
