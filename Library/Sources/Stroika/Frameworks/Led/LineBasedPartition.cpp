/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include "LineBasedPartition.h"

using namespace Stroika::Foundation;

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

/*
 ********************************************************************************
 ****************************** LineBasedPartition ******************************
 ********************************************************************************
 */

LineBasedPartition::LineBasedPartition (TextStore& textStore)
    : inherited (textStore)
{
    FinalConstruct ();
}

LineBasedPartition::LineBasedPartition (TextStore& textStore, SpecialHackToDisableInit /*hack*/)
    : inherited (textStore)
{
}

void LineBasedPartition::FinalConstruct ()
{
    inherited::FinalConstruct ();
    PartitionMarker* pm = GetFirstPartitionMarker ();
    // Perform any splits of the first created PM into lines (in case text buffer starts with text in it)
    for (size_t i = GetTextStore ().GetLength (); i > 0; i--) {
        Led_tChar c;
        GetTextStore ().CopyOut (i - 1, 1, &c);
        if (c == '\n') {
            Split (pm, i);
        }
    }
}

/*
@METHOD:        LineBasedPartition::UpdatePartitions
@DESCRIPTION:   <p>Hook notification that the given partition was updated. See if any newlines were added
    or deleted, and update the partitions as appropriate. Do not call directly.</p>
                <p>Call @'LineBasedPartition::CheckForSplits' for each character to see if any splits are needed,
            and call @'LineBasedPartition::NeedToCoalesce' to see if the given PM needs coalescing because of the update.</p>
*/
void LineBasedPartition::UpdatePartitions (PartitionMarker* pm, const UpdateInfo& updateInfo) noexcept
{
    RequireNotNull (pm);

    if (updateInfo.fTextModified) {
        /*
        *   First check if we had any newlines inserted into our middle. If so, then
        *   we must split ourselves. Then check if we retain our trailing newline (or
        *   are the last partition element) and if not - coalece with the next element.
        *
        *   When checking for newlines inserted - we iterate backwards because when
        *   we do splits - we lose the ability to process the rest of the insertion, and
        *   there could be multiple newlines inserted.
        */
        size_t startOfInsert = max (updateInfo.fReplaceFrom, pm->GetStart ());
        size_t endOfInsert   = min (updateInfo.GetResultingRHS (), pm->GetEnd ());
        for (size_t i = endOfInsert; i > startOfInsert; i--) {
            CheckForSplits (pm, updateInfo, i);
        }

        // See if after insertion of that text this PM needs to be coalesed with the next
        bool coalesce = NeedToCoalesce (pm);
        if (coalesce) {
            Coalece (pm); // 'pm' is DELETED BY THIS SO DO NOTHING to it AFTERWARDS!!!
        }
    }
}

/*
@METHOD:        LineBasedPartition::CheckForSplits
@ACCESS:        protected
@DESCRIPTION:   <p>Called by @'LineBasedPartition::UpdatePartitions' () to check if the given inserted text
            requires any partition elements to be split. 'i' is the position at which we may want
            to do a split (e.g. right after the newline character).</p>
*/
void LineBasedPartition::CheckForSplits (PartitionMarker* pm, const UpdateInfo& updateInfo, size_t i) noexcept
{
    Require (updateInfo.fTextModified); //so there is something in the fTextInserted area
    if (updateInfo.fTextInserted[i - updateInfo.fReplaceFrom - 1] == '\n') {
        Split (pm, i);
    }
}

/*
@METHOD:        LineBasedPartition::NeedToCoalesce
@ACCESS:        protected
@DESCRIPTION:   <p>Called by @'LineBasedPartition::UpdatePartitions' () to check if the given inserted text requires this PM to be coalesed with its
            following one.</p>
*/
bool LineBasedPartition::NeedToCoalesce (PartitionMarker* pm) noexcept
{
    // If after inserting a bunch of characters, and deleting some too, my
    // last character is no longer a newline - better Coalece...
    bool coalesce = false;
    if (pm->GetLength () == 0) {
        coalesce = true;
    }
    else {
        if (pm->GetNext () != nullptr) {
            size_t    end     = pm->GetEnd ();
            Led_tChar endChar = '\0';
            // Look at the character just BEFORE (rather than after) the end
            CopyOut (end - 1, 1, &endChar);
            if (endChar != '\n') {
                coalesce = true;
            }
        }
    }
    return coalesce;
}

#if qDebug
/*
@METHOD:        LineBasedPartition::Invariant_
@DESCRIPTION:   <p>Check internal consitency of data structures. Don't call this directly. Call Invariant instead.
    And only call at quiesent times; not in the midst of some update where data structures might not be fully consistent.</p>
*/
void LineBasedPartition::Invariant_ () const
{
    inherited::Invariant_ ();
    for (PartitionMarker* cur = GetFirstPartitionMarker (); cur != nullptr; cur = cur->GetNext ()) {
        AssertNotNull (cur);
        size_t start = cur->GetStart ();
        size_t end   = cur->GetEnd ();
        size_t len   = end - start;

        if (end > GetEnd ()) {
            len--; // Last partition extends past end of text
        }
        Memory::SmallStackBuffer<Led_tChar> buf (len);
        CopyOut (start, len, buf);
        for (size_t i = 1; i < len; i++) {
            Assert (buf[i - 1] != '\n');
        }
        if (cur->GetNext () != nullptr) { // All but the last partition must be NL terminated...
            Assert (buf[len - 1] == '\n');
        }
    }
}
#endif
