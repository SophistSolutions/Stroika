/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Shape__
#define	__Shape__

/*
 * $Header: /fuji/lewis/RCS/Shape.hh,v 1.8 1992/11/25 22:43:14 lewis Exp $
 *
 * Description:
 *
 *		Shapes are graphical entities that can be drawn on a Tablet. 
 *		A shape is a closed region within the plane, described in a formulaic, efficient representation. They 
 *		can either outline their boundary, or paint their interior. Shapes can be easily converted to and
 *		from regions. Shapes have no intrinsic location or size, so they must be passed in a bounding 
 *		rectangle for most operations.
 *
 * TODO:
 *		-	Redo the comments to make clean the substantial changes here - about using objectslicing,
 *			reference counting, inherited X(X&) etc... Lots done differently than the rest of stroika
 *			and comments out of date!!!!!!!!!
 *
 * New Design:
 *		- First, change over to envelope letter stategy, with most basic classes that already exist,
 *		beiing trivial subclasses of shape. Shape owns a ShapeRep, and does the ref counting on it.
 *		ScaledShape is a trivial subclass of Shape, but keeps ScaledShapeRep () only to supply new api
 *		functions - base class Shape does all mem management. (May want to now make EVERYTHING be a scaled
 *		shape).
 *
 *		- Switch to using Real/Fixed point coordinates, based on printers points. (same as Postscript).
 *
 *		PathCompument is new basic class that can be line, arc, or arbitrary curve.
 *
 *		Path= Sequence of Components. These can be Outlines (like Stroke in Postscript).
 *
 *		Paths can be CLOSED to make a ClosedPath. This is EXACTLY the same as a Stroika Shape (scaled).
 *		These can be ToRegioned() and Filled ().
 *
 *
 * Notes:
 *			-- DOCUMENT THAT WE EXPLICITLY ENDORSE and encourage object slicing here.
 *
 *		Rectangle	r;
 *		Shape		s = r;		// is just FINE!!!
 *
 *
 * Changes:
 *	$Log: Shape.hh,v $
 *		Revision 1.8  1992/11/25  22:43:14  lewis
 *		Started conversion to new genclass based templates, and newer container names.
 *
 *		Revision 1.7  1992/09/11  18:36:49  sterling
 *		use new Shared implementation
 *
 *		Revision 1.6  1992/09/05  16:14:25  lewis
 *		Renamed Nil->Nil.
 *
 *		Revision 1.5  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/08  01:54:13  lewis
 *		Rename PointInside->Contains () cuz sterl likes name better, and to be
 *		consistent with Region/Rect.
 *
 *		Revision 1.3  1992/07/04  14:49:47  lewis
 *		Added BlockAllocation/operator new/delete overrides to the shape reps for
 *		better performance.
 *
 *		Revision 1.2  1992/07/04  02:33:04  lewis
 *		Very substantial changes - Rename Shape->ShapeRepresentation, and make Shape
 *		own a Shared<ShapeRepresnetion>. Then do a parallel hierarchy
 *		off of shape, and ShapeRepresentation - just sometimes owning trivial pointers,
 *		and the other using more standard overriding of methods. Must document this
 *		stuff better, but I think this should be substantially easier to use.
 *		Maybe write a paper about this technique!!!
 *
 *		Revision 1.1  1992/06/19  22:33:01  lewis
 *		Initial revision
 *
 *		Revision 1.14  92/05/10  00:09:35  00:09:35  lewis (Lewis Pringle)
 *		Fixed minor syntax errors porting to BC++/templates.
 *		
 *		Revision 1.13  92/04/16  11:04:18  11:04:18  lewis (Lewis Pringle)
 *		Put back qMPW_CFRONT_2_1_SingleObjectSubclassThenAddPureVirtualBreaksLoadDump_BUG. I dont
 *		know what could have possessed me to remove it. I guess I wanted to know if the
 *		bug still existed - well - it does....
 *		
 *		Revision 1.12  92/04/15  14:30:36  14:30:36  lewis (Lewis Pringle)
 *		Adjust comments to be in radians since that is the default for Angle.
 *		
 *		Revision 1.10  92/04/14  07:10:25  07:10:25  lewis (Lewis Pringle)
 *		Typed in new shape design - letter envelopes, real coordinates, and postscript like path / stroke stuff.
 *		
 *		Revision 1.9  92/04/14  07:08:49  07:08:49  lewis (Lewis Pringle)
 *		Try using INHERRIT_FROM_SINGLE_OBJECT macro. May break loaddump on mac, and if does, revert to old.
 *		
 *		Revision 1.7  1992/02/21  18:03:37  lewis
 *		Got rid of qGPlus_ClassOpNewDelBroke workaround.
 *
 *
 *
 *
 *
 */

#include	"Angle.hh"
#include	"Shared.hh"
#include	"Sequence.hh"

#include	"Pen.hh"
#include	"Region.hh"



class	Tablet;
class	Shape;
class	ShapeRepresentation;


#if		!qRealTemplatesAvailable
	#include "TFileMap.hh"
	#include SharedOfShapeRepresentation_hh
#endif




class	Shape {
	public:
		// not sure if I want a no -arg shape ctor???

		Shape (ShapeRepresentation* shapePointer);
		Shape (const Shape& shape);
		~Shape ();

		nonvirtual	Shape&	operator= (const Shape& rhs);

		nonvirtual	Boolean	Contains (const Point& p, const Rect& shapeBounds) const;
		nonvirtual	void	OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const;
		nonvirtual	void	Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const;
		nonvirtual	Region	ToRegion (const Rect& shapeBounds) const;

		// GetLogicalSize added From Scaled Shape...
		nonvirtual	Point	GetLogicalSize ()	const;

	private:
#if		qRealTemplatesAvailable
		Shared<ShapeRepresentation>	fSharedPart;
#else
		Shared(ShapeRepresentation)	fSharedPart;
#endif
};





class	Tablet;
class	ShapeRepresentation
#if		!qMPW_CFRONT_2_1_SingleObjectSubclassThenAddPureVirtualBreaksLoadDump_BUG
	INHERRIT_FROM_SINGLE_OBJECT 
#endif
{
	protected:
		ShapeRepresentation () {}

	public:
		virtual	~ShapeRepresentation () {}

		virtual	Boolean	Contains (const Point& p, const Rect& shapeBounds) const;
		virtual	void	OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const		=	Nil;
		virtual	void	Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const	=	Nil;

		virtual	Region	ToRegion (const Rect& shapeBounds) const;

// GetLogicalSize added From Scaled Shape...
// probably should be pure virtual, but as quick hack - just have it return kZeroPoint by default
// Soon we must rethink this whole shape design!!! LGP 7/3/92
		virtual	Point	GetLogicalSize ()	const;
};








class	RectangleRepresentation;
class	Rectangle : public Shape {
	public:
		Rectangle ();
		Rectangle (RectangleRepresentation* rectangleRepresentation);
};

class	RectangleRepresentation : public ShapeRepresentation {
	public:
		RectangleRepresentation ();

		override	Boolean	Contains (const Point& p, const Rect& shapeBounds) const;
		override	void	OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const;
		override	void	Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const;

		override	Region	ToRegion (const Rect& shapeBounds) const;

	public:	// do block allocation for faster performance....
		static	void*	operator new (size_t n);
		static	void	operator delete (void* p);
};








class	RoundedRectangleRepresentation;
class	RoundedRectangle : public Shape {
	public:
		static	Point	kDefaultRounding;
		RoundedRectangle (const Point& rounding = kDefaultRounding);
		RoundedRectangle (RoundedRectangleRepresentation* roundedRectangleRepresentation);

		nonvirtual	Point	GetRounding ()	const;

	private:
		RoundedRectangleRepresentation*	fRepresentation;
};

class	RoundedRectangleRepresentation : public ShapeRepresentation {
	public:
		RoundedRectangleRepresentation (const Point& rounding);

		override	void	OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const;
		override	void	Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const;

		nonvirtual	Point	GetRounding ()	const;

	private:
		Point	fRounding;

	public:	// do block allocation for faster performance....
		static	void*	operator new (size_t n);
		static	void	operator delete (void* p);
};









class	OvalRepresentation;
class	Oval : public Shape {
	public:
		Oval ();
		Oval (OvalRepresentation* ovalRepresentation);
};

class	OvalRepresentation : public ShapeRepresentation {
	public:
		OvalRepresentation ();

		override	void	OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const;
		override	void	Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const;

	public:	// do block allocation for faster performance....
		static	void*	operator new (size_t n);
		static	void	operator delete (void* p);
};









/*
 * The start angle is relative to zero at 3 o-clock. The Angle, is the amount to arc from the
 * start, and is positive in the counter-clockwise direction. To specify a whole circle, SetAngle (2*kPi).
 * The start does not matter.
 *
 * We disallow angles less than zero, and greater than 2*kPie (this restriction maybe later lifted once
 * we've thought out a good definition. The most sensible definition in this case I think is to
 * assume the pen stayed down for that many degrees, and if it was a copy operation, then effectively
 * its the same as 0-2*kPie to say 0.2-3*kPi, but in xor mode very different, and a few pixels would be drawn,
 * and then erased. This could straightforwardly be simulated using the X and mac primitives,
 * if we wanted, but its not clear we want to.
 */
class	ArcRepresentation;
class	Arc : public Shape {
	public:
		Arc (Angle startAngle, Angle arcAngle);
		Arc (ArcRepresentation* arcRepresentation);

		nonvirtual	Angle	GetStart () const;
		nonvirtual	Angle	GetAngle () const;

	private:
		ArcRepresentation*	fRepresentation;
};

class	ArcRepresentation : public ShapeRepresentation {
	public:
		ArcRepresentation (Angle startAngle, Angle arcAngle);

		override	void	OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const;
		override	void	Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const;

		nonvirtual	Angle	GetStart () const;
		nonvirtual	Angle	GetAngle () const;

	private:
		Angle	fStartAngle;
		Angle	fArcAngle;

	public:	// do block allocation for faster performance....
		static	void*	operator new (size_t n);
		static	void	operator delete (void* p);
};








class	RegionShapeRepresentation;
class	RegionShape : public Shape {
	public:
		RegionShape (const Region& rgn);
		RegionShape (RegionShapeRepresentation* regionShapeRepresentation);
};

class	RegionShapeRepresentation : public ShapeRepresentation {
	public:
		RegionShapeRepresentation (const Region& rgn);

		override	void	OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const;
		override	void	Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const;

		override	Region	ToRegion (const Rect& shapeBounds) const;
		override	Point	GetLogicalSize ()	const;

	private:
		Region	fRegion;

	public:	// do block allocation for faster performance....
		static	void*	operator new (size_t n);
		static	void	operator delete (void* p);
};







#if		!qRealTemplatesAvailable
	#include "TFileMap.hh"
	#include SequenceOfPoint_hh
#endif


class	PolygonRepresentation;
class	Polygon : public Shape {
	public:
#if		qRealTemplatesAvailable
		Polygon (const Sequence<Point>& points);
#else
		Polygon (const Sequence(Point)& points);
#endif
		Polygon (PolygonRepresentation* polygonRepresentation);

#if		qRealTemplatesAvailable
		nonvirtual	const	Sequence<Point>&	GetPoints () const;
#else
		nonvirtual	const	Sequence(Point)&	GetPoints () const;
#endif

	private:
		PolygonRepresentation*	fRepresentation;
};

class	PolygonRepresentation : public ShapeRepresentation {
	public:
#if		qRealTemplatesAvailable
		PolygonRepresentation (const Sequence<Point>& points);
#else
		PolygonRepresentation (const Sequence(Point)& points);
#endif
		virtual ~PolygonRepresentation ();

		override	void	OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const;
		override	void	Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const;

		override	Point	GetLogicalSize ()	const;

		nonvirtual	const	Sequence(Point)&	GetPoints () const;

	private:
		nonvirtual	void	RebuildOSPolygon () const;	// Changes cached os poly only...

#if		qRealTemplatesAvailable
		Sequence<Point>	fPoints;
#else
		Sequence(Point)	fPoints;
#endif

#if		qMacGDI
		struct osPolygon**		fOSPolygon;
#endif	/*qMacGDI*/

	public:	// do block allocation for faster performance....
		static	void*	operator new (size_t n);
		static	void	operator delete (void* p);
};







class	LineRepresentation;
class	Line : public Shape {
	public:
		Line (const Point& from = kZeroPoint, const Point& to = kZeroPoint);
		Line (LineRepresentation* lineRepresentation);

		nonvirtual	Point	GetFrom ()	const;
		nonvirtual	Point	GetTo ()	const;

		nonvirtual	Real	GetSlope ()	const;
		nonvirtual	Real	GetXIntercept ()	const;
		nonvirtual	Real	GetYIntercept ()	const;
		nonvirtual	Point	GetPointOnLine (Real percentFrom)	const;
		nonvirtual	Line	GetPerpendicular (const Point& throughPoint)	const;
		nonvirtual	Line	GetBisector () const;
		nonvirtual	Real	GetLength ()	const;

	private:
		LineRepresentation*	fRepresentation;
};

class	LineRepresentation : public ShapeRepresentation {
	public:
		LineRepresentation (const Point& from = kZeroPoint, const Point& to = kZeroPoint);

		override	void	OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const;
		override	void	Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const;

		override	Region	ToRegion (const Rect& shapeBounds) const;
		override	Point	GetLogicalSize ()	const;
		
		nonvirtual	Point	GetFrom ()	const;
		nonvirtual	Point	GetTo ()	const;

		nonvirtual	Real	GetSlope ()	const;
		nonvirtual	Real	GetXIntercept ()	const;
		nonvirtual	Real	GetYIntercept ()	const;
		nonvirtual	Point	GetPointOnLine (Real percentFrom)	const;
		nonvirtual	Line	GetPerpendicular (const Point& throughPoint)	const;
		nonvirtual	Line	GetBisector () const;
		nonvirtual	Real	GetLength ()	const;

	private:
		Point	fFrom;
		Point	fTo;

	public:	// do block allocation for faster performance....
		static	void*	operator new (size_t n);
		static	void	operator delete (void* p);
};








// INCOMPLETE, AND UNUSED...
class	OrientedRectShapeRepresentation : public ShapeRepresentation {
	public:
		OrientedRectShapeRepresentation (const Rect& rect, Angle angle);
		OrientedRectShapeRepresentation (const Point& aCorner, const Point& bCorner, Coordinate height);

		override	void	OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const;
		override	void	Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const;

		override	Region	ToRegion (const Rect& shapeBounds) const;
		override	Point	GetLogicalSize ()	const;

	private:
		Rect	fRect;
		Angle	fAngle;
		Region	fRegion;
};









/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

// class	Shape
	inline	Shape::Shape (ShapeRepresentation* shapePointer):
		fSharedPart (shapePointer)
	{
	}

	inline	Shape::Shape (const Shape& shape):
		fSharedPart (shape.fSharedPart)
	{
	}

	inline	Shape::~Shape ()
	{
	}
	
	inline	Shape&	Shape::operator= (const Shape& rhs)
	{
		fSharedPart = rhs.fSharedPart;
		return (*this);
	}
	
	inline	Boolean	Shape::Contains (const Point& p, const Rect& shapeBounds) const
	{
		return (fSharedPart->Contains (p, shapeBounds));
	}
	
	inline	void	Shape::OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const
	{
		fSharedPart->OutLine (on, shapeBounds, pen);
	}
	
	inline	void	Shape::Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const
	{
		fSharedPart->Paint (on, shapeBounds, brush);
	}
	
	inline	Region	Shape::ToRegion (const Rect& shapeBounds) const
	{
		return (fSharedPart->ToRegion (shapeBounds));
	}
	
	inline	Point	Shape::GetLogicalSize () const
	{
		return (fSharedPart->GetLogicalSize ());
	}
	



// class	Rectangle
	inline	Rectangle::Rectangle ():
		Shape (new RectangleRepresentation ())
	{
	}

	inline	Rectangle::Rectangle (RectangleRepresentation* rectangleRepresentation):
		Shape (rectangleRepresentation)
	{
	}






// class	RoundedRectangle
	inline	RoundedRectangle::RoundedRectangle (RoundedRectangleRepresentation* roundedRectangleRepresentation):
		Shape (roundedRectangleRepresentation),
		fRepresentation (roundedRectangleRepresentation)
	{
	}

	inline	RoundedRectangle::RoundedRectangle (const Point& rounding):
		Shape (Nil),
		fRepresentation (Nil)
		
	{
		fRepresentation = new RoundedRectangleRepresentation (rounding);
		*this = fRepresentation;
	}

	inline	Point	RoundedRectangle::GetRounding ()	const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetRounding ());
	}








// class	Oval
	inline	Oval::Oval ():
		Shape (new OvalRepresentation ())
	{
	}

	inline	Oval::Oval (OvalRepresentation* ovalRepresentation):
		Shape (ovalRepresentation)
	{
	}





// class	Arc
	inline	Arc::Arc (ArcRepresentation* arcRepresentation):
		Shape (arcRepresentation),
		fRepresentation (arcRepresentation)
	{
	}

	inline	Arc::Arc (Angle startAngle, Angle arcAngle):
		Shape (Nil),
		fRepresentation (Nil)
	{
		fRepresentation = new ArcRepresentation (startAngle, arcAngle);
		*this = fRepresentation;
	}

	inline	Angle	Arc::GetStart () const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetStart ());
	}
	
	inline	Angle	Arc::GetAngle () const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetAngle ());
	}







// class	RegionShape

// class	RegionShape
	inline	RegionShape::RegionShape (const Region& rgn):
		Shape (new RegionShapeRepresentation (rgn))
	{
	}

	inline	RegionShape::RegionShape (RegionShapeRepresentation* regionShapeRepresentation):
		Shape (regionShapeRepresentation)
	{
	}






// class	Polygon
	inline	Polygon::Polygon (PolygonRepresentation* polygonRepresentation):
		Shape (polygonRepresentation),
		fRepresentation (polygonRepresentation)
	{
	}

#if		qRealTemplatesAvailable
	inline	Polygon::Polygon (const Sequence<Point>& points):
#else
	inline	Polygon::Polygon (const Sequence(Point)& points):
#endif
		Shape (Nil),
		fRepresentation (Nil)
	{
		fRepresentation = new PolygonRepresentation (points);
		*this = fRepresentation;
	}

#if		qRealTemplatesAvailable
	inline	const	Sequence<Point>&	Polygon::GetPoints () const
#else
	inline	const	Sequence(Point)&	Polygon::GetPoints () const
#endif
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetPoints ());
	}







// class	Line
	inline	Line::Line (LineRepresentation* lineRepresentation):
		Shape (lineRepresentation),
		fRepresentation (lineRepresentation)
	{
	}

	inline	Line::Line (const Point& from, const Point& to):
		Shape (Nil),
		fRepresentation (Nil)
	{
		fRepresentation = new LineRepresentation (from, to);
		*this = fRepresentation;
	}

	inline	Point	Line::GetFrom () const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetFrom ());
	}

	inline	Point	Line::GetTo () const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetTo ());
	}

	inline	Real	Line::GetSlope () const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetSlope ());
	}

	inline	Real	Line::GetXIntercept () const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetXIntercept ());
	}

	inline	Real	Line::GetYIntercept () const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetYIntercept ());
	}

	inline	Point	Line::GetPointOnLine (Real percentFrom) const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetPointOnLine (percentFrom));
	}

	inline	Line	Line::GetPerpendicular (const Point& throughPoint) const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetPerpendicular (throughPoint));
	}

	inline	Line	Line::GetBisector () const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetBisector ());
	}

	inline	Real	Line::GetLength () const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetLength ());
	}






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Shape__*/

