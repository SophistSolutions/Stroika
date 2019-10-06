/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_BiDiLayoutEngine_inl_
#define _Stroika_Frameworks_Led_BiDiLayoutEngine_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
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

    /*
     ********************************************************************************
     *********************** TextLayoutBlock::ScriptRunElt **************************
     ********************************************************************************
     */
    inline bool TextLayoutBlock::ScriptRunElt::operator== (const ScriptRunElt& rhs) const
    {
        return (fDirection == rhs.fDirection and
                fRealStart == rhs.fRealStart and
                fRealEnd == rhs.fRealEnd and
                fVirtualStart == rhs.fVirtualStart and
                fVirtualEnd == rhs.fVirtualEnd);
    }
    inline bool TextLayoutBlock::ScriptRunElt::operator!= (const ScriptRunElt& rhs) const
    {
        return not(*this == rhs);
    }

}

#endif /*_Stroika_Frameworks_Led_BiDiLayoutEngine_inl_*/
