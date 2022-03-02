/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Shape.cc,v 1.7 1992/09/11 18:41:55 sterling Exp $
 *
 * TODO:
 *
 *			-	Desperately need the ability to scale shapes - probably shouyld add general
 *				transform capabilty - at least scale, etc..
 *
 * Changes:
 *	$Log: Shape.cc,v $
 *		Revision 1.7  1992/09/11  18:41:55  sterling
 *		new Shape stuff, got rid of String Peek references
 *
 *		Revision 1.6  1992/09/05  16:14:25  lewis
 *		Renamed Nil->Nil.
 *
 *		Revision 1.5  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/08  02:11:42  lewis
 *		Renamed PointInside->Contains ().
 *
 *		Revision 1.3  1992/07/04  14:50:03  lewis
 *		Added BlockAllocation/operator new/delete overrides to the shape reps for
 *		better performance. Also, lots of cleanups including removing ifdefed out
 *		code.
 *
 *		Revision 1.2  1992/07/04  02:37:40  lewis
 *		See header for big picture - but mainly here we renamed all shape classes X,
 *		to XRepresention, and commented out methods that were setters - just recreate
 *		the object - with out new paradigm its a little harder to do sets - may support
 *		them again, if sterl thinks its worth it...
 *
 *		Revision 1.1  1992/06/19  22:34:01  lewis
 *		Initial revision
 *
 *		Revision 1.12  1992/05/23  00:10:07  lewis
 *		#include BlockAllocated instead of Memory.hh
 *
 *		Revision 1.11  92/04/15  14:31:18  14:31:18  lewis (Lewis Pringle)
 *		Adjust asserts to be call AsDegrees () when comparing with hardwired degress values.
 *		
 *		Revision 1.9  1992/02/21  18:06:44  lewis
 *		Got rid of qGPlus_ClassOpNewDelBroke workaround.
 *
 *
 *
 *
 */

#include	"OSRenamePre.hh"
#if		qMacGDI
#include	<Memory.h>
#include	<OSUtils.h>
#include	<QuickDraw.h>
#endif	/*qMacGDI*/
#include	"OSRenamePost.hh"


#include	"BlockAllocated.hh"
#include	"Debug.hh"
#include	"PixelMap.hh"
#include	"Tablet.hh"

#include	"Shape.hh"


#if		!qRealTemplatesAvailable
	Implement (Iterator, Point);
	Implement (Collection, Point);
	Implement (AbSequence, Point);
	Implement (Array, Point);
	Implement (Sequence_Array, Point);
	Implement (Sequence, Point);
#endif	/*!qRealTemplatesAvailable*/


#if		qMacGDI
struct	WorkTablet : Tablet {
	WorkTablet (osGrafPort* osg):
		Tablet (osg)
		{
			AssertNotNil (osg);
		}
};
static	struct	ModuleInit {
	ModuleInit ()
		{
			osGrafPort*	port	=	new (osGrafPort);
			::OpenPort (port);
			fTablet = new WorkTablet (port);
		}
	Tablet*		fTablet;
}	kModuleGlobals;
#endif	/*qMacGDI*/



#if		!qRealTemplatesAvailable
Implement (Shared, ShapeRepresentation);
#endif







/*
 ********************************************************************************
 ***************************** ShapeRepresentation ******************************
 ********************************************************************************
 */
Boolean	ShapeRepresentation::Contains (const Point& p, const Rect& shapeBounds) const
{
	if (shapeBounds.Contains (p)) {
		return (ToRegion (shapeBounds).Contains (p));
	}
	else {
		return (False);
	}
}

Region	ShapeRepresentation::ToRegion (const Rect& shapeBounds) const
{
#if		qMacGDI
	static	osRegion**	tmp			=	::NewRgn ();
	static	const	Pen	kRegionPen	=	Pen (kBlackTile, eCopyTMode, Point (1, 1));

	AssertNotNil (kModuleGlobals.fTablet->GetOSGrafPtr ());
	osGrafPort*	oldPort = qd.thePort;
	Tablet::xDoSetPort (kModuleGlobals.fTablet->GetOSGrafPtr ());
	::OpenRgn ();
	OutLine (*kModuleGlobals.fTablet, shapeBounds, kRegionPen);
	::CloseRgn (tmp);
	Tablet::xDoSetPort (oldPort);
	
	return (Region (tmp));
#elif	qXGDI
	// for now, hack and just return bounding rectable - we must fix this soon!!!
	return (shapeBounds);
#endif	/*qMacGDI || qXGDI*/
}

Point	ShapeRepresentation::GetLogicalSize ()	const
{
	return (kZeroPoint);
}







/*
 ********************************************************************************
 *************************** RectangleRepresentation ****************************
 ********************************************************************************
 */

RectangleRepresentation::RectangleRepresentation ()
{
}

Boolean	RectangleRepresentation::Contains (const Point& p, const Rect& shapeBounds) const
{
	return (shapeBounds.Contains (p));
}

void	RectangleRepresentation::Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const
{
	on.PaintRect (shapeBounds, brush);
}

void	RectangleRepresentation::OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const
{
	on.OutLineRect (shapeBounds, pen);
}

Region	RectangleRepresentation::ToRegion (const Rect& shapeBounds) const
{
	return (Region (shapeBounds));
}

#if		!qRealTemplatesAvailable
BlockAllocatedDeclare (RectangleRepresentation);
BlockAllocatedImplement (RectangleRepresentation);
#endif	/*!qRealTemplatesAvailable*/

void*	RectangleRepresentation::operator new (size_t n)
{
#if		qRealTemplatesAvailable
	return (BlockAllocated<RectangleRepresentation>::operator new (n));
#else	/*qRealTemplatesAvailable*/
	return (BlockAllocated(RectangleRepresentation)::operator new (n));
#endif	/*qRealTemplatesAvailable*/
}

void	RectangleRepresentation::operator delete (void* p)
{
#if		qRealTemplatesAvailable
	BlockAllocated<RectangleRepresentation>::operator delete (p);
#else	/*qRealTemplatesAvailable*/
	BlockAllocated(RectangleRepresentation)::operator delete (p);
#endif	/*qRealTemplatesAvailable*/
}







/*
 ********************************************************************************
 ****************************** RoundedRectangle ********************************
 ********************************************************************************
 */
Point	RoundedRectangle::kDefaultRounding = Point (10, 10);





/*
 ********************************************************************************
 *********************** RoundedRectangleRepresentation *************************
 ********************************************************************************
 */

RoundedRectangleRepresentation::RoundedRectangleRepresentation (const Point& rounding):
	fRounding (rounding)
{
	Require (rounding.GetH () == (short)rounding.GetH ());
	Require (rounding.GetV () == (short)rounding.GetV ());
}

void	RoundedRectangleRepresentation::Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const
{
	on.PaintRoundedRect (shapeBounds, fRounding, brush);
}

void	RoundedRectangleRepresentation::OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const
{
	on.OutLineRoundedRect (shapeBounds, fRounding, pen);
}

Point	RoundedRectangleRepresentation::GetRounding () const
{
	return (fRounding);
}

#if		!qRealTemplatesAvailable
BlockAllocatedDeclare (RoundedRectangleRepresentation);
BlockAllocatedImplement (RoundedRectangleRepresentation);
#endif	/*!qRealTemplatesAvailable*/

void*	RoundedRectangleRepresentation::operator new (size_t n)
{
#if		qRealTemplatesAvailable
	return (BlockAllocated<RoundedRectangleRepresentation>::operator new (n));
#else	/*qRealTemplatesAvailable*/
	return (BlockAllocated(RoundedRectangleRepresentation)::operator new (n));
#endif	/*qRealTemplatesAvailable*/
}

void	RoundedRectangleRepresentation::operator delete (void* p)
{
#if		qRealTemplatesAvailable
	BlockAllocated<RoundedRectangleRepresentation>::operator delete (p);
#else	/*qRealTemplatesAvailable*/
	BlockAllocated(RoundedRectangleRepresentation)::operator delete (p);
#endif	/*qRealTemplatesAvailable*/
}







/*
 ********************************************************************************
 ******************************** OvalRepresentation ****************************
 ********************************************************************************
 */
OvalRepresentation::OvalRepresentation ()
{
}

void	OvalRepresentation::Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const
{
	on.PaintOval (shapeBounds, brush);
}

void	OvalRepresentation::OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const
{
	on.OutLineOval (shapeBounds, pen);
}

#if		!qRealTemplatesAvailable
BlockAllocatedDeclare (OvalRepresentation);
BlockAllocatedImplement (OvalRepresentation);
#endif	/*!qRealTemplatesAvailable*/

void*	OvalRepresentation::operator new (size_t n)
{
#if		qRealTemplatesAvailable
	return (BlockAllocated<OvalRepresentation>::operator new (n));
#else	/*qRealTemplatesAvailable*/
	return (BlockAllocated(OvalRepresentation)::operator new (n));
#endif	/*qRealTemplatesAvailable*/
}

void	OvalRepresentation::operator delete (void* p)
{
#if		qRealTemplatesAvailable
	BlockAllocated<OvalRepresentation>::operator delete (p);
#else	/*qRealTemplatesAvailable*/
	BlockAllocated(OvalRepresentation)::operator delete (p);
#endif	/*qRealTemplatesAvailable*/
}






/*
 ********************************************************************************
 ******************************* ArcRepresentation  *****************************
 ********************************************************************************
 */
ArcRepresentation::ArcRepresentation (Angle startAngle, Angle arcAngle):
	fStartAngle (startAngle),
	fArcAngle (arcAngle)
{
	Require (startAngle.AsDegrees () >= 0);
	Require (arcAngle.AsDegrees () <= 360);
}

void	ArcRepresentation::Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const
{
	on.PaintArc (fStartAngle, fArcAngle, shapeBounds, brush);
}

void	ArcRepresentation::OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const
{
	on.OutLineArc (fStartAngle, fArcAngle, shapeBounds, pen);
}

Angle	ArcRepresentation::GetStart ()	const
{
	return (fStartAngle);
}

Angle	ArcRepresentation::GetAngle ()	const
{
	return (fArcAngle);
}

#if		!qRealTemplatesAvailable
BlockAllocatedDeclare (ArcRepresentation);
BlockAllocatedImplement (ArcRepresentation);
#endif	/*!qRealTemplatesAvailable*/

void*	ArcRepresentation::operator new (size_t n)
{
#if		qRealTemplatesAvailable
	return (BlockAllocated<ArcRepresentation>::operator new (n));
#else	/*qRealTemplatesAvailable*/
	return (BlockAllocated(ArcRepresentation)::operator new (n));
#endif	/*qRealTemplatesAvailable*/
}

void	ArcRepresentation::operator delete (void* p)
{
#if		qRealTemplatesAvailable
	BlockAllocated<ArcRepresentation>::operator delete (p);
#else	/*qRealTemplatesAvailable*/
	BlockAllocated(ArcRepresentation)::operator delete (p);
#endif	/*qRealTemplatesAvailable*/
}








/*
 ********************************************************************************
 ************************** RegionShapeRepresentation ***************************
 ********************************************************************************
 */
RegionShapeRepresentation::RegionShapeRepresentation (const Region& rgn):
	fRegion (rgn)
{
}

void	RegionShapeRepresentation::Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const
{
	on.PaintRegion (fRegion, shapeBounds, brush);
}

void	RegionShapeRepresentation::OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const
{
	on.OutLineRegion (fRegion, shapeBounds, pen);
}

Region	RegionShapeRepresentation::ToRegion (const Rect& shapeBounds) const
{
	if ((shapeBounds.Empty ()) or (fRegion == kEmptyRegion)) {
		return (kEmptyRegion);
	}
	if (shapeBounds == fRegion.GetBounds ()) {
		return (fRegion);
	}
	else {
		Region	tempRgn = fRegion;
#if		qMacGDI
		osRect	osr;
		osRect	osBounds;

		os_cvt (fRegion.GetBounds (), osBounds);
		os_cvt (shapeBounds, osr);
		::MapRgn (tempRgn.GetOSRegion (), &osBounds, &osr);
#elif	qXGDI
		return (shapeBounds);		// see Shape::ToRegion ()!!! THIS IS BROKEN!!!
#endif	/*qMacGDI*/
		return (tempRgn);
	}
}

Point	RegionShapeRepresentation::GetLogicalSize ()	const
{
	return (fRegion.GetBounds ().GetSize ());
}

#if		!qRealTemplatesAvailable
BlockAllocatedDeclare (RegionShapeRepresentation);
BlockAllocatedImplement (RegionShapeRepresentation);
#endif	/*!qRealTemplatesAvailable*/

void*	RegionShapeRepresentation::operator new (size_t n)
{
#if		qRealTemplatesAvailable
	return (BlockAllocated<RegionShapeRepresentation>::operator new (n));
#else	/*qRealTemplatesAvailable*/
	return (BlockAllocated(RegionShapeRepresentation)::operator new (n));
#endif	/*qRealTemplatesAvailable*/
}

void	RegionShapeRepresentation::operator delete (void* p)
{
#if		qRealTemplatesAvailable
	BlockAllocated<RegionShapeRepresentation>::operator delete (p);
#else	/*qRealTemplatesAvailable*/
	BlockAllocated(RegionShapeRepresentation)::operator delete (p);
#endif	/*qRealTemplatesAvailable*/
}










/*
 ********************************************************************************
 ******************************** PolygonRepresentation *************************
 ********************************************************************************
 */

PolygonRepresentation::PolygonRepresentation (const AbSequence(Point)& points):
	ShapeRepresentation (),
#if 	qMacGDI
	fOSPolygon (Nil),
#endif	/*qMacGDI*/
	fPoints (points)
{
	RebuildOSPolygon ();
#if		qXGDI
	AssertNotImplemented ();
#endif	/*qXGDI*/
}

PolygonRepresentation::~PolygonRepresentation ()
{
#if		qMacGDI
	if (fOSPolygon != Nil) {
		::KillPoly (fOSPolygon);
	}
#endif	/*qMacGDI*/
}

void	PolygonRepresentation::Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const
{
#if		qMacGDI
	on.PaintPolygon (fOSPolygon, shapeBounds, brush);
#endif	/*qMacGDI*/
}

void	PolygonRepresentation::OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const
{
#if		qMacGDI
	on.OutLinePolygon (fOSPolygon, shapeBounds, pen);
#endif	/*qMacGDI*/
}

Point	PolygonRepresentation::GetLogicalSize ()	const
{
#if		qMacGDI
	return (os_cvt ((*fOSPolygon)->polyBBox).GetSize ());
#endif	/*qMacGDI*/
}

const	AbSequence(Point)&	PolygonRepresentation::GetPoints () const
{
	return (fPoints);
}

void	PolygonRepresentation::RebuildOSPolygon () const
{
#if		qMacGDI
	if (fOSPolygon != Nil) {
		::KillPoly (fOSPolygon);
	}
	Tablet::xDoSetPort (kModuleGlobals.fTablet->GetOSGrafPtr ());
	*((osPolygon***)&fOSPolygon) = ::OpenPoly ();						// avoid const violation error
	if (fPoints.GetLength () >= 2) {
		Iterator(Point)* i = fPoints;
		Point	p = i->Current ();
		::MoveTo ((short)p.GetH (), (short)p.GetV ());
		for (i->Next (); not (i->Done ()); i->Next ()) {
			p = i->Current ();
			Assert ((short)p.GetH () == p.GetH ());
			Assert ((short)p.GetV () == p.GetV ());
			::LineTo ((short)p.GetH (), (short)p.GetV ());
		}
		delete i;
	}
	::ClosePoly ();
#endif	/*qMacGDI*/
}

#if		!qRealTemplatesAvailable
BlockAllocatedDeclare (PolygonRepresentation);
BlockAllocatedImplement (PolygonRepresentation);
#endif	/*!qRealTemplatesAvailable*/

void*	PolygonRepresentation::operator new (size_t n)
{
#if		qRealTemplatesAvailable
	return (BlockAllocated<PolygonRepresentation>::operator new (n));
#else	/*qRealTemplatesAvailable*/
	return (BlockAllocated(PolygonRepresentation)::operator new (n));
#endif	/*qRealTemplatesAvailable*/
}

void	PolygonRepresentation::operator delete (void* p)
{
#if		qRealTemplatesAvailable
	BlockAllocated<PolygonRepresentation>::operator delete (p);
#else	/*qRealTemplatesAvailable*/
	BlockAllocated(PolygonRepresentation)::operator delete (p);
#endif	/*qRealTemplatesAvailable*/
}








/*
 ********************************************************************************
 ******************************** LineRepresentation ****************************
 ********************************************************************************
 */
LineRepresentation::LineRepresentation (const Point& from, const Point& to):
	fFrom (from),
	fTo (to)
{
}

void	LineRepresentation::Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const
{
	/* lines have no interior, only an outline */
}

void	LineRepresentation::OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const
{
	// probably should somehow use shapeBounds, probably by scaling points (EG QD's ::MapRect, :MapRgn)
	// SORRY STERL, But this attemt is wrong! It does not
	// use the shapeSize, which it needs to.  Use QD mapPt() I think!!!, or do equiv.
	on.DrawLine (GetFrom () + shapeBounds.GetOrigin (), GetTo () + shapeBounds.GetOrigin (), pen);
}

Point	LineRepresentation::GetLogicalSize ()	const
{
	return (BoundingRect (fFrom, fTo).GetSize ());
}

Point	LineRepresentation::GetFrom ()	const
{
	return (fFrom);
}

Point	LineRepresentation::GetTo ()	const
{
	return (fTo);
}

Region	LineRepresentation::ToRegion (const Rect& shapeBounds) const
{
	/*
 	 * Unfortunately, (and I must think this out further) the definition of regions is
	 * that they are on a pixel basis and represent how many pixels are enclosed. Also,
	 * somewhat strangly, if a Region contains no pixels, we rename it kEmptyRegion. Since
	 * lines are infinitely thin, they contain no pixels, and thus their region is empty.
	 * We may want to consider changing lines to have a thickness, or to intrduce a new
	 * class that adds this concept.
	 */
	return (kEmptyRegion);
}

Real	LineRepresentation::GetSlope ()	const
{
	AssertNotReached ();
	return (0);
}

Real	LineRepresentation::GetXIntercept ()	const
{
	AssertNotReached ();
	return (0);
}

Real	LineRepresentation::GetYIntercept ()	const
{
	AssertNotReached ();
	return (0);
}

Point	LineRepresentation::GetPointOnLine (Real percentFrom)	const
{
	AssertNotReached ();
	return (kZeroPoint);
}

Line	LineRepresentation::GetPerpendicular (const Point& throughPoint)	const
{
	AssertNotReached ();
	return ((LineRepresentation*)this);	// just a hack - hack around const and later 
										// build real line...
}

Line	LineRepresentation::GetBisector () const
{
	return (GetPerpendicular (GetPointOnLine (0.5)));
}

Real	LineRepresentation::GetLength ()	const
{
	return (Distance (fFrom, fTo));
}

#if		!qRealTemplatesAvailable
BlockAllocatedDeclare (LineRepresentation);
BlockAllocatedImplement (LineRepresentation);
#endif	/*!qRealTemplatesAvailable*/

void*	LineRepresentation::operator new (size_t n)
{
#if		qRealTemplatesAvailable
	return (BlockAllocated<LineRepresentation>::operator new (n));
#else	/*qRealTemplatesAvailable*/
	return (BlockAllocated(LineRepresentation)::operator new (n));
#endif	/*qRealTemplatesAvailable*/
}

void	LineRepresentation::operator delete (void* p)
{
#if		qRealTemplatesAvailable
	BlockAllocated<LineRepresentation>::operator delete (p);
#else	/*qRealTemplatesAvailable*/
	BlockAllocated(LineRepresentation)::operator delete (p);
#endif	/*qRealTemplatesAvailable*/
}










/*
 ********************************************************************************
 *************************** OrientedRectShapeRepresentation ********************
 ********************************************************************************
 */

// INCOMPLETE!!!! MAYBE BAD IDEA???
OrientedRectShapeRepresentation::OrientedRectShapeRepresentation (const Rect& rect, Angle angle):
	ShapeRepresentation (),
	fRect (kZeroRect),
	fAngle (0),
	fRegion (kEmptyRegion)
{
}

OrientedRectShapeRepresentation::OrientedRectShapeRepresentation (const Point& aCorner, const Point& bCorner, Coordinate height):
	ShapeRepresentation (),
	fRect (kZeroRect),
	fAngle (0),
	fRegion (kEmptyRegion)
{
}

void	OrientedRectShapeRepresentation::OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const
{
}

void	OrientedRectShapeRepresentation::Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const
{
}

Region	OrientedRectShapeRepresentation::ToRegion (const Rect& shapeBounds) const
{
}

Point	OrientedRectShapeRepresentation::GetLogicalSize ()	const
{
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

