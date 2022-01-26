/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PixelMapEditor__
#define	__PixelMapEditor__

/*
 * $Header: /fuji/lewis/RCS/PixelMapEditor.hh,v 1.3 1992/09/01 15:54:46 sterling Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: PixelMapEditor.hh,v $
 *		Revision 1.3  1992/09/01  15:54:46  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  92/04/09  23:11:23  23:11:23  sterling (Sterling Wight)
 *		CalcDefaultSize
 *		
 *
 */

#include "PixelMap.hh"

#include "Button.hh"
#include "FocusItem.hh"
#include "View.hh"



/*
 * This simple class owns, and displays a pixelmap (similar to PixelMapButton), 
 * and it maps coords to and from Pixels. Overrides trackpress to toggle values of
 * its pixels...
 *
 * This class should be nested under PixelMapEditor.
 *
 */
class	PixEditView : public View {
	public:
		PixEditView ();

		nonvirtual	const	PixelMap&	GetPixelMap () const;
		nonvirtual	void				SetPixelMap (const PixelMap& pixelMap, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	UInt8	GetScale () const;
		nonvirtual	void	SetScale (UInt8 scale, UpdateMode updateMode = eDelayedUpdate);

		/*
		 * Return the rectangle containing the given pixel, in our display. It should
		 * be of width and height = GetScale().
		 */
	public:
		nonvirtual	Rect	GetPixelRect (const Point& whichPixel) const;

		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Draw (const Region& update);

	private:
		PixelMap	fPixelMap;
		UInt8		fScale;

		friend	class	PixelMapTracker;
};

class	PixelMapEditor : public View, public FocusOwner {
	public:

		PixelMapEditor ();
		~PixelMapEditor ();

		nonvirtual	const	PixelMap&	GetPixelMap () const;
		nonvirtual	void				SetPixelMap (const PixelMap& pixelMap);

		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Layout ();

	private:
		nonvirtual	void	ResizePixelMaps ();
		nonvirtual	void	ResetPixelMapDepth ();
		
		PixEditView					fSmallPixelMapView;
		PixEditView					fBigPixelMapView;
		class	PixelMapSizeInfo*	fSizeInfo;
		class	PixelColorInfo*		fColorInfo;
		
		friend class PixelMapSizeInfo;	// calls ResizePixelMaps
		friend class PixelColorInfo;	// calls ResetPixelMapDepth
};





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/* __PixelMapEditor__ */

