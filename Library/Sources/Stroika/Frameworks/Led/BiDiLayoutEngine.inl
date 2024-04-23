/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     ************************************ TextLayoutBlock ***************************
     ********************************************************************************
     */
    inline const Led_tChar* TextLayoutBlock::PeekAtRealText () const
    {
        const Led_tChar* s = nullptr;
        const Led_tChar* e = nullptr;
        PeekAtRealText_ (&s, &e);
        return s;
    }
    inline const Led_tChar* TextLayoutBlock::PeekAtVirtualText () const
    {
        const Led_tChar* s = nullptr;
        const Led_tChar* e = nullptr;
        PeekAtVirtualText_ (&s, &e);
        return s;
    }
    inline size_t TextLayoutBlock::GetTextLength () const
    {
        const Led_tChar* s = nullptr;
        const Led_tChar* e = nullptr;
        PeekAtVirtualText_ (&s, &e);
        return e - s;
    }
    inline void TextLayoutBlock::Invariant () const
    {
#if qDebug
        Invariant_ ();
#endif
    }

}
