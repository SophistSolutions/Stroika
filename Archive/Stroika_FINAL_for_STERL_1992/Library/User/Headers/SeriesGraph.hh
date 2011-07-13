/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__SeriesGraph__
#define	__SeriesGraph__

/*
 * $Header: /fuji/lewis/RCS/SeriesGraph.hh,v 1.6 1992/09/08 16:00:29 lewis Exp $
 *
 * Description:
 *		
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: SeriesGraph.hh,v $
 *		Revision 1.6  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:54:46  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  21:02:57  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.3  1992/07/03  02:24:16  lewis
 *		Conditional add macro based Containers for Real.
 *
 *		Revision 1.2  1992/07/02  05:09:34  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.1  1992/06/20  17:33:49  lewis
 *		Initial revision
 *
 *		Revision 1.1  1992/04/15  13:15:00  sterling
 *		Initial revision
 *
 *		
 *
 */

#include	"TextView.hh"
#include	"View.hh"

#include	"Axis.hh"


class	GraphSeries : public Sequence_Array(Real) {
	public:
		GraphSeries (const Tile& tile, const String& title);
		
		nonvirtual	Tile	GetTile () const;
		nonvirtual	void	SetTile (const Tile& tile);
	
		nonvirtual	String	GetTitle () const;
		nonvirtual	void	SetTitle (const String& title);
	
	private:
		Tile	fTile;
		String	fTitle;
};


#if		!qRealTemplatesAvailable
	typedef GraphSeries* GrSerPtr;
	Declare (Iterator, GrSerPtr);
	Declare (Collection, GrSerPtr);
	Declare (AbSequence, GrSerPtr);
	Declare (Array, GrSerPtr);
	Declare (Sequence_Array, GrSerPtr);
	Declare (Sequence, GrSerPtr);
	#define	GraphSeriesPtr	GrSerPtr
#endif


class	SeriesGraph : public View {
	protected:
		SeriesGraph ();
	
	public:
		nonvirtual	Axis&	GetXAxis () const;
		nonvirtual	Axis&	GetYAxis () const;
		
		nonvirtual	SequenceIterator(GraphSeriesPtr)*	MakeSeriesIterator (SequenceDirection d = eSequenceForward) const;
		
		nonvirtual	String	GetTitle () const;
		nonvirtual	void	SetTitle (const String& title, UpdateMode update = eDelayedUpdate);
		nonvirtual	Boolean	GetTitleShown () const;
		nonvirtual	void	SetTitleShown (Boolean shown, UpdateMode update = eDelayedUpdate);
		
		nonvirtual	Boolean	GetLegendShown () const;
		nonvirtual	void	SetLegendShown (Boolean shown, UpdateMode update = eDelayedUpdate);

	protected:
		virtual	Axis&	GetXAxis_ () const = Nil;
		virtual	Axis&	GetYAxis_ () const = Nil;
		
		virtual	SequenceIterator(GraphSeriesPtr)*	MakeSeriesIterator_ (SequenceDirection d) const	= Nil;

		virtual	String	GetTitle_ () const									= Nil;
		virtual	void	SetTitle_ (const String& title, UpdateMode update)	= Nil;
		virtual	Boolean	GetTitleShown_ () const								= Nil;
		virtual	void	SetTitleShown_ (Boolean shown, UpdateMode update)	= Nil;
		
		virtual	Boolean	GetLegendShown_ () const							= Nil;
		virtual	void	SetLegendShown_ (Boolean shown, UpdateMode update)	= Nil;
};


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/* __SeriesGraph__ */
