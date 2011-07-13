/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Pallet.cc,v 1.4 1992/09/05 16:14:25 lewis Exp $
 *
 *
 * TODO:
 *
 * Changes:
 *	$Log: Pallet.cc,v $
 *		Revision 1.4  1992/09/05  16:14:25  lewis
 *		Renamed NULL->Nil.
 *
 *		Revision 1.3  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/06/25  04:17:51  sterling
 *		Added PalletManager::MakeColorFromTile () method.
 *
 *		Revision 1.1  1992/06/19  22:34:01  lewis
 *		Initial revision
 *
 *		Revision 1.12  1992/06/09  14:41:29  sterling
 *		changed LRU algorithm to use linked list
 *
 *		Revision 1.10  92/04/30  17:09:49  17:09:49  lewis (Lewis Pringle)
 *		Use LRU code from InverseTable stuff in BitBlitter::GetInverseTable() in MakeTileFromColor().
 *		Had to fix now since existing code broke HP compiler - trouble with array of guys with CTOR, etc
 *		staticly scoped in funciton..
 *
 *		Revision 1.7  92/04/27  18:37:51  18:37:51  lewis (Lewis Pringle)
 *		Implemented MakeTileFromColor() - not great implemention, and untested, but should be adequate.
 *		
 *		Revision 1.5  1992/02/10  16:24:10  lewis
 *		Renamed from Palette to Pallet (as per sterls suggestion), and made palletmanager class.
 *		Starting to think about making this stuff real.
 *
 */

#if			qMacGDI
#include	"OSRenamePre.hh"
#include	<QuickDraw.h>
#include	"OSRenamePost.hh"
#endif	/* qMacGDI */

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"GDIConfiguration.hh"

#include	"Pallet.hh"






/*
 ********************************************************************************
 ************************************ PalletManager *****************************
 ********************************************************************************
 */

PalletManager*	PalletManager::sThe		=	Nil;

PalletManager::PalletManager ()
{
}

PalletManager::~PalletManager ()
{
	if (sThe == this) {
		sThe = Nil;
	}
}


// this should be moved and templated
#include	"BlockAllocated.hh"
struct	CTLink {
	CTLink (Color c, const Tile& t);

	nonvirtual	void*	operator new (size_t n);
	nonvirtual	void	operator delete (void* p);
	
	Color	fColor;
	Tile	fTile;
	CTLink*	fNext;
};

#if		!qRealTemplatesAvailable
BlockAllocatedDeclare (CTLink);
BlockAllocatedImplement (CTLink);
#endif	/*!qRealTemplatesAvailable*/

CTLink::CTLink (Color c, const Tile& t) :
	fColor (c),
	fTile (t),
	fNext (Nil)
{
}

void*	CTLink::operator new (size_t n)
{
#if		qRealTemplatesAvailable
	return (BlockAllocated<CTLink>::operator new (n));
#else	/*qRealTemplatesAvailable*/
	return (BlockAllocated(CTLink)::operator new (n));
#endif	/*qRealTemplatesAvailable*/
}

void	CTLink::operator delete (void* p)
{
#if		qRealTemplatesAvailable
	BlockAllocated<CTLink>::operator delete (p);
#else	/*qRealTemplatesAvailable*/
	BlockAllocated(CTLink)::operator delete (p);
#endif	/*qRealTemplatesAvailable*/
}

enum { eCacheSize = 256 };
static	CTLink*	sFirst = Nil;
static	int	sCacheLength = 0;

Tile	PalletManager::MakeTileFromColor (const Color& c)
{
#if		qMacGDI
	if ((not GDIConfiguration ().ColorQDAvailable ()) /* or max depth == 1*/) {
		const	Tile	kLightGrayTile	=	Tile (PixelMap (Rect (kZeroPoint, Point (8, 8)), &qd.ltGray, 1));
		const	Tile	kGrayTile		=	Tile (PixelMap (Rect (kZeroPoint, Point (8, 8)), &qd.gray, 1));
		const	Tile	kDarkGrayTile	=	Tile (PixelMap (Rect (kZeroPoint, Point (8, 8)), &qd.dkGray, 1));

		// match to black and white patterns
		if (c == kLightGrayColor) {
			return (kLightGrayTile);
		}
		else if (c == kGrayColor) {
			return (kGrayTile);
		}
		else if (c == kDarkGrayColor) {
			return (kDarkGrayTile);
		}
	}
#endif	

	/*
	 * LRU algorithm. We keep a linked list for our cache.
	 */
	CTLink* current = sFirst;
	CTLink*	previous = Nil;
	CTLink*	twoBack = Nil;
	while (current) {
		if (current->fColor == c) {
			if (current != sFirst) {
				// make first item
				if (previous != Nil) {
					previous->fNext = current->fNext;
					current->fNext = sFirst;
					sFirst = current;
				}
			}
			goto Done;
		}
		twoBack = previous;
		previous = current;
		current = current->fNext;
	}

	{
	/* Drock! not currently in the cache */

	static	PixelMap*	kBlackPixelMap	=	Nil;
	if (kBlackPixelMap == Nil) {
		kBlackPixelMap = new PixelMap (Rect (kZeroPoint, Point (16, 16)));
		for (int row = 0; row < 16; row++) {
			for (int col = 0; col < 16; col++) {
				kBlackPixelMap->SetPixel (Point (row, col), 1);
			}
		}
	}

	/*
	 * Copy the black pixelmap, and replace its clut (which should have been kBlackAndWhiteCLUT with one with 2 entries - white
	 * first for the zeros in the pixelmap, and our new color C for the 1 bits set in our pixelmap. Then, build a tile from
	 * that.
	 */
	PixelMap	result	(*kBlackPixelMap);

	ColorLookupTable	clut	=	result.GetColorLookupTable ();
	Assert (clut.GetEntryCount () == 2);
	Assert (clut [1] == kWhiteColor);
	clut.SetEntry (2, c);
	result.SetColorLookupTable (clut);

	Tile t = Tile (result);
	if (sCacheLength < eCacheSize) {
		current = new CTLink (c, t);
		current->fNext = sFirst;
		sFirst = current;
		sCacheLength++;
	}
	else {
		AssertNotNil (previous);
		if (twoBack != Nil) {
			twoBack->fNext = Nil;
		}
		previous->fColor = c;
		previous->fTile = t;
		previous->fNext = sFirst;
		sFirst = previous;
	}
	}

Done:
	EnsureNotNil (sFirst);
	Ensure (sFirst->fColor == c);

	return (sFirst->fTile);
}

Color	PalletManager::MakeColorFromTile (const Tile& t)
{
	const	PixelMap& p = t.PeekAtPixmap ();
	CTLink* current = sFirst;
	CTLink*	previous = Nil;
	CTLink*	twoBack = Nil;
	while (current) {
		const	PixelMap& p1 = current->fTile.PeekAtPixmap ();
		if (&p1 == &p) {
			if (current != sFirst) {
				// make first item
				if (previous != Nil) {
					previous->fNext = current->fNext;
					current->fNext = sFirst;
					sFirst = current;
				}
			}
			return (sFirst->fColor);
		}
		twoBack = previous;
		previous = current;
		current = current->fNext;
	}

	// should compute color through weighted average
	ColorLookupTable	clut	=	t.PeekAtPixmap ().GetColorLookupTable ();
	Assert (clut.GetEntryCount () >= 2);
	Assert (clut [1] == kWhiteColor);
	Color c = clut [2];

	if (sCacheLength < eCacheSize) {
		current = new CTLink (c, t);
		current->fNext = sFirst;
		sFirst = current;
		sCacheLength++;
	}
	else {
		AssertNotNil (previous);
		if (twoBack != Nil) {
			twoBack->fNext = Nil;
		}
		previous->fColor = c;
		previous->fTile = t;
		previous->fNext = sFirst;
		sFirst = previous;
	}

	return (c);
}


PalletManager&	PalletManager::Get ()
{
	if (sThe == Nil) {
		SetThe (new PalletManager ());
	}
	EnsureNotNil (sThe);
	return (*sThe);
}

void	PalletManager::SetThe (PalletManager* newPalletManager)
{
	Require (sThe != newPalletManager or newPalletManager == Nil);	// careful of (Set (Get()) should not be done!!! or at least shouldn't crash
	delete (sThe); Assert (sThe == Nil);
	sThe = newPalletManager;		// can be nil!!!
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

