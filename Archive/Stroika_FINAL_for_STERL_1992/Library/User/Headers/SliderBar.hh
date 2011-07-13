/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__SliderBar__
#define	__SliderBar__

/*
 * $Header: /fuji/lewis/RCS/SliderBar.hh,v 1.5 1992/09/08 16:00:29 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: SliderBar.hh,v $
 *		Revision 1.5  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:54:46  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/16  15:49:53  sterling
 *		added ticksize to replace pagesize
 *
 *		Revision 1.8  1992/04/30  13:28:48  sterling
 *		support for AbstractBordered
 *
 *		Revision 1.6  1992/03/05  21:46:06  sterling
 *		support for borders
 *
 *		Revision 1.5  1992/02/03  21:53:08  sterling
 *		massive (but trivail) interface changes inherited from Slider
 *
 *		Revision 1.4  1992/01/08  06:58:28  lewis
 *		Sterl- merged his changes - he added #define to configure if we use the picture optimization.
 *
 *
 */

#include	"Picture.hh"
#include	"Slider.hh"

#if		qMacToolkit
#define	qUsePictureOptimization	1
#else
#define	qUsePictureOptimization	0
#endif


class	SliderBar : public Slider {
	public:
		SliderBar (SliderController* sliderController = Nil);
		~SliderBar ();
		
		nonvirtual	Tile	GetFillTile () const;
		virtual		void	SetFillTile (const Tile& newFillTile, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	Real	GetTickSize () const;
		nonvirtual	void	SetTickSize (Real tickSize, UpdateMode update = eDelayedUpdate);
		
		nonvirtual	Real	GetSubTickSize () const;
		nonvirtual	void	SetSubTickSize (Real tickSize, UpdateMode update = eDelayedUpdate);
		
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Draw (const Region& update);

		override	Real	GetMin_ () const;
		override	Real	GetMax_ () const;
		override	void	SetBounds_ (Real newMin, Real newMax, Real newValue, UpdateMode update);
		override	Real	GetValue_ () const;
		override	void	SetValue_ (Real newValue, UpdateMode update);
		
#if		qUsePictureOptimization
		nonvirtual	void	InvalPicture ();
#endif

		virtual		void	DrawSliderBar (Rect barExtent);

	private:
		Real		fMin;
		Real		fMax;
		Real		fValue;
		Real		fTickSize;
		Real		fSubTickSize;
#if		qUsePictureOptimization
		Picture*	fPicture;
#endif
		Tile		fFillTile;
};


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__SliderBar__*/
