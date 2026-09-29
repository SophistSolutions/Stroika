/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <algorithm>

#include "Stroika/Foundation/Characters/CodePage.h"
#include "Stroika/Foundation/Containers/Support/ReserveTweaks.h"

#include "BiDiLayoutEngine.h"

using std::byte;

using namespace Stroika::Foundation;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

/*
 *  This hack reverses the direction results returned by this class for basic text parsing/layout. This is
 *  useful to debug/test - cuz then I can see ENGLISH text (words) displayed RTL (right to left) - which I can
 *  better identify and see if they are working properly.
 *
 *  Note that this ONLY reverses directions WITHIN a run - not the order of the runs. Thats still good enough
 *  to debug most display issues.
 */
#ifndef qDebugHack_ReverseDirections
#define qDebugHack_ReverseDirections 0
#endif

/*
 *  This hack treats all english aplhabetic upper case characters as RTL characters, and lower case characters
 *  as LTR characters. This hack COULD be improved to actually PRESERVE the original characters in the virtual
 *  output, but that hasn't been done yet.
 */
#ifndef qDebugHack_UpperCaseCharsTratedAsRTL
#define qDebugHack_UpperCaseCharsTratedAsRTL 0
#endif

/*
 ********************************************************************************
 ******************************** TextLayoutBlock *******************************
 ********************************************************************************
 */
/*
@METHOD:        TextLayoutBlock::GetCharacterDirection
@DESCRIPTION:
*/
TextDirection TextLayoutBlock::GetCharacterDirection (size_t realCharOffset) const
{
    vector<ScriptRunElt> runs = GetScriptRuns ();
    for (auto i = runs.begin (); i != runs.end (); ++i) {
        if ((*i).fRealStart <= realCharOffset and realCharOffset < (*i).fRealEnd) {
            return (*i).fDirection;
        }
    }
    Assert (false);
    return eLeftToRight;
}

/*
@METHOD:        TextLayoutBlock::MapRealOffsetToVirtual
@DESCRIPTION:
*/
size_t TextLayoutBlock::MapRealOffsetToVirtual (size_t /*i*/) const
{
    Assert (false); //NYI
    return 0;
}

/*
@METHOD:        TextLayoutBlock::MapVirtualOffsetToReal
@DESCRIPTION:
*/
size_t TextLayoutBlock::MapVirtualOffsetToReal (size_t i) const
{
    Require (i < GetTextLength ());
    vector<ScriptRunElt> runs = GetScriptRuns ();
    for (auto runIt = runs.begin (); runIt != runs.end (); ++runIt) {
        const ScriptRunElt& se = *runIt;
        if (se.fVirtualStart <= i and i < se.fVirtualEnd) {
            if (se.fDirection == eLeftToRight) {
                size_t result = (i - se.fVirtualStart) + se.fRealStart;
                Ensure (result < GetTextLength ());
                return (result);
            }
            else {
                size_t segLen = se.fRealEnd - se.fRealStart;
                size_t result = (segLen - (i - se.fVirtualStart)) - 1 + se.fRealStart;
                Ensure (result < GetTextLength ());
                return (result);
            }
        }
    }
    Assert (false); // BAD INDEX
    return 0;
}

void TextLayoutBlock::CopyOutRealText (const ScriptRunElt& scriptRunElt, Led_tChar* buf) const
{
    const Led_tChar* s = nullptr;
    const Led_tChar* e = nullptr;
    PeekAtRealText (scriptRunElt, &s, &e);
    copy (s, e, buf);
}

void TextLayoutBlock::CopyOutVirtualText (const ScriptRunElt& scriptRunElt, Led_tChar* buf) const
{
    const Led_tChar* s = nullptr;
    const Led_tChar* e = nullptr;
    PeekAtVirtualText (scriptRunElt, &s, &e);
    copy (s, e, buf);
}

void TextLayoutBlock::PeekAtRealText (const ScriptRunElt& scriptRunElt, const Led_tChar** startText, const Led_tChar** endText) const
{
    RequireNotNull (startText);
    RequireNotNull (endText);
    PeekAtRealText_ (startText, endText);
    *startText += scriptRunElt.fRealStart;
    size_t len = scriptRunElt.fRealEnd - scriptRunElt.fRealStart;
    Assert (*endText - *startText >= static_cast<ptrdiff_t> (len)); // make sure we are SHRINKING the text and not making it point past its end
    *endText = *startText + len;
}

Led_tString TextLayoutBlock::GetRealText () const
{
    const Led_tChar* s = nullptr;
    const Led_tChar* e = nullptr;
    PeekAtRealText_ (&s, &e);
    return Led_tString{s, e};
}

Led_tString TextLayoutBlock::GetRealText (const ScriptRunElt& scriptRunElt) const
{
    const Led_tChar* s = nullptr;
    const Led_tChar* e = nullptr;
    PeekAtRealText (scriptRunElt, &s, &e);
    return Led_tString{s, e};
}

void TextLayoutBlock::PeekAtVirtualText (const ScriptRunElt& scriptRunElt, const Led_tChar** startText, const Led_tChar** endText) const
{
    RequireNotNull (startText);
    RequireNotNull (endText);
    PeekAtVirtualText_ (startText, endText);
    *startText += scriptRunElt.fVirtualStart;
    size_t len = scriptRunElt.fVirtualEnd - scriptRunElt.fVirtualStart;
    Assert (*endText - *startText >= static_cast<ptrdiff_t> (len)); // make sure we are SHRINKING the text and not making it point past its end
    *endText = *startText + len;
}

Led_tString TextLayoutBlock::GetVirtualText () const
{
    const Led_tChar* s = nullptr;
    const Led_tChar* e = nullptr;
    PeekAtVirtualText_ (&s, &e);
    return Led_tString{s, e};
}

Led_tString TextLayoutBlock::GetVirtualText (const ScriptRunElt& scriptRunElt) const
{
    const Led_tChar* s = nullptr;
    const Led_tChar* e = nullptr;
    PeekAtVirtualText (scriptRunElt, &s, &e);
    return Led_tString{s, e};
}

#if qStroika_Foundation_Debug_AssertionsChecked
void TextLayoutBlock::Invariant_ () const
{
    size_t len = GetTextLength ();

    vector<ScriptRunElt> scriptRuns = GetScriptRuns ();

    // Make sure each scriptrun elt is non-empty
    {
        for (auto j = scriptRuns.begin (); j != scriptRuns.end (); ++j) {
            Assert ((*j).fRealStart < len);
            Assert ((*j).fRealEnd <= len);
            Assert ((*j).fVirtualStart < len);
            Assert ((*j).fVirtualEnd <= len);
            Assert ((*j).fRealStart < (*j).fRealEnd); // no empty elements
        }
    }

    /*
     *  Validate the fScriptRuns list. Make sure it completely covers the real and virtual range. Make
     *  sure each element has exactly LENGTH spaces used up for the real text and the virtual text.
     *  That - by itself - assures also that there are no overlapping elements.
     */
    {
        for (size_t i = 0; i < len; ++i) {
            // Assure pos 'i' found in source and virtual
            bool   foundReal     = false;
            bool   foundVirtual  = false;
            size_t nRealFound    = 0;
            size_t nVirtualFound = 0;
            for (auto j = scriptRuns.begin (); j != scriptRuns.end (); ++j) {
                if ((*j).fRealStart <= i and i < (*j).fRealEnd) {
                    Assert (not foundReal);
                    foundReal = true;
                }
                Assert ((*j).fVirtualStart < (*j).fVirtualEnd); // no empty elements
                if ((*j).fVirtualStart <= i and i < (*j).fVirtualEnd) {
                    Assert (not foundVirtual);
                    foundVirtual = true;
                }
                nRealFound += (*j).fRealEnd - (*j).fRealStart;
                nVirtualFound += (*j).fVirtualEnd - (*j).fVirtualStart;
            }
            Assert (foundReal);
            Assert (foundVirtual);
            Assert (nRealFound == len);
            Assert (nVirtualFound == len);
        }
    }
}
#endif

bool TextLayoutBlock::operator== (const TextLayoutBlock& rhs) const
{
    if (this->GetTextLength () != rhs.GetTextLength ()) {
        return false;
    }
    if (this->GetRealText () != rhs.GetRealText ()) {
        return false;
    }
    if (this->GetVirtualText () != rhs.GetVirtualText ()) {
        return false;
    }
    vector<TextLayoutBlock::ScriptRunElt> lhsSR = this->GetScriptRuns ();
    vector<TextLayoutBlock::ScriptRunElt> rhsSR = rhs.GetScriptRuns ();
    if (lhsSR.size () != rhsSR.size ()) {
        return false;
    }
    for (size_t i = 0; i < lhsSR.size (); ++i) {
        if (lhsSR[i] != rhsSR[i]) {
            return false;
        }
    }
    return true;
}

/*
 ********************************************************************************
 ****************************** TextLayoutBlock_Basic ***************************
 ********************************************************************************
 */
TextLayoutBlock_Basic::TextLayoutBlock_Basic (const Led_tChar* realText, const Led_tChar* realTextEnd)
    : fTextLength{0}
    , fRealText{0}
    , fVirtualText{0}
    , fScriptRuns{}
{
    Construct (realText, realTextEnd, nullptr);
}

TextLayoutBlock_Basic::TextLayoutBlock_Basic (const Led_tChar* realText, const Led_tChar* realTextEnd, TextDirection initialDirection)
    : fTextLength{0}
    , fRealText{0}
    , fVirtualText{0}
    , fScriptRuns{}
{
    Construct (realText, realTextEnd, &initialDirection);
}

void TextLayoutBlock_Basic::Construct (const Led_tChar* realText, const Led_tChar* realTextEnd, [[maybe_unused]] const TextDirection* initialDirection)
{
    size_t textLength = realTextEnd - realText;
    fTextLength       = textLength;
    fRealText.GrowToSize (textLength);
    fVirtualText.GrowToSize (textLength);
    copy (realText, realText + textLength, static_cast<Led_tChar*> (fRealText));

#if qDebugHack_UpperCaseCharsTratedAsRTL
    for (size_t i = 0; i < textLength; ++i) {
        if ('A' <= fRealText[i] and fRealText[i] <= 'Z') {
            fRealText[i] = 0xfe7d; // random arabic character
        }
    }
#endif

    // No bidirectional layout engine: each paragraph is one left-to-right run - see https://github.com/SophistSolutions/Stroika/issues/1182
    Construct_Default ();

    if constexpr (qDebugHack_ReverseDirections) {
        for (auto i = fScriptRuns.begin (); i != fScriptRuns.end (); ++i) {
            ScriptRunElt& se     = *i;
            TextDirection newDir = (se.fDirection == eLeftToRight) ? eRightToLeft : eLeftToRight;
            se.fDirection        = newDir;
            // now reverse the virtual text in the run...
            size_t                         runLen = se.fVirtualEnd - se.fVirtualStart;
            Memory::StackBuffer<Led_tChar> reverseBuf{runLen};
            for (size_t j = se.fVirtualStart; j < se.fVirtualEnd; ++j) {
                reverseBuf[runLen - 1 - (j - se.fVirtualStart)] = fVirtualText[j];
            }
            copy (static_cast<Led_tChar*> (reverseBuf), static_cast<Led_tChar*> (reverseBuf) + runLen,
                  static_cast<Led_tChar*> (fVirtualText) + se.fVirtualStart);
        }
    }

    Invariant ();
}

void TextLayoutBlock_Basic::Construct_Default ()
{
    // Only do something for UNICODE/wide-chars - otherwise - just copy the chars by value...
    copy (static_cast<Led_tChar*> (fRealText), static_cast<Led_tChar*> (fRealText) + fTextLength, static_cast<Led_tChar*> (fVirtualText));
    if (fTextLength != 0) {
        ScriptRunElt thisCharElt;
        thisCharElt.fDirection    = eLeftToRight;
        thisCharElt.fRealStart    = 0;
        thisCharElt.fRealEnd      = fTextLength;
        thisCharElt.fVirtualStart = 0;
        thisCharElt.fVirtualEnd   = fTextLength;
        fScriptRuns.push_back (thisCharElt);
    }
}

void TextLayoutBlock_Basic::PeekAtRealText_ (const Led_tChar** startText, const Led_tChar** endText) const
{
    RequireNotNull (startText);
    RequireNotNull (endText);
    *startText = fRealText.data ();
    *endText   = fRealText.data () + fTextLength;
}

void TextLayoutBlock_Basic::PeekAtVirtualText_ (const Led_tChar** startText, const Led_tChar** endText) const
{
    RequireNotNull (startText);
    RequireNotNull (endText);
    *startText = fVirtualText.data ();
    *endText   = fVirtualText.data () + fTextLength;
}

vector<TextLayoutBlock::ScriptRunElt> TextLayoutBlock_Basic::GetScriptRuns () const
{
    return fScriptRuns;
}

/*
 ********************************************************************************
 ***************************** TextLayoutBlock_Copy *****************************
 ********************************************************************************
 */
TextLayoutBlock_Copy::TextLayoutBlock_Copy (const TextLayoutBlock& from)
{
    /*
     *  Compute size - and then copy all the data into a continguous block of RAM.
     */
    size_t               strLength  = from.GetTextLength ();
    vector<ScriptRunElt> scriptRuns = from.GetScriptRuns ();

    // compute needed size
    size_t neededSize = sizeof (BlockRep);
    neededSize += sizeof (Led_tChar) * strLength;             // fRealText
    neededSize += sizeof (Led_tChar) * strLength;             // fVirtualText
    neededSize += sizeof (ScriptRunElt) * scriptRuns.size (); // fScriptRuns
    auto deleter = [] (BlockRep* br) { delete[] (reinterpret_cast<byte*> (br)); };
    fRep         = shared_ptr<BlockRep>{reinterpret_cast<BlockRep*> (new byte[neededSize]), deleter};

    fRep->fTextLength = strLength;

    fRep->fRealText = reinterpret_cast<Led_tChar*> (fRep.get () + 1);
    {
        const Led_tChar* s = nullptr;
        const Led_tChar* e = nullptr;
        from.PeekAtRealText_ (&s, &e);
        copy (s, e, const_cast<Led_tChar*> (fRep->fRealText));
    }

    fRep->fVirtualText = fRep->fRealText + strLength;
    {
        const Led_tChar* s = nullptr;
        const Led_tChar* e = nullptr;
        from.PeekAtVirtualText_ (&s, &e);
        copy (s, e, const_cast<Led_tChar*> (fRep->fVirtualText));
    }

    fRep->fScriptRuns    = reinterpret_cast<const ScriptRunElt*> (fRep->fVirtualText + strLength);
    fRep->fScriptRunsEnd = fRep->fScriptRuns + scriptRuns.size ();
    copy (scriptRuns.begin (), scriptRuns.end (), const_cast<ScriptRunElt*> (fRep->fScriptRuns));
}

TextLayoutBlock_Copy::TextLayoutBlock_Copy (const TextLayoutBlock_Copy& from)
    : inherited (from)
    , fRep (from.fRep)
{
}

void TextLayoutBlock_Copy::PeekAtRealText_ (const Led_tChar** startText, const Led_tChar** endText) const
{
    RequireNotNull (startText);
    RequireNotNull (endText);
    *startText = fRep->fRealText;
    *endText   = fRep->fRealText + fRep->fTextLength;
}

void TextLayoutBlock_Copy::PeekAtVirtualText_ (const Led_tChar** startText, const Led_tChar** endText) const
{
    RequireNotNull (startText);
    RequireNotNull (endText);
    *startText = fRep->fVirtualText;
    *endText   = fRep->fVirtualText + fRep->fTextLength;
}

vector<TextLayoutBlock_Copy::ScriptRunElt> TextLayoutBlock_Copy::GetScriptRuns () const
{
    return vector<ScriptRunElt> (fRep->fScriptRuns, fRep->fScriptRunsEnd);
}

/*
 ********************************************************************************
 ************************ TextLayoutBlock_Copy::BlockRep ************************
 ********************************************************************************
 */
void TextLayoutBlock_Copy::BlockRep::operator delete (void* p)
{
    // Because we allocate using new byte[] in TextLayoutBlock_Copy::CTOR (see SPR#1596)
    delete[] (reinterpret_cast<char*> (p));
}

/*
 ********************************************************************************
 *********************** TextLayoutBlock_VirtualSubset **************************
 ********************************************************************************
 */
TextLayoutBlock_VirtualSubset::TextLayoutBlock_VirtualSubset (const TextLayoutBlock& subsetOf, size_t start, size_t end)
    : fSubsetOf{subsetOf}
    , fStart{start}
    , fEnd{end}
    , fRealText{end - start}
{
    vector<ScriptRunElt> origRuns     = fSubsetOf.GetScriptRuns ();
    size_t               offsetSoFar  = 0;
    const Led_tChar*     fullRealText = fSubsetOf.PeekAtRealText ();
    for (auto i = origRuns.begin (); i != origRuns.end (); ++i) {
        size_t offsetStart = (*i).fVirtualStart;
        size_t offsetEnd   = (*i).fVirtualEnd;

        size_t runRelStart = max (offsetStart, static_cast<size_t> (fStart));
        size_t runRelEnd   = min (offsetEnd, static_cast<size_t> (fEnd));
        if (runRelStart < runRelEnd) {
            // then this is a run with some stuff in it
            ScriptRunElt s = *i;
            Assert (runRelStart >= fStart);
            Assert (runRelEnd >= fStart);
            s.fVirtualStart = runRelStart - fStart;
            s.fVirtualEnd   = runRelEnd - fStart;

            /*
             * Since the API of TextLayoutBlock assumes a contiguous piece of RAM for the REAL and Virtual text, and since
             * the source text is no longer continguous (if we used the full buffer then its length wouldn't match the length
             * of our virtual buffer), we must make a copy of the real text and use different adjusted offsets. This 'real' text then
             * isn't so real after all. It might not even be in an order that would have produced the given virtual text. BUT - if one
             * makes calls to get back the original real text for any given range of Virual text - you WILL at least get back the
             * right text.
             */
            Assert (runRelEnd >= runRelStart);
            size_t runEltLen = runRelEnd - runRelStart;
            copy (fullRealText + s.fRealStart, fullRealText + s.fRealStart + runEltLen, fRealText.data () + offsetSoFar);
            s.fRealStart = offsetSoFar;
            offsetSoFar += runEltLen;
            s.fRealEnd = offsetSoFar;
            Containers::Support::ReserveTweaks::Reserve4Add1 (fScriptRuns);
            fScriptRuns.push_back (s);
        }
    }

    Invariant ();
}

void TextLayoutBlock_VirtualSubset::PeekAtRealText_ (const Led_tChar** startText, const Led_tChar** endText) const
{
    RequireNotNull (startText);
    RequireNotNull (endText);
    *startText = fRealText.data ();
    *endText   = *startText + (fEnd - fStart);
}

void TextLayoutBlock_VirtualSubset::PeekAtVirtualText_ (const Led_tChar** startText, const Led_tChar** endText) const
{
    RequireNotNull (startText);
    RequireNotNull (endText);
    fSubsetOf.PeekAtVirtualText_ (startText, endText);
    *startText += fStart;
    Assert (fStart <= fEnd);
    Assert (*endText - *startText >= static_cast<int> (fEnd - fStart)); // make sure orig string >= shortened string
    *endText = *startText + (fEnd - fStart);
}

vector<TextLayoutBlock::ScriptRunElt> TextLayoutBlock_VirtualSubset::GetScriptRuns () const
{
    return fScriptRuns;
}
