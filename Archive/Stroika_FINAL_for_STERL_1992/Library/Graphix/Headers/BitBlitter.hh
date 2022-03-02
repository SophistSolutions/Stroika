/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__BitBlitter__
#define	__BitBlitter__

/*
 * $Header: /fuji/lewis/RCS/BitBlitter.hh,v 1.3 1992/11/25 22:38:06 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 *
 * Changes:
 *	$Log: BitBlitter.hh,v $
 *		Revision 1.3  1992/11/25  22:38:06  lewis
 *		Real->double - real obsolete.
 *
 *		Revision 1.2  1992/07/21  05:34:01  lewis
 *		Lots of workarounds for qCFront_NestedTypesHalfAssed  needed to get to compile under GCC.
 *
 *		Revision 1.1  1992/06/19  22:33:01  lewis
 *		Initial revision
 *
 *		Revision 1.5  1992/04/30  03:25:26  lewis
 *		Add GeneralCase_PortableApply method to pull stuff out of DoPortableApply.
 *
 *		Revision 1.4  92/04/29  12:15:07  12:15:07  lewis (Lewis Pringle)
 *		Added routine to get the inverse table to hide inverse table caching.
 *		Also, added three seperate routines for different kinds of special cases for bitblitter/portable, rather
 *		than having them all inlined in the main portable routine.
 *		
 *		Revision 1.3  92/04/02  11:04:54  11:04:54  lewis (Lewis Pringle)
 *		Added src/dst pixel/color fields to BitBlitter Info, and got rid of Pixel args to TransferFunction -
 *		though we retained the Pixel return value. Added a bunch of static functions to be used internally
 *		in implementing some of the various transfer modes.
 *		
 *		Revision 1.1  1992/03/19  20:36:55  lewis
 *		Initial revision
 *
 *
 *
 */

#include	"PixelMap.hh"


// Not really sure why this is a class??? Just as an organizational unit, for scoping, etc...
class	BitBlitter {
	public:
		BitBlitter ();

		/*
		 * A Transfer function typically takes two pixels, and combines them into one. Sometimes however,
		 * it needs to take into account information about surrounding pixels. Finding the values of Pixels
		 * given a Point offset into the PixelMap is much more expensive than finding successive pixel values
		 * as you go along. So this interface to TransferFunction represnents a compromise. Since we typically
		 * have these values handy anyhow, we pass them along, and in case your transfer function needs
		 *  surrounding information, in can get it from the BitBlitInfo.
		 */
		struct BitBlitInfo {
			BitBlitInfo (const BitBlitter& bitBlitter, const PixelMap& from, const Rect& fromRect,
						 PixelMap& to, const Rect& toRect, const Region* mask);
			const	BitBlitter&	fBitBlitter;
			const	PixelMap&	fSrcPixelMap;
			PixelMap&			fDstPixelMap;
			Rect				fSrcRect;
			Rect				fDstRect;
			const Region*		fMask;
			Point				fSrcAt;
			Point				fDstAt;
#if		qCFront_NestedTypesHalfAssed
			Pixel				fSrcPixel;
			Pixel				fDstPixel;
#else
			PixelMap::Pixel		fSrcPixel;
			PixelMap::Pixel		fDstPixel;
#endif
			Color				fSrcColor;
			Color				fDstColor;
		};
#if		qCFront_NestedTypesHalfAssed
		typedef	Pixel	(*TransferFunction) (const BitBlitInfo& bitBlitInfo);
#else
		typedef	PixelMap::Pixel	(*TransferFunction) (const BitBlitInfo& bitBlitInfo);
#endif


		/*
		 * Some common pre-defined Transfer Functions. Note, that some of these may not be implemented directly
		 * herein, butthe call to Apply may "cheat" and call a GDI system implementation to achieve better
		 * performing results. This first (and prefered interface) is to use that names without implementation
		 * annotation. The implemenation 
		 */
		static	const	TransferFunction		kCopy;
		static	const	TransferFunction		kOr;
		static	const	TransferFunction		kXor;
		static	const	TransferFunction		kBic;
		static	const	TransferFunction		kNotCopy;
		static	const	TransferFunction		kNotOr;
		static	const	TransferFunction		kNotXor;
		static	const	TransferFunction		kNotBic;

		/*
		 * In case you want to use a portable Stroika implementation. (For now, some of these not
		 * implemented).
		 */
		static	const	TransferFunction		kCopy_Portable;
		static	const	TransferFunction		kOr_Portable;
		static	const	TransferFunction		kXor_Portable;
		static	const	TransferFunction		kBic_Portable;
		static	const	TransferFunction		kNotCopy_Portable;
		static	const	TransferFunction		kNotOr_Portable;
		static	const	TransferFunction		kNotXor_Portable;
		static	const	TransferFunction		kNotBic_Portable;

#if		qMacGDI
		/*
		 * In case you want to use a portable Stroika implementation. (For now, some of these not
		 * implemented).
		 */
		static	const	TransferFunction		kCopy_MacGDI;
		static	const	TransferFunction		kOr_MacGDI;
		static	const	TransferFunction		kXor_MacGDI;
		static	const	TransferFunction		kBic_MacGDI;
		static	const	TransferFunction		kNotCopy_MacGDI;
		static	const	TransferFunction		kNotOr_MacGDI;
		static	const	TransferFunction		kNotXor_MacGDI;
		static	const	TransferFunction		kNotBic_MacGDI;
#endif	/*qMacGDI*/

#if		qXGDI
		/*
		 * In case you want to use a portable Stroika implementation. (For now, some of these not
		 * implemented).
		 */
		static	const	TransferFunction		kCopy_XGDI;
		static	const	TransferFunction		kOr_XGDI;
		static	const	TransferFunction		kXor_XGDI;
		static	const	TransferFunction		kBic_XGDI;
		static	const	TransferFunction		kNotCopy_XGDI;
		static	const	TransferFunction		kNotOr_XGDI;
		static	const	TransferFunction		kNotXor_XGDI;
		static	const	TransferFunction		kNotBic_XGDI;
#endif	/*qXGDI*/


		/*
		 * Lots of overloaded ways of saying to apply the given transfer function to copy pixels from the src
		 * pixelmap to the destination pixelmap.
		 *
		 *	Things that can be optionally specified include:
		 *		-	the fromRect rectangle which defaults to the from PixelMaps bounds.
		 *		-	the toRect rectangle which defaults to Rect (toPoint, fromRect.GetBounds ()).
		 *		-	the mask Region which defaults to not masking (ie very large mask).
		 */
		nonvirtual	void	Apply (const PixelMap& from, PixelMap& to, TransferFunction tFunction);
		nonvirtual	void	Apply (const PixelMap& from, PixelMap& to, TransferFunction tFunction,
								   const Region& mask);
		nonvirtual	void	Apply (const PixelMap& from, const Rect& fromRect, PixelMap& to,
								   const Point& toPoint, TransferFunction tFunction);
		nonvirtual	void	Apply (const PixelMap& from, const Rect& fromRect, PixelMap& to,
								   const Point& toPoint, TransferFunction tFunction, const Region& mask);
		nonvirtual	void	Apply (const PixelMap& from, const Rect& fromRect, PixelMap& to,
								    const Rect& toRect, TransferFunction tFunction);
		nonvirtual	void	Apply (const PixelMap& from, const Rect& fromRect, PixelMap& to,
								    const Rect& toRect, TransferFunction tFunction, const Region& mask);


	private:
		/*
		 * At this point we guarentee that fromRect is totally CONTAINED in from.GetBounds (), and simmilarly,
		 * for toRect/to.GetBounds (). Also, clip the rects to be within the mask bounds, if any.
		 * This guy does the real work.
		 */
		nonvirtual	void	DoApply (const PixelMap& from, const Rect& fromRect, PixelMap& to, const Rect& toRect,
									 TransferFunction tFunction, double scaleV, double scaleH, const Region* mask);
#if		qMacGDI
		nonvirtual	void	DoMacGDIApply (const PixelMap& from, const Rect& fromRect, PixelMap& to, const Rect& toRect,
									 	   int tFunction, double scaleV, double scaleH, const Region* mask);
#endif	/*qMacGDI*/
#if		qXGDI
		nonvirtual	void	DoXGDIApply (const PixelMap& from, const Rect& fromRect, PixelMap& to, const Rect& toRect,
									 	 int tFunction, double scaleV, double scaleH, const Region* mask);
#endif	/*qXGDI*/
		nonvirtual	void	DoPortableApply (const PixelMap& from, const Rect& fromRect, PixelMap& to, const Rect& toRect,
									 	     TransferFunction tFunction, double scaleV, double scaleH, const Region* mask);


#if		qCFront_NestedTypesHalfAssed
		static	Pixel	sCopyFunction (const BitBlitInfo& bitBlitInfo);
		static	Pixel	sOrFunction (const BitBlitInfo& bitBlitInfo);
		static	Pixel	sXOrFunction (const BitBlitInfo& bitBlitInfo);
		static	Pixel	sBicFunction (const BitBlitInfo& bitBlitInfo);
		static	Pixel	sNotCopyFunction (const BitBlitInfo& bitBlitInfo);
		static	Pixel	sNotOrFunction (const BitBlitInfo& bitBlitInfo);
		static	Pixel	sNotXOrFunction (const BitBlitInfo& bitBlitInfo);
		static	Pixel	sNotBicFunction (const BitBlitInfo& bitBlitInfo);
#else
		static	PixelMap::Pixel	sCopyFunction (const BitBlitInfo& bitBlitInfo);
		static	PixelMap::Pixel	sOrFunction (const BitBlitInfo& bitBlitInfo);
		static	PixelMap::Pixel	sXOrFunction (const BitBlitInfo& bitBlitInfo);
		static	PixelMap::Pixel	sBicFunction (const BitBlitInfo& bitBlitInfo);
		static	PixelMap::Pixel	sNotCopyFunction (const BitBlitInfo& bitBlitInfo);
		static	PixelMap::Pixel	sNotOrFunction (const BitBlitInfo& bitBlitInfo);
		static	PixelMap::Pixel	sNotXOrFunction (const BitBlitInfo& bitBlitInfo);
		static	PixelMap::Pixel	sNotBicFunction (const BitBlitInfo& bitBlitInfo);
#endif



		/*
		 * Some special cases where we can optimize the bitblit.
		 */
		nonvirtual	void	SpecialBitBlit_Copy_RectsSame_DepthSame_BoundsSame_ClutSame (const PixelMap& from,
								PixelMap& to);
		nonvirtual	void	SpecialBitBlit_Copy_PixelValuesSameBoundsMayDiffer (const PixelMap& from, PixelMap& to,
								const Rect& copyToRect, double scaleV, double scaleH);
		nonvirtual	void	SpecialBitBlit_Copy_WithCLUT (const PixelMap& from, PixelMap& to,
								const Rect& copyToRect, double scaleV, double scaleH);


		nonvirtual	void	GeneralCase_PortableApply (const PixelMap& from, const Rect& fromRect, PixelMap& to,
								const Rect& toRect,	TransferFunction tFunction, double scaleV, double scaleH,
								const Region* mask);


		static	const	InverseTable&	GetInverseTable (const ColorLookupTable& clut);
};




/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__BitBlitter__*/
