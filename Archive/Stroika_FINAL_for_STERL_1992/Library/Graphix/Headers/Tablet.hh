/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Tablet__
#define	__Tablet__

/*
 * $Header: /fuji/lewis/RCS/Tablet.hh,v 1.4 1992/09/05 16:14:25 lewis Exp $
 *
 * Description:
 *
 *		A Tablet is an entity which can supports drawing into 1 or more PixelMaps
 * all woven together into a tablet coordinate system.  It maps Tablet
 * coordinates to the coordinate systems of the various owned pixelmaps.
 *
 *		One common usage of a Tablet, is a Window.  A Window can span multiple
 * monitors, and thus must take special care when dispatching its draw
 * routines to dispatch them into the right PixelMap.  (On the mac,
 * this happens automagically - Color QD does it.  On X, multiple GDevices are not supported, and
 * so the issue does not come up.).
 *
 *		Another use of Tablets, is when a PixelMap is handed in.  This is an
 * offscreen tablet usually.
 *
 *		Other uses still, include creating pictures, and printing.
 *
 *		DO WE WANT TO SUPPORT OPACITY AS OPTION?
 *		Boolean	fOpaque;		Diff between MAC window and GrafPort
 *
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Tablet.hh,v $
 *		Revision 1.4  1992/09/05  16:14:25  lewis
 *		Renamed NULL->Nil.
 *
 *		Revision 1.3  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/16  18:01:38  lewis
 *		Added SetOSBackground method for the mac.
 *
 *		Revision 1.1  1992/06/19  22:33:01  lewis
 *		Initial revision
 *
 *		Revision 1.20  1992/04/07  01:02:45  lewis
 *		Add visual and CLUT for XGDI version of tablet, so we can do depth/color conversions on bitblits, and
 *		later on fills, and pen operations.
 *
 *		Revision 1.19  92/04/02  11:24:54  11:24:54  lewis (Lewis Pringle)
 *		Started to define an AbstractTablet class. Still not sure where I'm going with this, but
 *		to implement portable pictures, I needs something akin to this. Maybe also use it for consturcting
 *		regions (on other platforms that dont have the same hack as mac quickdraw).
 *		Tablet no longer inherits from SingleObject - maybe want to put that back???
 *		Made BitBlit a method of Tablet and got rid of Global BitBlit procesudes.
 *		Got rid of fOSDisplay, and sOSDisplay fields/ static members of tablet - use gDisplay instead.
 *		Got rid of GetClip/SetClip from TabletOwner - usually this will be done thru a seperate mechansim, and
 *		would encourage misuse if tabletowner made it too easy to set. If you must, you can still say
 *		GetTablet ()->SetClip (LocalToTablet (XXX)));
 *		Added more overloaded BitBlit routines and now they indirect to Tabeet instead of global functions.
 *		Was more carefuly about making things like Paint/DrawText etc virtual so in View we can override them
 *		and set the clip first.
 *		Added Tablet::GetDrawable accessor for XGDI.
 *		
 *		Revision 1.18  92/03/26  09:22:17  09:22:17  lewis (Lewis Pringle)
 *		Made TextWidht a const method of both Tablet, and TabletOwner.
 *		
 *		Revision 1.16  1992/02/21  21:08:38  lewis
 *		Use macro INHERRIT_FROM_SINGLE_OBJECT instead of class SingleObject so
 *		that crap doesnt appear in non-mac inheritance graphs.
 *
 *		Revision 1.15  1992/02/11  01:44:58  lewis
 *		Add call to do server grab (should this by XGDI only?).
 *
 *		Revision 1.14  1992/02/11  00:23:59  lewis
 *		Misc mac cleanups - got rid of param to InitFromPixelMap, and
 *		got rid of mkNewGraphPort routine (was for gworld stuff).
 *
 *		Revision 1.13  1992/02/11  00:04:52  lewis
 *		Got rid of gworld stuff for mac, and some cleanups.
 *
 *		Revision 1.11  1992/01/22  08:03:46  lewis
 *		Added Sync () method (mostly for X, but can be used on mac).
 *
 *		Revision 1.10  1992/01/22  04:15:57  lewis
 *		Add static member to access the display
 *
 *		Revision 1.9  1992/01/14  18:40:36  lewis
 *		Added X support for getting and setting the clip region, and the visregio, and added interface for
 *		this for the mac.
 *
 *		Revision 1.8  1992/01/14  05:58:53  lewis
 *		Implement setting of fonts into the GC when drawing text. Implemented
 *		TabletOwner. Deleted temp hack variables since I doublt they'll be useful anymore. We can do most initial implementation
 *		under UNIX, where the compilers are fast, and get most of the bugs out there, and then on mac should be no need for extra variables.
 *
 *		Revision 1.7  1992/01/10  03:02:12  lewis
 *		Added support to put Xlib interface into syncronous mode - for debugging.
 *
 *
 *
 */

#include	"Angle.hh"

#include	"Brush.hh"
#include	"Font.hh"
#include	"GraphixDevice.hh"
#include	"Pen.hh"
#include	"PixelMap.hh"
#include	"Region.hh"




class	Shape;
class	Picture;


// later have tablet be the abstract class?? For now, define AbstractTablet to be what all tablets must do..??
// Or maybe have mac/unix tables and Tablet be condiutionlly compiled to be one or the other, like with Widgets..}????



// Big design decision - try to move away from being pixel based/raster based. Remove all such assumtions. Only
// ones that should be left are to implement real based point/rect/region. Then this interface should be ttoally
// res independ.t????



class	AbstractTablet {
	protected:
		AbstractTablet ();

	public:
		virtual ~AbstractTablet ();

	private:
		AbstractTablet (const AbstractTablet&);							// unimplemented since never called (prevent copy)
		const AbstractTablet& operator= (const AbstractTablet&);		// unimplemented since never called (prevent copy)


	/*
	 * Clipping/Visible Area manipulation.
	 *
	 *		All drawing operatations are clipped to the intersection of the visible region, and the
	 *	ClipRegion (the net is called the EffectiveClipRegion ()). Users can set the ClipRegion.
	 */
	public:
		nonvirtual	Region	GetClip () const;
		nonvirtual	void	SetClip (const Region& newClip);

		nonvirtual	Region	GetVisibleArea () const;
		nonvirtual	void	SetVisibleArea (const Region& newVisibleArea);

		nonvirtual	Region	GetEffectiveClipRegion () const;


	/*
	 * Drawing Routines.
	 */
	public:
		nonvirtual	void	OutLine (const Shape& s, const Rect& shapeBounds, const Pen& pen);
		nonvirtual	void	Paint (const Shape& s, const Rect& shapeBounds, const Brush& brush);

	/*
	 * No wrapping.  Just plain drawing.
	 */
	public:
		nonvirtual	void		DrawText (const String& text, const Font& font, const Point& at);
		nonvirtual	Coordinate	TextWidth (const String& text, const Font& font, const Point& at = kZeroPoint) const;

	/*
	 * Great IDEA - better way of dealing with tile alignment problem.
	 */
	public:
		nonvirtual	Point	GetTileAlignement ();
		nonvirtual	void	SetTileAlignment (const Point& tileAlignment);


	public:
		nonvirtual	void	DrawPicture (const Picture& picture, const Rect& intoRect);
		nonvirtual	void	ScrollBits (const Rect& r, const Point& delta, const Tile& background);


	/*
	 * Force drawn bits to be rendered on the screen. This is mostly important under X, but also on
	 * the mac, in the presense of things like graphix accellerator cards, etc...
	 */
	public:
		nonvirtual	void	Sync ();


#if		qXGDI && qDebug
  /*
   * X Debugging support.
   */
	public:
		static	Boolean		GetSyncMode ();
		static	void		SetSyncMode (Boolean syncMode);
	private:
		static	Boolean		sSyncMode;
#endif	/*qXGDI && qDebug*/


	/*
	 * These protected routines actually implement the functionality required by the above interfaces.
	 */
	protected:
		virtual	void		Sync_ ()																	=	Nil;
		virtual	Region		GetClip_ () const															=	Nil;
		virtual	void		SetClip_ (const Region& newClip)											=	Nil;
		virtual	Region		GetVisibleArea_ () const													=	Nil;
		virtual	void		SetVisibleArea_ (const Region& newVisibleArea)								=	Nil;
		virtual	Region		GetEffectiveClipRegion_ () const											=	Nil;
		virtual	void		OutLine_ (const Shape& s, const Rect& shapeBounds, const Pen& pen)			=	Nil;
		virtual	void		Paint_ (const Shape& s, const Rect& shapeBounds, const Brush& brush)		=	Nil;
		virtual	void		DrawText_ (const String& text, const Font& font, const Point& at)			=	Nil;
		virtual	Coordinate	TextWidth_ (const String& text, const Font& font, const Point& at) const	=	Nil;
		virtual	Point		GetTileAlignement_ ()														=	Nil;
		virtual	void		SetTileAlignment_ (const Point& tileAlignment)								=	Nil;
		virtual	void		DrawPicture_ (const Picture& picture, const Rect& intoRect)					=	Nil;
		virtual	void		ScrollBits_ (const Rect& r, const Point& delta, const Tile& background)		=	Nil;
};





class	Tablet {
	public:
		enum { eMaxDeviceDepth = 0xFF /* special - means use max for any device attached */ };
		// If NOT given a grafPtr, we will destroy
		Tablet (const Point& size, UInt8 depth = eMaxDeviceDepth);
		Tablet (PixelMap* pixelMap);
#if		qMacGDI
		Tablet (struct osGrafPort* osGrafPtr);	// If given a grafPtr, builders responsibilty to destroy
#elif	qXGDI
		Tablet (unsigned long drawable);
#endif	/*qMacGDI*/

	public:
		virtual ~Tablet ();

	private:
		Tablet (const Tablet&);							// unimplemented since never called (prevent copy)
		const Tablet& operator= (const Tablet&);		// unimplemented since never called (prevent copy)

	/*
	 * Routines to access component PixelMaps/GraphixDevices, and total region they encompass.
	 * (See comment at top of file).
	 */
	public:
		/*
		 * Return region (in tablet coordinates) describing all union of all graphix devices spanned
		 * by this tablet.
		 *		On a mac, for example, the desktops port->GetBounds () would yeild the ::GrayRgn (),
		 * for a window, would yeild its bounds in local coordinates.
		 */
		virtual	Region	GetBounds () const;

		/*
		 * Access to Graphix devices we can image on.
		 */
		virtual	void*		MakeGraphixDeviceIterator ();		// NYI

		nonvirtual	UInt8	GetMaxDeviceDepth ();				// NYI
		nonvirtual	Boolean	DeviceSupportsColor ();				// NYI - probably nonsense

#if		qMacGDI
		nonvirtual	struct osGrafPort*	GetOSGrafPtr () const;		// Back door access.
#elif	qXGDI
		nonvirtual	unsigned long		GetDrawable () const;
#endif	/*qMacGDI*/




	/*
	 * Clipping/Visible Area manipulation.
	 *
	 *		All drawing operatations are clipped to the intersection of the visible region, and the
	 *	ClipRegion (the net is called the EffectiveClipRegion ()). Users can set the ClipRegion.
	 */
	public:
		nonvirtual	Region	GetClip () const;
		nonvirtual	void	SetClip (const Region& newClip);

		nonvirtual	Region	GetVisibleArea () const;
		nonvirtual	void	SetVisibleArea (const Region& newVisibleArea);

		nonvirtual	Region	GetEffectiveClipRegion () const;

	private:
#if		qXGDI
		Region	fClipRegion;
		Region	fVisibleArea;
#endif
		Region	fEffectiveClipRegion;
		Boolean	fEffectiveClipValid;






	/*
	 * Drawing Routines.
	 */
	public:
		nonvirtual	void	OutLine (const Shape& s, const Rect& shapeBounds, const Pen& pen);
		nonvirtual	void	Paint (const Shape& s, const Rect& shapeBounds, const Brush& brush);

		/*
		 * No wrapping.  Just plain drawing.
		 */
		nonvirtual	void		DrawText (const String& text, const Font& font, const Point& at);
		nonvirtual	Coordinate	TextWidth (const String& text, const Font& font, const Point& at = kZeroPoint) const;

		/*
		 * Great IDEA - better way of dealing with tile alignment problem.
		 */
		nonvirtual	Point	GetTileAlignement ();
		nonvirtual	void	SetTileAlignment (const Point& tileAlignment);

		/*
		 * p in tablet coordinates.  Since a tablet may overlap multiple PixMaps (eg monitors),
		 * we cannot return PixelValues - they dont make sense.
		 */
		nonvirtual	Color	GetColor (const Point& p) const;
		nonvirtual	void	SetColor (const Point& p, const Color& c);


		nonvirtual	void	DrawPicture (const class Picture& picture, const Rect& intoRect);
		nonvirtual	void	ScrollBits (const Rect& r, const Point& delta, const Tile& background);


	/*
	 * BitBlits from a given pixelmap to the tablet (with/without mask) and then going from the
	 * tablet, back to the given Pixelmap.
	 */
	public:
		nonvirtual	void	BitBlit (const PixelMap& from, const Rect& fromRect, const Rect& toRect, TransferMode tMode);
		nonvirtual	void	BitBlit (const PixelMap& from, const Rect& fromRect, const Rect& toRect, TransferMode tMode, const Region& mask);
		nonvirtual	void	BitBlit (const Rect& fromRect, PixelMap& to, const Rect& toRect, TransferMode tMode);
		nonvirtual	void	BitBlit (const Rect& fromRect, PixelMap& to, const Rect& toRect, TransferMode tMode, const Region& mask);


	/*
	 * This interface to drawing can be used, but is generally not recomended.
	 * use the shape interface in preference.
	 *
	 * THEN WHY DO WE HAVE BOTH!!!????!!!
	 */
	public:
		nonvirtual	void	OutLineRect (const Rect& r, const Pen& pen);
		nonvirtual	void	PaintRect (const Rect& r, const Brush& brush);
		nonvirtual	void	OutLineRoundedRect (const Rect& r, const Point& rounding, const Pen& pen);
		nonvirtual	void	PaintRoundedRect (const Rect& r, const Point& rounding, const Brush& brush);
		nonvirtual	void	OutLineOval (const Rect& r, const Pen& pen);
		nonvirtual	void	PaintOval (const Rect& r, const Brush& brush);
		nonvirtual	void	OutLineArc (Angle startAngle, Angle arcAngle, const Rect& r, const Pen& pen);
		nonvirtual	void	PaintArc (Angle startAngle, Angle arcAngle,   const Rect& r, const Brush& brush);
		nonvirtual	void	OutLineRegion (const Region& region, const Rect& r, const Pen& pen);
		nonvirtual	void	PaintRegion (const Region& region,   const Rect& r, const Brush& brush);

#if		qMacOS
		nonvirtual	void	OutLinePolygon (struct osPolygon** polygon, const Rect& r, const Pen& pen);
		nonvirtual	void	PaintPolygon (struct osPolygon** polygon, const Rect& r, const Brush& brush);
#endif	/*qMacOS*/

		nonvirtual	void	DrawLine (const Point& from, const Point& to, const Pen& pen);

		static	const	Point	kDontSetOrigin;
#if		qMacGDI
		static	void	SetupOSRepFromBrush (struct osGrafPort* grafPtr, const Brush& brush, const Point& origin = kDontSetOrigin);
		static	void	SetupOSRepFromPen (struct osGrafPort* grafPtr, const Pen& pen, const Point& origin = kDontSetOrigin);
		static	void	SetupOSRepFromFont (struct osGrafPort* grafPtr, TransferMode tMode, const Font& font, const Point& origin = kDontSetOrigin);
		static	void	SetupOSRepFromClip (struct osGrafPort* grafPtr, const Region& clip, const Point& origin = kDontSetOrigin);

		// mac hack to set the background field of the grafport - only to force things that
		// do direct quickdraw draws like TextEdit to use the proper background
		nonvirtual	void	SetOSBackground (const Tile& backGround);

#endif	/*qMacGDI*/
		nonvirtual	void	SetupOSRepFromBrush (const Brush& brush, const Point& origin = kDontSetOrigin);
		nonvirtual	void	SetupOSRepFromPen (const Pen& pen, const Point& origin = kDontSetOrigin);
		nonvirtual	void	SetupOSRepFromFont (TransferMode tMode, const Font& font, const Point& origin = kDontSetOrigin);
		nonvirtual	void	SetupOSRepFromClip (const Region& clip, const Point& origin = kDontSetOrigin);
#if		qMacGDI
		static	void	PrepareOSRep (struct osGrafPort* newWorkPort);
		static	void	RestoreOSRep ();
#endif	/*qMacGDI*/


	/*
	 * Force drawn bits to be rendered on the screen. This is mostly important under X, but also on
	 * the mac, in the presense of things like graphix accellerator cards, etc...
	 */
	public:
		nonvirtual	void	Sync ();

	/*
	 * Notify drawing primitives that we are doing an xor based operation,
	 * and so we must prevent other drawing in the meantime... (really
	 * do an XGrab - should this if #if qXgDI - thinkout better.
	 */
	public:
		nonvirtual	void	ServerGrab ();
		nonvirtual	void	ServerUnGrab ();


#if		qXGDI && qDebug
  /*
   * X Debugging support.
   */
	public:
		static	Boolean		GetSyncMode ();
		static	void		SetSyncMode (Boolean syncMode);
	private:
		static	Boolean		sSyncMode;
#endif	/*qXGDI && qDebug*/


	private:
#if		qXGDI
// intention is to use these on mac and unix and have them portable - on
// mac have done routine inline emptyu???
		nonvirtual	struct _XGC*	UseBrush (const Brush& brush);
		nonvirtual	void	DoneWithBrush (struct _XGC* theGC);
		nonvirtual	struct _XGC*	UsePen (const Pen& pen);
		nonvirtual	void	DoneWithPen (struct _XGC* theGC);
		nonvirtual	struct _XGC*	UseFont (const Font& font);
		nonvirtual	void	DoneWithFont (struct _XGC* theGC);

		nonvirtual	void	ApplyCurrentClip (struct _XGC* theGC);
#endif	/*qXGDI*/

	private:
		Point	fTileAlignment;
		Boolean	fWeDestroy;
#if		qMacGDI
		struct osGrafPort*	fOSGrafPtr;
		Boolean				fDestroyPixelMap;
#elif	qXGDI
		unsigned long		fDrawable;
		void*				fVisual;			// X11R4 headers define no struct tag so nothing reasonable we can declare
												// this as without include X11 headers...
		ColorLookupTable	fColorLookupTable;	// onlu defined if visual type is appropriate...
#endif	/*qMacGDI*/
		PixelMap*			fPixelMap;

	private:
		nonvirtual	void	InitFromPixelMap ();


#if		qXGDI
		friend	void	BitBlit (const PixelMap& from, const Rect& fromRect,
								 Tablet& to, const Rect& toRect, TransferMode tMode, const Region& mask);
#endif	/*qXGDI*/

// maybe should be protected?
#if		qMacGDI
public:
	nonvirtual	void	DoSetPort () const;
	static	void		xDoSetPort (struct osGrafPort* newPort);
	static	struct osGrafPort*	GetCurrentPort ();
#endif	/*qMacGDI*/
};




class	TabletOwner {
	protected:
		TabletOwner ();

	public:
		virtual ~TabletOwner ();

		nonvirtual	Point	LocalToTablet (const Point& p) const;
		nonvirtual	Point	TabletToLocal (const Point& p) const;
		nonvirtual	Rect	LocalToTablet (const Rect& r) const;
		nonvirtual	Rect	TabletToLocal (const Rect& r) const;
		nonvirtual	Region	LocalToTablet (const Region& r) const;
		nonvirtual	Region	TabletToLocal (const Region& r) const;


		virtual	Tablet*	GetTablet () const									=			Nil;


		/*
		 * Simple wrappers on tablet calls that convert to/from Tablet coordinates...
		 *
		 *	Note:	It is an error to call these routines are when there is no tablet present. This
		 *		restriction may someday be lifted, but its safer to restrict now while were deciding...
		 *
		 *	Also, note that routines which draw onto the tablet are virtual so that subclasses (like view)
		 *	can override these routines to provide additional behaviour (like preseting the clip).
		 */
//<<< Because of this usage, it may make sense to get rid of Get/SetClip as methods since they really wont have
//    the desired affect anyhow>>>
//		nonvirtual	Region		GetClip () const;
//		nonvirtual	void		SetClip (const Region& newClip);
		virtual		void		OutLine (const Shape& s, const Rect& shapeBounds, const Pen& pen);
		virtual		void		Paint (const Shape& s, const Rect& shapeBounds, const Brush& brush);
		virtual		void		DrawText (const String& text, const Font& font, const Point& at);
		nonvirtual	Coordinate	TextWidth (const String& text, const Font& font, const Point& at = kZeroPoint) const;
		virtual		void		DrawPicture (const Picture& picture, const Rect& intoRect);
		virtual		void		DrawPicture (const Picture& picture, const Point& at = kZeroPoint);
		virtual		void		ScrollBits (const Rect& r, const Point& delta, const Tile& background);
		virtual		void		BitBlit (const PixelMap& from, const Rect& fromRect, const Rect& toRect, TransferMode tMode = eCopyTMode);
		virtual		void		BitBlit (const PixelMap& from, const Rect& fromRect, const Rect& toRect, TransferMode tMode, const Region& mask);
		nonvirtual	void		BitBlit (const Rect& fromRect, PixelMap& to, const Rect& toRect, TransferMode tMode = eCopyTMode);
		nonvirtual	void		BitBlit (const Rect& fromRect, PixelMap& to, const Rect& toRect, TransferMode tMode, const Region& mask);

	protected:
		virtual	Point	LocalToTablet_ (const Point& p)	const				=			Nil;
		virtual	Point	TabletToLocal_ (const Point& p)	const				=			Nil;
};



#if		0
// superceded - use BitBlitter or methods of Tablet...
extern	void	BitBlit (const Tablet& from, const Rect& fromRect,
						 PixelMap& to, const Rect& toRect, TransferMode tMode);
extern	void	BitBlit (const Tablet& from, const Rect& fromRect,
						 PixelMap& to, const Rect& toRect, TransferMode tMode, const Region& mask);
extern	void	BitBlit (const PixelMap& from, const Rect& fromRect,
						 Tablet& to, const Rect& toRect, TransferMode tMode);
extern	void	BitBlit (const PixelMap& from, const Rect& fromRect,
						 Tablet& to, const Rect& toRect, TransferMode tMode, const Region& mask);
extern	void	BitBlit (const Tablet& from, const Rect& fromRect,
						 Tablet& to, const Rect& toRect, TransferMode tMode);
extern	void	BitBlit (const Tablet& from, const Rect& fromRect,
						 Tablet& to, const Rect& toRect, TransferMode tMode, const Region& mask);

#endif




/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	Point			Tablet::GetTileAlignement ()		{ return (fTileAlignment); }
#if		qMacGDI
inline	osGrafPort*		Tablet::GetOSGrafPtr () const		{ EnsureNotNil (fOSGrafPtr); return (fOSGrafPtr); }
#elif	qXGDI
inline	unsigned long	Tablet::GetDrawable () const		{ EnsureNotNil (fDrawable); return (fDrawable); }
#endif	/*qMacGDI*/



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Tablet__*/


