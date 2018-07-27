/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_BiDiLayoutEngine_h_
#define _Stroika_Frameworks_Led_BiDiLayoutEngine_h_ 1

#include "../../Foundation/StroikaPreComp.h"

#include <memory>

#include "../../Foundation/Memory/SmallStackBuffer.h"

/*
@MODULE:    BiDiLayout
@DESCRIPTION:   <p>Code to break up a string to be displayed - which might contain right-to-left text mixed
            with left-to-right text into logical and display text runs according to the UNICODE Bidirectional
            editing algorithm.
            </p>

 */

#include "GDI.h"
#include "Support.h"

namespace Stroika::Frameworks::Led {

/*
    @CONFIGVAR:     qUseFriBidi
    @DESCRIPTION:   <p>Defines whether or not we use the FriBidi package to generate text layouts for bidirectional text
                such as Hebrew or Arabic. This defaults to off, because it requires linking with an extra library.</p>
                    <p>Note that if BOTH @'qUseFriBidi' and @'qTryToUseUNISCRIBEForTextRuns' are defined, then
                @'qUseFriBidi' will take precedence.</p>
        */
#ifndef qUseFriBidi
#define qUseFriBidi 0
#endif

/*
    @CONFIGVAR:     qTryToUseUNISCRIBEForTextRuns
    @DESCRIPTION:   <p>Defines whether or not we try to use the UNISCRIBE Windows SDK to generate text layouts for bidirectional text
                such as Hebrew or Arabic. This defaults to true iff @'qWideCharacters' and @'qPlatform_Windows' is true.
                Note that UNISCRIBE isn't necessarily available on a particular system. This code just tries to use
                UNISCRIBE if its available. So - you MAY want to use this option TOGETHER with @'qUseFriBidi'.</p>
                    <p>Note that if BOTH @'qUseFriBidi' and @'qTryToUseUNISCRIBEForTextRuns' are defined, then
                @'qUseFriBidi' will take precedence.</p>
        */
#ifndef qTryToUseUNISCRIBEForTextRuns
#if qWideCharacters && qPlatform_Windows && qUniscribeAvailableWithSDK
#define qTryToUseUNISCRIBEForTextRuns 1
#else
#define qTryToUseUNISCRIBEForTextRuns 0
#endif
#endif

/*
    @CONFIGVAR:     qUseICUBidi
    @DESCRIPTION:   <p>Defines whether or not we use the IBM ICU package to generate text layouts for bidirectional text
                such as Hebrew or Arabic. THIS IS NOT YET IMPLEMENTED (as of 3.1a3) and may never be - as the ICU package is huge,
                and only setup easily to work with a DLL copy of itself.</p>
        */
#ifndef qUseICUBidi
#define qUseICUBidi 0
#endif

    /*
    @CLASS:         TextLayoutBlock
    @DESCRIPTION:   <p>xxx -- XPLAIN - Add ref to URL docs for this API. Note always pass in PARAGRAPH as unit for layout (cuz in spec)
                </p>
    */
    class TextLayoutBlock {
    protected:
        TextLayoutBlock ();
        virtual ~TextLayoutBlock ();

    public:
        nonvirtual const Led_tChar* PeekAtRealText () const;
        nonvirtual const Led_tChar* PeekAtVirtualText () const;
        nonvirtual size_t GetTextLength () const;

    public:
        virtual void PeekAtRealText_ (const Led_tChar** startText, const Led_tChar** endText) const    = 0;
        virtual void PeekAtVirtualText_ (const Led_tChar** startText, const Led_tChar** endText) const = 0;

    public:
        nonvirtual TextDirection GetCharacterDirection (size_t realCharOffset) const;

    public:
        nonvirtual size_t MapRealOffsetToVirtual (size_t i) const;
        nonvirtual size_t MapVirtualOffsetToReal (size_t i) const;

    public:
        struct ScriptRunElt {
            // NB:      Since length of a run is always the same for virtual and real - we COULD make this struct smaller!
            TextDirection fDirection;
            size_t        fRealStart;
            size_t        fRealEnd;
            size_t        fVirtualStart;
            size_t        fVirtualEnd;

            nonvirtual bool operator== (const ScriptRunElt& rhs) const;
            nonvirtual bool operator!= (const ScriptRunElt& rhs) const;
        };
        struct LessThanVirtualStart;

    public:
        virtual vector<ScriptRunElt> GetScriptRuns () const = 0;

    public:
        nonvirtual void CopyOutRealText (const ScriptRunElt& scriptRunElt, Led_tChar* buf) const;
        nonvirtual void CopyOutVirtualText (const ScriptRunElt& scriptRunElt, Led_tChar* buf) const;

    public:
        nonvirtual void PeekAtRealText (const ScriptRunElt& scriptRunElt, const Led_tChar** startText, const Led_tChar** endText) const;
        nonvirtual Led_tString GetRealText () const;
        nonvirtual Led_tString GetRealText (const ScriptRunElt& scriptRunElt) const;
        nonvirtual void        PeekAtVirtualText (const ScriptRunElt& scriptRunElt, const Led_tChar** startText, const Led_tChar** endText) const;
        nonvirtual Led_tString GetVirtualText () const;
        nonvirtual Led_tString GetVirtualText (const ScriptRunElt& scriptRunElt) const;

        // Debug support
    public:
        nonvirtual void Invariant () const;
#if qDebug
    protected:
        virtual void Invariant_ () const;
#endif
    };

    bool operator== (const TextLayoutBlock& lhs, const TextLayoutBlock& rhs);

    /*
    @CLASS:         TextLayoutBlock::LessThanVirtualStart
    @BASES:
    @DESCRIPTION:
        <p>Use this class when you have a vector of ScriptRunElt and want to sort it by virtual start.</p>
        <p>As in:
        </p>
        <code><pre>
        vector&lt;TextLayoutBlock::ScriptRunElt&gt; runs = get_em ();
        sort (runs.begin (), runs.end (), TextLayoutBlock::LessThanVirtualStart ());
        </pre></code>
    */
    struct TextLayoutBlock::LessThanVirtualStart {
        bool operator() (const ScriptRunElt& lhs, const ScriptRunElt& rhs)
        {
            int diff = int(lhs.fVirtualStart) - int(rhs.fVirtualStart);
            return (diff < 0);
        }
    };

    /*
    @CLASS:         TextLayoutBlock_Basic
    @DESCRIPTION:   <p>NB: the CTOR copies the argument string by value, and doesn't retain the pointer itself.
                </p>
    */
    class TextLayoutBlock_Basic : public TextLayoutBlock {
    private:
        using inherited = TextLayoutBlock;

    public:
        TextLayoutBlock_Basic (const Led_tChar* realText, const Led_tChar* realTextEnd);
        TextLayoutBlock_Basic (const Led_tChar* realText, const Led_tChar* realTextEnd, TextDirection initialDirection);

    private:
        nonvirtual void Construct (const Led_tChar* realText, const Led_tChar* realTextEnd, const TextDirection* initialDirection);

    private:
#if qTryToUseUNISCRIBEForTextRuns
        nonvirtual bool Construct_UNISCRIBE (const TextDirection* initialDirection);
#endif
#if qUseFriBidi
        nonvirtual void Construct_FriBidi (const TextDirection* initialDirection);
#endif
#if qUseICUBidi
        nonvirtual void Construct_ICU (const TextDirection* initialDirection);
#endif
        nonvirtual void Construct_Default ();

    public:
        virtual void PeekAtRealText_ (const Led_tChar** startText, const Led_tChar** endText) const override;
        virtual void PeekAtVirtualText_ (const Led_tChar** startText, const Led_tChar** endText) const override;

    public:
        virtual vector<ScriptRunElt> GetScriptRuns () const override;

    private:
        size_t                                          fTextLength;
        Foundation::Memory::SmallStackBuffer<Led_tChar> fRealText;
        Foundation::Memory::SmallStackBuffer<Led_tChar> fVirtualText;
        vector<ScriptRunElt>                            fScriptRuns;
    };

    /*
    @CLASS:         TextLayoutBlock_Copy
    @DESCRIPTION:   <p>This class is designed to allow for very cheap and quick copying of TextLayoutBlocks. This class
                and be stack/assign copied (not just by pointer). It is an indexpensive way to cache a TextLayoutBlock, or
                to return one from a function.
                </p>
    */
    class TextLayoutBlock_Copy : public TextLayoutBlock {
    private:
        using inherited = TextLayoutBlock;

    public:
        TextLayoutBlock_Copy (const TextLayoutBlock& from);
        TextLayoutBlock_Copy (const TextLayoutBlock_Copy& from);

    public:
        virtual void PeekAtRealText_ (const Led_tChar** startText, const Led_tChar** endText) const override;
        virtual void PeekAtVirtualText_ (const Led_tChar** startText, const Led_tChar** endText) const override;

    public:
        virtual vector<ScriptRunElt> GetScriptRuns () const override;

    private:
        struct BlockRep {
            nonvirtual void operator delete (void* p);

            size_t              fTextLength;
            const Led_tChar*    fRealText;
            const Led_tChar*    fVirtualText;
            const ScriptRunElt* fScriptRuns;
            const ScriptRunElt* fScriptRunsEnd;
            // and we allocate extra space off the end of this object for the acutal data...
        };
        shared_ptr<BlockRep> fRep;
    };

    /*
    @CLASS:         TextLayoutBlock_VirtualSubset
    @DESCRIPTION:   <p>NB: Saves a reference to its CTOR argument- 'subsetOf' so this must be destroyed AFTER
                the original.</p>
                    <p>It is not obvious if this code should may a COPY of the 'real text' or refer back to the same original text with its original
                offsets. However - the problem with doing so is that since we've picked a contigous range of the virtual space, that does
                not coorespond to a contiguous space in the real space necessarily - except for the entire original buffer. That wouldn't be
                the same size - however - as the virtual buffer. Our API assumes these two spaces are the same size. So - the best compromise
                appears to be to copy the source text into a new buffer with new offsets that range from 0 to max-length and have nothing todo with
                the original buffer offsets.
                </p>
    */
    class TextLayoutBlock_VirtualSubset : public TextLayoutBlock {
    private:
        using inherited = TextLayoutBlock;

    public:
        TextLayoutBlock_VirtualSubset (const TextLayoutBlock& subsetOf, size_t vStart, size_t vEnd);

    public:
        virtual void PeekAtRealText_ (const Led_tChar** startText, const Led_tChar** endText) const override;
        virtual void PeekAtVirtualText_ (const Led_tChar** startText, const Led_tChar** endText) const override;

    public:
        virtual vector<ScriptRunElt> GetScriptRuns () const override;

    private:
        const TextLayoutBlock&                          fSubsetOf;
        size_t                                          fStart;
        size_t                                          fEnd;
        vector<ScriptRunElt>                            fScriptRuns;
        Foundation::Memory::SmallStackBuffer<Led_tChar> fRealText;
    };

    /*
        ********************************************************************************
        ***************************** Implementation Details ***************************
        ********************************************************************************
        */

    //  class   TextLayoutBlock;
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

    // class    TextLayoutBlock::ScriptRunElt
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

#endif /*_Stroika_Frameworks_Led_BiDiLayoutEngine_h_*/
