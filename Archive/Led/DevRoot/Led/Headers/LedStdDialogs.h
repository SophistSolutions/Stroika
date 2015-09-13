/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	____Led_StdDialogs__h__
#define	____Led_StdDialogs__h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/LedStdDialogs.h,v 2.60 2004/02/11 23:57:50 lewis Exp $
 */


/*
@MODULE:	LedStdDialogs
@DESCRIPTION:
		<p>Pre-canned (though subclassable) dialogs which tend to come in handy in Led-based applications.
	These are not always available for all platforms. Also - some minimal effort is made to make it a bit
	easier to do somewhat platoform-independent dialog code (very minimal support for this as of Led 3.0).
	</p>
 */


/*
 * Changes:
 *	$Log: LedStdDialogs.h,v $
 *	Revision 2.60  2004/02/11 23:57:50  lewis
 *	SPR#1576: Update: added ReplaceAllInSelection functionality (except must re-layout MacOS dialog)
 *	
 *	Revision 2.59  2004/02/11 22:39:13  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.58  2004/02/11 02:52:07  lewis
 *	MWERKS resource compiler doesn't like #if XXX where XXX is undefined - so fixup ifdef
 *	check to make it happy
 *	
 *	Revision 2.57  2004/02/11 01:09:52  lewis
 *	SPR#1635: qTemplatesWithMixOfNestedClassesWithCommonNamesGetsConfusedBug - disable SpellCheck
 *	for MSVC60 - cuz of compiler bugs (and little need to support MSVC6)
 *	
 *	Revision 2.56  2003/12/31 04:01:57  lewis
 *	SPR#1580: renamed qSupportLedDialogWidget to qSupportLedDialogWidgets. Added new LedComboBoxWidget
 *	class to implement Led-based ComboBox (still not perfect, but pretty close to releasable). Then used
 *	that in the Find and Replace dialogs for the search string.
 *	
 *	Revision 2.55  2003/12/11 02:57:50  lewis
 *	SPR#1590: respoect AddToDictioanryEnabled callback value to disable the addtodict dialog button. Also -
 *	added similar OptionsDIalogEnabled callback and respected that so button disabled (til we've had time to
 *	implement an options dialog)
 *	
 *	Revision 2.54  2003/12/11 01:31:50  lewis
 *	SPR#1589: Implemented 'lookup on web' button in spell check dialog. Only tested/tuned dialog resource for
 *	Win32. Must test/tune for MacOS/XWin.
 *	
 *	Revision 2.53  2003/06/03 23:32:45  lewis
 *	fix typename usage
 *	
 *	Revision 2.52  2003/06/03 22:13:15  lewis
 *	SPR#1513: macos spellcheck dialog work
 *	
 *	Revision 2.51  2003/06/03 20:28:56  lewis
 *	SPR#1513: added support for ignoreall button, clicking in the listbox, and double clicking in it.
 *	Also implemetned the Change functionality and ChangeAll (all for spellcheck dialog)
 *	
 *	Revision 2.50  2003/06/02 16:13:46  lewis
 *	SPR#1513: first draft of (windows only so far) simple spellchecking dialog. Basically working (75%).
 *	A few fixes to generic dialog support code
 *	
 *	Revision 2.49  2003/05/30 17:30:23  lewis
 *	SPR#1517: finish getting Find/Replace dialog working for Gtk(though its quite ugly)
 *	
 *	Revision 2.48  2003/05/30 16:04:57  lewis
 *	SPR#1517: Linux/Gtk replace dialog support
 *	
 *	Revision 2.47  2003/05/30 15:57:19  lewis
 *	SPR#1417: fix replace dlg support for MacOS
 *	
 *	Revision 2.46  2003/05/30 03:06:43  lewis
 *	SPR#1517: preliminary version of REPLACE DIALOG support (WIN32 only so far).
 *	
 *	Revision 2.45  2003/05/14 20:47:08  lewis
 *	SPR#1399 and SPR#1401: finished MacOS implemenation of add table /edit table dialogs
 *	
 *	Revision 2.44  2003/05/14 17:52:37  lewis
 *	SPR#1399: first cut at MacOS AddTable/EditTable dialogs
 *	
 *	Revision 2.43  2003/05/13 22:27:42  lewis
 *	fix so compiles for MacOS
 *	
 *	Revision 2.42  2003/05/13 21:49:28  lewis
 *	SPR#1399: finished Win32 support for EditTableProperties dialog (mostly fixed StdColorPickBox)
 *	
 *	Revision 2.41  2003/05/13 20:44:47  lewis
 *	SPR#1399: progress checking on table getproperties dialog support
 *	
 *	Revision 2.40  2003/05/12 17:17:42  lewis
 *	SPR#1401: Added GetDialogSupport ().AddNewTableDialog () and Led_StdDialogHelper_AddNewTableDialog.
 *	
 *	Revision 2.39  2003/04/03 16:41:21  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using templated command classes,
 *	just builtin to TextInteractor/WordProcessor (and instead of template params use new
 *	TextInteractor/WordProcessor::DialogSupport etc
 *	
 *	Revision 2.38  2003/02/01 00:33:27  lewis
 *	SPR#1270 - added basic edit commands (cut/copy/paste/selectall) to the edit widget via standard control chars (^A^X etc)
 *	
 *	Revision 2.37  2003/01/30 22:16:30  lewis
 *	SPR#1266 - OnBadUserInput override for LedDialogWidget
 *	
 *	Revision 2.36  2003/01/30 21:14:49  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.35  2003/01/30 21:05:21  lewis
 *	SPR#1266- support qSupportLedDialogWidget flag and fix GetText/SetText methods of LedDialogText(WIN)
 *	
 *	Revision 2.34  2003/01/30 19:35:03  lewis
 *	SPR#1269 - use new Led_Win32_SimpleWndProc_HelperWithSDKMessages in LedDialogWidget
 *	
 *	Revision 2.33  2003/01/30 15:01:58  lewis
 *	SPR#1266- preliminary support for a Windows LedDialogWidget and use of said in Find dialog
 *	
 *	Revision 2.32  2003/01/22 22:48:08  lewis
 *	SPR#1250 - cleaned up UI for most MacOS dialog boxes. SPR#1256 - finished cleaning up and
 *	moving Other size and Paragraph spacing dialogs on MacOS. SPR#1186 - got working/polished 
 *	Paragraph Indents dialog on MacOS.
 *	
 *	Revision 2.31  2003/01/22 01:09:09  lewis
 *	SPR#1256 - OtherSize&ParagraphSpacing dialogs moved to LedStdDialogs(untested on Mac).
 *	
 *	Revision 2.30  2003/01/21 22:51:36  lewis
 *	for SPR#1186- fix IDs of ParagraphIndents dialogitems so they work on Mac (there they must
 *	correspond to listed order)
 *	
 *	Revision 2.29  2003/01/21 19:26:06  lewis
 *	SPR#1255 - added various portable helper APIs to Led_StdDialogHelper - like SetItemText()).
 *	Use that in a couple dialogs like FIND dialog and ParagraphIndents. For SPR#1186 -
 *	entered basic MacOS support (not tested yet)
 *	
 *	Revision 2.28  2003/01/21 13:26:53  lewis
 *	SPR#1186 - first cut at Indents dialog support (MacOS dialog still not done)
 *	
 *	Revision 2.27  2002/10/22 00:38:43  lewis
 *	Add URL context/menu command - SPR#1136
 *	
 *	Revision 2.26  2002/05/06 21:33:26  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.25  2001/11/27 00:29:39  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.24  2001/10/20 13:38:55  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.23  2001/10/17 20:42:49  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.22  2001/09/26 15:41:15  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.21  2001/09/19 20:09:47  lewis
 *	SPR#1039 (Added Led_StdAlertHelper)
 *	
 *	Revision 2.20  2001/09/16 18:33:57  lewis
 *	SPR#1033- added qSupportUpdateWin32FileAssocDlg for querying user about updating
 *	file assocs. Added Led_StdDialogHelper::ReplaceAllTokens helper
 *	
 *	Revision 2.19  2001/08/29 23:00:15  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.18  2001/08/28 18:43:24  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.17  2001/08/17 15:59:29  lewis
 *	disable qSupportUnknownEmbeddingInfoDlg/qSupportURLXEmbeddingInfoDlg for XWindows (SPR#0967)
 *	
 *	Revision 2.16  2001/08/17 00:00:02  lewis
 *	SPR#0959- Fix GetSelInfo stuff for MacOS (dialogs)
 *	
 *	Revision 2.15  2001/07/31 17:12:07  lewis
 *	SPR#0959- at least got new Led_StdDialogHelper_URLXEmbeddingInfoDialog etc dialogs
 *	compiling for X-Windows - though still far from complete
 *	
 *	Revision 2.14  2001/07/17 19:09:36  lewis
 *	SPR#0959- added preliminary RightClick-to-show-edit-embedding-properties support
 *	
 *	Revision 2.13  2001/06/05 18:17:17  lewis
 *	current about box code requires gtk_window - not gtk_dialog - no idea why - for X/Gtk only
 *	
 *	Revision 2.12  2001/06/05 15:44:50  lewis
 *	SPR#950- XWindows/Gtk support for Led_StdDialogHelper_FindDialog (though very primitive so far)
 *	
 *	Revision 2.11  2001/06/05 13:34:32  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.10  2001/06/04 19:31:51  lewis
 *	SPR#0931 - added StdFilePickBox for Linux
 *	
 *	Revision 2.9  2001/05/24 13:43:40  lewis
 *	be more careful about getting LedStdDialogs code to compile for X-Windows if NOT using
 *	Gtk (not tested - but hopefully right). And SPR#0927 - center Gtk dialogs by default.
 *	
 *	Revision 2.8  2001/05/22 21:37:55  lewis
 *	SPR#0923- Added qUseGTKForLedStandardDialogs support to LedStdDialogs module. Added support
 *	for Led_StdDialogHelper class with Gtk - as well as StdFontPickBox and StdColorPickBox
 *	
 *	Revision 2.7  2001/05/18 20:58:34  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.6  2000/10/19 15:50:23  lewis
 *	lots more work on dialogs on Mac. Use dlgx resource to support movable modal. Cleanup display.
 *	Support togglying checkbox, and full grabing / setting of values in dialog. Support event filterproc.
 *	Now portable mac/windows dialog support does about everything I need to replace what I was getting
 *	from PowerPlant
 *	
 *	Revision 2.5  2000/10/18 20:39:54  lewis
 *	Added Led_StdDialogHelper_FindDialog support. Works on Windows and prelim for Mac
 *	
 *	Revision 2.4  2000/10/17 18:16:10  lewis
 *	mac aboutbox work
 *	
 *	Revision 2.3  2000/10/16 22:49:10  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.2  2000/10/16 18:51:59  lewis
 *	for basic support for Led_StdDialogHelper_AboutBox etc working on MAC
 *	
 *	Revision 2.1  2000/10/16 00:15:43  lewis
 *	first cut at sharable code for various dialogs used in many Led-based apps
 *	
 *
 */


#if		defined (__cplusplus)
	#include	"LedSupport.h"
	#include	"LedGDI.h"
#endif

#if		qMacOS && defined (__cplusplus)
#include	<Controls.h>
#endif




/*
@CONFIGVAR:		qUseGTKForLedStandardDialogs
@DESCRIPTION:	<p>Generally we try to keep toolkit dependencies restricted to the Led_TOOLKIT module(s), such as Led_Gtk.
			Thats pretty well doable where there is decent native dialog support, such as with Win32 and MacOS. But this isn't
			very practical for X-Windows. So - I've added a Gtk-dependency to this module as well - just for implementing
			these standard dialogs.</p>
				<p>This flag defaults to true, iff qXWindows is true.</p>
 */
#ifndef	qUseGTKForLedStandardDialogs
	#define	qUseGTKForLedStandardDialogs	qXWindows
#endif

#if		qUseGTKForLedStandardDialogs
#include	<gtk/gtk.h>
#endif








// MFC must define something like this someplace, but I haven't found where....
// Use this for now, so I can update things more easily when I find the MFC definition...
#ifndef	kLedStdDlgCommandBase
	#if		qMacOS
		#define	kLedStdDlgCommandBase		0x0
	#elif	qWindows
		#define	kLedStdDlgCommandBase		0x1000
	#endif
#endif

// NOTE - due to quirks of MSDEV .rc compiler - you CANNOT change this #. If you do - it will be out of sync with
// kLedStdDlg_AboutBoxID etc below. You cannot make kLedStdDlg_AboutBoxID etc below be based on this - or the .rc compiler
// will silently - but surely - generate the wrong resids. Amazing but true stories -- LGP 2000-10-16
#define	kLedStdDlgIDBase			0x1000










/*
@CONFIGVAR:		qSupportLedDialogWidgets
@DESCRIPTION:	<p>Support the @'LedDialogWidget' class. This requires you link with SimpleTextStore.cpp,
			SimpleTextImager.cpp, and SimpleTextInteractor.cpp.</p>
				<p>This flag defaults to true, iff qWindows is true.</p>
 */
#ifndef	qSupportLedDialogWidgets
#define	qSupportLedDialogWidgets		(qWindows)
#endif





#if		qSupportLedDialogWidgets && qWindows && defined (__cplusplus)
	#include	"Led_Win32.h"
	#include	"SimpleTextStore.h"
	#include	"SimpleTextInteractor.h"
#endif









#if		defined (__cplusplus) && qLedUsesNamespaces
namespace	Led {
#endif




#if		defined (__cplusplus)
class	StdColorPopupHelper {
	public:
		StdColorPopupHelper (bool allowNone);
		~StdColorPopupHelper ();

	public:
		nonvirtual	bool	GetSelectedColor (Led_Color* c) const;
		nonvirtual	void	SetSelectedColor (const Led_Color& c);
		nonvirtual	void	SetNoSelectedColor ();

	private:
		bool		fIsSelectedColor;
		Led_Color	fSelectedColor;

	private:
		nonvirtual	size_t		MapColorIdx (const Led_Color& c) const;
		nonvirtual	Led_Color	MapColorIdx (size_t i) const;

	private:
		bool	fAllowNone;

#if		qMacOS
	public:
		nonvirtual	void	Attach (ControlRef popup);
	private:
		ControlRef	fControl;
#elif	qWindows
	public:
		nonvirtual	void	Attach (HWND popup);
	private:
		HWND	fHWnd;
#endif

#if		qMacOS || qWindows
	public:
		nonvirtual	void	OnSelChange ();
#endif
#if		qMacOS || qWindows
	private:
		nonvirtual	void	DoMenuAppends ();
		nonvirtual	void	AppendMenuString (const Led_SDK_String& s);
#endif
};
#endif




#if		qSupportLedDialogWidgets && defined (__cplusplus)

	namespace LedDialogWidget_Private {
		typedef Led_Win32_SimpleWndProc_HelperWithSDKMessages <Led_Win32_Helper <SimpleTextInteractor> >	LedDialogWidget_BASE;
	}

	class	LedDialogWidget : public LedDialogWidget_Private::LedDialogWidget_BASE {
		private:
			typedef	LedDialogWidget_Private::LedDialogWidget_BASE	inherited;

		public:
			LedDialogWidget ();
		protected:
			enum TS_SET_OUTSIDE_BWA { eTS_SET_OUTSIDE_BWA };	/// FILL IN RIGHT LED_CONFIG BWA DEFINE FOR THIS CRAPOLA
			LedDialogWidget (TS_SET_OUTSIDE_BWA);
		public:
			virtual ~LedDialogWidget ();

		public:
			override	void	OnBadUserInput ();

		public:
			override	void	OnTypedNormalCharacter (Led_tChar theChar, bool /*optionPressed*/, bool /*shiftPressed*/, bool /*commandPressed*/, bool controlPressed, bool /*altKeyPressed*/);

		protected:
			virtual	CommandNumber	CharToCommand (Led_tChar theChar) const;

		public:
			nonvirtual	Led_tString	GetText () const;
			nonvirtual	void		SetText (const Led_tString& t);

		public:
			SimpleTextStore				fTextStore;
			SingleUndoCommandHandler	fCommandHandler;
	};
#endif






#if		qSupportLedDialogWidgets && defined (__cplusplus)
	/*
	@CLASS:			LedComboBoxWidget
	@DESCRIPTION:	<p></p>
	*/
	class	LedComboBoxWidget
			#if		qWindows
				: public SimpleWin32WndProcHelper
			#endif
			{
	#if		qWindows
		private:
			typedef	SimpleWin32WndProcHelper	inherited;
	#endif

		public:
			LedComboBoxWidget ();
			virtual ~LedComboBoxWidget ();

	#if		qWindows
		public:
			nonvirtual	bool	ReplaceWindow (HWND hWnd);
	#endif

		public:
			nonvirtual	Led_tString	GetText () const;
			nonvirtual	void		SetText (const Led_tString& t);

		public:
			nonvirtual	vector<Led_tString>	GetPopupItems () const;
			nonvirtual	void				SetPopupItems (const vector<Led_tString>& pi);
		private:
			vector<Led_tString>	fPopupItems;


		#if		qWindows
			public:
				override	LRESULT	WndProc (UINT message, WPARAM wParam, LPARAM lParam);
			protected:
				virtual		LRESULT	OnCreate_Msg (WPARAM wParam, LPARAM lParam);
				virtual		LRESULT	OnSize_Msg (WPARAM wParam, LPARAM lParam);
		#endif

		#if		qWindows
			protected:
				struct	MyButton : public SimpleWin32WndProcHelper {
					typedef	SimpleWin32WndProcHelper	inherited;

					MyButton ();
					override	LRESULT	WndProc (UINT message, WPARAM wParam, LPARAM lParam);

					LedComboBoxWidget*	fComboBox;
					Led_Bitmap			fDropDownArrow;
				};
				friend	struct	MyButton;
		#endif

		#if		qWindows
			protected:
				struct	MyComboListBoxPopup : public SimpleWin32WndProcHelper {
					typedef	SimpleWin32WndProcHelper	inherited;

					MyComboListBoxPopup ();
					override	LRESULT		WndProc (UINT message, WPARAM wParam, LPARAM lParam);
					nonvirtual	void		UpdatePopupItems ();
					nonvirtual	void		MadeSelection ();
					nonvirtual	void		ComputePreferedHeight (Led_Distance* prefHeight, size_t* nEltsShown) const;

					LedComboBoxWidget*	fComboBox;
				};
				friend	struct	MyComboListBoxPopup;
		#endif

		#if		qWindows
			protected:
				struct	MyTextWidget : public LedDialogWidget {
					typedef	LedDialogWidget	inherited;

					MyTextWidget ();
					~MyTextWidget ();
					override	LRESULT	WndProc (UINT message, WPARAM wParam, LPARAM lParam);

					LedComboBoxWidget*	fComboBox;
				};
				friend	struct	MyTextWidget;
		#endif

		protected:
			nonvirtual	void	ShowPopup ();
			nonvirtual	void	HidePopup ();
			nonvirtual	bool	PopupShown () const;
			nonvirtual	void	TogglePopupShown ();

		private:
		#if		qWindows
			MyButton			fPopupButton;
			MyComboListBoxPopup	fComboListBoxPopup;
			MyTextWidget		fTextWidget;
			Led_FontObject*		fUseWidgetFont;
		#endif
	};
#endif





#if		defined (__cplusplus)
	/*
	@CLASS:			Led_StdDialogHelper
	@DESCRIPTION:	<p>A very minimalistic attempt at providing cross-platform dialog support.</p>
	*/
	class	Led_StdDialogHelper {
		public:
		#if		qMacOS
			Led_StdDialogHelper (int resID);
		#elif	qWindows
			Led_StdDialogHelper (HINSTANCE hInstance, const Led_SDK_Char* resID, HWND parentWnd);
		#elif	(qXWindows && qUseGTKForLedStandardDialogs)
			Led_StdDialogHelper (GtkWindow* parentWindow);
		#endif

		public:
			virtual ~Led_StdDialogHelper ();

		public:
			virtual		bool	DoModal ();

		public:
			nonvirtual	void	ReplaceAllTokens (Led_SDK_String* m, const Led_SDK_String& token, const Led_SDK_String& with);

		protected:
			#if		(qXWindows && qUseGTKForLedStandardDialogs)
			virtual	GtkWidget*	MakeWindow ();
			#endif
			virtual	void	PreDoModalHook ();

	#if		qMacOS
		protected:
			virtual	bool	HandleCommandClick (int itemNum);
	#endif

		public:
			#if		qMacOS
			nonvirtual	int	GetResID () const { return fResID; }
			#endif

		private:
			#if		qMacOS
				int					fResID;
				bool				fDialogClosing;
			#elif	qWindows
				HINSTANCE			fHINSTANCE;
				const Led_SDK_Char*	fResID;	// not a REAL string  - fake one for MAKEINTRESOURCE - which is why we dont copy with 'string' class
				HWND				fParentWnd;
			#elif	qXWindows && qUseGTKForLedStandardDialogs
				GtkWindow*			fParentWindow;
			#endif

		public:
			virtual	void	OnOK ();
			virtual	void	OnCancel ();

		public:
			nonvirtual	bool	GetWasOK () const;
		private:
			bool	fWasOK;

	/*
	 *	Some cross-platform portability helper functions.
	 */
	public:
		#if		qMacOS || qWindows
			typedef	int			DialogItemID;
		#elif	qXWindows && qUseGTKForLedStandardDialogs
			typedef	GtkWidget*	DialogItemID;
		#endif
	public:
		#if		qMacOS || qWindows || (qXWindows && qUseGTKForLedStandardDialogs)
		nonvirtual	Led_SDK_String	GetItemText (DialogItemID itemID) const;
		nonvirtual	void			SetItemText (DialogItemID itemID, const Led_SDK_String& text);
		nonvirtual	void			SelectItemText (DialogItemID itemID, size_t from = 0, size_t to = static_cast<size_t> (-1));
		nonvirtual	bool			GetItemChecked (DialogItemID itemID) const;
		nonvirtual	void			SetItemChecked (DialogItemID itemID, bool checked);
		nonvirtual	bool			GetItemEnabled (DialogItemID itemID) const;
		nonvirtual	void			SetItemEnabled (DialogItemID itemID, bool enabled);
		nonvirtual	void			SetFocusedItem (DialogItemID itemID);
		#endif

	private:
		#if		qWindows
			bool	fSetFocusItemCalled;
		#endif

	#if		qMacOS
		public:
			nonvirtual	DialogPtr	GetDialogPtr () const;
			nonvirtual	void		SetDialogPtr (DialogPtr d);
		private:
			DialogPtr	fDialogPtr;
	#elif	qWindows
		public:
			nonvirtual	HWND	GetHWND () const;
			nonvirtual	void	SetHWND (HWND hWnd);
		private:
			HWND	fHWnd;
	#elif	qXWindows && qUseGTKForLedStandardDialogs
		public:
			nonvirtual	GtkWidget*	GetWindow () const;
			nonvirtual	void		SetWindow (GtkWidget* w);
		private:
			GtkWidget*	fWindow;

		public:
			nonvirtual	GtkWidget*	GetOKButton () const;
			nonvirtual	void		SetOKButton (GtkWidget* okButton);
		private:
			GtkWidget*	fOKButton;
	
		public:
			nonvirtual	GtkWidget*	GetCancelButton () const;
			nonvirtual	void		SetCancelButton (GtkWidget* cancelButton);
		private:
			GtkWidget*	fCancelButton;
	#endif


	#if		qMacOS
	protected:
		static	pascal	Boolean	StaticEventFilter (DialogPtr dialog, EventRecord* eventRecord, short* itemHit);
		virtual	bool			EventFilter (DialogPtr dialog, EventRecord* eventRecord, short* itemHit);
	#endif

	#if		qWindows
		protected:
			override	BOOL	OnInitDialog ();
	#endif

	#if		qWindows
		public:
			static	BOOL	CALLBACK	StaticDialogProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		protected:
			virtual	BOOL	DialogProc (UINT message, WPARAM wParam, LPARAM lParam);
	#endif

	#if		qXWindows && qUseGTKForLedStandardDialogs
		public:
			static	void	Static_OnOKButtonClick (GtkWidget* widget, gpointer data);
			static	void	Static_OnCancelButtonClick (GtkWidget* widget, gpointer data);
			static	void	Static_OnWindowDeleteRequest (GtkWidget* widget);
	#endif
	};
#endif









#if		qMacOS
	#if		defined (__cplusplus)
		/*
		@CLASS:			Led_StdAlertHelper
		@DESCRIPTION:	<p>MacOS only</p>
		*/
		class	Led_StdAlertHelper : public Led_StdDialogHelper {
			private:
				typedef	Led_StdDialogHelper	inherited;
			public:
				Led_StdAlertHelper (int resID);

			public:
				override	bool	DoModal ();
		};
	#endif

#endif











#if		qMacOS
	// Some dialogs require their own menus and CNTL resources
	#ifndef	kLedStdDlgMENUBase
		#define	kLedStdDlgMENUBase		kLedStdDlgIDBase
	#endif
	#ifndef	kLedStdDlgCNTLBase
		#define	kLedStdDlgCNTLBase		kLedStdDlgIDBase
	#endif
#endif




#ifndef	qSupportStdAboutBoxDlg
#define	qSupportStdAboutBoxDlg		(qMacOS || qWindows || qUseGTKForLedStandardDialogs)
#endif

#if		qSupportStdAboutBoxDlg
	#define kLedStdDlg_AboutBoxID			0x1001
		//NB: order of these must track declared order in Mac .r file!
		#define	kLedStdDlg_AboutBox_InfoLedFieldID		(kLedStdDlgCommandBase+1)
		#define	kLedStdDlg_AboutBox_LedWebPageFieldID	(kLedStdDlgCommandBase+2)
		#define	kLedStdDlg_AboutBox_BigPictureFieldID	(kLedStdDlgCommandBase+3)
		#define	kLedStdDlg_AboutBox_VersionFieldID		(kLedStdDlgCommandBase+4)

	#if		defined (__cplusplus)
		/*
		@CLASS:			Led_StdDialogHelper_AboutBox
		@DESCRIPTION:	<p>You can define qSupportStdAboutBoxDlg to 0 to disable inclusion of this resource/code (for size reasons).</p>
		*/
		class	Led_StdDialogHelper_AboutBox : public Led_StdDialogHelper {
			private:
				typedef	Led_StdDialogHelper	inherited;
			public:
			#if		qMacOS
				Led_StdDialogHelper_AboutBox (int resID = kLedStdDlg_AboutBoxID);
			#elif	qWindows
				Led_StdDialogHelper_AboutBox (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_AboutBoxID));
			#elif	qXWindows && qUseGTKForLedStandardDialogs
				Led_StdDialogHelper_AboutBox (GtkWindow* parentWindow);
			#endif

		#if		qMacOS
			protected:
				override	void	PreDoModalHook ();
				
				nonvirtual	void	SimpleLayoutHelper (short pictHeight, short pictWidth, Led_Rect infoField, Led_Rect webPageField, const Led_SDK_String versionStr);
		#endif

			#if		qXWindows
			override	GtkWidget*	MakeWindow ();
			#endif

			#if		qMacOS
			protected:
				override	bool	EventFilter (DialogPtr dialog, EventRecord* eventRecord, short* itemHit);
			#endif

			protected:
			#if		qMacOS
				override	bool	HandleCommandClick (int itemNum);
			#elif	qWindows
				override	BOOL	DialogProc (UINT message, WPARAM wParam, LPARAM lParam);
			#endif

			public:
				virtual	void	OnClickInInfoField ();
				virtual	void	OnClickInLedWebPageField ();
		};
	#endif

#endif





#ifndef	qSupportStdFindDlg
#define	qSupportStdFindDlg		qMacOS || qWindows || qXWindows
#endif

#if		qSupportStdFindDlg
	#define kLedStdDlg_FindBoxID					0x1002
		#define	kLedStdDlg_FindBox_FindText				(kLedStdDlgCommandBase+1)
		#define	kLedStdDlg_FindBox_WrapAtEndOfDoc		(kLedStdDlgCommandBase+2)
		#define	kLedStdDlg_FindBox_WholeWord			(kLedStdDlgCommandBase+3)
		#define	kLedStdDlg_FindBox_IgnoreCase			(kLedStdDlgCommandBase+4)
		#define	kLedStdDlg_FindBox_Find					(kLedStdDlgCommandBase+5)
		#define	kLedStdDlg_FindBox_Cancel				(kLedStdDlgCommandBase+6)


	#if		defined (__cplusplus)
		/*
		@CLASS:			Led_StdDialogHelper_FindDialog
		@DESCRIPTION:	<p>You can define qSupportStdFindDlg to 0 to disable inclusion of this resource/code (for size reasons).</p>
		*/
		class	Led_StdDialogHelper_FindDialog : public Led_StdDialogHelper {
			private:
				typedef	Led_StdDialogHelper	inherited;
			public:
			#if		qMacOS
				Led_StdDialogHelper_FindDialog (int resID = kLedStdDlg_FindBoxID);
			#elif	qWindows
				Led_StdDialogHelper_FindDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_FindBoxID));
			#elif	qXWindows && qUseGTKForLedStandardDialogs
				Led_StdDialogHelper_FindDialog (GtkWindow* parentWindow);
			#endif

			protected:
			#if		qMacOS
				override	bool	HandleCommandClick (int itemNum);
			#elif	qWindows
				override	BOOL	DialogProc (UINT message, WPARAM wParam, LPARAM lParam);
			#endif

			public:
				Led_tString			fFindText;
				vector<Led_tString>	fRecentFindTextStrings;
				bool				fWrapSearch;
				bool				fWholeWordSearch;
				bool				fCaseSensativeSearch;
				bool				fPressedOK;

			protected:
				#if		qSupportLedDialogWidgets
					LedComboBoxWidget	fFindTextWidget;
				#endif

			protected:
				override	void	PreDoModalHook ();

			public:
				virtual	void	OnFindButton ();
				virtual	void	OnDontFindButton ();

			#if		qXWindows && qUseGTKForLedStandardDialogs
			private:
				GtkWidget*	fLookupTextWidget;
			private:
				static	void	Static_OnFindButtonClick (GtkWidget* widget, gpointer data);
				static	void	Static_OnDontFindButtonClick (GtkWidget* widget, gpointer data);
			#endif
		};
	#endif
#endif





#ifndef	qSupportStdReplaceDlg
#define	qSupportStdReplaceDlg		qMacOS || qWindows || qXWindows
#endif

#if		qSupportStdReplaceDlg
	#define kLedStdDlg_ReplaceBoxID					0x1003
		#define	kLedStdDlg_ReplaceBox_FindText				(kLedStdDlgCommandBase+1)
		#define	kLedStdDlg_ReplaceBox_ReplaceText			(kLedStdDlgCommandBase+2)
		#define	kLedStdDlg_ReplaceBox_WrapAtEndOfDoc		(kLedStdDlgCommandBase+3)
		#define	kLedStdDlg_ReplaceBox_WholeWord				(kLedStdDlgCommandBase+4)
		#define	kLedStdDlg_ReplaceBox_IgnoreCase			(kLedStdDlgCommandBase+5)
		#define	kLedStdDlg_ReplaceBox_Find					(kLedStdDlgCommandBase+6)
		#define	kLedStdDlg_ReplaceBox_Cancel				(kLedStdDlgCommandBase+7)
		#define	kLedStdDlg_ReplaceBox_Replace				(kLedStdDlgCommandBase+8)
		#define	kLedStdDlg_ReplaceBox_ReplaceAll			(kLedStdDlgCommandBase+9)
		#define	kLedStdDlg_ReplaceBox_ReplaceAllInSelection	(kLedStdDlgCommandBase+10)


	#if		defined (__cplusplus)
		/*
		@CLASS:			Led_StdDialogHelper_ReplaceDialog
		@DESCRIPTION:	<p>You can define qSupportStdReplaceDlg to 0 to disable inclusion of this resource/code (for size reasons).</p>
		*/
		class	Led_StdDialogHelper_ReplaceDialog : public Led_StdDialogHelper {
			private:
				typedef	Led_StdDialogHelper	inherited;
			public:
			#if		qMacOS
				Led_StdDialogHelper_ReplaceDialog (int resID = kLedStdDlg_ReplaceBoxID);
			#elif	qWindows
				Led_StdDialogHelper_ReplaceDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_ReplaceBoxID));
			#elif	qXWindows && qUseGTKForLedStandardDialogs
				Led_StdDialogHelper_ReplaceDialog (GtkWindow* parentWindow);
			#endif

			protected:
			#if		qMacOS
				override	bool	HandleCommandClick (int itemNum);
			#elif	qWindows
				override	BOOL	DialogProc (UINT message, WPARAM wParam, LPARAM lParam);
			#endif

			public:
				Led_tString			fFindText;
				vector<Led_tString>	fRecentFindTextStrings;
				Led_tString			fReplaceText;
				bool				fWrapSearch;
				bool				fWholeWordSearch;
				bool				fCaseSensativeSearch;
			public:
				enum	ButtonPressed { eCancel, eFind, eReplace, eReplaceAll, eReplaceAllInSelection };
				ButtonPressed		fPressed;

			protected:
				#if		qSupportLedDialogWidgets
					LedComboBoxWidget	fFindTextWidget;
					LedDialogWidget		fReplaceTextWidget;
				#endif

			protected:
				override	void	PreDoModalHook ();

			public:
				virtual	void	OnFindButton ();
				virtual	void	OnReplaceButton ();
				virtual	void	OnReplaceAllButton ();
				virtual	void	OnReplaceAllInSelectionButton ();
				virtual	void	OnDontFindButton ();

			protected:
				virtual	void	SaveItems ();

			#if		qXWindows && qUseGTKForLedStandardDialogs
			private:
				GtkWidget*	fLookupTextWidget;
				GtkWidget*	fReplaceTextWidget;
			private:
				static	void	Static_OnFindButtonClick (GtkWidget* widget, gpointer data);
				static	void	Static_OnDontFindButtonClick (GtkWidget* widget, gpointer data);
				static	void	Static_OnReplaceButtonClick (GtkWidget* widget, gpointer data);
				static	void	Static_OnReplaceAllButtonClick (GtkWidget* widget, gpointer data);
				static	void	Static_OnReplaceAllInSelectionButtonClick (GtkWidget* widget, gpointer data);
			#endif
		};
	#endif
#endif





#if		qUseGTKForLedStandardDialogs && qXWindows && defined (__cplusplus)
/*
@CLASS:			StdFontPickBox
@DESCRIPTION:	<p>XWindows only. You can define qUseGTKForLedStandardDialogs to 0 to disable inclusion of this resource/code (for size reasons).</p>
*/
class	StdFontPickBox : public Led_StdDialogHelper {
	private:
		typedef	Led_StdDialogHelper	inherited;

	public:
		StdFontPickBox (GtkWindow* modalParentWindow, const Led_FontSpecification& initialFont);

	public:
		override	GtkWidget*	MakeWindow ();
		override	void	PreDoModalHook ();
	public:
		override	void	OnOK ();

	public:
		Led_FontSpecification	fFont;
};
#endif




#ifndef	qSupportStdColorPickBox
#define	qSupportStdColorPickBox		(qMacOS || qWindows || (qUseGTKForLedStandardDialogs && qXWindows))
#endif

#if		qSupportStdColorPickBox && defined (__cplusplus)
/*
@CLASS:			StdColorPickBox
@DESCRIPTION:	<p>Only defined if @'qSupportStdColorPickBox' defined. For XWindows, we do our own dialog box using Gtk, but
			for MacOS, and Win32, this doesn't even use @'Led_StdDialogHelper' - but instead - uses other SDK calls to
			display the dialog.</p>
				<p>Either way - you still invoke it with a DoModal () call and consider that OK was pressed and
			the fColor field filled in if DoModal returns true.
			</p>
*/
class	StdColorPickBox
	#if		qUseGTKForLedStandardDialogs && qXWindows
		: public Led_StdDialogHelper
	#endif
	{
	#if		qUseGTKForLedStandardDialogs && qXWindows
	private:
		typedef	Led_StdDialogHelper	inherited;
	#endif

	public:
		#if		qMacOS
			StdColorPickBox (const Led_Color& initialColor);
		#elif	qWindows
			StdColorPickBox (const Led_Color& initialColor);
			StdColorPickBox (HINSTANCE hInstance, HWND parentWnd, const Led_Color& initialColor);
		#elif	qXWindows && qUseGTKForLedStandardDialogs
			StdColorPickBox (GtkWindow* modalParentWindow, const Led_Color& initialColor);
		#endif

	#if		qWindows
	private:
		HWND	fParentWnd;
	#endif

	#if		qMacOS || qWindows
	public:
		virtual		bool	DoModal ();
	#endif

	#if		qUseGTKForLedStandardDialogs && qXWindows
	public:
		override	GtkWidget*	MakeWindow ();
		override	void	PreDoModalHook ();

	public:
		override	void	OnOK ();
	#endif

	#if		qWindows
	private:
		static	UINT CALLBACK	ColorPickerINITPROC (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	#endif

	public:
		Led_Color	fColor;
};
#endif





#ifndef	qSupportStdFileDlg
#define	qSupportStdFileDlg		qXWindows
#endif


#if		qSupportStdFileDlg && defined (__cplusplus)
/*
@CLASS:			StdFilePickBox
@DESCRIPTION:	<p>XWindows only.</p>
*/
class	StdFilePickBox : public Led_StdDialogHelper {
	private:
		typedef	Led_StdDialogHelper	inherited;

	public:
		StdFilePickBox (GtkWindow* modalParentWindow, const Led_SDK_String& title, bool saveDialog, const Led_SDK_String& fileName);

	public:
		override	GtkWidget*	MakeWindow ();
		override	void	PreDoModalHook ();
	public:
		override	void	OnOK ();

	public:
		nonvirtual	Led_SDK_String	GetFileName () const;

	protected:
		Led_SDK_String	fTitle;
		bool			fSaveDialog;
		Led_SDK_String	fFileName;
};
#endif




#ifndef	qSupportUpdateWin32FileAssocDlg
#define	qSupportUpdateWin32FileAssocDlg		qWindows
#endif

#if		qSupportUpdateWin32FileAssocDlg
	#define kLedStdDlg_UpdateWin32FileAssocsDialogID					0x1004
		#define	kLedStdDlg_UpdateWin32FileAssocsDialog_Msg							(kLedStdDlgCommandBase+1)
		#define	kLedStdDlg_UpdateWin32FileAssocsDialog_KeepCheckingCheckboxMsg		(kLedStdDlgCommandBase+2)

	#if		defined (__cplusplus)
		/*
		@CLASS:			Led_StdDialogHelper_UpdateWin32FileAssocsDialog
		@DESCRIPTION:	<p>Windows only.</p>
		*/
		class	Led_StdDialogHelper_UpdateWin32FileAssocsDialog : public Led_StdDialogHelper {
			private:
				typedef	Led_StdDialogHelper	inherited;
			public:
				Led_StdDialogHelper_UpdateWin32FileAssocsDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_UpdateWin32FileAssocsDialogID));

			public:
				Led_SDK_String	fAppName;
				Led_SDK_String	fTypeList;
				bool			fKeepChecking;

			protected:
				override	void	PreDoModalHook ();
			public:
				override	void	OnOK ();
				override	void	OnCancel ();
		};
	#endif
#endif








#ifndef	qSupportParagraphIndentsDlg
#define	qSupportParagraphIndentsDlg		qMacOS || qWindows
#endif

#if		qSupportParagraphIndentsDlg
	#define kLedStdDlg_ParagraphIndentsID					0x1005
		#define	kLedStdDlg_ParagraphIndents_LeftMarginFieldID		(kLedStdDlgCommandBase+4)
		#define	kLedStdDlg_ParagraphIndents_RightMarginFieldID		(kLedStdDlgCommandBase+6)
		#define	kLedStdDlg_ParagraphIndents_FirstIndentFieldID		(kLedStdDlgCommandBase+8)

	#if		defined (__cplusplus)
		/*
		@CLASS:			Led_StdDialogHelper_ParagraphIndentsDialog
		@DESCRIPTION:	<p>You can define qSupportParagraphIndentsDlg to exlude this from your build (for size reasons)</p>
		*/
		class	Led_StdDialogHelper_ParagraphIndentsDialog : public Led_StdDialogHelper {
			private:
				typedef	Led_StdDialogHelper	inherited;
			public:
			#if		qMacOS
				Led_StdDialogHelper_ParagraphIndentsDialog (int resID = kLedStdDlg_ParagraphIndentsID);
			#elif	qWindows
				Led_StdDialogHelper_ParagraphIndentsDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_ParagraphIndentsID));
			#endif

			public:
				virtual	void	InitValues (Led_TWIPS leftMargin, bool leftMarginValid, Led_TWIPS rightMargin, bool rightMarginValid, Led_TWIPS firstIndent, bool firstIndentValid);

			protected:
				override	void	PreDoModalHook ();
			public:
				override	void	OnOK ();

			public:
				bool			fLeftMargin_Valid;
				Led_TWIPS		fLeftMargin_Orig;
				Led_TWIPS		fLeftMargin_Result;
				bool			fRightMargin_Valid;
				Led_TWIPS		fRightMargin_Orig;
				Led_TWIPS		fRightMargin_Result;
				bool			fFirstIndent_Valid;
				Led_TWIPS		fFirstIndent_Orig;
				Led_TWIPS		fFirstIndent_Result;
		};
	#endif
#endif







#ifndef	qSupportParagraphSpacingDlg
#define	qSupportParagraphSpacingDlg		qMacOS || qWindows
#endif

#if		qSupportParagraphSpacingDlg
	#define kLedStdDlg_ParagraphSpacingID					0x1006
		#define	kParagraphSpacing_Dialog_SpaceBeforeFieldID			(kLedStdDlgCommandBase+4)
		#define	kParagraphSpacing_Dialog_SpaceAfterFieldID			(kLedStdDlgCommandBase+6)
		#define	kParagraphSpacing_Dialog_LineSpaceModeFieldID		(kLedStdDlgCommandBase+7)
		#define	kParagraphSpacing_Dialog_LineSpaceArgFieldID		(kLedStdDlgCommandBase+8)

		#if		qMacOS
			#define	kParagraphSpacing_Dialog_LineSpaceMENUID			(kLedStdDlgMENUBase+1)
			#define	kParagraphSpacing_Dialog_LineSpaceCNTLID			(kLedStdDlgCNTLBase+1)
		#endif

	#if		defined (__cplusplus)
		/*
		@CLASS:			Led_StdDialogHelper_ParagraphSpacingDialog
		@DESCRIPTION:	<p>You can define qSupportParagraphSpacingDlg to exlude this from your build (for size reasons)</p>
		*/
		class	Led_StdDialogHelper_ParagraphSpacingDialog : public Led_StdDialogHelper {
			private:
				typedef	Led_StdDialogHelper	inherited;
			public:
			#if		qMacOS
				Led_StdDialogHelper_ParagraphSpacingDialog (int resID = kLedStdDlg_ParagraphSpacingID);
			#elif	qWindows
				Led_StdDialogHelper_ParagraphSpacingDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_ParagraphSpacingID));
			#endif

			public:
				virtual	void	InitValues (Led_TWIPS spaceBefore, bool spaceBeforeValid, Led_TWIPS spaceAfter, bool spaceAfterValid, Led_LineSpacing lineSpacing, bool lineSpacingValid);

			protected:
				override	void	PreDoModalHook ();
			public:
				override	void	OnOK ();

			public:
				bool			fSpaceBefore_Valid;
				Led_TWIPS		fSpaceBefore_Orig;
				Led_TWIPS		fSpaceBefore_Result;
				bool			fSpaceAfter_Valid;
				Led_TWIPS		fSpaceAfter_Orig;
				Led_TWIPS		fSpaceAfter_Result;
				bool			fLineSpacing_Valid;
				Led_LineSpacing	fLineSpacing_Orig;
				Led_LineSpacing	fLineSpacing_Result;
		};
	#endif
#endif





#ifndef	qSupportOtherFontSizeDlg
#define	qSupportOtherFontSizeDlg		qMacOS || qWindows
#endif

#if		qSupportOtherFontSizeDlg
	#define kLedStdDlg_OtherFontSizeID					0x1007
		#define	kOtherFontSize_Dialog_FontSizeEditFieldID	(kLedStdDlgCommandBase+4)


	#if		defined (__cplusplus)
		/*
		@CLASS:			Led_StdDialogHelper_OtherFontSizeDialog
		@DESCRIPTION:	<p>You can define qSupportOtherFontSizeDlg to exlude this from your build (for size reasons)</p>
		*/
		class	Led_StdDialogHelper_OtherFontSizeDialog : public Led_StdDialogHelper {
			private:
				typedef	Led_StdDialogHelper	inherited;
			public:
			#if		qMacOS
				Led_StdDialogHelper_OtherFontSizeDialog (int resID = kLedStdDlg_OtherFontSizeID);
			#elif	qWindows
				Led_StdDialogHelper_OtherFontSizeDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_OtherFontSizeID));
			#endif

			public:
				virtual	void	InitValues (Led_Distance origFontSize);

			protected:
				override	void	PreDoModalHook ();
			public:
				override	void	OnOK ();

			public:
				Led_Distance	fFontSize_Orig;
				Led_Distance	fFontSize_Result;
		};
	#endif
#endif







#ifndef	qSupportUnknownEmbeddingInfoDlg
#define	qSupportUnknownEmbeddingInfoDlg		qMacOS || qWindows
#endif

#if		qSupportUnknownEmbeddingInfoDlg
	#define kLedStdDlg_UnknownEmbeddingInfoBoxID					0x1008
		#define	kLedStdDlg_UnknownEmbeddingInfoBox_TypeTextMsg				(kLedStdDlgCommandBase+1)


	#if		defined (__cplusplus)
		/*
		@CLASS:			Led_StdDialogHelper_UnknownEmbeddingInfoDialog
		@DESCRIPTION:	<p>You can define qSupportUnknownEmbeddingInfoDlg to exlude this from your build (for size reasons)</p>
		*/
		class	Led_StdDialogHelper_UnknownEmbeddingInfoDialog : public Led_StdDialogHelper {
			private:
				typedef	Led_StdDialogHelper	inherited;
			public:
			#if		qMacOS
				Led_StdDialogHelper_UnknownEmbeddingInfoDialog (int resID = kLedStdDlg_UnknownEmbeddingInfoBoxID);
			#elif	qWindows
				Led_StdDialogHelper_UnknownEmbeddingInfoDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_UnknownEmbeddingInfoBoxID));
			#elif	qXWindows && qUseGTKForLedStandardDialogs
				Led_StdDialogHelper_UnknownEmbeddingInfoDialog (GtkWindow* parentWindow);
			#endif

			public:
				Led_SDK_String	fEmbeddingTypeName;

			protected:
				override	void	PreDoModalHook ();
		};
	#endif
#endif









#ifndef	qSupportURLXEmbeddingInfoDlg
#define	qSupportURLXEmbeddingInfoDlg		qMacOS || qWindows
#endif

#if		qSupportURLXEmbeddingInfoDlg
	#define kLedStdDlg_URLXEmbeddingInfoBoxID					0x1009
		#define	kLedStdDlg_URLXEmbeddingInfoBox_TypeTextMsg				(kLedStdDlgCommandBase+1)
		#define	kLedStdDlg_URLXEmbeddingInfoBox_TitleText				(kLedStdDlgCommandBase+4)
		#define	kLedStdDlg_URLXEmbeddingInfoBox_URLText					(kLedStdDlgCommandBase+6)


	#if		defined (__cplusplus)
		/*
		@CLASS:			Led_StdDialogHelper_URLXEmbeddingInfoDialog
		@DESCRIPTION:	<p>You can define qSupportURLXEmbeddingInfoDlg to exlude this from your build (for size reasons)</p>
		*/
		class	Led_StdDialogHelper_URLXEmbeddingInfoDialog : public Led_StdDialogHelper {
			private:
				typedef	Led_StdDialogHelper	inherited;
			public:
			#if		qMacOS
				Led_StdDialogHelper_URLXEmbeddingInfoDialog (int resID = kLedStdDlg_URLXEmbeddingInfoBoxID);
			#elif	qWindows
				Led_StdDialogHelper_URLXEmbeddingInfoDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_URLXEmbeddingInfoBoxID));
			#elif	qXWindows && qUseGTKForLedStandardDialogs
				Led_StdDialogHelper_URLXEmbeddingInfoDialog (GtkWindow* parentWindow);
			#endif

			public:
				Led_SDK_String	fEmbeddingTypeName;
				Led_SDK_String	fTitleText;
				Led_SDK_String	fURLText;

			protected:
				override	void	PreDoModalHook ();

			public:
				override	void	OnOK ();

			#if		qXWindows && qUseGTKForLedStandardDialogs
			private:
				GtkWidget*	fTitleTextWidget;
				GtkWidget*	fURLTextWidget;
			#endif
		};
	#endif



	#define kLedStdDlg_AddURLXEmbeddingInfoBoxID					0x100a
		#define	kLedStdDlg_AddURLXEmbeddingInfoBox_TitleText				(kLedStdDlgCommandBase+4)
		#define	kLedStdDlg_AddURLXEmbeddingInfoBox_URLText					(kLedStdDlgCommandBase+6)


	#if		defined (__cplusplus)
		/*
		@CLASS:			Led_StdDialogHelper_AddURLXEmbeddingInfoDialog
		@DESCRIPTION:	<p>You can define qSupportURLXEmbeddingInfoDlg to exlude this from your build (for size reasons)</p>
		*/
		class	Led_StdDialogHelper_AddURLXEmbeddingInfoDialog : public Led_StdDialogHelper {
			private:
				typedef	Led_StdDialogHelper	inherited;
			public:
			#if		qMacOS
				Led_StdDialogHelper_AddURLXEmbeddingInfoDialog (int resID = kLedStdDlg_AddURLXEmbeddingInfoBoxID);
			#elif	qWindows
				Led_StdDialogHelper_AddURLXEmbeddingInfoDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_AddURLXEmbeddingInfoBoxID));
			#elif	qXWindows && qUseGTKForLedStandardDialogs
				Led_StdDialogHelper_AddURLXEmbeddingInfoDialog (GtkWindow* parentWindow);
			#endif

			public:
				Led_SDK_String	fTitleText;
				Led_SDK_String	fURLText;

			protected:
				override	void	PreDoModalHook ();

			public:
				override	void	OnOK ();

			#if		qXWindows && qUseGTKForLedStandardDialogs
			private:
				GtkWidget*	fTitleTextWidget;
				GtkWidget*	fURLTextWidget;
			#endif
		};
	#endif
#endif







#ifndef	qSupportAddNewTableDlg
#define	qSupportAddNewTableDlg		qWindows || qMacOS
#endif

#if		qSupportAddNewTableDlg
	#define kLedStdDlg_AddNewTableBoxID					0x100b
		#define	kLedStdDlg_AddNewTableBox_RowCount				(kLedStdDlgCommandBase+5)
		#define	kLedStdDlg_AddNewTableBox_ColCount				(kLedStdDlgCommandBase+7)


	#if		defined (__cplusplus)
		/*
		@CLASS:			Led_StdDialogHelper_AddNewTableDialog
		@DESCRIPTION:	<p>You can define qSupportAddNewTableDlg to exlude this from your build (for size reasons)</p>
		*/
		class	Led_StdDialogHelper_AddNewTableDialog : public Led_StdDialogHelper {
			private:
				typedef	Led_StdDialogHelper	inherited;
			public:
			#if		qMacOS
				Led_StdDialogHelper_AddNewTableDialog (int resID = kLedStdDlg_AddNewTableBoxID);
			#elif	qWindows
				Led_StdDialogHelper_AddNewTableDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_AddNewTableBoxID));
			#elif	qXWindows && qUseGTKForLedStandardDialogs
				Led_StdDialogHelper_AddNewTableDialog (GtkWindow* parentWindow);
			#endif

			public:
				size_t	fRows;
				size_t	fColumns;

			protected:
				override	void	PreDoModalHook ();

			public:
				override	void	OnOK ();
		};
	#endif
#endif








#ifndef	qSupportEditTablePropertiesDlg
#define	qSupportEditTablePropertiesDlg	qWindows || qMacOS
#endif

#if		qSupportEditTablePropertiesDlg
	#define kLedStdDlg_EditTablePropertiesBoxID					0x100c
		#define	kLedStdDlg_EditTablePropertiesBox_CellMarginTop					(kLedStdDlgCommandBase+10)
		#define	kLedStdDlg_EditTablePropertiesBox_CellMarginLeft				(kLedStdDlgCommandBase+12)
		#define	kLedStdDlg_EditTablePropertiesBox_CellMarginBottom				(kLedStdDlgCommandBase+14)
		#define	kLedStdDlg_EditTablePropertiesBox_CellMarginRight				(kLedStdDlgCommandBase+16)

		#define	kLedStdDlg_EditTablePropertiesBox_DefaultCellSpacing			(kLedStdDlgCommandBase+18)

		#define	kLedStdDlg_EditTablePropertiesBox_CellBackgroundColor			(kLedStdDlgCommandBase+23)

		#define	kLedStdDlg_EditTablePropertiesBox_ColumnWidth					(kLedStdDlgCommandBase+21)

		#define	kLedStdDlg_EditTablePropertiesBox_BorderWidth					(kLedStdDlgCommandBase+6)
		#define	kLedStdDlg_EditTablePropertiesBox_BorderColor					(kLedStdDlgCommandBase+7)

		#if		qMacOS
			#define	kLedStdDlg_EditTablePropertiesBox_BorderColorMENUID			(kLedStdDlgMENUBase+2)
			#define	kLedStdDlg_EditTablePropertiesBox_BorderColorCNTLID			(kLedStdDlgCNTLBase+2)
			
			#define	kLedStdDlg_EditTablePropertiesBox_CellBackgroundColorMENUID	(kLedStdDlgMENUBase+3)
			#define	kLedStdDlg_EditTablePropertiesBox_CellBackgroundColorCNTLID	(kLedStdDlgCNTLBase+3)
		#endif


	#if		defined (__cplusplus)
		/*
		@CLASS:			Led_StdDialogHelper_EditTablePropertiesDialog
		@DESCRIPTION:	<p>You can define qSupportEditTablePropertiesDlg to exlude this from your build (for size reasons)</p>
		*/
		class	Led_StdDialogHelper_EditTablePropertiesDialog : public Led_StdDialogHelper {
			private:
				typedef	Led_StdDialogHelper	inherited;
			public:
			#if		qMacOS
				Led_StdDialogHelper_EditTablePropertiesDialog (int resID = kLedStdDlg_EditTablePropertiesBoxID);
			#elif	qWindows
				Led_StdDialogHelper_EditTablePropertiesDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_EditTablePropertiesBoxID));
			#elif	qXWindows && qUseGTKForLedStandardDialogs
				Led_StdDialogHelper_EditTablePropertiesDialog (GtkWindow* parentWindow);
			#endif

			public:
				struct	Info {
					Info ();

					Led_TWIPS		fTableBorderWidth;
					Led_Color		fTableBorderColor;

					Led_TWIPS_Rect	fDefaultCellMargins;
					Led_TWIPS		fCellSpacing;

					bool			fCellWidth_Common;
					Led_TWIPS		fCellWidth;

					bool			fCellBackgroundColor_Common;
					Led_Color		fCellBackgroundColor;
				};
				Info	fInfo;

			protected:
				override	void	PreDoModalHook ();

				protected:
			#if		qMacOS
				override	bool	HandleCommandClick (int itemNum);
			#elif	qWindows
				override	BOOL	DialogProc (UINT message, WPARAM wParam, LPARAM lParam);
			#endif

			public:
				override	void	OnOK ();

			protected:
				StdColorPopupHelper	fBorderColorPopup;
				StdColorPopupHelper	fCellBackgroundColorPopup;

			public:
				template	<typename T1, typename T2>
					static	void	cvt (typename T1* o, const typename T2& i);
		};

		template	<typename T1, typename T2>
			void	Led_StdDialogHelper_EditTablePropertiesDialog::cvt (typename T1* o, const typename T2& i)
			{
				o->fTableBorderWidth = i.fTableBorderWidth;
				o->fTableBorderColor = i.fTableBorderColor;
				o->fDefaultCellMargins = i.fDefaultCellMargins;
				o->fCellSpacing = i.fCellSpacing;
				o->fCellWidth_Common = i.fCellWidth_Common;
				o->fCellWidth = i.fCellWidth;
				o->fCellBackgroundColor_Common = i.fCellBackgroundColor_Common;
				o->fCellBackgroundColor = i.fCellBackgroundColor;
			}
		#if		qTemplatedMemberFunctionsFailBug
			template	<typename T1, typename T2>
				void	Led_StdDialogHelper_EditTablePropertiesDialog_cvt (typename T1* o, const typename T2& i)
				{
					o->fTableBorderWidth = i.fTableBorderWidth;
					o->fTableBorderColor = i.fTableBorderColor;
					o->fDefaultCellMargins = i.fDefaultCellMargins;
					o->fCellSpacing = i.fCellSpacing;
					o->fCellWidth_Common = i.fCellWidth_Common;
					o->fCellWidth = i.fCellWidth;
					o->fCellBackgroundColor_Common = i.fCellBackgroundColor_Common;
					o->fCellBackgroundColor = i.fCellBackgroundColor;
				}
		#endif
		inline	Led_StdDialogHelper_EditTablePropertiesDialog::Info::Info ():
			fTableBorderWidth (Led_TWIPS (0)),
			fTableBorderColor (Led_Color::kWhite),
			fDefaultCellMargins (),
			fCellSpacing (Led_TWIPS (0)),
			fCellWidth_Common (false),
			fCellWidth (Led_TWIPS (0)),
			fCellBackgroundColor_Common (false),
			fCellBackgroundColor (Led_Color::kWhite)
			{
			}
	#endif
#endif



#if		defined (qTemplatesWithMixOfNestedClassesWithCommonNamesGetsConfusedBug)
	#if		qTemplatesWithMixOfNestedClassesWithCommonNamesGetsConfusedBug
		#define	qSupportStdSpellCheckDlg	0
	#endif
#endif


#ifndef	qSupportStdSpellCheckDlg
#define	qSupportStdSpellCheckDlg		qMacOS || qWindows || qXWindows
#endif

#if		qSupportStdSpellCheckDlg
	#define kLedStdDlg_SpellCheckBoxID					0x100d
		#define	kLedStdDlg_SpellCheckBox_UnknownWordText		(kLedStdDlgCommandBase+1)
		#define	kLedStdDlg_SpellCheckBox_ChangeText				(kLedStdDlgCommandBase+2)
		#define	kLedStdDlg_SpellCheckBox_SuggestedList			(kLedStdDlgCommandBase+3)
		#define	kLedStdDlg_SpellCheckBox_IgnoreOnce				(kLedStdDlgCommandBase+4)
		#define	kLedStdDlg_SpellCheckBox_IgnoreAll				(kLedStdDlgCommandBase+5)
		#define	kLedStdDlg_SpellCheckBox_ChangeOnce				(kLedStdDlgCommandBase+6)
		#define	kLedStdDlg_SpellCheckBox_ChangeAll				(kLedStdDlgCommandBase+7)
		#define	kLedStdDlg_SpellCheckBox_AddDictionary			(kLedStdDlgCommandBase+8)
		#define	kLedStdDlg_SpellCheckBox_LookupOnWeb			(kLedStdDlgCommandBase+9)
		#define	kLedStdDlg_SpellCheckBox_Options				(kLedStdDlgCommandBase+10)
		#define	kLedStdDlg_SpellCheckBox_Close					(kLedStdDlgCommandBase+11)

		#if             qMacOS
			#define kLedStdDlg_SpellCheckBox_SuggestionsCNTLID		(kLedStdDlgCNTLBase+4)
		#endif

	#if		defined (__cplusplus)
		/*
		@CLASS:			Led_StdDialogHelper_SpellCheckDialog
		@DESCRIPTION:	<p>You can define qSupportStdSpellCheckDlg to 0 to disable inclusion of this resource/code (for size reasons).</p>
		*/
		class	Led_StdDialogHelper_SpellCheckDialog : public Led_StdDialogHelper {
			private:
				typedef	Led_StdDialogHelper	inherited;
			public:
				class	SpellCheckDialogCallback;
				struct	MisspellingInfo;

			public:
			#if		qMacOS
				Led_StdDialogHelper_SpellCheckDialog (SpellCheckDialogCallback& callback, int resID = kLedStdDlg_SpellCheckBoxID);
			#elif	qWindows
				Led_StdDialogHelper_SpellCheckDialog (SpellCheckDialogCallback& callback, HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_SpellCheckBoxID));
			#elif	qXWindows && qUseGTKForLedStandardDialogs
				Led_StdDialogHelper_SpellCheckDialog (SpellCheckDialogCallback& callback, GtkWindow* parentWindow);
			#endif
				~Led_StdDialogHelper_SpellCheckDialog ();

			protected:
				SpellCheckDialogCallback&	fCallback;

			protected:
				MisspellingInfo*	fCurrentMisspellInfo;

			protected:
			#if		qMacOS
				override	bool	HandleCommandClick (int itemNum);
			#elif	qWindows
				override	BOOL	DialogProc (UINT message, WPARAM wParam, LPARAM lParam);
			#endif

			protected:
				#if		qSupportLedDialogWidgets
					LedDialogWidget	fUndefinedWordWidget;
					LedDialogWidget	fChangeTextWidget;
				#endif

			protected:
				override	void	PreDoModalHook ();

			public:
				virtual	void	OnIgnoreButton ();
				virtual	void	OnIgnoreAllButton ();
				virtual	void	OnChangeButton ();
				virtual	void	OnChangeAllButton ();
				virtual	void	OnAddToDictionaryButton ();
				virtual	void	OnLookupOnWebButton ();
				virtual	void	OnOptionsDialogButton ();
				virtual	void	OnCloseButton ();
				virtual	void	OnSuggestionListChangeSelection ();
				virtual	void	OnSuggestionListDoubleClick ();

			protected:
				virtual	void	DoFindNextCall ();

			#if		qXWindows && qUseGTKForLedStandardDialogs
			private:
				GtkWidget*	fLookupTextWidget;
				GtkWidget*	fChangeTextWidget;
			private:
				static	void	Static_OnIgnoreButtonClick (GtkWidget* widget, gpointer data);
				static	void	Static_OnIgnoreAllButtonClick (GtkWidget* widget, gpointer data);
				static	void	Static_OnChangeButtonClick (GtkWidget* widget, gpointer data);
				static	void	Static_OnChangeAllButtonClick (GtkWidget* widget, gpointer data);
				static	void	Static_OnAddToDictionaryButtonClick (GtkWidget* widget, gpointer data);
				static	void	Static_OnLookupOnWebButtonClick (GtkWidget* widget, gpointer data);
				static	void	Static_OnOptionsDialogButtonClick (GtkWidget* widget, gpointer data);
				static	void	Static_OnCloseButtonClick (GtkWidget* widget, gpointer data);
			#endif

		public:
				template	<typename DEL>
					class	CallbackDelegator;
		};


struct	Led_StdDialogHelper_SpellCheckDialog::MisspellingInfo {
	Led_tString			fUndefinedWord;
	vector<Led_tString>	fSuggestions;
};

/*
@CLASS:			Led_StdDialogHelper_SpellCheckDialog::SpellCheckDialogCallback
@DESCRIPTION:	<p>This interface is called by the actual spellcheck dialog implematation back to the implementer
			of the real spellchecking functionality.</p>
*/
class	Led_StdDialogHelper_SpellCheckDialog::SpellCheckDialogCallback {
	public:
		typedef	Led_StdDialogHelper_SpellCheckDialog::MisspellingInfo	MisspellingInfo;

	public:
		virtual	MisspellingInfo*	GetNextMisspelling ()				=	0;

	public:
		virtual	void	DoIgnore ()										=	0;
		virtual	void	DoIgnoreAll ()									=	0;
		virtual	void	DoChange (const Led_tString& changeTo)			=	0;
		virtual	void	DoChangeAll (const Led_tString& changeTo)		=	0;
		virtual	bool	AddToDictionaryEnabled () const					=	0;
		virtual	void	AddToDictionary (const Led_tString& newWord)	=	0;
		virtual	void	LookupOnWeb (const Led_tString& word)			=	0;
		virtual	bool	OptionsDialogEnabled () const					=	0;
		virtual	void	OptionsDialog ()								=	0;
};

template	<typename DEL>
	class	Led_StdDialogHelper_SpellCheckDialog::CallbackDelegator : public Led_StdDialogHelper_SpellCheckDialog::SpellCheckDialogCallback {
		private:
			typedef	SpellCheckDialogCallback	inherited;
		public:
			CallbackDelegator (DEL& del):
				inherited (),
				fDelegate (del)
				{
				}

		public:
			override	MisspellingInfo*	GetNextMisspelling ()
				{
					// This is the line that confuses MSVC60 - qTemplatesWithMixOfNestedClassesWithCommonNamesGetsConfusedBug
					typename DEL::MisspellingInfo*	delResult	=	fDelegate.GetNextMisspelling ();
					if (delResult != NULL) {
						MisspellingInfo*	result	=	new MisspellingInfo;
						result->fUndefinedWord = delResult->fUndefinedWord;
						result->fSuggestions = delResult->fSuggestions;
						delete delResult;
						return result;
					}
					return NULL;
				}

		public:
			override	void	DoIgnore ()											{	fDelegate.DoIgnore ();	}
			override	void	DoIgnoreAll ()										{	fDelegate.DoIgnoreAll ();	}
			override	void	DoChange (const Led_tString& changeTo)				{	fDelegate.DoChange (changeTo);	}
			override	void	DoChangeAll (const Led_tString& changeTo)			{	fDelegate.DoChangeAll (changeTo);	}
			override	bool	AddToDictionaryEnabled () const						{	return fDelegate.AddToDictionaryEnabled (); }
			override	void	AddToDictionary (const Led_tString& newWord)		{	fDelegate.AddToDictionary (newWord);	}
			override	void	LookupOnWeb (const Led_tString& word)				{	fDelegate.LookupOnWeb (word);	}
			override	bool	OptionsDialogEnabled () const						{	return fDelegate.OptionsDialogEnabled (); }
			override	void	OptionsDialog ()									{	fDelegate.OptionsDialog ();	}

		private:
			DEL&	fDelegate;
	};
	#endif
#endif









/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

	#if		qSupportLedDialogWidgets && defined (__cplusplus)
		//	class	LedComboBoxWidget
			inline	vector<Led_tString>	LedComboBoxWidget::GetPopupItems () const
				{
					return fPopupItems;
				}
	#endif





#if		defined (__cplusplus) && qLedUsesNamespaces
}
#endif


#endif	/*____Led_StdDialogs__h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
