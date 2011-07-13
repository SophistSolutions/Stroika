/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Printing.cc,v 1.3 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * PORTING NOTES:
 *
 *		Consider using our own define for iPrAbort since that usage for the error number makes
 *		much code non-portable needelessly. Make IsAbort () a method of PrintExpection...
 *
 * Changes:
 *	$Log: Printing.cc,v $
 *		Revision 1.3  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.15  1992/05/14  15:15:30  lewis
 *		Work around compiler bug in workaround for mac compiler bug. Ya gotta love it.
 *
 *		Revision 1.14  92/05/09  00:08:55  00:08:55  lewis (Lewis Pringle)
 *		Bobs changes for mac printing code - Delete print tablet before PrClose () to
 *		avoid nasty crash. And, in ScreenPrinter::Preview() wrap call to print, and
 *		destroy stackbased objects.
 *		
 *		Revision 1.13  92/02/11  01:17:14  01:17:14  lewis (Lewis Pringle)
 *		Comment out more illicit window usage code in Preview window. Must rewrite that code to be friendly with new
 *		Window/Shell stuff.
 *		
 *		Revision 1.11  1992/01/14  06:04:43  lewis
 *		Made Printer be a TabletOwner.
 *
 *		Revision 1.9  1992/01/06  05:09:50  lewis
 *		Disabled use of PrintPreview so we could get things too compile.
 *		It did not properly use the MainView stuff.
 *
 *
 *
 */



#include	"OSRenamePre.hh"
#if		qMacOS
#include	<Memory.h>
#include	<Printing.h>
#include	<Resources.h>
#include	<Traps.h>
#endif	/*qMacOS*/
#include	"OSRenamePost.hh"


#include	"Debug.hh"
#include	"Exception.hh"
#include	"Memory.hh"
#include	"StreamUtils.hh"

#include	"Application.hh"
#include	"Button.hh"
#include	"DeskTop.hh"
#include	"Printing.hh"
#include	"PushButton.hh"
#include	"Scroller.hh"
#include	"Window.hh"






class	PrintTablet : public Tablet {
	public:
#if		qMacGDI		
		PrintTablet (struct osPrintRecord** printInfo);
#endif
		~PrintTablet ();

#if		qMacGDI		
		nonvirtual	osTPrPort*	GetOSPrintGrafPtr () const;
#endif
};






/*
 ********************************************************************************
 ************************************* Printable ********************************
 ********************************************************************************
 */
Printable::Printable ()
{
}





/*
 ********************************************************************************
 ********************************** PrintException ******************************
 ********************************************************************************
 */
PrintException			gPrintException;

#if		!qMacGDI
#define	noErr 0
#endif

PrintException::PrintException ():
	Exception (),
	fErr (noErr)
{
}

PrintException::PrintException (const PrintException& isAnException):
	Exception (isAnException),
	fErr (noErr)
{
}

void	PrintException::Raise ()
{
	if (Exception::GetCurrent () != this) {		// if were not the current exception return something
												// indicating unknown error, otherwise preserve our
												// error #
		fErr = noErr;
	}
	Exception::Raise ();
}

void	PrintException::Raise (Int16 osErr)
{
	fErr = osErr;
#if 	qMacGDI		
	::PrSetError (noErr);
#endif
	Exception::Raise ();
}

short	PrintException::GetErrNumber () const
{
	return (fErr);
}







/*
 ********************************************************************************
 ********************************* PrintTablet **********************************
 ********************************************************************************
 */
#if 	qMacGDI		
PrintTablet::PrintTablet (struct osPrintRecord** printInfo):
	Tablet ((osGrafPort *) ::PrOpenDoc (printInfo, Nil, Nil))
{
	if (::PrError () != noErr) {
		gPrintException.Raise (::PrError ()); 
	}
}

PrintTablet::~PrintTablet ()
{
	::PrCloseDoc (GetOSPrintGrafPtr ());
	if (::PrError () != noErr) {
		gPrintException.Raise (::PrError ()); 
	}
}

osTPrPort*	PrintTablet::GetOSPrintGrafPtr () const
{
	return ((osTPrPort*) (GetOSGrafPtr ()));
}
#endif	/*qMacGDI */










/*
 ********************************************************************************
 ********************************* Printer **************************************
 ********************************************************************************
 */
Printer::Printer ():
	fTablet (Nil),
#if		qMacGDI
	fPrintRecord (Nil),
#endif
	fFirstPage (0),
	fLastPage (0),
	fPageRect (kZeroRect)
{
#if		qMacGDI
	InitializePrintRecord ();

	AssertNotNil (fPrintRecord);
	::PrOpen ();
	::PrintDefault (fPrintRecord);
	::PrClose ();
#endif
}

Printer::Printer (Printer& printer):
	fTablet (Nil),
#if		qMacGDI
	fPrintRecord (printer.GetPrintRecord ()),
#endif
	fFirstPage (printer.fFirstPage),
	fLastPage (printer.fLastPage),
	fPageRect (printer.fPageRect)
{
#if		qMacGDI
	InitializePrintRecord ();

	AssertNotNil (fPrintRecord);
	::PrOpen ();
	::PrValidate (fPrintRecord);
	::PrClose ();
#endif
}

#if		qMacGDI		
Printer::Printer (osPrintRecord** printRecord):
	fTablet (Nil),
	fPrintRecord (printRecord),
	fFirstPage (0),
	fLastPage (0),
	fPageRect (kZeroRect)
{
	InitializePrintRecord ();

	AssertNotNil (fPrintRecord);
	::PrOpen ();
	::PrValidate (fPrintRecord);
	::PrClose ();
}
#endif	/*qMacGDI*/

Printer::~Printer ()
{
#if		qMacGDI
	if (fPrintRecord != Nil) {
		::DisposHandle (osHandle (fPrintRecord));
	}
#endif	/*qMacGDI*/
}

Tablet*	Printer::GetTablet () const
{
	return (fTablet);
}

Panel*	Printer::GetParentPanel () const
{
	return (Nil);
}

#if		qMacGDI		
osPrintRecord** Printer::GetPrintRecord () const
{
	return (fPrintRecord);
}

void	Printer::SetPrintRecord (osPrintRecord** printRecord)
{
	if (fPrintRecord != Nil) {
		::DisposHandle (osHandle (fPrintRecord));
	}
	fPrintRecord = printRecord;
	if (fPrintRecord != Nil) {
		OSErr	err = ::HandToHand ((osHandle*)&fPrintRecord);
		if (err != noErr) {
			fPrintRecord = Nil;
			gMemoryException.Raise (err);
		}
	}
}
#endif	/*qMacGDI*/

PageNumber	Printer::GetFirstPage () const
{
	return (fFirstPage);
}

PageNumber	Printer::GetLastPage () const
{
	return (fLastPage);
}

Rect	Printer::GetPageRect () const
{
	return (fPageRect);
}

void	Printer::DoPageSetup ()
{
#if		qMacGDI
	RequireNotNil (fPrintRecord);
	::PrOpen ();
	(void) ::PrStlDialog (fPrintRecord);
	::PrClose ();
	if (::PrError () != noErr) {
		gPrintException.Raise (::PrError ()); 
	}
#endif	/*qMacGDI*/
}

Boolean	Printer::QueryUser ()
{
#if		qMacGDI
	::PrOpen ();
	Boolean	result =  Boolean (::PrJobDialog (fPrintRecord));
	::PrClose ();
	if (::PrError () != noErr) {
		gPrintException.Raise (::PrError ()); 
	}
	return (result);
#endif	/*qMacGDI*/
}

PageNumber	Printer::SetUpPrintRecord (Printable& printable, Boolean queryUser)
{
#if		qMacGDI
	if ((queryUser) and (not QueryUser ())) {
		gPrintException.Raise (iPrAbort); 
	}

	fPageRect = os_cvt ((*fPrintRecord)->prInfo.rPage);
	if (queryUser) {
		fFirstPage = (*fPrintRecord)->prJob.iFstPage;
		fLastPage = (*fPrintRecord)->prJob.iLstPage;
		printable.CalcPages (fFirstPage, fLastPage, fPageRect);
	}
	else {
		fFirstPage = 1;
		fLastPage = printable.CalcAllPages (fPageRect);
	}
	(*fPrintRecord)->prJob.iFstPage = 1;
	(*fPrintRecord)->prJob.iLstPage = 9999;

	PageNumber	copies = ((*fPrintRecord)->prJob.bJDocLoop == bSpoolLoop) 
		? 1
		: (*fPrintRecord)->prJob.iCopies;
	return (copies);
#endif	/*qMacGDI*/
}

void	Printer::Print (Printable& printable, PageNumber copies)
{
#if		qMacGDI
	AssertNotNil (fPrintRecord);
	Boolean	pageIsOpen = False;
	
	if (::PrError () != noErr) {
		gPrintException.Raise (::PrError ()); 
	}
	::PrOpen ();
	if (::PrError () != noErr) {
		gPrintException.Raise (::PrError ()); 
	}
	Int16	oldResFile = ::CurResFile ();
	
	Try {
		Int16	printMgrsResFile = ::CurResFile ();
		for (; copies > 0; copies--) {
#if 0
// should support an idle task dialog
(*fPrintRecord)->prJob.pIdleProc = someCIdleTaskFunction;
#endif
			UseResFile (printMgrsResFile);
	
			Assert (fTablet == Nil);
			fTablet = new PrintTablet (fPrintRecord);
			EnsureNotNil (fTablet);
			for (PageNumber current = fFirstPage; current <= fLastPage; current++) {
				::PrOpenPage (fTablet->GetOSPrintGrafPtr (), Nil);
				pageIsOpen = True;
				if (::PrError () != noErr) {
					gPrintException.Raise (::PrError ()); 
				}
				printable.PrintPage (current, *this);
				::PrClosePage (fTablet->GetOSPrintGrafPtr ());
				pageIsOpen = False;
				if (::PrError () != noErr) {
					gPrintException.Raise (::PrError ()); 
				}
			}
			AssertNotNil (fTablet);
			delete fTablet;
			fTablet = Nil;
		}

		if ((*fPrintRecord)->prJob.bJDocLoop == bSpoolLoop) {
			TPrStatus	theStatus;
			// Could use theStatus for print spooling status dialog - probably
			// should make available, and do default dialog....
			::PrPicFile (fPrintRecord, Nil, Nil, Nil, &theStatus);
		}
		if (::PrError () != noErr) {
			gPrintException.Raise (::PrError ()); 
		}		
	}
	Catch () {
		if (pageIsOpen) {
			::PrClosePage (fTablet->GetOSPrintGrafPtr ());
		}
		if (fTablet != Nil) {
			delete fTablet;
			fTablet = Nil;
		}
		::PrClose ();
		UseResFile (oldResFile);
		if ((not _this_catch_->Match (gPrintException)) or 
			(gPrintException.GetErrNumber () != iPrAbort)) {
			
			_this_catch_->Raise ();		// propogate exception
		}
	}

	::PrClose ();
	UseResFile (oldResFile);
#endif	/*qMacGDI*/
}

void	Printer::Print (Printable& printable, Boolean queryUser)
{
#if		qMacGDI
	Try {		
		PageNumber	copies = SetUpPrintRecord (printable, queryUser);
		Print (printable, copies);
	}
	Catch1 (gPrintException) {
		if (gPrintException.GetErrNumber () == iPrAbort) {
			return;
		}
		else {
			_this_catch_->Raise ();		// propogate exception
		}
	}
#endif	/*qMacGDI*/
}

void	Printer::DrawView (View& v, const Point& at)
{
	Tablet*	oldTablet = v.GetTablet ();
	Point	oldOrigin = v.GetOrigin ();
	Boolean	oldVisible = v.GetVisible ();
	View*	oldParent = v.GetParentView ();
	CollectionSize	oldIndex = 0;
	if (oldParent != Nil) {
		oldIndex = oldParent->GetSubViewIndex (&v);
		oldParent->RemoveSubView (&v);
	}
	
	v.View::SetOrigin (oldOrigin + at - v.LocalToTablet (kZeroPoint), View::eNoUpdate);
	v.SetTablet (GetTablet ());
	v.View::SetVisible (True, View::eNoUpdate);
	v.ProcessLayout ();
	v.Render (v.GetVisibleArea ());
	v.SetTablet (oldTablet);
	if (oldParent != Nil) {
		oldParent->AddSubView (&v, oldIndex);
	}
	v.View::SetOrigin (oldOrigin, View::eNoUpdate);
	v.View::SetVisible (oldVisible, View::eNoUpdate);
}

void	Printer::InitializePrintRecord ()
{
#if		qMacGDI
	if (fPrintRecord == Nil) {
		fPrintRecord	=	(osPrintRecord**) ::NewHandle (sizeof (osPrintRecord));
		if (MemError () != noErr) {
			fPrintRecord = Nil;
			gMemoryException.Raise (MemError ());
		}
	}
	else {
		OSErr	err = ::HandToHand ((osHandle*)&fPrintRecord);
		if (err != noErr) {
			fPrintRecord = Nil;
			gMemoryException.Raise (err);
		}
	}
	EnsureNotNil (fPrintRecord);
#endif	/*qMacGDI*/
}

Point	Printer::LocalToTablet_ (const Point& p)	const
{
	return (p);
}

Point	Printer::TabletToLocal_ (const Point& p)	const
{
	return (p);
}









/*
 ********************************************************************************
 ********************************* iostream support *****************************
 ********************************************************************************
 */
class ostream&	operator<< (class ostream& out, const Printer& s)
{
AssertNotReached ();	// NYI
return (out);
}

class istream&	operator>> (class istream& in, Printer& s)
{
AssertNotReached ();	// NYI
return (in);
}



/*
 ********************************************************************************
 ********************************* ScreenPrinter ********************************
 ********************************************************************************
 */
class	PicturesPrintable : public Printable {
	public:
		PicturesPrintable (const AbSequence(Picture)& pictures) :
			fPictures (pictures)
		{
		}
		
		override	void		PrintPage (PageNumber pageNumber, class Printer& printer)
		{
			Assert (pageNumber <= fPictures.GetLength ());
			printer.DrawPicture (fPictures[pageNumber], fPictures[pageNumber].GetFrame ());
		}
		
		override	void		CalcPages (PageNumber& userStart, PageNumber& userEnd, const Rect& pageRect)
		{
			if (userStart > fPictures.GetLength ()) {
				userStart = 1;
			}
			if (userEnd > fPictures.GetLength ()) {
				userEnd = fPictures.GetLength ();
			}
		}
		
		override	PageNumber	CalcAllPages (const Rect& pageRect)
		{
			return (fPictures.GetLength ());
		}
		
	private:
		Sequence(Picture)& fPictures;
};

class	PictureView : public View {
	public:
		PictureView (const Picture& picture) :
			fPicture (picture)
		{
		}
	
		nonvirtual	Picture	GetPicture () const	
		{ 
			return (fPicture); 
		}
		
		nonvirtual	void	SetPicture (const Picture& picture, UpdateMode updateMode = eDelayedUpdate)	
		{
			fPicture = picture; 
			Refresh (updateMode); 
		}
		
		override	void	Draw (const Region& update)
		{
			DrawPicture (fPicture, GetLocalExtent ());
		}
		
	private:
		Picture	fPicture;
};

class	Previewer : public Window, public ButtonController {
	public:
		Previewer (const AbSequence(Picture)& pictures);
		
		override	void	ButtonPressed (AbstractButton* button);

		virtual	void	Pose ();

		nonvirtual	Boolean		GetDismissed () const;
		nonvirtual	Boolean		GetCancelled () const;

	protected:
		override	void	AdjustMainViewExtent ();
		override	void	Layout ();

	private:
		nonvirtual	void	SetUpViews ();
		
		Scroller*				fScroller;
		CollectionSize			fCurrentIndex;
		Sequence(Picture)&  	fPictures;
		AbstractPushButton*		fDismisser;
		AbstractPushButton*		fCancel;
		AbstractPushButton*		fPrint;
		AbstractPushButton*		fPrevious;
		AbstractPushButton*		fNext;
		PictureView*			fPictureView;
};


Previewer::Previewer (const AbSequence(Picture)& pictures) :
//	TitledWindow (*DeskTop::Get ().GetDialogPlane (), Point (300, 400), "Preview",  False, True),
// DID YOU REALLY MEAN - GetDialogPlane - if so, will fix soon...
// anyhow - should fix params...
	Window (),
	fScroller (Nil),
	fPictures (pictures),
	fDismisser (Nil),
	fCurrentIndex (1),
	fPictureView (Nil),
	fCancel (Nil),
	fPrint (Nil),
	fPrevious (Nil),
	fNext (Nil)
{
// LGP Jan 5, 1992 - this stuff is out of date - use a mainview...
AssertNotReached ();

	fScroller = new Scroller ();
	SetMainViewAndTargets (fScroller, fScroller, fScroller);

	Point	centerPoint = CalcRectPoint (DeskTop::Get ().GetMainDeviceDeskBounds (), 0.5, 0.5);
	SetOrigin (centerPoint - GetSize ()/2);
AssertNotImplemented ();
#if 0
	AddSubView (fCancel 	= new PushButton ("Cancel", 	this));
	AddSubView (fPrint 		= new PushButton ("Print", 		this));
	AddSubView (fPrevious 	= new PushButton ("Previous", 	this));
	AddSubView (fNext 		= new PushButton ("Next", 		this));
#endif
	if (fPictures.GetLength () == 0) {
		fDismisser = fCancel;
	}
	fPictureView = new PictureView (fPictures[fCurrentIndex]);
	fScroller->SetScrolledViewAndTargets (fPictureView, Nil, Nil);
	
	SetUpViews ();
}

void	Previewer::AdjustMainViewExtent ()
{
// LGP Jan 5, 1992 - this stuff is out of date - use a mainview...
#if 0
// must fix this code - no more adjust mainview extent ...

	Window::AdjustMainViewExtent ();
	View*	v	=	GetMainView ();
	if (v != Nil) {
		v->SetSize (v->GetSize () - Point (35, 0), eNoUpdate);
	}
#endif
}

void	Previewer::Layout ()
{
// LGP Jan 5, 1992 - this stuff is out of date - use a mainview...
#if 0
	fCancel->SetSize 	(fCancel->CalcDefaultSize 	(kZeroPoint));
	fPrint->SetSize 	(fPrint->CalcDefaultSize 	(kZeroPoint));
	fPrevious->SetSize 	(fPrevious->CalcDefaultSize (kZeroPoint));
	fNext->SetSize 		(fNext->CalcDefaultSize 	(kZeroPoint));
	
	Coordinate	whereV = GetSize ().GetV () - fCancel->GetSize ().GetV () - 10;
	Coordinate	midPointH = GetSize ().GetH () / 2;
	fCancel->SetOrigin 		(Point (whereV, 10));
	fPrevious->SetOrigin 	(Point (whereV, midPointH - fPrevious->GetSize ().GetH () - 5));
	fNext->SetOrigin 		(Point (whereV, midPointH + 5));
	fPrint->SetOrigin 		(Point (whereV, GetSize ().GetH () - fPrint->GetSize ().GetH () - 10));

	Window::Layout ();
#endif
}

void	Previewer::Pose ()
{
	Select ();
Continue:
	Try {
		while (not GetDismissed ()) {
			Application::Get ().RunOnce ();
		}
	}
	Catch () {
		Application::Get ().ShowMessageForException (*_this_catch_);
		goto Continue;
	}
	Close ();
}

Boolean	Previewer::GetDismissed () const
{
	return (Boolean (fDismisser != Nil));
}

Boolean	Previewer::GetCancelled () const
{
	return (Boolean (fDismisser == fCancel));
}

void	Previewer::ButtonPressed (AbstractButton* button)
{
	if (button == fPrint) {
		fDismisser = fPrint;
	}
	else if (button == fCancel) {
		fDismisser = fCancel;
	}
	else if (button == fPrevious) {
		Assert (fCurrentIndex > 1);
		fCurrentIndex--;
		SetUpViews ();
	}
	else if (button == fNext) {
		Assert (fCurrentIndex < fPictures.GetLength ());
		fCurrentIndex++;
		SetUpViews ();
	}
	else {
		AssertNotReached ();
	}
}

void	Previewer::SetUpViews ()
{
	fPrevious->SetEnabled (Boolean (fCurrentIndex > 1));
	fNext->SetEnabled (Boolean (fCurrentIndex < fPictures.GetLength ()));
	fPictureView->SetPicture (fPictures[fCurrentIndex]);
	fPictureView->SetSize (fPictureView->GetPicture ().GetFrame ().GetSize ());
}


ScreenPrinter::ScreenPrinter (Printer& printer) :
	Printer (printer),
	fCurrentTablet (Nil)
{
}

Tablet*	ScreenPrinter::GetTablet () const
{
	if (fCurrentTablet != Nil) {
		return (fCurrentTablet);
	}
	return (Printer::GetTablet ());
}

void	ScreenPrinter::Preview (Printable& printable, Boolean queryUser)
{
#if		qMacGDI
	Try {		
		PageNumber copies = SetUpPrintRecord (printable, queryUser);

		for (PageNumber current = GetFirstPage (); current <= GetLastPage (); current++) {
			PictureTablet	tablet = PictureTablet (GetPageRect ().GetSize ());
			fCurrentTablet = &tablet;
			Try {
				printable.PrintPage (current, *this);
				Picture	p = tablet.GetResult ();
				fPictures.Append (p);
			}
			Catch () {
Picture	p = tablet.GetResult ();	// picture tablet needs to override destructor to ensure a closepicture is called
				tablet.PictureTablet::~PictureTablet ();
				_this_catch_->Raise ();		// propogate exception
			}
			fCurrentTablet = Nil;
		}
		Assert (fPictures.GetLength () == (GetLastPage () - GetFirstPage () + 1));
		Previewer previewer = 	Previewer (fPictures);
		previewer.Pose ();
		if (not previewer.GetCancelled ()) {
			PicturesPrintable	pp = PicturesPrintable (fPictures);
			Try {
				Print ((Printable&)pp, copies);
			}
			Catch () {
// No DTOR and CFront 2.1 crashes MPW on Mac - LGP may 13, 1992							pp.PicturesPrintable::~PicturesPrintable ();
				previewer.Previewer::~Previewer ();
				_this_catch_->Raise ();		// propogate exception
			}
		}
		fPictures.RemoveAll ();
	}
	Catch1 (gPrintException) {
		fPictures.RemoveAll ();
		fCurrentTablet = Nil;
		if (gPrintException.GetErrNumber () == iPrAbort) {
			return;
		}
		else {
			_this_catch_->Raise ();		// propogate exception
		}
	}
	Catch () {
		fPictures.RemoveAll ();
		fCurrentTablet = Nil;
		_this_catch_->Raise ();		// propogate exception
	}
#endif	/*qMacGDI*/
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


