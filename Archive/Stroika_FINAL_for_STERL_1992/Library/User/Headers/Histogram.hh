/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Histogram__
#define	__Histogram__

/*
 * $Header: /fuji/lewis/RCS/Histogram.hh,v 1.3 1992/09/01 15:54:46 sterling Exp $
 *
 * Description:
 *		Simple histogram classes.
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Histogram.hh,v $
 *		Revision 1.3  1992/09/01  15:54:46  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/04/15  13:14:52  sterling
 *		graph support
 *
Revision 1.1  92/04/10  00:32:48  00:32:48  lewis (Lewis Pringle)
Initial revision

 *
 */

#include	"View.hh"

#include	"Axis.hh"


class	Histogram : public View {
	public:
		Histogram ();

	public:
		nonvirtual	CollectionSize	GetBars () const;
		nonvirtual	void			SetBars (CollectionSize bars, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	Tile			GetBarTile (CollectionSize bar) const;
		nonvirtual	void			SetBarTile (CollectionSize bar, const Tile& tile, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	Real			GetBarValue (CollectionSize bar) const;
		nonvirtual	void			SetBarValue (CollectionSize bar, Real value);

		nonvirtual	Real			GetTotalValue () const;
		nonvirtual	Real			GetPercentage (CollectionSize bar) const;

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Draw (const Region& update);

	private:
		CollectionSize	fBars;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/* __Histogram__ */

