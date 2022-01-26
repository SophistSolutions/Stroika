/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Pen__
#define	__Pen__

/*
 * $Header: /fuji/lewis/RCS/Pen.hh,v 1.1 1992/06/19 22:33:01 lewis Exp $
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
 *	$Log: Pen.hh,v $
 *		Revision 1.1  1992/06/19  22:33:01  lewis
 *		Initial revision
 *
 *		Revision 1.6  1992/05/19  09:39:24  sterling
 *		inserter and extractor
 *		,
 *
 *
 *
 *
 */
 
#include	"Brush.hh"


class	Pen : public Brush {
	public:
#if		qDoesntAutoGenXOfXRefRight
		Pen (const Pen& p): Brush (p), fPenSize (p.fPenSize) {}
#endif	/*qDoesntAutoGenXOfXRefRight*/
		Pen (const Tile& tile);
		Pen (const Tile& tile, TransferMode tMode);
		Pen (const Tile& tile, const Point& penSize);
		Pen (const Tile& tile, TransferMode tMode, const Point& penSize);

		nonvirtual	const	Point&	GetPenSize () const;
		nonvirtual	void			SetPenSize (const Point& newPenSize);

	private:
		Point	fPenSize;
};

class ostream&	operator<< (class ostream& out, const Pen& pen);
class istream&	operator>> (class istream& in, Pen& pen);



/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
inline		const	Point&	Pen::GetPenSize () const						{ 	return (fPenSize); 		}
inline		void			Pen::SetPenSize (const Point& newPenSize)		{	fPenSize = newPenSize; 	}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Pen__*/
