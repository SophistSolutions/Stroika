/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedTestXWindows/Sources/Main.cpp,v 1.25 2004/01/02 05:57:09 lewis Exp $
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
 *	$Log: Main.cpp,v $
 *	Revision 1.25  2004/01/02 05:57:09  lewis
 *	SPR#1613: lose WordProcessorHScrollbarHelper<>
 *	
 *	Revision 1.24  2003/05/09 23:58:30  lewis
 *	upgrade project file to MSVC.net 2003
 *	
 *	Revision 1.23  2003/04/04 16:26:34  lewis
 *	SPR#1407- getting new command processing code working on X-Windows
 *	
 *	Revision 1.22  2002/11/04 01:04:03  lewis
 *	lose fCommandHander arg to UninformHidableTextMarkerOwner
 *	
 *	Revision 1.21  2002/05/06 21:36:21  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 1.20  2001/11/27 00:34:27  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.19  2001/08/30 01:01:27  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.18  2001/05/11 19:31:56  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.17  2001/05/08 21:42:23  lewis
 *	SPR#0890- lots of changes to support (minimally) file IO. Open/SaveAs etc dialogs, and fileIO
 *	code included. VERY primitive (no dirty flag etc, or warnings on destroy user data)
 *	
 *	Revision 1.16  2001/05/05 09:39:11  lewis
 *	SPR#0888- added font style/size/color menus. All working except font size. Also - added
 *	XSetErrorHandler () overide cuz default Gdk one exits without letting me set breakpoint.
 *	By default - I only override if debugging.
 *	
 *	Revision 1.15  2001/05/04 20:44:56  lewis
 *	font name menu (SPR#0888)
 *	
 *	Revision 1.14  2001/05/02 15:28:36  lewis
 *	qUseLedTextWidget so I can build with either gtktext widget or with the Led widget (for
 *	tesing/comparison sake)
 *	
 *	Revision 1.13  2001/05/02 14:55:42  lewis
 *	SPR#0882 - add in scrollbars (v,h). Also - mixin WordProcessorHScrollbarHelper<> to get right
 *	word-processor RHS of scrollbar handling
 *	
 *	Revision 1.12  2001/04/30 19:22:21  lewis
 *	SPR#0885- lots of work to get menu commands working better and to use WordProcessorCommonCommandHelper_Gtk<>
 *	etc...
 *	
 *	Revision 1.11  2001/04/27 15:25:43  lewis
 *	SPR#0885- more work on getting menu commands to work - mostly there - but needs cleanups still
 *	
 *	Revision 1.10  2001/04/25 22:54:53  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.9  2001/04/25 22:54:21  lewis
 *	qSlowXDebugSyncMode support. Lose GetLayoutMargins hack override. Call gtk_set_locale () cuz some sample
 *	code said good idea.
 *	
 *	Revision 1.8  2001/04/23 17:28:04  lewis
 *	get working with WordProcessor include and class used in editor class
 *	
 *	Revision 1.7  2001/04/20 21:49:10  lewis
 *	hack override of GetLayoutMargins ()- based on windowrect for now
 *	
 *	Revision 1.6  2000/09/05 22:37:08  lewis
 *	Moved prelim Led_Gtk template into its own file - Led_Gtk.h
 *	
 *	Revision 1.5  2000/09/05 21:28:08  lewis
 *	got minimal text editing (type chars and they appear) for GTK. Next move this GTK wrapper to its own file!
 *	
 *	Revision 1.4  2000/09/05 14:36:04  lewis
 *	preliminary (half-working) switch-over to using GTK lib
 *	
 *	Revision 1.3  2000/04/16 14:12:30  lewis
 *	small tweeks to get compiling with new namespac and other changes
 *	
 *	Revision 1.2  2000/03/14 00:33:20  lewis
 *	Added primite 'HELLO WORLD' drawing support - can enter text into a TextStore and call Draw ith a TrivialImager
 *	
 *	Revision 1.1  2000/01/15 16:59:26  lewis
 *	first cut X-test app - compiles and displays hellow-word graphix - still not REALLY linknig in any Led stuff
 *	
 *
 *
 *
 */
#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<unistd.h>
#include	<gtk/gtk.h>

#include	"ChunkedArrayTextStore.h"
#include	"Led_Gtk.h"
#include	"Led_Gtk_WordProcessor.h"
#include	"StyledTextIO_HTML.h"
#include	"StyledTextIO_LedNative.h"
#include	"StyledTextIO_PlainText.h"
#include	"StyledTextIO_RTF.h"
#include	"TextInteractor.h"
#include	"WordProcessor.h"
#include	"WordWrappedTextImager.h"
#include	"WordWrappedTextInteractor.h"



#if		qLedUsesNamespaces
using	namespace	Led;
#endif



#define	qSlowXDebugSyncMode	0
//#define	qSlowXDebugSyncMode	qDebug




// define this false to use the gtktext.c widget (for testing/comparison purposes)
#ifndef	qUseLedTextWidget
#define	qUseLedTextWidget	1
#endif

#ifndef	qUseMyXErrorHandlers
#define	qUseMyXErrorHandlers	qDebug
#endif

#if		qUseMyXErrorHandlers
static int	MyXErrorHandler (Display* display, XErrorEvent* error)
{
	if (error->error_code) {
		if (gdk_error_warnings) {
			char buf[64];
			XGetErrorText (display, error->error_code, buf, 63);
			g_error ("%s\n  serial %ld error_code %d request_code %d minor_code %d\n", buf,	error->serial, error->error_code, error->request_code, error->minor_code);
		}
		gdk_error_code = error->error_code;
	}
	return 0;
}
#endif






enum	FileFormat {
	eTextFormat,
	eLedPrivateFormat, 
	eRTFFormat, 
	eHTMLFormat, 
	eUnknownFormat,
	eDefaultFormat		=	eRTFFormat,
};




// Command numbers...
enum	{
	kNewDocumentCmdID,
	kOpenDocumentCmdID,
	kSaveDocumentCmdID,
	kSaveAsDocumentCmdID,
	kQuitCmdID,
};



class	My_CMDNUM_MAPPING : public Gtk_CommandNumberMapping {
	public:
		My_CMDNUM_MAPPING ()
			{
			}
	public:
		override	CommandNumber	Lookup (int tcn) const
			{
				return tcn;
			}
		override	int				ReverseLookup (CommandNumber cmdNum) const
			{
				return cmdNum;
			}
};
My_CMDNUM_MAPPING	sMy_CMDNUM_MAPPING;







class	LedItView : public WordProcessorCommonCommandHelper_Gtk<Led_Gtk_Helper<WordProcessor> > {
	private:
		typedef	WordProcessorCommonCommandHelper_Gtk<Led_Gtk_Helper<WordProcessor> >	inherited;

	public:
		LedItView (TextStore* ts, StyleDatabasePtr sdb, ParagraphDatabasePtr pdb, HidableTextDatabasePtr htdb, CommandHandler* ch):
				inherited ()
			{
				SpecifyTextStore (ts);
				SetStyleDatabase (sdb);
				SetParagraphDatabase (pdb);
				SetHidableTextDatabase (htdb);
				//SetShowHiddenText (Options ().GetShowHiddenText ());
				SetCommandHandler (ch);
			}
		~LedItView ()
			{
				SetCommandHandler (NULL);
				SpecifyTextStore (NULL);
			}
};

GtkWidget*	gtk_LedEditor_new (TextStore* ts, LedItView::StyleDatabasePtr sdb, LedItView::ParagraphDatabasePtr pdb, LedItView::HidableTextDatabasePtr htdb, CommandHandler* ch)
{
	return (new LedItView (ts, sdb, pdb, htdb, ch))->Get_GtkWidget ();
}





class	AppWindow {
	private:
		enum	{ kMaxNumUndoLevels = 10 };
	public:
		AppWindow ():
			fInstalledFonts (GDK_DISPLAY ()),
			fAppWindow (NULL),
			fCurFileDialog (NULL),
			fDoingSaveAsDialog (false),
			fPathName (),
			fTextStore (),
			fRTFInfo (),
			fStyleDatabase (),
			fParagraphDatabase (),
			fHidableTextDatabase (),
			fCommandHandler (kMaxNumUndoLevels),
			fFileFormat (eDefaultFormat),
			fHTMLInfo ()
			{
				Led_Assert (sTHIS == NULL);
				sTHIS = this;

				fStyleDatabase = new StandardStyledTextImager::StyleDatabaseRep (fTextStore);
				fParagraphDatabase = new WordProcessor::ParagraphDatabaseRep (fTextStore);
				fHidableTextDatabase = new UniformHidableTextMarkerOwner (fTextStore);

				fAppWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
				gtk_signal_connect (GTK_OBJECT (fAppWindow), "destroy", GTK_SIGNAL_FUNC (xdestroy), this);
				gtk_window_set_title (GTK_WINDOW(fAppWindow), "Led X-Windows (GTK) Test Program");
				gtk_widget_set_usize (GTK_WIDGET(fAppWindow), 300, 200);
				gtk_widget_set_usize (fAppWindow, 600, 500);
				gtk_window_set_policy (GTK_WINDOW(fAppWindow), TRUE, TRUE, FALSE);  
				gtk_container_set_border_width (GTK_CONTAINER (fAppWindow), 0);

				GtkWidget*	main_vbox = gtk_vbox_new (FALSE, 1);
				gtk_container_border_width (GTK_CONTAINER (main_vbox), 1);
				gtk_container_add (GTK_CONTAINER (fAppWindow), main_vbox);
				gtk_widget_show (main_vbox);

				GtkWidget *menubar	=	get_main_menu (fAppWindow);

				gtk_box_pack_start (GTK_BOX (main_vbox), menubar, FALSE, TRUE, 0);
				gtk_widget_show (menubar);

				{
					GtkWidget*	box1 = gtk_vbox_new (FALSE, 0);
					gtk_container_add (GTK_CONTAINER (main_vbox), box1);
					gtk_widget_show (box1);

					GtkWidget*	box2 = gtk_vbox_new (FALSE, 10);
					gtk_container_set_border_width (GTK_CONTAINER (box2), 10);
					gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);
					gtk_widget_show (box2);

					GtkWidget*	table = gtk_table_new (2, 2, false);
					gtk_table_set_row_spacing (GTK_TABLE (table), 0, 2);
					gtk_table_set_col_spacing (GTK_TABLE (table), 0, 2);
					gtk_box_pack_start (GTK_BOX (box2), table, TRUE, TRUE, 0);
					gtk_widget_show (table);

					/* Create the GtkText widget */
					#if		qUseLedTextWidget
						fTextEditor = gtk_LedEditor_new (&fTextStore, fStyleDatabase, fParagraphDatabase, fHidableTextDatabase, &fCommandHandler);
					#else
						fTextEditor = gtk_text_new (NULL, NULL);
						gtk_text_set_editable (GTK_TEXT (fTextEditor), TRUE);
					#endif

					gtk_table_attach (GTK_TABLE (table), fTextEditor, 0, 1, 0, 1,
						static_cast<GtkAttachOptions> (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
						static_cast<GtkAttachOptions> (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
						0, 0
					);
					gtk_widget_show (fTextEditor);

					#if 1
						/* Add a vertical scrollbar to the text widget */
						#if		qUseLedTextWidget
							GtkAdjustment*	vAdj		=	GET_LED_GTK_HELPER<WordProcessor,GtkBaseInfo_GtkEditable> (fTextEditor)->GetAdjustmentObject (TextInteractor::v);
						#else
							GtkAdjustment*	vAdj		=	GTK_TEXT (fTextEditor)->vadj;
						#endif
						GtkWidget*		vscrollbar = gtk_vscrollbar_new (vAdj);
						gtk_table_attach (GTK_TABLE (table), vscrollbar, 1, 2, 0, 1, static_cast<GtkAttachOptions> (GTK_FILL), static_cast<GtkAttachOptions> (GTK_EXPAND | GTK_SHRINK | GTK_FILL), 0, 0);
						gtk_widget_show (vscrollbar);
					
						/* Add a horizontal scrollbar to the text widget */
						#if		qUseLedTextWidget
							GtkAdjustment*	hAdj		=	GET_LED_GTK_HELPER<WordProcessor,GtkBaseInfo_GtkEditable> (fTextEditor)->GetAdjustmentObject (TextInteractor::h);
						#else
							GtkAdjustment*	hAdj		=	GTK_TEXT (fTextEditor)->hadj;
						#endif
						GtkWidget*	hscrollbar = gtk_hscrollbar_new (hAdj);
						gtk_table_attach (GTK_TABLE (table), hscrollbar, 0, 1, 1, 2, static_cast<GtkAttachOptions> (GTK_EXPAND | GTK_SHRINK | GTK_FILL), static_cast<GtkAttachOptions> (GTK_FILL), 0, 0);
						gtk_widget_show (hscrollbar);
					#endif
				}

				gtk_widget_show (fAppWindow);
		}
	public:
		~AppWindow ()
			{
				Led_RequireNotNil (sTHIS);
				sTHIS = NULL;
			}
	public:
		static	AppWindow&	GET ()
			{
				Led_RequireNotNil (sTHIS);
				return *sTHIS;
			}
	private:
		static	AppWindow*	sTHIS;

	private:
		GtkWidget*	get_main_menu (GtkWidget  *window);




	public:
		static	gint	delete_event (GtkWidget *widget, gpointer   data)
			{
			// FIND ORIG GTK SAMPLE CODE AND SEE WHAT THIS USED TO GET HOOKED TO!!!!!!

				/* If you return FALSE in the "delete_event" signal handler,
				* GTK will emit the "destroy" signal. Returning TRUE means
				* you don't want the window to be destroyed.
				* This is useful for popping up 'are you sure you want to quit?'
				* type dialogs. */

				g_print ("delete event occurred\n");

				/* Change TRUE to FALSE and the main window will be destroyed with
				* a "delete_event". */

				return(TRUE);
			}

		static	void	xdestroy (GtkWidget *widget, gpointer   data)
			{
				AppWindow*	THIS	=	reinterpret_cast<AppWindow*> (data);
				THIS->OnQuitCommand ();
			}

	private:
		static	void FileDialogOKButton ( GtkWidget *widget,      gpointer   data )
			{
				AppWindow*	THIS		=	(AppWindow*)data;
				Led_AssertNotNil (THIS);
				Led_AssertNotNil (THIS->fCurFileDialog);
				string		fileName	=	gtk_file_selection_get_filename (THIS->fCurFileDialog);
				try {
					if (fileName.empty ()) {
						throw "EMPTY";
					}
					if (THIS->fDoingSaveAsDialog) {
						THIS->SaveAs (fileName);
					}
					else {
						THIS->LoadFromFile (fileName);
					}
				}
				catch (...) {
					// should print error dialog on errors...
				}
				gtk_widget_destroy (GTK_WIDGET (THIS->fCurFileDialog));
			}

		static	void	OnDestroyFileDialog (GtkWidget *widget, gpointer data)
			{
				AppWindow*	THIS	=	(AppWindow*)data;
				Led_Assert (GTK_WIDGET (THIS->fCurFileDialog) == widget);
				THIS->fCurFileDialog = NULL;
			}
		nonvirtual	void	StartFilePicker (const string& title, bool saveDialog, const string& fileName)
			{
				if (fCurFileDialog != NULL) {
					Led_Assert (false);	// shouldn't happen?
					return;
				}
				fCurFileDialog	= GTK_FILE_SELECTION (gtk_file_selection_new (title.c_str ()));
				fDoingSaveAsDialog = saveDialog;
				gtk_window_set_transient_for (GTK_WINDOW (fCurFileDialog), GTK_WINDOW (fAppWindow));
				gtk_window_set_modal (GTK_WINDOW (fCurFileDialog), true);
				gtk_signal_connect (GTK_OBJECT (fCurFileDialog), "destroy",	(GtkSignalFunc)OnDestroyFileDialog, this);
				gtk_signal_connect (GTK_OBJECT (fCurFileDialog->ok_button), "clicked", (GtkSignalFunc) FileDialogOKButton, this);
				gtk_signal_connect_object (GTK_OBJECT (fCurFileDialog->cancel_button), "clicked", (GtkSignalFunc) gtk_widget_destroy, GTK_OBJECT (fCurFileDialog));
				gtk_file_selection_hide_fileop_buttons (fCurFileDialog);	// they look terrible...
				if (saveDialog) {
					gtk_file_selection_set_filename (fCurFileDialog, fileName.empty ()? "untitled": fileName.c_str ());
				}
				gtk_widget_show (GTK_WIDGET (fCurFileDialog));
			}
	private:
		GtkFileSelection*	fCurFileDialog;
		bool				fDoingSaveAsDialog;	// else open dialog

	public:
		nonvirtual	void	OnNewDocumentCommand ()
			{
				WordProcessor*	wp	=	GET_LED_GTK_HELPER<WordProcessor,GtkBaseInfo_GtkEditable> (fTextEditor);
				TextStore&		ts	=	wp->GetTextStore ();
				ts.Replace (ts.GetStart (), ts.GetEnd (), LED_TCHAR_OF (""), 0);
				wp->SetDefaultFont (wp->GetStaticDefaultFont ());
			}
		nonvirtual	void	OnOpenDocumentCommand ()
			{
				g_message ("Handle OPEN_DOC command\n");
				StartFilePicker ("Open new document", false, string ());
			}
		nonvirtual	void	OnSaveDocumentCommand ()
			{
				#if		qPrintGLIBTradeMessages
					g_message ("Entering OnSaveDocumentCommand\n");
				#endif
				if (fPathName.empty ()) {
					OnSaveAsDocumentCommand ();
				}
				else {
					Save ();
				}
			}
		nonvirtual	void	OnSaveAsDocumentCommand ()
			{
				#if		qPrintGLIBTradeMessages
					g_message ("Entering OnSaveAsDocumentCommand\n");
				#endif
				StartFilePicker ("Save document as:", true, fPathName);
			}
		nonvirtual	void	OnQuitCommand ()
			{
				#if		qPrintGLIBTradeMessages
					g_message ("Entering OnQuitCommand\n");
				#endif
				gtk_widget_destroy (fTextEditor);
				fTextEditor = NULL;
				gtk_main_quit ();
			}

	public:
		nonvirtual	void	LoadFromFile (const string& fileName)
			{
				Led_Require (not fileName.empty ());
				fPathName = fileName;
				
				WordProcessor*	wp	=	GET_LED_GTK_HELPER<WordProcessor,GtkBaseInfo_GtkEditable> (fTextEditor);
				TextStore&		ts	=	wp->GetTextStore ();
				ts.Replace (ts.GetStart (), ts.GetEnd (), LED_TCHAR_OF (""), 0);
				wp->SetDefaultFont (wp->GetStaticDefaultFont ());

				// Now do actual reading stuff..
				g_message ("DOING LOADFROMFILE HERE- '%s'\n", fPathName.c_str ());
				size_t						fileLen	=	0;
				Led_SmallStackBuffer<char>	fileData (fileLen);
				int	fd	=	::open (fPathName.c_str (), O_RDONLY);
				if (fd == -1) {
					Led_ThrowOutOfMemoryException ();	// WRONG EXCEPTION
				}
				fileLen	= ::lseek (fd, 0, SEEK_END);
				fileData.GrowToSize (fileLen);
				try {
					::lseek (fd, 0, SEEK_SET);
					if (::read (fd, fileData, fileLen) != int (fileLen)) {
						Led_ThrowOutOfMemoryException ();	// WRONG EXCEPTION
					}
				}
				catch (...) {
					::close (fd);
					throw;
				}
				::close (fd);

		
				StyledTextIOSrcStream_Memory					source (fileData, fileLen);
				WordProcessor::WordProcessorTextIOSinkStream	sink (&fTextStore, fStyleDatabase, fParagraphDatabase, fHidableTextDatabase);

	ReRead:
				switch (fFileFormat) {
					case	eTextFormat: {
						StyledTextIOReader_PlainText	textReader (&source, &sink);
						textReader.Read ();
					}
					break;

					case	eLedPrivateFormat: {
						StyledTextIOReader_LedNativeFileFormat	textReader (&source, &sink);
						textReader.Read ();
					}
					break;

					case	eRTFFormat: {
						StyledTextIOReader_RTF	textReader (&source, &sink, &fRTFInfo);
						textReader.Read ();
					}
					break;

					case	eHTMLFormat: {
						StyledTextIOReader_HTML	textReader (&source, &sink, &fHTMLInfo);
						textReader.Read ();
					}
					break;

					case	eUnknownFormat: {
						/*
						*	Should enhance this unknown/format reading code to take into account file suffix in our guess.
						*/

						// Try RTF	
						try {
							StyledTextIOReader_RTF	reader (&source, &sink, &fRTFInfo);
							if (reader.QuickLookAppearsToBeRightFormat ()) {
								fFileFormat = eRTFFormat;
								goto ReRead;
							}
						}
						catch (...) {
							// ignore any errors, and proceed to next file type
						}

						// Try LedNativeFileFormat	
						try {
							StyledTextIOReader_LedNativeFileFormat	reader (&source, &sink);
							if (reader.QuickLookAppearsToBeRightFormat ()) {
								fFileFormat = eLedPrivateFormat;
								goto ReRead;
							}
						}
						catch (...) {
							// ignore any errors, and proceed to next file type
						}

						// Try HTML	
						try {
							StyledTextIOReader_HTML	reader (&source, &sink);
							if (reader.QuickLookAppearsToBeRightFormat ()) {
								fFileFormat = eHTMLFormat;
								goto ReRead;
							}
						}
						catch (...) {
							// ignore any errors, and proceed to next file type
						}

						// Nothing left todo but to read the text file as plain text, as best we can...
						fFileFormat = eTextFormat;
						goto ReRead;
					}
					break;

					default: {
						Led_Assert (false);	// don't support reading that format (yet?)!
					}
					break;
				}
				sink.Flush ();		// explicit Flush () call - DTOR would have done it - but there exceptions get silently eaten - this will at least show them...
		
		
		
			}
		nonvirtual	void	SaveAs (const string& fileName)
			{
				Led_Require (not fileName.empty ());
				fPathName = fileName;
	//tmphack 
	fFileFormat = eRTFFormat;
				Save ();

			}
		nonvirtual	void	Save ()
			{
				// Now do actual reading stuff..
				g_message ("DOING Save- '%s'\n", fPathName.c_str ());
				Led_Require (fFileFormat != eUnknownFormat);	// We must have chosen a file format by now...

				WordProcessor::WordProcessorTextIOSrcStream		source (&fTextStore, fStyleDatabase, fParagraphDatabase, fHidableTextDatabase);
				StyledTextIOWriterSinkStream_Memory				sink;

				switch (fFileFormat) {
					case	eTextFormat: {
						StyledTextIOWriter_PlainText	textWriter (&source, &sink);
						textWriter.Write ();
					}
					break;

					case	eRTFFormat: {
						StyledTextIOWriter_RTF	textWriter (&source, &sink, &fRTFInfo);
						textWriter.Write ();
					}
					break;

					case	eHTMLFormat: {
						StyledTextIOWriter_HTML	textWriter (&source, &sink, &fHTMLInfo);
						textWriter.Write ();
					}
					break;

					case	eLedPrivateFormat: {
						StyledTextIOWriter_LedNativeFileFormat	textWriter (&source, &sink);
						textWriter.Write ();
					}
					break;

					default: {
						Led_Assert (false);	// don't support writing that format (yet?)!
					}
					break;
				}
				int	fd	=	::open (fPathName.c_str (), O_RDWR | O_CREAT, 0666);
				if (fd == -1) {
					Led_ThrowOutOfMemoryException ();	// WRONG EXCEPTION
				}
				try {
					::lseek (fd, 0, SEEK_SET);
					if (::write (fd, sink.PeekAtData (), sink.GetLength ()) != int (sink.GetLength ())) {
						Led_ThrowOutOfMemoryException ();	// WRONG EXCEPTION
					}
				}
				catch (...) {
					::close (fd);
					throw;
				}
				::close (fd);
			}
	private:
		string		fPathName;
		FileFormat	fFileFormat;

	public:
		static	void	AppCmdDispatcher (gpointer callback_data,
												guint callback_action,
												GtkWidget *widget
											)
			{
				AppWindow*	THIS	=	reinterpret_cast<AppWindow*> (callback_data);
				GtkWidget*	w		=	THIS->fTextEditor;
				switch (callback_action) {
					case	kNewDocumentCmdID:		THIS->OnNewDocumentCommand (); break;
					case	kOpenDocumentCmdID:		THIS->OnOpenDocumentCommand (); break;
					case	kSaveDocumentCmdID:		THIS->OnSaveDocumentCommand (); break;
					case	kSaveAsDocumentCmdID:	THIS->OnSaveAsDocumentCommand (); break;
					case	kQuitCmdID:				THIS->OnQuitCommand (); break;
					#if		qUseLedTextWidget
					default:						LedItView::DispatchCommandCallback (LedItView::WidgetToTHIS (w), callback_action, widget);
					#endif
				}
			}

	public:
		GtkWidget*	fAppWindow;
		GtkWidget*	fTextEditor;

	
	public:
		ChunkedArrayTextStore						fTextStore;
		RTFInfo										fRTFInfo;
		HTMLInfo									fHTMLInfo;
		StandardStyledTextImager::StyleDatabasePtr	fStyleDatabase;
		WordProcessor::ParagraphDatabasePtr			fParagraphDatabase;
		WordProcessor::HidableTextDatabasePtr		fHidableTextDatabase;
		MultiLevelUndoCommandHandler				fCommandHandler;

	private:
		static GtkItemFactoryEntry kMenuItemResources[];
	public:
		Led_InstalledFonts	fInstalledFonts;	// Keep a static copy for speed, and so font#s are static throughout the life of the applet
};
AppWindow*	AppWindow::sTHIS	=	NULL;




struct	LedTestXWin_DialogSupport : TextInteractor::DialogSupport, WordProcessor::DialogSupport {
	public:
		typedef	TextInteractor::DialogSupport::CommandNumber	CommandNumber;

	public:
		LedTestXWin_DialogSupport ()
			{
				TextInteractor::SetDialogSupport (this);
				WordProcessor::SetDialogSupport (this);
			}
		~LedTestXWin_DialogSupport ()
			{
				WordProcessor::SetDialogSupport (NULL);
				TextInteractor::SetDialogSupport (NULL);
			}

	//	TextInteractor::DialogSupport
	public:


	//	WordProcessor::DialogSupport
	public:
		override	FontNameSpecifier	CmdNumToFontName (CommandNumber cmdNum)
			{
				const vector<Led_SDK_String>&	fontNames	=	AppWindow::GET ().fInstalledFonts.GetUsableFontNames ();
				return (fontNames[cmdNum-WordProcessor::kFontMenuFirst_CmdID]);
			}
};
static	LedTestXWin_DialogSupport	sLedIt_DialogSupport;





/* This is the GtkItemFactoryEntry structure used to generate new menus.
   Item 1: The menu path. The letter after the underscore indicates an
           accelerator key once the menu is open.
   Item 2: The accelerator key for the entry
   Item 3: The callback function.
   Item 4: The callback action.  This changes the parameters with
           which the function is called.  The default is 0.
   Item 5: The item type, used to define what kind of an item it is.
           Here are the possible values:

           NULL               -> "<Item>"
           ""                 -> "<Item>"
           "<Title>"          -> create a title item
           "<Item>"           -> create a simple item
           "<CheckItem>"      -> create a check item
           "<ToggleItem>"     -> create a toggle item
           "<RadioItem>"      -> create a radio item
           <path>             -> path of a radio item to link against
           "<Separator>"      -> create a separator
           "<Branch>"         -> create an item to hold sub items (optional)
           "<LastBranch>"     -> create a right justified branch 
*/
GtkItemFactoryEntry AppWindow::kMenuItemResources[] = {
	{ "/_File",								NULL,			NULL,											0,										"<Branch>" },
	{ "/File/_New",							"<control>N",	GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	kNewDocumentCmdID,						NULL },
	{ "/File/_Open...",						"<control>O",	GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	kOpenDocumentCmdID,						NULL },
	{ "/File/_Save",						"<control>S",	GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	kSaveDocumentCmdID,						NULL },
	{ "/File/Save _As...",					NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	kSaveAsDocumentCmdID,					NULL },
	{ "/File/sep1",							NULL,			NULL,											0,										"<Separator>" },
	{ "/File/Quit",							"<control>Q",	GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	kQuitCmdID,								NULL },

	{ "/_Edit",								NULL,			NULL,											0,										"<Branch>" },
	{ "/Edit/_Undo",						"<control>Z",	GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kUndo_CmdID,					NULL },
	{ "/Edit/_Redo",						"<control>R",	GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kRedo_CmdID,					NULL },
	{ "/Edit/",								NULL,			NULL,											0,										"<Separator>" },
	{ "/Edit/_Cut",							"<control>X",	GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kCut_CmdID,					NULL },
	{ "/Edit/Co_py",						"<control>C",	GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kCopy_CmdID,					NULL },
	{ "/Edit/_Paste",						"<control>V",	GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kPaste_CmdID,				NULL },
	{ "/Edit/_Clear",						NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kClear_CmdID,				NULL },
	{ "/Edit/",								NULL,			NULL,											0,										"<Separator>" },
	{ "/Edit/_Select All",					"<control>A",	GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kSelectAll_CmdID,			NULL },
	{ "/Edit/",								NULL,			NULL,											0,										"<Separator>" },
	{ "/Edit/_Hide Selection",				NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kHideSelection_CmdID,		NULL },
	{ "/Edit/_Unhide Selection",			NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kUnHideSelection_CmdID,		NULL },

	{ "/_Format",							NULL,			NULL,											0,										"<Branch>" },
	{ "/_Format/Font _Name",				NULL,			NULL,											0,										"<Branch>" },
	{ "/_Format/Font _Style",				NULL,			NULL,											0,										"<Branch>" },
	{ "/_Format/Font _Style/Plain",			"<control>T",	GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontStylePlain_CmdID,		NULL },
	{ "/_Format/Font _Style/sep1",			NULL,			NULL,											0,										"<Separator>" },
	{ "/_Format/Font _Style/Bold",			"<control>B",	GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontStyleBold_CmdID,		NULL },
	{ "/_Format/Font _Style/Italic",		"<control>I",	GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontStyleItalic_CmdID,		NULL },
	{ "/_Format/Font _Style/Underline",		"<control>U",	GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontStyleUnderline_CmdID,	NULL },
	{ "/_Format/Font _Style/Subscript",		NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kSubScriptCommand_CmdID,		NULL },
	{ "/_Format/Font _Style/Superscript",	NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kSuperScriptCommand_CmdID,	NULL },
	{ "/_Format/Font Si_ze",				NULL,			NULL,											0,										"<Branch>" },
	{ "/_Format/Font Si_ze/9",				NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontSize9_CmdID,			NULL },
	{ "/_Format/Font Si_ze/10",				NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontSize10_CmdID,			NULL },
	{ "/_Format/Font Si_ze/12",				NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontSize12_CmdID,			NULL },
	{ "/_Format/Font Si_ze/14",				NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontSize14_CmdID,			NULL },
	{ "/_Format/Font Si_ze/18",				NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontSize18_CmdID,			NULL },
	{ "/_Format/Font Si_ze/24",				NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontSize24_CmdID,			NULL },
	{ "/_Format/Font Si_ze/36",				NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontSize36_CmdID,			NULL },
	{ "/_Format/Font Si_ze/48",				NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontSize48_CmdID,			NULL },
	{ "/_Format/Font Si_ze/72",				NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontSize72_CmdID,			NULL },
	{ "/_Format/Font Si_ze/sep1",			NULL,			NULL,											0,										"<Separator>" },
	{ "/_Format/Font Si_ze/Smaller",		NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontSizeSmaller_CmdID,		NULL },
	{ "/_Format/Font Si_ze/Larger",			NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontSizeLarger_CmdID,		NULL },
	{ "/_Format/Font Si_ze/sep2",			NULL,			NULL,											0,										"<Separator>" },
	{ "/_Format/Font Si_ze/Other...",		NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontSizeOther_CmdID,		NULL },
	{ "/_Format/Font _Color",				NULL,			NULL,											0,										"<Branch>" },
	{ "/_Format/Font _Color/Black",			NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontColorBlack_CmdID,		NULL },
	{ "/_Format/Font _Color/Maroon",		NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontColorMaroon_CmdID,		NULL },
	{ "/_Format/Font _Color/Green",			NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontColorGreen_CmdID,		NULL },
	{ "/_Format/Font _Color/Olive",			NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontColorOlive_CmdID,		NULL },
	{ "/_Format/Font _Color/Navy",			NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontColorNavy_CmdID,		NULL },
	{ "/_Format/Font _Color/Purple",		NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontColorPurple_CmdID,		NULL },
	{ "/_Format/Font _Color/Teal",			NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontColorTeal_CmdID,		NULL },
	{ "/_Format/Font _Color/Gray",			NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontColorGray_CmdID,		NULL },
	{ "/_Format/Font _Color/Silver",		NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontColorSilver_CmdID,		NULL },
	{ "/_Format/Font _Color/Red",			NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontColorRed_CmdID,			NULL },
	{ "/_Format/Font _Color/Lime",			NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontColorLime_CmdID,		NULL },
	{ "/_Format/Font _Color/Yellow",		NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontColorYellow_CmdID,		NULL },
	{ "/_Format/Font _Color/Blue",			NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontColorBlue_CmdID,		NULL },
	{ "/_Format/Font _Color/Fuchsia",		NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontColorFuchsia_CmdID,		NULL },
	{ "/_Format/Font _Color/Aqua",			NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontColorAqua_CmdID,		NULL },
	{ "/_Format/Font _Color/White",			NULL,			GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher),	LedItView::kFontColorOther_CmdID,		NULL },

	{ "/_Options",							NULL,			NULL,											0,										"<Branch>" },
	{ "/Options/Test",						NULL,			NULL,											0,										NULL },

	{ "/_Help",								NULL,			NULL,											0,										"<LastBranch>" },
	{ "/_Help/About",						NULL,			NULL,											0,										NULL },
};

	GtkWidget*	AppWindow::get_main_menu (GtkWidget  *window)
		{
			GtkAccelGroup*	accel_group = gtk_accel_group_new ();

			/* This function initializes the item factory.
			 Param 1: The type of menu - can be GTK_TYPE_MENU_BAR, GTK_TYPE_MENU,
					  or GTK_TYPE_OPTION_MENU.
			 Param 2: The path of the menu.
			 Param 3: A pointer to a gtk_accel_group.  The item factory sets up
					  the accelerator table while generating menus.
			*/

			GtkItemFactory*	item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);

			/* This function generates the menu items. Pass the item factory,
			 the number of items in the array, the array itself, and any
			 callback data for the the menu items. */
			gtk_item_factory_create_items (item_factory, Led_NEltsOf (kMenuItemResources), kMenuItemResources, this);

			/* Attach the new accelerator group to the window. */
			gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);


			{
				const	string	kFontMenuPath	=	"/Format/Font Name";
				GtkWidget*	fontNameMenu	=	gtk_item_factory_get_widget (item_factory, kFontMenuPath.c_str ());
				vector<Led_SDK_String>	fontNames	=	AppWindow::GET ().fInstalledFonts.GetUsableFontNames ();
				for (vector<Led_SDK_String>::const_iterator i = fontNames.begin (); i != fontNames.end (); ++i) {
					GtkItemFactoryEntry	entry;
					memset (&entry, 0, sizeof (entry));
					string	fontName	=	*i;	// really should munge it so its assured not to have any bad chars in it!!!
					string	tmpPath		=	kFontMenuPath + "/" + fontName;
					entry.path = const_cast<char*> (tmpPath.c_str ());
					entry.callback = GTK_SIGNAL_FUNC (AppWindow::AppCmdDispatcher);
					entry.callback_action = (i-fontNames.begin ()) + WordProcessor::kFontMenuFirst_CmdID;
					gtk_item_factory_create_items (item_factory, 1, &entry, this);
				}
			}
			
			/* Finally, return the actual menu bar created by the item factory. */ 
			return gtk_item_factory_get_widget (item_factory, "<main>");
		}



int	main (int argc, char** argv)
{
	gtk_set_locale ();
	gtk_init (&argc, &argv);
	#if		qUseMyXErrorHandlers
		XSetErrorHandler (MyXErrorHandler);
	#endif

	#if		qSlowXDebugSyncMode
		(void)XSynchronize (GDK_DISPLAY(), true);
	#endif

	AppWindow	app;
	gtk_main ();

	return(0);
}


