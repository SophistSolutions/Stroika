/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Printing__
#define	__Printing__

/*
 * $Header: /fuji/lewis/RCS/Printing.hh,v 1.4 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *			PrintException should be scoped, and much of this should be moved to the
 *		graphix layer.
 *			Also, print preview window should have one main view, and have the scroller
 *		and buttons be subviews of that instead of doing magic to make them subviews of the
 *		window.
 *
 * Changes:
 *	$Log: Printing.hh,v $
 *		Revision 1.4  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/01/14  06:04:20  lewis
 *		Make Printer be a TabletOwner.
 *
 *		Revision 1.5  1991/12/12  02:59:29  lewis
 *		Got rid of dependency on scroller window and noted cleanups to be done.
 *
 *
 *
 */

#include	"Exception.hh"
#include	"File.hh"

#include	"Picture.hh"
#include	"Tablet.hh"

#include	"View.hh"



typedef	Coordinate	PageNumber;


class	Printable {
	public:
		Printable ();
		virtual	void		PrintPage (PageNumber pageNumber, class Printer& printer)	= Nil;
		virtual	void		CalcPages (PageNumber& userStart, PageNumber& userEnd, const Rect& pageRect)	= Nil;
		virtual	PageNumber	CalcAllPages (const Rect& pageRect) = Nil;
};

#if		qMacGDI
struct	osPrintRecord;
#endif

class	Printer : public Panel, public TabletOwner {
	public:
		Printer	();
		Printer	(Printer& printer);
#if		qMacGDI
		Printer (osPrintRecord** printRecord);
#endif
		~Printer ();

		override	Tablet*	GetTablet () const;
		override	Panel*	GetParentPanel () const;
		
		virtual		void	DoPageSetup ();
		virtual		void	Print (Printable& printable, Boolean queryUser = True);
		
		nonvirtual	void	DrawView (View& v, const Point& at);	

#if 	qMacGDI		
		nonvirtual	osPrintRecord** GetPrintRecord () const;
		virtual		void			SetPrintRecord (osPrintRecord** printRecord);
#endif

		nonvirtual	PageNumber	GetFirstPage () const;
		nonvirtual	PageNumber	GetLastPage () const;
		nonvirtual	Rect		GetPageRect () const;

	protected:
		virtual	Boolean		QueryUser ();
		virtual	PageNumber	SetUpPrintRecord (Printable& printable, Boolean queryUser);
		virtual	void		Print (Printable& printable, PageNumber copies);	// assumes valid print record

	protected:
		override	Point	LocalToTablet_ (const Point& p)	const;
		override	Point	TabletToLocal_ (const Point& p)	const;

	private:
		nonvirtual	void		InitializePrintRecord ();
		
		class	PrintTablet*	fTablet;
#if		qMacGDI
		osPrintRecord**			fPrintRecord;
#endif
		PageNumber				fFirstPage;
		PageNumber				fLastPage;
		Rect					fPageRect;
		Boolean					fPrinting;
};


class	ScreenPrinter : public Printer {
	public:
		ScreenPrinter (Printer& printer);
		
		override	Tablet*	GetTablet () const;
		virtual		void	Preview (Printable& printable, Boolean queryUser = True);

	private:
		Tablet*					fCurrentTablet;
		Sequence_Array(Picture)	fPictures;
};


class ostream&	operator<< (class ostream& out, const Printer& s);
class istream&	operator>> (class istream& in, Printer& s);

class	PrintException : public Exception {
	public:
		PrintException ();
		PrintException (const PrintException& isAnException);

		override	void	Raise ();

		nonvirtual	void	Raise (Int16 osErr);

		nonvirtual	short	GetErrNumber () const;

	private:
		Int16	fErr;
};

extern	PrintException	gPrintException;


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Printing__*/

