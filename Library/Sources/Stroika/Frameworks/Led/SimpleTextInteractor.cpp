/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "SimpleTextInteractor.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

#if qStroika_Frameworks_Led_SupportGDI

/*
 ********************************************************************************
 ***************************** SimpleTextInteractor *****************************
 ********************************************************************************
 */
SimpleTextInteractor::SimpleTextInteractor ()
    : InteractorImagerMixinHelper<SimpleTextImager> ()
{
}

void SimpleTextInteractor::SetDefaultFont (const IncrementalFontSpecification& defaultFont)
{
    SimpleTextImager::SetDefaultFont (defaultFont);
}

void SimpleTextInteractor::SetTopRowInWindow (size_t newTopRow)
{
    PreScrollInfo preScrollInfo;
    PreScrollHelper (eDefaultUpdate, &preScrollInfo);
    SimpleTextImager::SetTopRowInWindow (newTopRow);
    PostScrollHelper (preScrollInfo);
}

void SimpleTextInteractor::SetTopRowInWindow (RowReference row)
{
    PreScrollInfo preScrollInfo;
    PreScrollHelper (eDefaultUpdate, &preScrollInfo);
    SimpleTextImager::SetTopRowInWindow (row);
    PostScrollHelper (preScrollInfo);
}

void SimpleTextInteractor::SetTopRowInWindowByMarkerPosition (size_t markerPos, UpdateMode updateMode)
{
    SetTopRowInWindow (GetRowReferenceContainingPosition (markerPos), updateMode);
}

void SimpleTextInteractor::TabletChangedMetrics ()
{
    SimpleTextImager::TabletChangedMetrics ();
    Refresh ();
}

void SimpleTextInteractor::ChangedInterLineSpace (PartitionMarker* pm)
{
    SimpleTextImager::ChangedInterLineSpace (pm);
    Refresh ();
}
#endif