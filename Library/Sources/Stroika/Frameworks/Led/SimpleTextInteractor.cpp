/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include "SimpleTextInteractor.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

/*
 ********************************************************************************
 ***************************** SimpleTextInteractor *****************************
 ********************************************************************************
 */
SimpleTextInteractor::SimpleTextInteractor ()
    : InteractorImagerMixinHelper<SimpleTextImager> ()
{
}

void SimpleTextInteractor::SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont)
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
