/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveLedIt/Sources/ActiveLedItControl.cpp,v 2.142 2004/02/11 22:41:41 lewis Exp $
 *
 * Description:
 *		Implementation of the ActiveLedItControl ActiveX Control class.
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: ActiveLedItControl.cpp,v $
 *	Revision 2.142  2004/02/11 22:41:41  lewis
 *	SPR#1576: update: added 'Replace Again' command to LedIt, LedLineItMFC, ActiveLedIt
 *	
 *	Revision 2.141  2004/02/11 01:17:37  lewis
 *	fixed to use _T() for a couple strings where needed
 *	
 *	Revision 2.140  2004/02/10 03:20:03  lewis
 *	SPR#1634: add a few NonIdleContext calls around OLE AUT calls in ActiveLedIt (since they have similar effects to commands). SPR#1631: Used new OnEnterIdle (insead of SpendIdleTime()) to handle updating toolbars
 *	
 *	Revision 2.139  2004/02/09 16:25:10  lewis
 *	SPR#1599: Added qKeepListOfALInstancesForSPR_1599BWA  for MSIE quit without closing last
 *	control bug. For SPR#1641: added assert to check MODULE::m_nObjectCount = 0. Had  to be much
 *	more careful about using OLE_GetXXX functions that return IDispatch*. Instead - do non-OLE_ versions
 *	that return CComPtr<IDispatch> - so we don't get redundant refcounts.
 *	
 *	Revision 2.138  2004/02/08 14:12:09  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.137  2004/02/08 14:09:13  lewis
 *	SPR#1640: fix display of multiple toolbars by creating a window (with toolbar edges) assocaited with
 *	the toolbar list, and get rid of edges with toolbar itself. Had to change some COM apis and way
 *	AL/Toolbarlist interaction/creation worked (make it more like toolbarlist/toolbar interaction).
 *	Seems to work pretty well now
 *	
 *	Revision 2.136  2004/02/07 23:39:00  lewis
 *	a whole bunch of new icons commands for toolbar - for SPR#1639
 *	
 *	Revision 2.135  2004/02/07 21:20:16  lewis
 *	SPR#1627: add IALToolbar::MergeAdd () method - and use it internally, and in a bunch of samples in
 *	javascript. Also - fixed bug with popup menu handling enabling code (hierarchical menus) and tested
 *	that in javascript testing sample
 *	
 *	Revision 2.134  2004/02/07 01:32:41  lewis
 *	SPR#1629: update: Added font-color icon, and associated the icon with the color popup menu. Added
 *	support to ActiveLedIt_IconButtonToolbarElement to make that work. Including - changed ToggleButton
 *	property to a 'ButtonStyle' property - where one style was 'ICON/POPUP MENU'.
 *	
 *	Revision 2.133  2004/02/06 16:12:19  lewis
 *	SPR#1629: Update: Lose private 'width' property for combobox class, and instead - autocompute the
 *	prefered width based on the attached menu. Improved the automatic LAYOUT code so that changes to
 *	the menu should resize the combobox.
 *	
 *	Revision 2.132  2004/02/06 14:34:28  lewis
 *	SPR#1629: updated font-size toolbar combobox item menu to not have a few items
 *	
 *	Revision 2.131  2004/02/06 01:36:03  lewis
 *	SPR#1629: added fairly functional (not perfect) ActiveLedIt_ComboBoxToolbarElement - and used that to
 *	implement first cut at fontname and fontsize combo boxes.
 *	
 *	Revision 2.130  2004/02/04 21:57:08  lewis
 *	SPR#1628: fix ActiveLedItControl::mkIconElement () to avoid handle mem leak
 *	
 *	Revision 2.129  2004/02/04 21:16:16  lewis
 *	SPR#1618: added justification and liststyle icons to standard icon bar (builtins). And allow different
 *	default internal/external cmd nmae s(BuiltinCmd class)
 *	
 *	Revision 2.128  2004/02/03 22:29:31  lewis
 *	SPR#1618: add ToggleButton property to IconButtonToolbarItem, and CommandChecked () method to
 *	ActiveLedIt - to support checked buttons - like BOLD, ITALIC, and UNDERLINE
 *	
 *	Revision 2.127  2004/02/03 14:51:31  lewis
 *	SPR#1618: added bold, italics, underline (fixed paste icon) to toolbar. Fixed SETFOCUS issue clicking on toolbar buttons
 *	
 *	Revision 2.126  2004/02/03 03:39:41  lewis
 *	SPR#1618: added print command to toolbar. Forced fixing a couple bugs - using MFC UnUpdate dispatcher (CCmdUI.DoUpdate)
 *	instead of Led one - because currently some commands - like print - use the MFC dispatch mechanism (could have
 *	redone to juse use Led one). Also - fixed commandname matcher to match properly on kAllCmds list using
 *	INTENRALNAME mapper
 *	
 *	Revision 2.125  2004/02/03 02:23:08  lewis
 *	SPR#1618: Added undo/redo buttons, and separator support
 *	
 *	Revision 2.124  2004/02/02 23:02:38  lewis
 *	SPR#1618: Started CommandEnabled support - and got associated command (to toolbar icon) code working -
 *	so for simple cases - at least - it invokes the command)
 *	
 *	Revision 2.123  2004/02/02 21:15:13  lewis
 *	SPR#1618: Added preliminary toolbar support. A small toolbar with a few icons appears - but no DO command
 *	or UPDATE_ENABLE_DISABLE CMD support yet (and just plain icon cmd items)
 *	
 *	Revision 2.122  2004/01/30 04:36:33  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.121  2004/01/25 16:19:27  lewis
 *	SPR#1617: lose old resource-based contextmenu/accelerators, and use new COM-based accelerator
 *	mechanism (part of new COM-based command support)
 *	
 *	Revision 2.120  2004/01/24 03:50:38  lewis
 *	InvokeCommand () functionality implemented
 *	
 *	Revision 2.119  2004/01/23 23:11:22  lewis
 *	SPR#1617: Added preliminary (largely functional) implementation of OLE automation-based context
 *	menus. Tons of new OLE APIs. Can create context menus from scratch, or built from building blocks
 *	of existing submenus (like font menu).
 *	
 *	Revision 2.118  2003/12/31 03:52:37  lewis
 *	fix to use CA2W instead of CT2W for COM string method names. Needed to compile with FULL UNICODE
 *	
 *	Revision 2.117  2003/12/17 21:26:21  lewis
 *	SPR#1585: Finishe updating ActiveLedit for various ActiveSpelledIt / SpellCheck API changes
 *	(FindWordBreaks, PeekAtTextBReaksUsed, GetUDInterface, AddWOrdToUserDictionary, etc)
 *	
 *	Revision 2.116  2003/12/06 19:08:01  lewis
 *	SPR#1547: handle OLE_Get/SetEnabled calls - even when window not realized (as I had done for SetReadOnly)
 *	
 *	Revision 2.115  2003/11/03 20:18:38  lewis
 *	add Win32ErrorException/HRESULTErrorException catch support
 *	
 *	Revision 2.114  2003/07/15 21:24:44  lewis
 *	SPR#1548: use CLSIDFromProgID - not CLSIDFromProgIDEx (so runs on Win98)
 *	
 *	Revision 2.113  2003/07/07 15:33:09  lewis
 *	SPR#1547: fix OLE_SetReadOnly (and OLE_GetReadOnly) to work better if called before OnCreate call
 *	
 *	Revision 2.112  2003/07/02 21:22:29  lewis
 *	SPR#1546: Supported ReadOnly <PARAM> argument to <OBJECT>
 *	
 *	Revision 2.111  2003/06/12 17:15:46  lewis
 *	SPR#1526: Added COM-based spellchecker support to ActiveLedIt. Now you can access SpellCheck menu command if
 *	there is a COM-spellchecker associated with ActiveLedIt. New property 'SpellChecker' you can get/set to specify
 *	the spell checker. Seems to work as well as with LedIt!
 *	
 *	Revision 2.110  2003/06/03 16:35:21  lewis
 *	SPR#1522: have control goto Active state - rather than UIActive - on OLEVERB_SHOW
 *	
 *	Revision 2.109  2003/06/03 14:42:14  lewis
 *	SPR#1521: added 'SelListStyle' support. Somewhat related - also changed build of
 *	ActiveLedIt to include the _h.h file generated from the IDL for access to enums rather
 *	than redefining them in the ActiveLedItControl sources.
 *	
 *	Revision 2.108  2003/05/30 14:26:16  lewis
 *	SPR#1517: Added find/replace dialog and OLE_FindReplace and OLE_LaunchReplaceDialog new
 *	OLE AUTOMATION methods
 *	
 *	Revision 2.107  2003/05/29 14:56:37  lewis
 *	SPR#1510: dont call GetClientRect in draw - but use WindowRect/GetDefaultWindowMargins since
 *	we may have been called from metafile draw or other. Also - added DrawExtraDesignModeBorder ()
 *	so we don't display border in some cases where it looks ugly (not really related to SPR#1510 -
 *	but I noticed it while debugging and looking at display in MSVC.Net 2003 dialog editor).
 *	Careful about call to GetStyle (since vectors to MFC version in LedItView) - and that fails
 *	if window is NULL (as it is sometimes in MSVC .Net 2003 dialog editor)
 *	
 *	Revision 2.106  2003/05/28 16:56:19  lewis
 *	SPR#1509: Added qFunnyDisplayInDesignMode define to ActiveLedItConfig, and ifdefined
 *	(yes by default), then check for AMBIENT_DESIGN_MODE and change the drawing in
 *	LedItView::EraseBackground () slightly. Added a small watermarkdisplay saying 'design mode'
 *	and a border in the color opposite to the background. Hooked OnAmbientPropertyChange() to
 *	detect change and refresh screen.
 *	
 *	Revision 2.105  2003/05/20 22:30:04  lewis
 *	SPR#1493: react to change in args for CommandHandler::DoUndo/DoRedo
 *	
 *	Revision 2.104  2003/05/06 19:43:40  lewis
 *	SPR#1168: work on this SPR. Not complete. Cleaned much up, and narrowed in on problem.
 *	Still not there yet - though.
 *	
 *	Revision 2.103  2003/04/29 21:59:48  lewis
 *	SPR#1168: Added ActiveLedItControl::OnGetNaturalExtent and OnEraseBkgnd to help reduce
 *	flicker and drawing bugs with resizing AL controls in various containers (like MSVS.NET
 *	2002 and ActiveX Control Container and MS Word XP, and LedIt 3.1a6x)
 *	
 *	Revision 2.102  2003/04/29 13:47:10  lewis
 *	SPR#1168: cleanups to resize/OnDraw code to debug problem with AL resizing occasionally
 *	leaving cruft. Rework AL::OnDraw() - rarely called. Don't need to reset windowRect
 *	in that routine anymore.
 *	
 *	Revision 2.101  2003/04/22 15:50:40  lewis
 *	SPR#1447: override LedItView::UpdateScrollBars () so we can disable it during metafile
 *	draws. Override ActiveLedItControl::OnDrawMetafile () since default one doesn't work
 *	right for ActiveLedIt. Now we draw (metafile) properly when embedded in MSWord XP
 *	
 *	Revision 2.100  2003/03/27 16:37:36  lewis
 *	SPR#1384: ActiveLedItControl::DidUpdateText () checks updateInfo.fRealContentUpdate
 *	and only set dirtyflag if real update
 *	
 *	Revision 2.99  2003/03/25 15:59:36  lewis
 *	SPR#1375- because of recent change to Led - must call InteractiveSetFont() instead of
 *	SetDefaultFont in most places I was calling it
 *	
 *	Revision 2.98  2003/03/20 22:31:04  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.97  2003/03/13 18:08:14  lewis
 *	fix aboutbox code to use qWideCharacters instead of qLed_CharacterSet
 *	
 *	Revision 2.96  2003/03/11 02:33:16  lewis
 *	SPR#1288 - use new MakeSophistsAppNameVersionURL and add new CheckForUpdatesWebPageCommand,
 *	and other cleanups to Led_URLManager calls and context menu
 *	
 *	Revision 2.95  2003/03/05 22:39:31  lewis
 *	SPR#1320 - Added stock Appearance and BorderStyle property support. Also, changed my
 *	existing HWND property to using the stock HWND property code.
 *	
 *	Revision 2.94  2003/02/12 15:59:49  lewis
 *	SPR#1298- added ShowSecondaryHilight property to the control, and default to TRUE
 *	
 *	Revision 2.93  2003/01/23 01:28:01  lewis
 *	SPR#1257 - try/catch for Led_URLManager::Open call in about box
 *	
 *	Revision 2.92  2003/01/17 18:09:05  lewis
 *	SPR#1241 - be much more aggressive when demo has expired
 *	
 *	Revision 2.91  2003/01/15 18:57:29  lewis
 *	SPR#1221- added SmartQuoteMode OLE automation
 *	
 *	Revision 2.90  2003/01/09 14:09:23  lewis
 *	SPR#1227- added hWnd read-only property to ActiveLedIt control
 *	
 *	Revision 2.89  2002/12/16 19:23:49  lewis
 *	SPR#1208- added UseSelectEOLBOLRowHilightStyle property
 *	
 *	Revision 2.88  2002/11/21 16:03:10  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.87  2002/11/14 17:05:26  lewis
 *	SPR#1171- use SetSelection() override instead of obsolete NotificationOf_SelectionChanged routine, and
 *	call fEditor.SetEmptySelectionStyle() in a few places where we may have made changes to the text that
 *	warrent grabbing that selection info.
 *	
 *	Revision 2.86  2002/10/30 22:45:34  lewis
 *	SPR#1158 - SetDefaultWindowMargins () API now uses Led_TWIPS
 *	
 *	Revision 2.85  2002/10/30 16:55:40  lewis
 *	SPR#1153- added PrintMargins OLE AUT interface to ActiveLedIt
 *	
 *	Revision 2.84  2002/10/30 15:13:23  lewis
 *	SPR#1156 - WindowMarins property API is now in TWIPS
 *	
 *	Revision 2.83  2002/10/28 21:47:13  lewis
 *	SPR#0678 - Added OLE AUT methods for Get/Set DefaultWindowMargins
 *	
 *	Revision 2.82  2002/10/25 14:31:56  lewis
 *	SPR#1150- tweeks to ActiveLedItControl::OnDraw/OnSize to cleanup and help support margins. Then -
 *	major amount of text changes (though all trivial) in many OLE property accessors to lose asserts 
 *	about m_hWnd and sometimes replace with THROW_IF_NULL_EDITOR(), and lots more wrapping with
 *	try/CATCH_AND_HANDLE_EXCEPTIONS - ALL THAT to fix SPR#1151 - about Visual Interdev barfing when
 *	you view as object in its HTML view
 *	
 *	Revision 2.81  2002/05/06 21:34:30  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.80  2001/11/27 00:32:37  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.79  2001/09/24 14:29:07  lewis
 *	fix minor loadfile bug
 *	
 *	Revision 2.78  2001/09/06 20:27:54  lewis
 *	SPR#1003- cleanup about box
 *	
 *	Revision 2.77  2001/08/30 00:35:57  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.76  2001/08/27 13:43:43  lewis
 *	SPR#0972- add ListStyle popup menu, and fix LedItView::GetLayoutMargins () so bullets display properly
 *	
 *	Revision 2.75  2001/08/23 20:47:02  lewis
 *	SPR#0972- new EnableAutoChangesBackgroundColor feature
 *	
 *	Revision 2.74  2001/08/15 21:09:43  lewis
 *	SPR#0965- support OLE_COLOR for HidableTextColor and SelColor attributes
 *	
 *	Revision 2.73  2001/08/01 23:51:42  lewis
 *	use a few more STOCK_PROPRETY DISPIDs. And SPR# 0964 - ReadOnly/Enabled property supported
 *	
 *	Revision 2.72  2001/08/01 22:00:20  lewis
 *	SPR#0963- added support for BackColor stock property
 *	
 *	Revision 2.71  2001/07/31 15:39:09  lewis
 *	use static_cast<> to silence warnings
 *	
 *	Revision 2.70  2001/05/30 16:23:04  lewis
 *	SPR#0945- new VersionNumber/ShortVersionString OLEAUT APIs
 *	
 *	Revision 2.69  2001/04/17 16:50:29  lewis
 *	SPR#0610- Added PrinterSetupDialog, PrintDialog and PrintOnce OLE AUT methods. Also made plain
 *	Added PrinterSetupDialog, PrintDialog and PrintOnce OLE AUT methods. Also made plain
 *	
 *	Revision 2.68  2001/04/12 18:58:12  lewis
 *	cleanup DemoMode warning expired code (so easier to include in other apps)
 *	
 *	Revision 2.67  2001/04/11 22:59:46  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.66  2001/04/11 22:55:24  lewis
 *	SPR#0861- DEMO MODE annoyance dialog. Keep track in registry of how long demo been going on.
 *	Bring up annoyance dialog after a while - when demo expired. And add note in about box saying
 *	how long left in demo
 *	
 *	Revision 2.65  2001/04/09 21:54:58  lewis
 *	SPR#0860- fix ActiveLedItControl::SetBufferText () when compiled for _UNICODE
 *	
 *	Revision 2.64  2001/04/09 20:43:34  lewis
 *	SPR#0859- Added LaunchFontSettingsDialog/LaunchParagraphSettingsDialog methods.
 *	
 *	Revision 2.63  2001/04/09 19:01:31  lewis
 *	SPR#0858- Added DefaultFontFace/DefaultFontSize params to OCX (<param> tags).
 *	
 *	Revision 2.62  2001/04/09 15:12:54  lewis
 *	SPR#0733- return value from TextBitmap property is now an OLE Picture - supporting IPicture
 *	interface created with OleCreatePictureIndirect
 *	
 *	Revision 2.61  2001/03/21 15:19:41  lewis
 *	SPR#0856- fix GetHeight() method of ActiveX control
 *	
 *	Revision 2.60  2001/02/12 15:02:33  lewis
 *	SPR#0854- added OLE Automation GetHeight method - to compute pixelheight of a given range of text
 *	
 *	Revision 2.59  2000/11/09 05:00:58  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.58  2000/11/09 04:57:00  lewis
 *	SPR#0845- fix UNICODE build 'TextRTF' param method (and more)
 *	
 *	Revision 2.57  2000/10/16 22:50:21  lewis
 *	use new Led_StdDialogHelper_AboutBox
 *	
 *	Revision 2.56  2000/10/14 13:56:22  lewis
 *	update GOTO URLs - and fixed broken URL pict location for clicking in about box
 *	
 *	Revision 2.55  2000/10/06 03:37:55  lewis
 *	react to code cleanups of showhide scrollbar code in TextInteractor/Led_Win32
 *	
 *	Revision 2.54  2000/10/05 02:38:22  lewis
 *	moved MyHidableTextMarkerOwner to shared class ColoredUniformHidableTextMarkerOwner in Led.
 *	Wrap some more OLE calls in try/catch to make them more bullet-proof.
 *	
 *	Revision 2.53  2000/09/30 19:36:29  lewis
 *	lose LedItView::OnSelectAllCommand/nUpdateCutCopyClearComma/OnUpdatePasteCommand etc
 *	since now handled by TextInteractorCommonCommandHelper_MFC
 *	
 *	Revision 2.52  2000/09/28 13:58:01  lewis
 *	some comments and a bit more try/catch support to avoid OLE crashes (cuz doesnt like C++ expcetions)
 *	
 *	Revision 2.51  2000/09/14 19:05:14  lewis
 *	catches to prevent crash when failing loading RTF etc (MFC doesnt catch non-mfc expcetions properly
 *	
 *	Revision 2.50  2000/09/06 21:39:05  lewis
 *	SPR#0835- support a few named properties for loading from html PARAM-tags. Also - wrap SetAsXXX
 *	text guys (a few) with try/catch- to catch C++ exceptions and turn them into ones that MFC knows
 *	about - cuz now badly formatted text produces them (in HTML for example) - and that results in
 *	uncaught excpetions
 *	
 *	Revision 2.49  2000/08/28 21:51:12  lewis
 *	SPR#0830- added TextColored IDL proprty - so you can use the old or new display algorith for hidden
 *	text. Defaults to MSWord style display.
 *	
 *	Revision 2.48  2000/08/28 21:18:30  lewis
 *	SPR#0829- VK_TAB ignored (handled by browser) in MSIE5.5. I HOPE this is fixed by my modest
 *	changes to ActiveLedItControl::PreTranslateMessage (). It appears to be some quiirk of MSIE5.5.
 *	I HOPE my changes don't break the tab char processing in other browsers!
 *	
 *	Revision 2.47  2000/07/08 23:43:28  lewis
 *	globalfree in commented out hack for one activeledit customer
 *	
 *	Revision 2.46  2000/06/13 19:10:22  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.45  2000/06/13 18:55:19  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.44  2000/04/12 05:56:32  lewis
 *	GetAsTextCRLF should not call PlainTextReader class - cuz that maps to ANSI chars (was also
 *	buggy - but now fixed). This really wants to return the UNICODE cahrs - if we're using
 *	wide cahrs internally
 *	
 *	Revision 2.43  2000/04/11 18:01:56  lewis
 *	SPR#0737- fix GetHTMLText OLEAUT method for qWideCharacters case
 *	
 *	Revision 2.42  2000/04/10 16:26:52  lewis
 *	SPR#0735- added SelHidable OLE AUT property
 *	
 *	Revision 2.41  2000/04/04 20:51:27  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.40  2000/04/03 15:39:57  lewis
 *	SPR#0728- Incorporate HiddenText OLEAUT support into the OCX
 *	
 *	Revision 2.39  2000/04/03 03:44:22  lewis
 *	SPR#0716- added FindUI and Find(noUI) OLEAutomation methods
 *	
 *	Revision 2.38  2000/03/15 21:27:52  lewis
 *	SPR#0715- added a read/write SelJustification property
 *	
 *	Revision 2.37  2000/03/15 19:20:26  lewis
 *	SPR#0714: support IDL dirty flag. Also - change default size of new ActiveLedIt! control
 *	
 *	Revision 2.36  2000/03/15 16:51:27  lewis
 *	Support a bevy of new UNDO released OLE_AUT commands to set maxUndo level etc.
 *	SPR#0712 - for RandomHouse
 *	
 *	Revision 2.35  2000/03/13 20:16:08  lewis
 *	shut off qCopyDIBToClipToTest. Try making my PackBytesToSafeArray routine use VT_I1 instead
 *	of VT_UI1 - as a wide guess how to make the OMNIs enviroemnt talk happier to it (since my
 *	experince with VBScript is that IT tends to be happier with ints rather than unsinged ints
 *	
 *	Revision 2.34  2000/03/13 19:28:10  lewis
 *	SPR#0647- fix remaining cases where UNICODE control didn't handle SelTextRTF etc properties properly
 *	
 *	Revision 2.33  1999/12/27 20:16:26  lewis
 *	SPR#0669- partial - now support these 3 properties for show/hide space/paragraphs etc
 *	
 *	Revision 2.32  1999/12/24 03:04:59  lewis
 *	cleanup AsBitMap code - and add try/catch to restore windowrect etc on throw.
 *	And save/restore scrollpos when doing asbitmap () call
 *	
 *	Revision 2.31  1999/12/24 00:55:01  lewis
 *	make the AsTextBitmap() method work better. autosize H-Size of it better. Use auto_ptr<> to
 *	make delete of bitmap safer (if excpetions). Dont call WindowDrawHelper - but directly call
 *	fEditor.Draw() WindowDrawHelper() re-invokes OnSize() code (cuz of scrollbar changes).
 *	
 *	Revision 2.30  1999/12/21 20:49:54  lewis
 *	SPR# 0655- partialUNICODE and FULLUNICODE support. And - SPR#0672- AsTextBitmap() support.
 *	SPR#0647- a partial fix on UNICODE version of GetTextAsRTF().
 *	
 *	Revision 2.29  1999/12/18 04:13:06  lewis
 *	mostly just react to spr#0662- break StyledTextIO into parts
 *	
 *	Revision 2.28  1999/12/09 17:36:26  lewis
 *	adjust aboutbox to say 'UNICODE' for UNICODE build
 *	
 *	Revision 2.27  1999/12/09 03:16:52  lewis
 *	spr#0645 - -D_UNICODE support
 *	
 *	Revision 2.26  1999/11/16 15:17:36  lewis
 *	react to small Led changes - so Led doesn't depend on MFC
 *	
 *	Revision 2.25  1999/11/13 16:53:09  lewis
 *	Fixed spr#0623 - added ScrollToSelection IDL method
 *	
 *	Revision 2.24  1999/09/20 13:17:01  lewis
 *	some hacks todo interum 2.3.2 release - but mostly support for new DEMO_MODE build
 *	
 *	Revision 2.23  1999/08/28 13:55:10  lewis
 *	spr#0616- re-fixed arrow key stuff in ActiveLedIt! emebdded in MSIE
 *	
 *	Revision 2.22  1999/07/21 20:33:17  lewis
 *	changed about box to not say shareware - but to contain copyright and distribution warnings
 *	
 *	Revision 2.21  1999/07/16 21:24:40  lewis
 *	spr#0601- change the SetSBar/GetSBarType IDL methods to operate on TRISTATE - like Led_MFC,
 *	so default can be autoshow sbar. THEN - fix bug where hsbar resizing wasn't working properly -
 *	code from LedItMFC was needed - now moved to common shared template WordProcessorHScrollbarHelper<>
 *	
 *	Revision 2.20  1999/06/28 14:09:04  lewis
 *	spr#0528- Add hooks to various other windows messages, and add wrappers on FireEvent() to fire
 *	the corresponding other events. Borrowed from the pattern in COleControl::ButtonDblClk, etc,
 *	for when to fire which events. Tested under activex control test container, and all the
 *	events seemed to fire properly (with right args).
 *	
 *	Revision 2.19  1999/06/26 20:46:48  lewis
 *	work on spr#0528: Catch when focus is set to owning COleControl and pass it along to LedItView.
 *	And handle SelChange and Change notifications (events). Still several more events I must handle,
 *	since I cannot use the COleControl automatic handling - due to the compiler bug which
 *	prevents me merging tehse into a single class.
 *	
 *	Revision 2.18  1999/06/26 17:09:47  lewis
 *	Proivate LedItViewController::ForceUIActivateHook() so its override OnMouseActiveate can
 *	force the OLEControl to go into UIActivate state when user clicks on us- part of spr# 0596
 *	
 *	Revision 2.17  1999/06/25 14:49:17  lewis
 *	first cut at adding fire event support
 *	
 *	Revision 2.16  1999/06/25 14:08:53  lewis
 *	make IDL name MaxInputLength -> MaxLength to match MS RichTextControl 6.0
 *	
 *	Revision 2.15  1999/06/25 00:50:36  lewis
 *	spr#0595- add 'MaximumInputLength property to control. Most of this was trivial. We do a throw
 *	from AboutToUpdateText() method if too much text entered. Easy. But the trick is that for an OCX -
 *	there is no top level catcher to handle these excptions. They go unhandled. To address this, the
 *	Led_MFC_ExceptionHandlerHelper<> template as added
 *	
 *	Revision 2.14  1999/06/23 21:57:40  lewis
 *	add a HorzSBar and WrapToWindow toggle button
 *	
 *	Revision 2.13  1999/06/16 14:47:37  lewis
 *	spr#0589- implement component categories stuff(http://msdn.microsoft.com/workshop/components/com/IObjectSafetyExtensions.asp) -
 *	so we don't get annoying warnings from MSIE
 *	
 *	Revision 2.12  1999/03/10 21:39:13  lewis
 *	call new WindowDrawHelper Led_MFC_..<> utility
 *	
 *	Revision 2.11  1998/11/02 13:23:25  lewis
 *	Changed from usin vector<char*> to vector<string> - and cleaned up font name list gen code slightly.
 *	And lost LedItView::SetWindowRect() overrides - SB very soon obsolete due to new margin/hscroll
 *	support in wordprocessor class.
 *	
 *	Revision 2.10  1998/04/08 02:59:26  lewis
 *	WordProcessor::WordProcessorTextIOSinkStream instead of StyledTextIOSinkStream_StandardStyledTextImager etc...
 *	
 *	Revision 2.9  1998/03/04  20:44:54  lewis
 *	Major cleanups and bugfixes, but mostly fixing/addition of OLEAUT support.
 *
 *	Revision 2.8  1997/12/24  04:44:53  lewis
 *	*** empty log message ***
 *
 *	Revision 2.7  1997/12/24  04:32:23  lewis
 *	new RTF/HTML/Text properties (get/set).
 *	Fixed (halfway) the horizontal sbar property - still doesn't redisplay properly.
 *
 *	Revision 2.6  1997/07/27  16:00:23  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.5  1997/07/23  23:24:27  lewis
 *	OnBrowseHelpCommand ()
 *
 *	Revision 2.4  1997/07/14  01:22:10  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1997/06/28  17:45:58  lewis
 *	Lots of cleanups and bugfixes. Mostly broke out LedItView stuff into separate file (cloned for
 *	LedIt! app version of LedItView, so I got most of that functionality).
 *
 *	Revision 2.2  1997/06/24  03:51:32  lewis
 *	override GetLayoutWidth() and call InvalidateAllCahces() instead of (obsolete) SetLayoutWidth()
 *	in SetWindowRect() override - all part of spr#0450 fix.
 *
 *	Revision 2.1  1997/06/23  16:58:48  lewis
 *	new about box, and fixed no display on deactive bug
 *
 *	Revision 2.0  1997/06/18  03:25:57  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 *
 */

#include	<climits>

#include	<fcntl.h>
#include	<io.h>
#include	<sys/stat.h>


#include	<afxctl.h>

#include	<atlbase.h>

#include	"LedStdDialogs.h"
#include	"StyledTextIO_PlainText.h"
#include	"StyledTextIO_LedNative.h"

#include	"ActiveLedIt.h"
#include	"ActiveLedItPpg.h"
#include	"DispIDs.h"
#include	"FontMenu.h"
#include	"Toolbar.h"
#include	"UserConfigCommands.h"

#include	"ActiveLedItControl.h"

#include	"ActiveLedIt_h.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif






// Not sure about this - experiment... See spr#0521
#define	qDisableEditorWhenNotActive	0





#define	CATCH_AND_HANDLE_EXCEPTIONS()\
	catch (HRESULT hr) {\
		AfxThrowOleException (hr);\
	}\
	catch (Win32ErrorException& we) {\
		AfxThrowOleException (HRESULT_FROM_WIN32 (we));\
	}\
	catch (HRESULTErrorException& h) {\
		AfxThrowOleException (h);\
	}\
	catch (...) {\
		AfxThrowMemoryException ();\
	}\





const	kEditorWindowID		=	100;
















/*
 ********************************************************************************
 ************************** COMBased_SpellCheckEngine ***************************
 ********************************************************************************
 */

COMBased_SpellCheckEngine::COMBased_SpellCheckEngine (IDispatch* engine):
	inherited (),
	fEngine (engine)
{
	// NB: We use IDispatch interface rather than the vtable interface to avoid having to directly include the spellchecker IDL here,
	// and having to compile it ourselves in this project
}

bool	COMBased_SpellCheckEngine::ScanForUndefinedWord (const Led_tChar* startBuf, const Led_tChar* endBuf, const Led_tChar** cursor,
									const Led_tChar** wordStartResult, const Led_tChar** wordEndResult
							)
{
	Led_RequireNotNil (startBuf);
	Led_Require (startBuf <= endBuf);

	if (fEngine != NULL) {
		size_t	textBufLen	=	endBuf-startBuf;
		size_t	cursorIdx	=	*cursor==NULL? 0: (*cursor-startBuf);

		BSTR	bstrVal = ::SysAllocStringLen (startBuf, textBufLen);
		CComVariant	textToScan (bstrVal);
		::SysFreeString (bstrVal);
		CComVariant	result;
		Led_ThrowIfErrorHRESULT (fEngine.Invoke2 (CA2W("CreateScanContext"), &textToScan, &CComVariant (cursorIdx), &result));
		Led_ThrowIfErrorHRESULT (result.ChangeType (VT_DISPATCH));
		if (result.vt == VT_DISPATCH) {
			CComPtr<IDispatch>	scanContext	=	result.pdispVal;
			CComVariant	comCursor;
			if (SUCCEEDED (scanContext.GetPropertyByName (CA2W("Cursor"), &comCursor))) {
				size_t	sbCursor	=	comCursor.uintVal;
				sbCursor = max (sbCursor, cursorIdx+1);
				sbCursor = min (sbCursor, textBufLen);
				*cursor = startBuf + sbCursor;
			}
			CComVariant	comWordFound;
			if (SUCCEEDED (scanContext.GetPropertyByName (CA2W("WordFound"), &comWordFound)) and comWordFound.boolVal) {
				CComVariant	comWordStart;
				CComVariant	comWordEnd;
				if (SUCCEEDED (scanContext.GetPropertyByName (CA2W("WordStart"), &comWordStart)) and
					SUCCEEDED (scanContext.GetPropertyByName (CA2W("WordEnd"), &comWordEnd)) and
					SUCCEEDED (comWordStart.ChangeType (VT_UI4)) and
					SUCCEEDED (comWordEnd.ChangeType (VT_UI4))
					) {
					size_t	newWS	=	comWordStart.uintVal;
					size_t	newWE	=	comWordEnd.uintVal;
					newWS = min (newWS, textBufLen);
					newWE = min (newWE, textBufLen);
					newWE = max (newWS, newWE);
					*wordStartResult = startBuf + newWS;
					*wordEndResult = startBuf + newWE;
					return true;
				}
			}
		}
	}
	return false;
}

bool	COMBased_SpellCheckEngine::LookupWord_ (const Led_tString& checkWord, Led_tString* matchedWordResult)
{
	CComVariant	comMissingWord;
	CComVariant	result;
	Led_ThrowIfErrorHRESULT (fEngine.Invoke2 (CA2W("LookupWord"), &CComVariant (checkWord.c_str ()), matchedWordResult==NULL? NULL: &comMissingWord, &result));
	if (SUCCEEDED (result.ChangeType (VT_BOOL)) and
		result.boolVal
		) {
		if (matchedWordResult != NULL) {
			if (SUCCEEDED (comMissingWord.ChangeType (VT_BSTR))) {
				*matchedWordResult = Led_tString (comMissingWord.bstrVal);
			}
			else {
				// even if MW found - if call requested matchedWord and COM didn't provide - treat that as failure...
				return false;
			}
		}
		return true;
	}
	return false;
}

vector<Led_tString>	COMBased_SpellCheckEngine::GenerateSuggestions (const Led_tString& misspelledWord)
{
	CComVariant	comResult;
	Led_ThrowIfErrorHRESULT (fEngine.Invoke1 (CA2W("GenerateSuggestions"), &CComVariant (misspelledWord.c_str ()), &comResult));
	return UnpackVectorOfStringsFromVariantArray (comResult);
}

TextBreaks*		COMBased_SpellCheckEngine::PeekAtTextBreaksUsed ()
{
	return this;
}

void	COMBased_SpellCheckEngine::FindWordBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
											size_t* wordStartResult, size_t* wordEndResult, bool* wordReal
										) const
{
	Led_tString	text			=	Led_tString (startOfText, startOfText + lengthOfText);
	CComVariant	wordInfoResult;
	CComPtr<IDispatch>	engine	=	fEngine;
	Led_ThrowIfErrorHRESULT (engine.Invoke2 (CA2W("FindWordBreaks"), &CComVariant (text.c_str ()), &CComVariant (textOffsetToStartLookingForWord), &wordInfoResult));
	Led_ThrowIfErrorHRESULT (wordInfoResult.ChangeType (VT_DISPATCH));
	if (wordInfoResult.vt == VT_DISPATCH) {
		CComPtr<IDispatch>	wordInfo	=	wordInfoResult.pdispVal;
		// Validate COM results. They COULD be bogus - and that shouldn't crash/assert-out LED
		{
			CComVariant	val;
			Led_ThrowIfErrorHRESULT (wordInfo.GetPropertyByName (CA2W("WordStart"), &val));
			Led_ThrowIfErrorHRESULT (val.ChangeType (VT_UI4));
			*wordStartResult = val.uintVal;
			*wordStartResult = min (*wordStartResult, lengthOfText);
		}
		{
			CComVariant	val;
			Led_ThrowIfErrorHRESULT (wordInfo.GetPropertyByName (CA2W("WordEnd"), &val));
			Led_ThrowIfErrorHRESULT (val.ChangeType (VT_UI4));
			*wordEndResult = val.uintVal;
			*wordEndResult = min (*wordEndResult, lengthOfText);
		}
		{
			CComVariant	val;
			Led_ThrowIfErrorHRESULT (wordInfo.GetPropertyByName (CA2W("WordReal"), &val));
			Led_ThrowIfErrorHRESULT (val.ChangeType (VT_BOOL));
			*wordReal = !!val.boolVal;
		}
	}
}

void	COMBased_SpellCheckEngine::FindLineBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
											size_t* wordEndResult, bool* wordReal
										) const
{
	// This really shouldn't be called - since the line-breaks code for a spellchecker isn't necessarily very reasonable.
	// Anyhow - if it ever does get called - this should produce a vaguley reasonable result... LGP 2003-12-16
	size_t	wordStartIgnored	=	0;
	FindWordBreaks (startOfText, lengthOfText, textOffsetToStartLookingForWord, &wordStartIgnored, wordEndResult, wordReal);
}

COMBased_SpellCheckEngine::UDInterface*	COMBased_SpellCheckEngine::GetUDInterface ()
{
	return this;
}

bool	COMBased_SpellCheckEngine::AddWordToUserDictionarySupported () const
{
	CComVariant			comResult;
	CComPtr<IDispatch>	engine	=	fEngine;
	Led_ThrowIfErrorHRESULT (engine.GetPropertyByName (CA2W("AddWordToUserDictionarySupported"), &comResult));
	return	SUCCEEDED (comResult.ChangeType (VT_BOOL)) and comResult.boolVal;
}

void	COMBased_SpellCheckEngine::AddWordToUserDictionary (const Led_tString& word)
{
	Led_ThrowIfErrorHRESULT (fEngine.Invoke1 (CA2W("AddWordToUserDictionary"), &CComVariant (word.c_str ())));
}







/*
 ********************************************************************************
 ************* ActiveLedItControl::ActiveLedItControlFactory ********************
 ********************************************************************************
 */

// Control type information
static const DWORD BASED_CODE _dwActiveLedItOleMisc =
	OLEMISC_SIMPLEFRAME |
	OLEMISC_ACTIVATEWHENVISIBLE |			/* See SPR#1522 & qDontUIActivateOnOpen about this */
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(ActiveLedItControl, IDS_ACTIVELEDIT, _dwActiveLedItOleMisc)


// ActiveLedItControl::ActiveLedItControlFactory::UpdateRegistry -
// Adds or removes system registry entries for ActiveLedItControl
BOOL	ActiveLedItControl::ActiveLedItControlFactory::UpdateRegistry (BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegInsertable | afxRegApartmentThreading to afxRegInsertable.
	if (bRegister) {
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_ACTIVELEDIT,
			IDB_ACTIVELEDIT,
			afxRegInsertable | afxRegApartmentThreading,
			_dwActiveLedItOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	}
	else {
		return AfxOleUnregisterClass (m_clsid, m_lpszProgID);
	}
}






/*
 ********************************************************************************
 ******************************** ActiveLedItControl ****************************
 ********************************************************************************
 */
typedef	ActiveLedItControl	ALIC;
IMPLEMENT_DYNCREATE(ActiveLedItControl, COleControl)

BEGIN_MESSAGE_MAP(ActiveLedItControl, COleControl)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED ()
	ON_WM_SETFOCUS()
	ON_WM_ERASEBKGND()
	ON_COMMAND			(ID_APP_ABOUT,				AboutBox)
	ON_OLEVERB			(AFX_IDS_VERB_EDIT,			OnEdit)
	ON_OLEVERB			(AFX_IDS_VERB_PROPERTIES,	OnProperties)
END_MESSAGE_MAP()



BEGIN_DISPATCH_MAP(ActiveLedItControl, COleControl)

//	Properties
	DISP_STOCKPROP_APPEARANCE()
	DISP_STOCKPROP_BACKCOLOR()
	DISP_STOCKPROP_BORDERSTYLE()
	DISP_STOCKPROP_HWND()

	
	DISP_FUNCTION_ID	(ALIC,	"InvalidateLayout",			DISPID_InvalidateLayout,		OLE_InvalidateLayout, VT_EMPTY, VTS_NONE)

	DISP_FUNCTION_ID	(ALIC,	"VersionNumber",			DISPID_VersionNumber,			OLE_VersionNumber, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"ShortVersionString",		DISPID_ShortVersionString,		OLE_ShortVersionString, VT_BSTR, VTS_NONE)
	DISP_PROPERTY_EX_ID	(ALIC,	"ReadOnly",					DISPID_ReadOnly,				OLE_GetReadOnly, OLE_SetReadOnly, VT_BOOL)
	DISP_PROPERTY_EX_ID	(ALIC,	"Enabled",					DISPID_ENABLED,					OLE_GetEnabled, OLE_SetEnabled, VT_BOOL)
	DISP_PROPERTY_EX_ID	(ALIC,	"EnableAutoChangesBackgroundColor",
															DISPID_EnableAutoChangesBackgroundColor,
																							OLE_GetEnableAutoChangesBackgroundColor, OLE_SetEnableAutoChangesBackgroundColor, VT_BOOL)
	DISP_PROPERTY_EX_ID	(ALIC,	"WindowMarginTop",			DISPID_WindowMarginTop,			OLE_GetWindowMarginTop, OLE_SetWindowMarginTop, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"WindowMarginLeft",			DISPID_WindowMarginLeft,		OLE_GetWindowMarginLeft, OLE_SetWindowMarginLeft, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"WindowMarginBottom",		DISPID_WindowMarginBottom,		OLE_GetWindowMarginBottom, OLE_SetWindowMarginBottom, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"WindowMarginRight",		DISPID_WindowMarginRight,		OLE_GetWindowMarginRight, OLE_SetWindowMarginRight, VT_I4)																							
	DISP_PROPERTY_EX_ID	(ALIC,	"PrintMarginTop",			DISPID_PrintMarginTop,			OLE_GetPrintMarginTop, OLE_SetPrintMarginTop, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"PrintMarginLeft",			DISPID_PrintMarginLeft,			OLE_GetPrintMarginLeft, OLE_SetPrintMarginLeft, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"PrintMarginBottom",		DISPID_PrintMarginBottom,		OLE_GetPrintMarginBottom, OLE_SetPrintMarginBottom, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"PrintMarginRight",			DISPID_PrintMarginRight,		OLE_GetPrintMarginRight, OLE_SetPrintMarginRight, VT_I4)																							
	DISP_PROPERTY_EX_ID	(ALIC,	"HasVerticalScrollBar",		DISPID_HasVerticalScrollBar,	GetHasVerticalScrollBar, SetHasVerticalScrollBar, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"HasHorizontalScrollBar",	DISPID_HasHorizontalScrollBar,	GetHasHorizontalScrollBar, SetHasHorizontalScrollBar, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"Text",						DISPID_Text,					GetBufferText, SetBufferText, VT_BSTR)
	DISP_PROPERTY_EX_ID	(ALIC,	"TextCRLF",					DISPID_TextCRLF,				GetBufferTextCRLF, SetBufferTextCRLF, VT_BSTR)
	DISP_PROPERTY_EX_ID	(ALIC,	"TextRTF",					DISPID_TextRTF,					GetBufferTextAsRTF, SetBufferTextAsRTF, VT_BSTR)
	DISP_PROPERTY_EX_ID	(ALIC,	"TextHTML",					DISPID_TextHTML,				GetBufferTextAsHTML, SetBufferTextAsHTML, VT_BSTR)
	DISP_FUNCTION_ID	(ALIC,	"TextBitmap",				DISPID_TextBitmap,				GetBufferTextAsDIB, VT_VARIANT, VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"Length",					DISPID_Length,					GetBufferLength, VT_I4, VTS_NONE)
	DISP_PROPERTY_EX_ID	(ALIC,	"MaxLength",				DISPID_MaxLength,				GetMaxLength, SetMaxLength, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"SupportContextMenu",		DISPID_SupportContextMenu,		GetSupportContextMenu, SetSupportContextMenu, VT_BOOL)
	DISP_PROPERTY_EX_ID	(ALIC,	"ContextMenu",				DISPID_ContextMenu,				OLE_GetContextMenu, OLE_SetContextMenu, VT_VARIANT)
	DISP_PROPERTY_EX_ID	(ALIC,	"Toolbars",					DISPID_ToolbarList,				OLE_GetToolbarList, OLE_SetToolbarList, VT_VARIANT)
	DISP_PROPERTY_EX_ID	(ALIC,	"HideDisabledContextMenuItems",
															DISPID_HideDisabledContextMenuItems,
																							OLE_GetHideDisabledContextMenuItems, OLE_SetHideDisabledContextMenuItems, VT_BOOL)
	DISP_FUNCTION_ID	(ALIC,	"BuiltinCommands",			DISPID_BuiltinCommands,			OLE_GetBuiltinCommands, VT_VARIANT, VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"PredefinedMenus",			DISPID_PredefinedMenus,			OLE_GetPredefinedMenus, VT_VARIANT, VTS_NONE)
	DISP_PROPERTY_EX_ID	(ALIC,	"AcceleratorTable",			DISPID_AcceleratorTable,		OLE_GetAcceleratorTable, OLE_SetAcceleratorTable, VT_VARIANT)
	DISP_FUNCTION_ID	(ALIC,	"GetDefaultContextMenu",	DISPID_GetDefaultContextMenu,	OLE_GetDefaultContextMenu,	VT_DISPATCH,	VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"GetDefaultAcceleratorTable",DISPID_GetDefaultAcceleratorTable,	OLE_GetDefaultAcceleratorTable,	VT_DISPATCH,	VTS_NONE)
	
	DISP_FUNCTION_ID	(ALIC,	"MakeNewPopupMenuItem",		DISPID_MakeNewPopupMenuItem,	OLE_MakeNewPopupMenuItem,	VT_DISPATCH,	VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"MakeNewUserMenuItem",		DISPID_MakeNewUserMenuItem,		OLE_MakeNewUserMenuItem,	VT_DISPATCH,	VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"MakeNewAcceleratorElement",DISPID_MakeNewAcceleratorElement,OLE_MakeNewAcceleratorElement,	VT_DISPATCH,	VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"InvokeCommand",			DISPID_InvokeCommand,			OLE_InvokeCommand,	VT_EMPTY,	VTS_VARIANT)
	DISP_FUNCTION_ID	(ALIC,	"CommandEnabled",			DISPID_CommandEnabled,			OLE_CommandEnabled,	VT_BOOL,	VTS_VARIANT)
	DISP_FUNCTION_ID	(ALIC,	"CommandChecked",			DISPID_CommandChecked,			OLE_CommandChecked,	VT_BOOL,	VTS_VARIANT)
	DISP_FUNCTION_ID	(ALIC,	"MakeNewToolbarList",		DISPID_MakeNewToolbarList,		OLE_MakeNewToolbarList,	VT_DISPATCH,	VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"MakeNewToolbar",			DISPID_MakeNewToolbar,			OLE_MakeNewToolbar,	VT_DISPATCH,	VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"MakeIconButtonToolbarItem",DISPID_MakeIconButtonToolbarItem,OLE_MakeIconButtonToolbarItem,	VT_DISPATCH,	VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"MakeSeparatorToolbarItem",	DISPID_MakeSeparatorToolbarItem,OLE_MakeSeparatorToolbarItem,	VT_DISPATCH,	VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"MakeBuiltinToolbar",		DISPID_MakeBuiltinToolbar,		OLE_MakeBuiltinToolbar,	VT_DISPATCH,	VTS_WBSTR)
	DISP_FUNCTION_ID	(ALIC,	"MakeBuiltinToolbarItem",	DISPID_MakeBuiltinToolbarItem,	OLE_MakeBuiltinToolbarItem,	VT_DISPATCH,	VTS_WBSTR)
	DISP_FUNCTION_ID	(ALIC,	"CurrentEventArguments",	DISPID_CurrentEventArguments,	OLE_GetCurrentEventArguments, VT_VARIANT, VTS_NONE)
	DISP_PROPERTY_EX_ID	(ALIC,	"SmartCutAndPaste",			DISPID_SmartCutAndPaste,		GetSmartCutAndPaste, SetSmartCutAndPaste, VT_BOOL)
	DISP_PROPERTY_EX_ID	(ALIC,	"SmartQuoteMode",			DISPID_SmartQuoteMode,			OLE_GetSmartQuoteMode, OLE_SetSmartQuoteMode, VT_BOOL)
	DISP_PROPERTY_EX_ID	(ALIC,	"WrapToWindow",				DISPID_WrapToWindow,			GetWrapToWindow, SetWrapToWindow, VT_BOOL)
	DISP_PROPERTY_EX_ID	(ALIC,	"ShowParagraphGlyphs",		DISPID_ShowParagraphGlyphs,		GetShowParagraphGlyphs, SetShowParagraphGlyphs, VT_BOOL)
	DISP_PROPERTY_EX_ID	(ALIC,	"ShowTabGlyphs",			DISPID_ShowTabGlyphs,			GetShowTabGlyphs, SetShowTabGlyphs, VT_BOOL)
	DISP_PROPERTY_EX_ID	(ALIC,	"ShowSpaceGlyphs",			DISPID_ShowSpaceGlyphs,			GetShowSpaceGlyphs, SetShowSpaceGlyphs, VT_BOOL)
	DISP_PROPERTY_EX_ID	(ALIC,	"UseSelectEOLBOLRowHilightStyle",
															DISPID_UseSelectEOLBOLRowHilightStyle,
																							OLE_GetUseSelectEOLBOLRowHilightStyle, OLE_SetUseSelectEOLBOLRowHilightStyle, VT_BOOL)
	DISP_PROPERTY_EX_ID	(ALIC,	"ShowSecondaryHilight",		DISPID_ShowSecondaryHilight,	OLE_GetShowSecondaryHilight, OLE_SetShowSecondaryHilight, VT_BOOL)
	DISP_PROPERTY_EX_ID	(ALIC,	"ShowHidableText",			DISPID_ShowHidableText,			OLE_GetShowHidableText, OLE_SetShowHidableText, VT_BOOL)
	DISP_PROPERTY_EX_ID	(ALIC,	"HidableTextColor",			DISPID_HidableTextColor,		OLE_GetHidableTextColor, OLE_SetHidableTextColor, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"HidableTextColored",		DISPID_HidableTextColored,		OLE_GetHidableTextColored, OLE_SetHidableTextColored, VT_BOOL)
	DISP_PROPERTY_EX_ID	(ALIC,	"SpellChecker",				DISPID_SpellChecker,			OLE_GetSpellChecker, OLE_SetSpellChecker, VT_VARIANT)
	DISP_PROPERTY_EX_ID	(ALIC,	"SelStart",					DISPID_SELSTART,				GetSelStart, SetSelStart, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"SelLength",				DISPID_SELLENGTH,				GetSelLength, SetSelLength, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"SelText",					DISPID_SELTEXT,					GetSelText, SetSelText, VT_BSTR)
	DISP_PROPERTY_EX_ID	(ALIC,	"SelTextRTF",				DISPID_SelTextRTF,				GetSelTextAsRTF, SetSelTextAsRTF, VT_BSTR)
	DISP_PROPERTY_EX_ID	(ALIC,	"SelTextHTML",				DISPID_SelTextHTML,				GetSelTextAsHTML,	SetSelTextAsHTML, VT_BSTR)
	DISP_PROPERTY_EX_ID	(ALIC,	"SelColor",					DISPID_SelColor,				GetSelColor, SetSelColor, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"SelFontFace",				DISPID_SelFontFace,				GetSelFontFace, SetSelFontFace, VT_BSTR)
	DISP_PROPERTY_EX_ID	(ALIC,	"SelFontSize",				DISPID_SelFontSize,				GetSelFontSize, SetSelFontSize, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"SelBold",					DISPID_SelBold,					GetSelBold, SetSelBold, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"SelItalic",				DISPID_SelItalic,				GetSelItalic, SetSelItalic, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"SelStrikeThru",			DISPID_SelStrikeThru,			GetSelStrikeThru, SetSelStrikeThru, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"SelUnderline",				DISPID_SelUnderline,			GetSelUnderline, SetSelUnderline, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"SelJustification",			DISPID_SelJustification,		OLE_GetSelJustification, OLE_SetSelJustification, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"SelListStyle",				DISPID_SelListStyle,			OLE_GetSelListStyle, OLE_SetSelListStyle, VT_I4)
	DISP_PROPERTY_EX_ID	(ALIC,	"SelHidable",				DISPID_SelHidable,				OLE_GetSelHidable, OLE_SetSelHidable, VT_I4)


//	Functions
	DISP_FUNCTION_ID	(ALIC,	"AboutBox",					DISPID_ABOUTBOX,				AboutBox,				VT_EMPTY,	VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"LoadFile",					DISPID_LoadFile,				LoadFile,				VT_EMPTY,	VTS_BSTR)
	DISP_FUNCTION_ID	(ALIC,	"SaveFile",					DISPID_SaveFile,				SaveFile,				VT_EMPTY,	VTS_BSTR)
	DISP_FUNCTION_ID	(ALIC,	"SaveFileCRLF",				DISPID_SaveFileCRLF,			SaveFileCRLF,			VT_EMPTY,	VTS_BSTR)
	DISP_FUNCTION_ID	(ALIC,	"SaveFileRTF",				DISPID_SaveFileRTF,				SaveFileRTF,			VT_EMPTY,	VTS_BSTR)
	DISP_FUNCTION_ID	(ALIC,	"SaveFileHTML",				DISPID_SaveFileHTML,			SaveFileHTML,			VT_EMPTY,	VTS_BSTR)
	DISP_FUNCTION_ID	(ALIC,	"Refresh",					DISPID_REFRESH,					Refresh,				VT_EMPTY,	VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"ScrollToSelection",		DISPID_ScrollToSelection,		ScrollToSelection,		VT_EMPTY,	VTS_NONE)

//UNDO support
	DISP_PROPERTY_EX_ID	(ALIC,	"MaxUndoLevel",				DISPID_MaxUndoLevel,			OLE_GetMaxUndoLevel, OLE_SetMaxUndoLevel, VT_I4)
	DISP_FUNCTION_ID	(ALIC,	"CanUndo",					DISPID_CanUndo,					OLE_GetCanUndo,			VT_BOOL, VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"CanRedo",					DISPID_CanRedo,					OLE_GetCanRedo,			VT_BOOL, VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"Undo",						DISPID_Undo,					OLE_Undo,				VT_EMPTY,	VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"Redo",						DISPID_Redo,					OLE_Redo,				VT_EMPTY,	VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"CommitUndo",				DISPID_CommitUndo,				OLE_CommitUndo,			VT_EMPTY,	VTS_NONE)

// Font/Para DLG
	DISP_FUNCTION_ID	(ALIC,	"LaunchFontSettingsDialog",			DISPID_LaunchFontSettingsDialog,		OLE_LaunchFontSettingsDialog,	VT_EMPTY,	VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"LaunchParagraphSettingsDialog",	DISPID_LaunchParagraphSettingsDialog,	OLE_LaunchParagraphSettingsDialog,	VT_EMPTY,	VTS_NONE)

// Find support
	DISP_FUNCTION_ID	(ALIC,	"LaunchFindDialog",			DISPID_LaunchFindDialog,		OLE_LaunchFindDialog,	VT_EMPTY,	VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"Find",						DISPID_Find,					OLE_Find,				VT_I4,	VTS_I4 VTS_VARIANT VTS_BOOL VTS_BOOL VTS_BOOL)

// Replace support
	DISP_FUNCTION_ID	(ALIC,	"LaunchReplaceDialog",		DISPID_LaunchReplaceDialog,		OLE_LaunchReplaceDialog,VT_EMPTY,	VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"FindReplace",				DISPID_FindReplace,				OLE_FindReplace,		VT_I4,	VTS_I4 VTS_VARIANT VTS_VARIANT VTS_BOOL VTS_BOOL VTS_BOOL)

// Print support
	DISP_FUNCTION_ID	(ALIC,	"PrinterSetupDialog",		DISPID_PrinterSetupDialog,		OLE_PrinterSetupDialog,	VT_EMPTY,	VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"PrintDialog",				DISPID_PrintDialog,				OLE_PrintDialog,		VT_EMPTY,	VTS_NONE)
	DISP_FUNCTION_ID	(ALIC,	"PrintOnce",				DISPID_PrintOnce,				OLE_PrintOnce,			VT_EMPTY,	VTS_NONE)


// GetHeight
	DISP_FUNCTION_ID	(ALIC,	"GetHeight",				DISPID_GetHeight,				OLE_GetHeight,			VT_I4,	VTS_I4 VTS_I4)

//DIRTY support
	DISP_PROPERTY_EX_ID	(ALIC,	"Dirty",					DISPID_Dirty,					OLE_GetDirty, OLE_SetDirty, VT_BOOL)

END_DISPATCH_MAP()








BEGIN_EVENT_MAP(ActiveLedItControl, COleControl)
	EVENT_CUSTOM_ID			("Change", DISPID_Change, _IGNORED_, VTS_NONE)
	EVENT_STOCK_CLICK		()
	EVENT_STOCK_DBLCLICK	()
	EVENT_STOCK_KEYDOWN		()
	EVENT_STOCK_KEYUP		()
	EVENT_STOCK_KEYPRESS	()
	EVENT_STOCK_MOUSEDOWN	()
	EVENT_STOCK_MOUSEMOVE	()
	EVENT_STOCK_MOUSEUP		()
	EVENT_STOCK_KEYPRESS	()
	EVENT_CUSTOM_ID			("SelChange", DISPID_SelChange, _IGNORED_, VTS_NONE)
	EVENT_CUSTOM_ID			("UpdateUserCommand", DISPID_UpdateUserCommand, _IGNORED_, VTS_NONE)
	EVENT_CUSTOM_ID			("UserCommand", DISPID_UserCommand, _IGNORED_, VTS_NONE)
END_EVENT_MAP()




#if		qKeepListOfALInstancesForSPR_1599BWA
set<ActiveLedItControl*>	ActiveLedItControl::sAll;

const set<ActiveLedItControl*>&	ActiveLedItControl::GetAll ()
{
	return sAll;
}
#endif
	



BEGIN_PROPPAGEIDS(ActiveLedItControl, 1)
	PROPPAGEID(ActiveLedItPropPage::guid)
END_PROPPAGEIDS(ActiveLedItControl)


// Initialize class factory and guid
IMPLEMENT_OLECREATE_EX(ActiveLedItControl, "ActiveLedIt.ActiveLedItCtrl.1",
	0x9a013a76, 0xad34, 0x11d0, 0x8d, 0x9b, 0, 0xa0, 0xc9, 0x8, 0xc, 0x73)

// Type library ID and version
IMPLEMENT_OLETYPELIB(ActiveLedItControl, _tlid, _wVerMajor, _wVerMinor)



ActiveLedItControl::ActiveLedItControl ():
	COleControl (),
	MarkerOwner (),
	LedItViewController (),
	fSpellChecker (NULL),
	fLedSpellCheckWrapper (NULL),
	fConextMenu (),
	fToolbarList (),
	fBuiltinCommands (),
	fPredefinedMenus (),
	fAcceleratorTable (),
	fWin32AccelTable (NULL),
	fLastAccelTableUpdateAt (0),
	fCurrentEventArguments (),
	fEditor (),
	fDataDirty (false)
{
	fConextMenu.Attach (OLE_GetDefaultContextMenu ());
	fToolbarList.Attach (OLE_MakeNewToolbarList ());
	fAcceleratorTable.Attach (OLE_GetDefaultAcceleratorTable ());

	InitializeIIDs (&DIID__DActiveLedIt, &DIID__DActiveLedItEvents);

	SetInitialSize (300, 150);	// Specify default control size... Really should be based on GetSystemMetrics, or screen resolution or something...

	EnableSimpleFrame ();

	fTextStore.AddMarkerOwner (this);
	fEditor.SetController (this);
	#if		qKeepListOfALInstancesForSPR_1599BWA
		sAll.insert (this);
	#endif
}

ActiveLedItControl::~ActiveLedItControl ()
{
	#if		qKeepListOfALInstancesForSPR_1599BWA
		sAll.erase (this);
	#endif
	if (fSpellChecker != NULL) {
		fSpellChecker->Release ();
		fSpellChecker = NULL;
		fEditor.SetSpellCheckEngine (NULL);
	}
	fEditor.SetController (NULL);
	fTextStore.RemoveMarkerOwner (this);
	if (fWin32AccelTable != NULL) {
		::DestroyAcceleratorTable (fWin32AccelTable);
	}
}

void	ActiveLedItControl::DidUpdateText (const UpdateInfo& updateInfo) throw ()
{
	if (updateInfo.fRealContentUpdate) {
		SetModifiedFlag ();
		fDataDirty = true;
		FireOLEEvent (DISPID_Change);
	}
}

TextStore*	ActiveLedItControl::PeekAtTextStore () const
{
	return &const_cast<ActiveLedItControl*> (this)->fTextStore;
}

void	ActiveLedItControl::OnDraw (CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	Led_RequireNotNil (pdc);
	TRACE ("ActiveLedItControl::OnDraw (rcBounds= (%d, %d, %d, %d), rcInvalid= (%d, %d, %d, %d))\n",
			rcBounds.top, rcBounds.left, rcBounds.bottom, rcBounds.right,
			rcInvalid.top, rcInvalid.left, rcInvalid.bottom, rcInvalid.right
		);
	Led_Rect	invalRect	=	AsLedRect (rcInvalid);
	if (invalRect.IsEmpty ()) {
		return;
	}

	Led_Rect	oldWinRect	=	fEditor.GetWindowRect ();
	try {
		Led_Rect	newWinRect	=	oldWinRect;
		if (m_hWnd == NULL) {
			/*
			 *	This can occur occasionally, when called from places like MSWord XP, and wordpad.
			 *	This MAYBE because these apps don't respect OLEMISC_SETCLIENTSITEFIRST?
			 */
			fEditor.SetDefaultTextColor (TextImager::eDefaultBackgroundColor, Led_Color (TranslateColor (GetBackColor ())));

			Led_Size	controlSize	=	Led_Size (0, 0);
			{
				int cx;
				int cy;
				GetControlSize (&cx, &cy);
				controlSize = Led_Size (cy, cx);
			}
			newWinRect = Led_Rect (AsLedRect (rcBounds).GetOrigin (), controlSize);
		}
		else {
			/*
			 *	Queer logic, and I'm not really sure what is right to do here. If the given rcBounds
			 *	are the same as the last size handled to our last OnWindowPosChanged () - then this uses
			 *	the same WindowRect for Led. But otherwise, it offsets it. Without this logic, AL instances
			 *	embedded in WordPad or LedIt mess up badly when not leftmost in the embedding editors
			 *	window. -- LGP 2003-05-06
			 */
			RECT	editorWndRct;
			Led_Verify (::GetWindowRect (m_hWnd, &editorWndRct));
			ScreenToClient (&editorWndRct);
			newWinRect	+=	AsLedRect (rcBounds).GetOrigin () - AsLedRect (editorWndRct).GetOrigin ();
		}
		fEditor.SetWindowRect (newWinRect);
		Led_MFC_TabletFromCDC			useTablet	=	Led_MFC_TabletFromCDC (pdc);
		LedItView::TemporarilyUseTablet	tmpUseTablet (fEditor, useTablet);
		fEditor.WindowDrawHelper (useTablet, invalRect, false);
		fEditor.SetWindowRect (oldWinRect);
	}
	catch (...) {
		// Ignore exceptions on display. Would be had. Assert error if ever happens, but in release control,
		// not much we can usefully do...
		Led_Assert (false);
		fEditor.SetWindowRect (oldWinRect);
	}
}

void	ActiveLedItControl::OnDrawMetafile (CDC* pDC, const CRect& rcBounds)
{
	/*
	 *	Override COleControl::OnDrawMetafile - to properly handle drawing to a metafile.
	 *	See SPR#1447.
	 */
	Led_RequireNotNil (pDC);
	TRACE("ActiveLedItControl::OnDrawMetafile (rcBounds= (%d, %d, %d, %d))\n",
			rcBounds.top, rcBounds.left, rcBounds.bottom, rcBounds.right
		);

	{
		int cx;
		int cy;
		GetControlSize (&cx, &cy);
		CRect rc (0, 0, cx, cy);
		pDC->SetViewportOrg (0, 0);
		pDC->SetViewportExt (cx, cy);
	}

	Led_MFC_TabletFromCDC	useTablet							=	Led_MFC_TabletFromCDC (pDC);
	bool					oldImageUsingOffscreenBitmapsFlag	=	fEditor.GetImageUsingOffscreenBitmaps ();
	bool					oldInDrawMetaFileMode				=	fEditor.fInDrawMetaFileMode;
	Led_Rect				oldWinRect							=	fEditor.GetWindowRect ();
	try {
		Led_Rect	newWinRect	=	AsLedRect (rcBounds);

		{
			Led_Rect		wmr	=	useTablet->CvtFromTWIPS (fEditor.GetDefaultWindowMargins ());
			newWinRect.top += wmr.GetTop ();
			newWinRect.left += wmr.GetLeft ();
			newWinRect.bottom -= wmr.GetBottom ();
			newWinRect.right -= wmr.GetRight ();
		}

		fEditor.SetImageUsingOffscreenBitmaps (false);
		fEditor.fInDrawMetaFileMode = true;
		if (not newWinRect.IsEmpty ()) {
			LedItView::TemporarilyUseTablet	tmpUseTablet (fEditor, useTablet);
			fEditor.SetWindowRect (newWinRect);
			fEditor.WindowDrawHelper (useTablet, AsLedRect (rcBounds), false);
		//	fEditor.WindowDrawHelper (useTablet, newWinRect, true);
			fEditor.SetWindowRect (oldWinRect);
		}
		fEditor.fInDrawMetaFileMode = oldInDrawMetaFileMode;
		fEditor.SetWindowRect (oldWinRect);
		fEditor.SetImageUsingOffscreenBitmaps (oldImageUsingOffscreenBitmapsFlag);
	}
	catch (...) {
		// Ignore exceptions on display. Would be had. Assert error if ever happens, but in release control,
		// not much we can usefully do...
		Led_Assert (false);
		fEditor.SetImageUsingOffscreenBitmaps (oldImageUsingOffscreenBitmapsFlag);
		fEditor.fInDrawMetaFileMode = oldInDrawMetaFileMode;
		fEditor.SetWindowRect (oldWinRect);
	}
}

BOOL	ActiveLedItControl::OnEraseBkgnd (CDC* pDC)
{
	Led_RequireNotNil (pDC);
	return true;	// cuz we erase our own background...
}

void	ActiveLedItControl::DoPropExchange (CPropExchange* pPX)
{
	Led_RequireNotNil (pPX);
	ExchangeVersion (pPX, MAKELONG (_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange (pPX);
	ExchangeTextAsRTFBlob (pPX);

	// For SPR#0835- added the ability to load these selected properties. May want to add more in the future.
	if (pPX->IsLoading ()) {
		try {
			// default font face for the control
			CString	aProp;
			PX_String (pPX, _T("DefaultFontFace"), aProp, _T(""));
			if (not aProp.IsEmpty ()) {
				Led_IncrementalFontSpecification	applyFontSpec;
				applyFontSpec.SetFontName ((const TCHAR*)aProp);
				fEditor.SetDefaultFont (applyFontSpec);
			}
		}
		catch (...) {
		}

		try {
			// default font size for the control
			CString	aProp;
			PX_String (pPX, _T("DefaultFontSize"), aProp, _T(""));
			if (not aProp.IsEmpty ()) {
				Led_IncrementalFontSpecification	applyFontSpec;
				applyFontSpec.SetPointSize (_ttoi ((const TCHAR*)aProp));
				fEditor.SetDefaultFont (applyFontSpec);
			}
		}
		catch (...) {
		}

		try {
			// 
			BOOL	aProp	=	false;
			if (PX_Bool (pPX, _T("ReadOnly"), aProp)) {
				OLE_SetReadOnly (aProp);
			}
		}
		catch (...) {
		}

		try {
			// Plain "Text" property
			CString	aProp;
			PX_String (pPX, _T("Text"), aProp, _T(""));
			if ((fTextStore.GetLength () == 0) and (not aProp.IsEmpty ())) {
				SetBufferTextCRLF (aProp);
			}
		}
		catch (...) {
		}

		try {
			// "TextRTF" property
			CString	aProp;
			PX_String (pPX, _T("TextRTF"), aProp, _T(""));
			if ((fTextStore.GetLength () == 0) and (not aProp.IsEmpty ())) {
				SetBufferTextAsRTF (aProp);
			}
		}
		catch (...) {
		}

		try {
			// "TextHTML" property
			CString	aProp;
			PX_String (pPX, _T("TextHTML"), aProp, _T(""));
			if ((fTextStore.GetLength () == 0) and (not aProp.IsEmpty ())) {
				SetBufferTextAsHTML (aProp);
			}
		}
		catch (...) {
		}
	}
}

// ActiveLedItControl::GetControlFlags -
// Flags to customize MFC's implementation of ActiveX controls.
//
// For information on using these flags, please see MFC technical note
// #nnn, "Optimizing an ActiveX Control".
DWORD	ActiveLedItControl::GetControlFlags ()
{
	DWORD dwFlags = COleControl::GetControlFlags ();
#if 0
	// The control will not be redrawn when making the transition
	// between the active and inactivate state.
	dwFlags |= noFlickerActivate;

//DISABLED CUZ WE DONT SUPPORT THIS IN OUR ONDRAW() method...
	// The control can optimize its OnDraw method, by not restoring
	// the original GDI objects in the device context.
	dwFlags |= canOptimizeDraw;
#endif
	return dwFlags;
}

void	ActiveLedItControl::OnGetControlInfo (LPCONTROLINFO pControlInfo)
{
	Led_RequireNotNil (pControlInfo);
	::memset (pControlInfo, 0, sizeof (*pControlInfo));
	pControlInfo->cb = sizeof (*pControlInfo);

	// Unfortunately, the Win32 SDK documeantion is vague about the lifetime of the array returns
	// as part of the CONTROLINFO structure. Hopefully this will cause no problems. Empirically - it
	// doesn't seem to get called very often. And - the value should rarely change (but under OLE AUT control,
	// so it COULD) -- LGP 2004-01-24
	HACCEL	hAccel	=	GetCurrentWin32AccelTable ();

	//pControlInfo->hAccel = ::LoadAccelerators (AfxGetResourceHandle (),MAKEINTRESOURCE (kAcceleratorTableID));
	pControlInfo->hAccel = hAccel;
	// NB: queer - but CopyAcceleratorTable also counts # of entries...
	pControlInfo->cAccel = pControlInfo->hAccel == NULL? 0: ::CopyAcceleratorTable (pControlInfo->hAccel, NULL, 0);
	pControlInfo->dwFlags = CTRLINFO_EATS_RETURN;
}

BOOL	ActiveLedItControl::PreTranslateMessage (MSG* pMsg)
{
	Led_RequireNotNil (pMsg);
	switch (pMsg->message) {
		case WM_KEYDOWN:
		case WM_KEYUP:
		switch (pMsg->wParam) {
			case	VK_TAB: {
				// Quirks here are for SPR#0829
				if (pMsg->message == WM_KEYDOWN) {
					fEditor.SendMessage (WM_CHAR, VK_TAB, pMsg->lParam);
				}
				return TRUE;
			}
			break;
			case	VK_UP:
			case	VK_DOWN:
			case	VK_LEFT:
			case	VK_RIGHT:
			case	VK_HOME:
			case	VK_END:
			case	VK_PRIOR:
			case	VK_NEXT:
			fEditor.SendMessage (pMsg->message, pMsg->wParam, pMsg->lParam);
			return TRUE;
		}
		break;
	}

	HACCEL	hAccel	=	GetCurrentWin32AccelTable ();
	if (hAccel != NULL) {
		return ::TranslateAccelerator (pMsg->hwnd, hAccel, pMsg);
	}
	return false;	// so command dispatched normally
}

void	ActiveLedItControl::AddFrameLevelUI ()
{
// Kindof a hack - seems this is called when we get activated, and RemoveFrameLevelUI () is called when we are
// deactivated. This is about the behavior we want for those cases. Sadly, I've found no docs to see the
// "right" way to handle this...
	COleControl::AddFrameLevelUI ();
	#if		qDisableEditorWhenNotActive
		fEditor.ModifyStyle (WS_DISABLED, 0);
		fEditor.SetFocus ();
	#endif
}

void	ActiveLedItControl::RemoveFrameLevelUI ()
{
	#if		qDisableEditorWhenNotActive
		fEditor.ModifyStyle (0, WS_DISABLED);
		if (CWnd::GetFocus () == &fEditor) {
			// Not sure who to set the focus to, but just anybody but ME...
			CWnd::GetDesktopWindow ()->SetFocus ();
		}
	#endif
	COleControl::RemoveFrameLevelUI ();
}

BOOL	ActiveLedItControl::OnSetObjectRects (LPCRECT lprcPosRect, LPCRECT lprcClipRect)
{
	#if		qDefaultTracingOn
		if (lprcClipRect == NULL) {
			LedDebugTrace (Led_SDK_TCHAROF ("ActiveLedItControl::OnSetObjectRects (m_bUIActive=%d, m_bInPlaceSiteWndless=%d, lprcPosRect=(%d, %d, %d, %d), NULL)\n"),
					m_bUIActive, m_bInPlaceSiteWndless,
					lprcPosRect->top, lprcPosRect->left, lprcPosRect->bottom, lprcPosRect->right
				);
		}
		else {
			LedDebugTrace (Led_SDK_TCHAROF ("ActiveLedItControl::OnSetObjectRects (m_bUIActive=%d, m_bInPlaceSiteWndless=%d, lprcPosRect=(%d, %d, %d, %d), lprcClipRect=(%d, %d, %d, %d))\n"),
					m_bUIActive, m_bInPlaceSiteWndless,
					lprcPosRect->top, lprcPosRect->left, lprcPosRect->bottom, lprcPosRect->right,
					lprcClipRect->top, lprcClipRect->left, lprcClipRect->bottom, lprcClipRect->right
				);
		}
	#endif
	#if		0
		// keep debugging hack for SPR#1168
		if (lprcClipRect != NULL) {
			if (AsLedRect (*lprcPosRect) != AsLedRect (*lprcClipRect)) {
				int breakHEre = 1;
			}
		}
	#endif
	BOOL	r	=	COleControl::OnSetObjectRects (lprcPosRect, lprcClipRect);
	return r;
}

BOOL ActiveLedItControl::OnGetNaturalExtent (DWORD /* dwAspect */, LONG /* lindex */,
		DVTARGETDEVICE* /* ptd */, HDC /* hicTargetDev */,
		DVEXTENTINFO* /*pExtentInfo*/, LPSIZEL /*psizel*/
	)
{
	// Tried overriding this to address teh funny size of the AL widget when inserted into
	// LedIt 3.1a6x. Its a very small issue. BUt - for whatever reason - this method never
	// seems to get called - LGP 2003-04-29.
	return FALSE;
}

void	ActiveLedItControl::OnResetState ()
{
	COleControl::OnResetState ();  // Resets defaults found in DoPropExchange
}

void	ActiveLedItControl::ExchangeTextAsRTFBlob (CPropExchange* pPX)
{
	const	LPCTSTR	kTextAsRTFBlob	=	_T("TextAsRTFBlob");
	Led_RequireNotNil (pPX);
	if (pPX->IsLoading ()) {
		HGLOBAL	hglobal	=	NULL;
		PX_Blob (pPX, kTextAsRTFBlob, hglobal);
		if (hglobal != NULL) {
			try {
				Led_StackBasedHandleLocker	hdl (hglobal);
				const Byte*	data = reinterpret_cast<Byte*> (hdl.GetPointer ());
				if (data != NULL) {
					size_t	size	=	*(size_t*)data;
					string	s		=	string (((const char*)data) + sizeof (size_t), size);
					SetBufferTextAsRTF (Led_ANSI2SDKString (s).c_str ());
				}
				::GlobalFree (hglobal);
			}
			catch (...) {
				if (hglobal != NULL) {
					::GlobalFree (hglobal);
				}
				throw;
			}
		}
	}
	else {
		string	s		=	GetBufferTextAsRTF_ ();
		size_t	len		=	s.length ();
		HGLOBAL	hglobal	=	::GlobalAlloc (GMEM_MOVEABLE, len + sizeof (size_t));
		if (hglobal != NULL) {
			{
				Led_StackBasedHandleLocker	hdl (hglobal);
				void* pvBlob = hdl.GetPointer ();
				Led_AssertNotNil (pvBlob);
				*(long*)pvBlob = len;
				::memcpy (reinterpret_cast<char*> (pvBlob) + sizeof (size_t), s.c_str (), len);
			}
			try {
				PX_Blob (pPX, kTextAsRTFBlob, hglobal);
				::GlobalFree (hglobal);
			}
			catch (...) {
				::GlobalFree (hglobal);
				throw;
			}
		}
	}
}

void	ActiveLedItControl::AboutBox ()
{
	OnAboutBoxCommand ();
}

Led_FileFormat	ActiveLedItControl::GuessFormatFromName (LPCTSTR name)
{
	Led_FileFormat	format		=	eUnknownFormat;
	Led_SDK_String	pathName	=	name;
	Led_SDK_String	fileName	=	pathName.substr (pathName.rfind (_T ("\\")) + 1);
	Led_SDK_String	suffix		=	(fileName.rfind (_T (".")) == Led_SDK_String::npos)? _T (""): fileName.substr (fileName.rfind (_T ("."))+1);
	if (::_tcsicmp (suffix.c_str(), _T ("txt")) == 0) {
		format = eTextFormat;
	}
	else if (::_tcsicmp (suffix.c_str(), _T ("led")) == 0) {
		format = eLedPrivateFormat;
	}
	else if (::_tcsicmp (suffix.c_str(), _T ("rtf")) == 0) {
		format = eRTFFormat;
	}
	else if (::_tcsicmp (suffix.c_str(), _T ("htm")) == 0 or ::_tcsicmp (suffix.c_str(), _T ("html")) == 0) {
		format = eHTMLFormat;
	}
	return format;
}

void	ActiveLedItControl::DoReadFile (LPCTSTR filename, Led_SmallStackBuffer<char>* buffer, size_t* size)
{
	int	fd	=	::_topen (filename, O_RDONLY | O_BINARY, _S_IREAD);
	try {
		if (fd < 0) {
			AfxThrowFileException (CFileException::fileNotFound, -1, filename);
		}
		long	eof	=	::_lseek (fd, 0, SEEK_END);
		if (eof < 0) {
			AfxThrowFileException (CFileException::badSeek, -1, filename);
		}
		buffer->GrowToSize (eof);
		if (size != NULL) {
			*size = eof;
		}
		(void)::_lseek (fd, 0, SEEK_SET);
		int	nBytes	=	::_read (fd, *buffer, eof);
		if (nBytes != eof) {
			AfxThrowFileException (CFileException::generic, -1, filename);
		}
		(void)::_close (fd);
	}
	catch (...) {
		if (fd >= 0) {
			(void)::_close (fd);
		}
		throw;
	}
}

void	ActiveLedItControl::WriteBytesToFile (LPCTSTR filename, const void* buffer, size_t size)
{
	(void)::_tunlink (filename);
	int	fd	=	::_topen (filename, O_CREAT | O_RDWR | O_TRUNC | O_BINARY, _S_IREAD | _S_IWRITE);
	try {
		if (fd < 0) {
			AfxThrowFileException (CFileException::fileNotFound, -1, filename);
		}
		int	nBytes	=	::_write (fd, buffer, size);
		if (nBytes != size) {
			AfxThrowFileException (CFileException::generic, -1, filename);
		}
		(void)::_close (fd);
	}
	catch (...) {
		if (fd >= 0) {
			(void)::_close (fd);
		}
		throw;
	}
}

void	ActiveLedItControl::LoadFile (LPCTSTR filename)
{
	fCommandHandler.Commit ();
	fEditor.Replace (0, fEditor.GetEnd (), LED_TCHAR_OF (""), 0);

	Led_SmallStackBuffer<char>	buffer (0);
	size_t						size	=	0;
	DoReadFile (filename, &buffer, &size);

	StyledTextIOSrcStream_Memory					source (buffer, size);
	WordProcessor::WordProcessorTextIOSinkStream	sink (&fEditor);

	Led_FileFormat	format	=	GuessFormatFromName (filename);

ReRead:
	switch (format) {
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
			StyledTextIOReader_RTF	textReader (&source, &sink);
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
				StyledTextIOReader_RTF	reader (&source, &sink);
				if (reader.QuickLookAppearsToBeRightFormat ()) {
					format = eRTFFormat;
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
					format = eLedPrivateFormat;
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
					format = eHTMLFormat;
					goto ReRead;
				}
			}
			catch (...) {
				// ignore any errors, and proceed to next file type
			}

			// Nothing left todo but to read the text file as plain text, as best we can...
			format = eTextFormat;
			goto ReRead;
		}
		break;

		default: {
			Led_Assert (false);	// don't support reading that format (yet?)!
		}
		break;
	}
	fEditor.SetEmptySelectionStyle ();
}

void	ActiveLedItControl::SaveFile (LPCTSTR filename)
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	WordProcessor::WordProcessorTextIOSrcStream		source (&fEditor);
	StyledTextIOWriterSinkStream_Memory				sink;
	switch (GuessFormatFromName (filename)) {
		case	eTextFormat: {
			StyledTextIOWriter_PlainText	textWriter (&source, &sink);
			textWriter.Write ();
		}
		break;

		case	eRTFFormat: {
			StyledTextIOWriter_RTF	textWriter (&source, &sink);
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
			StyledTextIOWriter_RTF	textWriter (&source, &sink);
			textWriter.Write ();
		}
		break;
	}
	WriteBytesToFile (filename, sink.PeekAtData (), sink.GetLength ());
}

void	ActiveLedItControl::SaveFileCRLF (LPCTSTR filename)
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	WordProcessor::WordProcessorTextIOSrcStream	source (&fEditor);
	StyledTextIOWriterSinkStream_Memory			sink;
	StyledTextIOWriter_PlainText				textWriter (&source, &sink);
	textWriter.Write ();
	WriteBytesToFile (filename, sink.PeekAtData (), sink.GetLength ());
}

void	ActiveLedItControl::SaveFileRTF (LPCTSTR filename)
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	WordProcessor::WordProcessorTextIOSrcStream	source (&fEditor);
	StyledTextIOWriterSinkStream_Memory			sink;
	StyledTextIOWriter_RTF						textWriter (&source, &sink);
	textWriter.Write ();
	WriteBytesToFile (filename, sink.PeekAtData (), sink.GetLength ());
}

void	ActiveLedItControl::SaveFileHTML (LPCTSTR filename)
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	WordProcessor::WordProcessorTextIOSrcStream	source (&fEditor);
	StyledTextIOWriterSinkStream_Memory			sink;
	StyledTextIOWriter_HTML						textWriter (&source, &sink);
	textWriter.Write ();
	WriteBytesToFile (filename, sink.PeekAtData (), sink.GetLength ());
}

void	ActiveLedItControl::Refresh ()
{
	fEditor.Refresh ();
}

void	ActiveLedItControl::ScrollToSelection ()
{
	fEditor.ScrollToSelection ();
}

long	ActiveLedItControl::OLE_GetMaxUndoLevel ()
{
	return fCommandHandler.GetMaxUnDoLevels ();
}

void	ActiveLedItControl::OLE_SetMaxUndoLevel (long maxUndoLevel)
{
	// sanity check arguments
	maxUndoLevel = max (0, maxUndoLevel);
	maxUndoLevel = min (maxUndoLevel, 10);
	fCommandHandler.SetMaxUnDoLevels (maxUndoLevel);
}

BOOL	ActiveLedItControl::OLE_GetCanUndo ()
{
	return fCommandHandler.CanUndo () or (fCommandHandler.GetMaxUnDoLevels ()==1 and fCommandHandler.CanRedo ());
}

BOOL	ActiveLedItControl::OLE_GetCanRedo ()
{
	return fCommandHandler.CanRedo ();
}

void	ActiveLedItControl::OLE_Undo ()
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	if (fCommandHandler.CanUndo ()) {
		fCommandHandler.DoUndo (fEditor);
	}
	else if (fCommandHandler.GetMaxUnDoLevels ()==1 and fCommandHandler.CanRedo ()) {
		fCommandHandler.DoRedo (fEditor);
	}
	else {
		// ignore bad undo request - no ability to return errors here til we redo this in ATL
	}
}

void	ActiveLedItControl::OLE_Redo ()
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	if (fCommandHandler.CanRedo ()) {
		fCommandHandler.DoRedo (fEditor);
	}
	else {
		// ignore bad undo request - no ability to return errors here til we redo this in ATL
	}
}

void	ActiveLedItControl::OLE_CommitUndo ()
{
	fCommandHandler.Commit ();
}

void	ActiveLedItControl::OLE_LaunchFontSettingsDialog ()
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	fEditor.OnChooseFontCommand ();
}

void	ActiveLedItControl::OLE_LaunchParagraphSettingsDialog ()
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	fEditor.OnParagraphSpacingChangeCommand ();
}

void	ActiveLedItControl::OLE_LaunchFindDialog ()
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	fEditor.OnFindCommand ();
}

long	ActiveLedItControl::OLE_Find (long searchFrom, const VARIANT& findText, BOOL wrapSearch, BOOL wholeWordSearch, BOOL caseSensativeSearch)
{
	// don't have any better error checking technology here - should return E_INVALIDARG!!!
	if (searchFrom < 0) {
		return -1;
	}
	if (static_cast<size_t> (searchFrom) > fTextStore.GetEnd ()) {
		return -1;
	}
	if (findText.vt != VT_BSTR) {
		return -1;
	}

	TextStore::SearchParameters	parameters;

	#if		qWideCharacters
		parameters.fMatchString = findText.bstrVal;
	#else
		USES_CONVERSION;
		parameters.fMatchString = OLE2A (findText.bstrVal);
	#endif
	parameters.fWrapSearch = wrapSearch;
	parameters.fWholeWordSearch = wholeWordSearch;
	parameters.fCaseSensativeSearch = caseSensativeSearch;

	size_t	whereTo	=	fTextStore.Find (parameters, searchFrom);

	return whereTo;
}

void	ActiveLedItControl::OLE_LaunchReplaceDialog ()
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	fEditor.OnReplaceCommand ();
}

long	ActiveLedItControl::OLE_FindReplace (long searchFrom, const VARIANT& findText, const VARIANT& replaceText, BOOL wrapSearch, BOOL wholeWordSearch, BOOL caseSensativeSearch)
{
	// don't have any better error checking technology here - should return E_INVALIDARG!!!
	if (searchFrom < 0) {
		return -1;
	}
	if (static_cast<size_t> (searchFrom) > fTextStore.GetEnd ()) {
		return -1;
	}
	if (findText.vt != VT_BSTR) {
		return -1;
	}
	if (replaceText.vt != VT_BSTR) {
		return -1;
	}

	TextStore::SearchParameters	parameters;

	#if		qWideCharacters
		Led_tString	findStr			=	findText.bstrVal;
		Led_tString	replaceStr		=	replaceText.bstrVal;
	#else
		USES_CONVERSION;
		Led_tString	findStr			=	OLE2A (findText.bstrVal);
		Led_tString	replaceStr		=	OLE2A (replaceText.bstrVal);
	#endif
	return fEditor.OLE_FindReplace (searchFrom, findStr, replaceStr, wrapSearch, wholeWordSearch, caseSensativeSearch);
}

void	ActiveLedItControl::OLE_PrinterSetupDialog ()
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	fEditor.OnFilePrintSetup ();
}

void	ActiveLedItControl::OLE_PrintDialog ()
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	fEditor.OnFilePrint ();
}

void	ActiveLedItControl::OLE_PrintOnce ()
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	fEditor.OnFilePrintOnce ();
}

long	ActiveLedItControl::OLE_GetHeight (long from, long to)
{
	typedef	MultiRowTextImager::RowReference	RowReference;
	if (from < 0) {
		from = 0;
	}
	if (to < 0) {
		to = fTextStore.GetEnd ();
	}
	if (from > to) {
		// throw invalid input
		return -1;
	}
	if (static_cast<size_t> (to) > fTextStore.GetEnd ()) {
		// throw invalid input
		return -1;
	}
	RowReference	startingRow	=	fEditor.GetRowReferenceContainingPosition (from);
	RowReference	endingRow	=	fEditor.GetRowReferenceContainingPosition (to);
	/*
	 *	Always take one more row than they asked for, since they will expect if you start and end on a given row - you'll get
	 *	the height of that row.
	 */
	return fEditor.GetHeightOfRows (startingRow, fEditor.CountRowDifference (startingRow, endingRow) + 1);
}

BOOL	ActiveLedItControl::OLE_GetDirty ()
{
	return fDataDirty;
}

void	ActiveLedItControl::OLE_SetDirty (BOOL dirty)
{
	fDataDirty = dirty;
}

void	ActiveLedItControl::OnBrowseHelpCommand ()
{
	Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/ActiveLedIt/UserDocs.asp", kAppName, kURLDemoFlag));
}

void	ActiveLedItControl::OnAboutBoxCommand ()
{
	class	MyAboutBox : public Led_StdDialogHelper_AboutBox {
		private:
			typedef	Led_StdDialogHelper_AboutBox	inherited;
		public:
				MyAboutBox (HINSTANCE hInstance, HWND parentWnd):
					inherited (hInstance, parentWnd)
				{
				}
		public:
			override	BOOL	OnInitDialog ()
				{
					BOOL	result	=	inherited::OnInitDialog ();

					// Cuz of fact that dlog sizes specified in dlog units, and that doesn't work well for bitmaps
					// we must resize our dlog on the fly based on pict resource size...
					const	int kPictWidth	=	437;	// must agree with ACTUAL bitmap size
					const	int kButHSluff	=	17;
					#if		qDemoMode
						const	int kButVSluff	=	136;
						const	int kPictHeight	=	393;
					#else
						const	int kButVSluff	=	61;
						const	int kPictHeight	=	318;
					#endif
					{
						RECT	windowRect;
						::GetWindowRect (GetHWND (), &windowRect);
						// figure size of non-client area...
						int	ncWidth		=	0;
						int	ncHeight	=	0;
						{
							RECT	clientRect;
							::GetClientRect (GetHWND (), &clientRect);
							ncWidth = AsLedRect (windowRect).GetWidth () - AsLedRect (clientRect).GetWidth ();
							ncHeight = AsLedRect (windowRect).GetHeight () - AsLedRect (clientRect).GetHeight ();
						}
						::MoveWindow (GetHWND (), windowRect.left, windowRect.top, kPictWidth+ncWidth, kPictHeight+ncHeight, false);
					}

					// Place and fill in version information
					{
						HWND	w	=	::GetDlgItem (GetHWND (), kLedStdDlg_AboutBox_VersionFieldID);
						Led_AssertNotNil (w);
						const	int	kVERWidth	=	230;
						#if		qDemoMode
							::MoveWindow (w, kPictWidth/2 - kVERWidth/2, 35, 230, 2*14, false);
						#else
							::MoveWindow (w, kPictWidth/2 - kVERWidth/2, 35, 230, 14, false);
						#endif
						#if		_UNICODE
							#define	kUNICODE_NAME_ADORNER	L" [UNICODE]"
						#elif	qWideCharacters
							#define	kUNICODE_NAME_ADORNER	" [Internal UNICODE]"
						#else
							#define	kUNICODE_NAME_ADORNER
						#endif
						#if		qDemoMode
							TCHAR	nDaysString[1024];
							(void)::_stprintf (nDaysString, _T("%d"), DemoPrefs ().GetDemoDaysLeft ());
						#endif
						::SetWindowText (w,
											(
												Led_SDK_String (_T (qLed_ShortVersionString) kUNICODE_NAME_ADORNER _T (" (") _T (__DATE__) _T (")"))
												#if		qDemoMode
												 + Led_SDK_String (_T("\r\nDemo expires in ")) + nDaysString + _T(" days.")
												#endif
											).c_str ()
										);
					}

					// Place hidden buttons which map to URLs
					{
						HWND	w	=	::GetDlgItem (GetHWND (), kLedStdDlg_AboutBox_InfoLedFieldID);
						Led_AssertNotNil (w);
						::MoveWindow (w, 15, 159, 142, 17, false);
						w	=	::GetDlgItem (GetHWND (), kLedStdDlg_AboutBox_LedWebPageFieldID);
						Led_AssertNotNil (w);
						::MoveWindow (w, 227, 159, 179, 17, false);
					}

					// Place OK button
					{
						HWND	w	=	::GetDlgItem (GetHWND (), IDOK);
						Led_AssertNotNil (w);
						RECT	tmp;
						::GetWindowRect (w, &tmp);
						::MoveWindow (w, kButHSluff, kPictHeight - AsLedRect (tmp).GetHeight ()-kButVSluff, AsLedRect (tmp).GetWidth (), AsLedRect (tmp).GetHeight (), false);	// width/height we should presevere
					}

					::SetWindowText (GetHWND (), _T ("About ActiveLedIt!"));

					return (result);
				}
		public:
			override	void	OnClickInInfoField ()
				{
					try {
						Led_URLManager::Get ().Open ("mailto:info-led@sophists.com");
					}
					catch (...) {
						// ignore for now - since errors here prent dialog from dismissing (on MacOSX)
					}
					inherited::OnClickInInfoField ();
				}

			override	void	OnClickInLedWebPageField ()
				{
					try {
						Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/ActiveLedIt/Default.asp", kAppName, kURLDemoFlag));
					}
					catch (...) {
						// ignore for now - since errors here prent dialog from dismissing (on MacOSX)
					}
					inherited::OnClickInLedWebPageField ();
				}
	};
	MyAboutBox	dlg (::AfxGetResourceHandle (), fEditor.GetHWND ());
	dlg.DoModal ();
}

void	ActiveLedItControl::ForceUIActive ()
{
	OnActivateInPlace (TRUE, NULL); // == UI-Activate the control
}

void	ActiveLedItControl::FireOLEEvent (DISPID eventID)
{
	FireOLEEvent (eventID, EVENT_PARAM(VTS_NONE));
}

void	ActiveLedItControl::FireOLEEvent (DISPID dispid, BYTE* pbParams, ...)
{
	// Clone of COleControl::FireEvent, but cannot call directly cuz of the va_list stuff.
	va_list argList;
	va_start(argList, pbParams);
	FireEventV (dispid, pbParams, argList);
	va_end(argList);
}

void ActiveLedItControl::FireUpdateUserCommand (const wstring& internalCmdName, VARIANT_BOOL* enabled, VARIANT_BOOL* checked, wstring* name)
{
	try {
		CComObject<ActiveLedIt_CurrentEventArguments>* o = NULL;
		Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_CurrentEventArguments>::CreateInstance (&o));
		o->fInternalName = internalCmdName;
		o->fName = *name;
		o->fEnabled = *enabled;
		o->fChecked = *checked;
		fCurrentEventArguments = o;
		FireOLEEvent (DISPID_UpdateUserCommand);
		*name = o->fName;
		*enabled = o->fEnabled;
		*checked = o->fChecked;
	}
	catch (...) {
		fCurrentEventArguments.Release ();
		throw;
	}
	fCurrentEventArguments.Release ();
}

void ActiveLedItControl::FireUserCommand (const wstring& internalCmdName)
{
	try {
		CComObject<ActiveLedIt_CurrentEventArguments>* o = NULL;
		Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_CurrentEventArguments>::CreateInstance (&o));
		o->fInternalName = internalCmdName;
		fCurrentEventArguments = o;
		FireOLEEvent (DISPID_UserCommand);
	}
	catch (...) {
		fCurrentEventArguments.Release ();
		throw;
	}
	fCurrentEventArguments.Release ();
}

#if		qFunnyDisplayInDesignMode
bool	ActiveLedItControl::IsInDesignMode () const
{
	return not const_cast<ActiveLedItControl*> (this)->AmbientUserMode ();
}

bool	ActiveLedItControl::DrawExtraDesignModeBorder () const
{
	return	const_cast<ActiveLedItControl*> (this)->GetAppearance () == 0 and
			const_cast<ActiveLedItControl*> (this)->GetBorderStyle() == 0
		;
}
#endif

HMENU	ActiveLedItControl::GenerateContextMenu ()
{
	if (fConextMenu != NULL) {
		CComQIPtr<IALCommandList>	cm	=	fConextMenu;
		if (cm != NULL) {
			HMENU	menu	=	NULL;
			if (SUCCEEDED (cm->GeneratePopupMenu (fAcceleratorTable, &menu))) {
				return menu;
			}
		}
	}
	return NULL;
}

int		ActiveLedItControl::OnCreate (LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate (lpCreateStruct) == -1) {
		return -1;
	}

	fEditor.SetDefaultTextColor (TextImager::eDefaultBackgroundColor, Led_Color (TranslateColor (GetBackColor ())));

	CRect	clientRect;
	GetClientRect (&clientRect);
	if (fEditor.Create (WS_CHILD | WS_VISIBLE, clientRect, this, kEditorWindowID) == 0) {
		return -1;
	}

	CComQIPtr<IALToolbarList>	tbl	=	fToolbarList;
	if (tbl.p != NULL) {
		tbl->NotifyOfOwningActiveLedIt (CComQIPtr<IDispatch> (GetControllingUnknown ()), m_hWnd);
	}

	if (fOnCreateExtras.get () != NULL) {
		OLE_SetReadOnly (fOnCreateExtras.get ()->fReadOnly);
		OLE_SetEnabled (fOnCreateExtras.get ()->fEnabled);
		fOnCreateExtras.release ();
	}

	return 0;
}

void	ActiveLedItControl::OnWindowPosChanged (WINDOWPOS* lpwndpos)
{
	Led_RequireNotNil (lpwndpos);
	LedDebugTrace (Led_SDK_TCHAROF ("ActiveLedItControl::OnWindowPosChanged (cx=%d, cy=%d, flags=0x%x)\n"), lpwndpos->cx, lpwndpos->cy, lpwndpos->flags);

	IdleManager::NonIdleContext	nonIdleContext;

	/*
	 *	Used to adjust bounds of fEditor in the OnSize () method. Not entire sure why that wasn't good
	 *	enough. But - doing it here, and redundantly like below - seems to make the display glitch in SPR#1168
	 *	go away.	-- LGP 2003-05-06
	 */
	COleControl::OnWindowPosChanged (lpwndpos);

	Layout ();
}

void	ActiveLedItControl::OnSetFocus (CWnd* pOldWnd)
{
	IdleManager::NonIdleContext	nonIdleContext;
	COleControl::OnSetFocus (pOldWnd);
	fEditor.SetFocus ();
}

void	ActiveLedItControl::Layout ()
{
	IdleManager::NonIdleContext	nonIdleContext;
	if (m_hWnd != NULL) {
		Led_Distance	toolbarHeightUsed	=	0;

		CRect	cr;
		GetClientRect (&cr);

		try {
			CComQIPtr<IALToolbarList>	tbl	=	fToolbarList;
			if (tbl.p != NULL) {
				UINT	preferredHeight	=	0;
				Led_ThrowIfErrorHRESULT (tbl->get_PreferredHeight (&preferredHeight));
				toolbarHeightUsed = preferredHeight;
				CRect	tblRect	=	cr;
				tblRect.bottom = tblRect.top + preferredHeight;
				Led_ThrowIfErrorHRESULT (tbl->SetRectangle (tblRect.left, tblRect.top, tblRect.Width (), tblRect.Height ()));
			}
		}
		catch (...) {
		}

		CRect	editRect	=	cr;
		editRect.top += toolbarHeightUsed;
		fEditor.MoveWindow (editRect);
	}
}

void	ActiveLedItControl::OLE_InvalidateLayout ()
{
	Layout ();
}

#if		qDontUIActivateOnOpen
HRESULT	ActiveLedItControl::OnOpen (BOOL /*bTryInPlace*/, LPMSG pMsg)
{
	return OnActivateInPlace (false, pMsg);
}
#endif

void	ActiveLedItControl::OnBackColorChanged ()
{
	COleControl::OnBackColorChanged ();

	Led_Color c	=	Led_Color (TranslateColor (GetBackColor ()));
	if (fEditor.GetDefaultTextColor (TextImager::eDefaultBackgroundColor) == NULL or
		*fEditor.GetDefaultTextColor (TextImager::eDefaultBackgroundColor) != c
		) {
		fEditor.SetDefaultTextColor (TextImager::eDefaultBackgroundColor, c);
		fEditor.Refresh ();
	}
}

#if		qFunnyDisplayInDesignMode
void	ActiveLedItControl::OnAmbientPropertyChange (DISPID dispid)
{
	COleControl::OnAmbientPropertyChange (dispid);
	if (dispid == DISPID_AMBIENT_USERMODE) {
		InvalidateControl ();
		::InvalidateRect (fEditor.GetHWND (), NULL, true);	// so we get the border too
	}
}
#endif

UINT	ActiveLedItControl::OLE_VersionNumber ()
{
	return kActiveLedItDWORDVersion;
}

BSTR	ActiveLedItControl::OLE_ShortVersionString ()
{
	try {
		string	result	=	qLed_ShortVersionString + string (kDemoString);
		return CString (result.c_str ()).AllocSysString ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false); /*NOTREACHED*/ return NULL;
}

BOOL	ActiveLedItControl::OLE_GetReadOnly ()
{
	if (fEditor.m_hWnd == NULL) {
		// Avoid barfing if no window...
		if (fOnCreateExtras.get () == NULL) {
			return false;
		}
		else {
			return fOnCreateExtras.get ()->fReadOnly;
		}
	}
	try {
		bool	result	=	fEditor.GetStyle () & ES_READONLY;
		return result;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	return false;
}

void	ActiveLedItControl::OLE_SetReadOnly (BOOL bNewValue)
{
	try {
		if (fEditor.m_hWnd == NULL) {
			if (fOnCreateExtras.get () == NULL) {
				fOnCreateExtras = auto_ptr<OnCreateExtras> (new OnCreateExtras ());
			}
			fOnCreateExtras.get ()->fReadOnly = static_cast<bool> (bNewValue);
		}
		else {
			fEditor.SetReadOnly (bNewValue);
		}
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

BOOL	ActiveLedItControl::OLE_GetEnabled ()
{
	if (fEditor.m_hWnd == NULL) {
		// Avoid barfing if no window...
		if (fOnCreateExtras.get () == NULL) {
			return true;
		}
		else {
			return fOnCreateExtras.get ()->fEnabled;
		}
	}
	try {
		return not (fEditor.GetStyle () & WS_DISABLED);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	return false;
}

void	ActiveLedItControl::OLE_SetEnabled (BOOL bNewValue)
{
	try {
		if (fEditor.m_hWnd == NULL) {
			if (fOnCreateExtras.get () == NULL) {
				fOnCreateExtras = auto_ptr<OnCreateExtras> (new OnCreateExtras ());
			}
			fOnCreateExtras.get ()->fEnabled = static_cast<bool> (bNewValue);
		}
		else {
			if (bNewValue) {
				fEditor.ModifyStyle (WS_DISABLED, 0);
			}
			else {
				fEditor.ModifyStyle (0, WS_DISABLED);
			}
			fEditor.Refresh ();
		}
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

BOOL	ActiveLedItControl::OLE_GetEnableAutoChangesBackgroundColor ()
{
	return fEditor.fEnableAutoChangesBackgroundColor;
}

void	ActiveLedItControl::OLE_SetEnableAutoChangesBackgroundColor (BOOL bNewValue)
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		fEditor.fEnableAutoChangesBackgroundColor = bNewValue;
		fEditor.Refresh ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

int	ActiveLedItControl::OLE_GetWindowMarginTop ()
{
	return fEditor.GetDefaultWindowMargins ().GetTop ();
}

void	ActiveLedItControl::OLE_SetWindowMarginTop (int windowMarginTop)
{
	try {
		Led_TWIPS_Rect	margRect	=	fEditor.GetDefaultWindowMargins ();
		margRect.top = Led_TWIPS (windowMarginTop);
		fEditor.SetDefaultWindowMargins (margRect);
		Invalidate ();
		fEditor.Invalidate ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

int	ActiveLedItControl::OLE_GetWindowMarginLeft ()
{
	return fEditor.GetDefaultWindowMargins ().GetLeft ();
}

void	ActiveLedItControl::OLE_SetWindowMarginLeft (int windowMarginLeft)
{
	try {
		Led_TWIPS_Rect	margRect	=	fEditor.GetDefaultWindowMargins ();
		margRect.left = Led_TWIPS (windowMarginLeft);
		fEditor.SetDefaultWindowMargins (margRect);
		Invalidate ();
		fEditor.Invalidate ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

int	ActiveLedItControl::OLE_GetWindowMarginBottom ()
{
	return fEditor.GetDefaultWindowMargins ().GetBottom ();
}

void	ActiveLedItControl::OLE_SetWindowMarginBottom (int windowMarginBottom)
{
	try {
		Led_TWIPS_Rect	margRect	=	fEditor.GetDefaultWindowMargins ();
		margRect.SetBottom (Led_TWIPS (windowMarginBottom));
		fEditor.SetDefaultWindowMargins (margRect);
		Invalidate ();
		fEditor.Invalidate ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

int	ActiveLedItControl::OLE_GetWindowMarginRight ()
{
	return fEditor.GetDefaultWindowMargins ().GetRight ();
}

void	ActiveLedItControl::OLE_SetWindowMarginRight (int windowMarginRight)
{
	try {
		Led_TWIPS_Rect	margRect	=	fEditor.GetDefaultWindowMargins ();
		margRect.SetRight (Led_TWIPS (windowMarginRight));
		fEditor.SetDefaultWindowMargins (margRect);
		Invalidate ();
		fEditor.Invalidate ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

int	ActiveLedItControl::OLE_GetPrintMarginTop ()
{
	return fEditor.GetPrintMargins ().GetTop ();
}

void	ActiveLedItControl::OLE_SetPrintMarginTop (int printMarginTop)
{
	try {
		Led_TWIPS_Rect	margRect	=	fEditor.GetPrintMargins ();
		margRect.SetTop (Led_TWIPS (printMarginTop));
		fEditor.SetPrintMargins (margRect);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

int	ActiveLedItControl::OLE_GetPrintMarginLeft ()
{
	return fEditor.GetPrintMargins ().GetLeft ();
}

void	ActiveLedItControl::OLE_SetPrintMarginLeft (int printMarginLeft)
{
	try {
		Led_TWIPS_Rect	margRect	=	fEditor.GetPrintMargins ();
		margRect.SetLeft (Led_TWIPS (printMarginLeft));
		fEditor.SetPrintMargins (margRect);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

int	ActiveLedItControl::OLE_GetPrintMarginBottom ()
{
	return fEditor.GetPrintMargins ().GetBottom ();
}

void	ActiveLedItControl::OLE_SetPrintMarginBottom (int printMarginBottom)
{
	try {
		Led_TWIPS_Rect	margRect	=	fEditor.GetPrintMargins ();
		margRect.SetBottom (Led_TWIPS (printMarginBottom));
		fEditor.SetPrintMargins (margRect);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

int	ActiveLedItControl::OLE_GetPrintMarginRight ()
{
	return fEditor.GetPrintMargins ().GetRight ();
}

void	ActiveLedItControl::OLE_SetPrintMarginRight (int printMarginRight)
{
	try {
		Led_TWIPS_Rect	margRect	=	fEditor.GetPrintMargins ();
		margRect.SetRight (Led_TWIPS (printMarginRight));
		fEditor.SetPrintMargins (margRect);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

UINT	ActiveLedItControl::GetHasVerticalScrollBar () 
{
	switch (fEditor.GetScrollBarType (LedItView::v)) {
		case	LedItView::eScrollBarNever:		return	eNoScrollBar;
		case	LedItView::eScrollBarAsNeeded:	return	eShowScrollbarIfNeeded;
		case	LedItView::eScrollBarAlways:	return	eShowScrollBar;
	}
	return eNoScrollBar;
}

void ActiveLedItControl::SetHasVerticalScrollBar (UINT bNewValue) 
{
	if (bNewValue != GetHasVerticalScrollBar ()) {
		IdleManager::NonIdleContext	nonIdleContext;
		SetModifiedFlag ();
		switch (bNewValue) {
		case	eNoScrollBar:				fEditor.SetScrollBarType (LedItView::v, LedItView::eScrollBarNever); break;
			case	eShowScrollBar:			fEditor.SetScrollBarType (LedItView::v, LedItView::eScrollBarAlways); break;
			case	eShowScrollbarIfNeeded:	fEditor.SetScrollBarType (LedItView::v, LedItView::eScrollBarAsNeeded); break;
		}
	}
}

UINT	ActiveLedItControl::GetHasHorizontalScrollBar () 
{
	switch (fEditor.GetScrollBarType (LedItView::h)) {
		case	LedItView::eScrollBarNever:		return	eNoScrollBar;
		case	LedItView::eScrollBarAsNeeded:	return	eShowScrollbarIfNeeded;
		case	LedItView::eScrollBarAlways:	return	eShowScrollBar;
	}
	return eNoScrollBar;
}

void	ActiveLedItControl::SetHasHorizontalScrollBar (UINT bNewValue) 
{
	if (bNewValue != GetHasHorizontalScrollBar ()) {
		IdleManager::NonIdleContext	nonIdleContext;
		SetModifiedFlag ();
		switch (bNewValue) {
			case	eNoScrollBar:			fEditor.SetScrollBarType (LedItView::h, LedItView::eScrollBarNever); break;
			case	eShowScrollBar:			fEditor.SetScrollBarType (LedItView::h, LedItView::eScrollBarAlways); break;
			case	eShowScrollbarIfNeeded:	fEditor.SetScrollBarType (LedItView::h, LedItView::eScrollBarAsNeeded); break;
		}
	}
}

BSTR	ActiveLedItControl::GetBufferText () 
{
	size_t	len	=		fEditor.GetLength ();
	Led_SmallStackBuffer<Led_tChar>	buf (len + 1);
	fEditor.CopyOut (0, len, buf);
	buf[len] = '\0';
	return CString (buf).AllocSysString ();
}

void	ActiveLedItControl::SetBufferText (LPCTSTR text) 
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		fCommandHandler.Commit ();
		#if		_UNICODE
			size_t							len			=	text==NULL? 0: ::_tcslen (text);
			Led_SmallStackBuffer<wchar_t>	buf (len+1);
			buf[0] = 0xfeff;
			memcpy (&buf[1], text, len*sizeof (wchar_t));
			StyledTextIOSrcStream_Memory				source (buf, (len+1)*sizeof (wchar_t));
		#else
			StyledTextIOSrcStream_Memory				source (text, text==NULL? 0: ::_tcslen (text));
		#endif
		WordProcessor::WordProcessorTextIOSinkStream	sink (&fEditor);
		StyledTextIOReader_PlainText					textReader (&source, &sink);
		fEditor.Replace (0, fEditor.GetEnd (), LED_TCHAR_OF (""), 0);		// clear out current text
		textReader.Read ();
		fEditor.SetEmptySelectionStyle ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

BSTR	ActiveLedItControl::GetBufferTextCRLF () 
{
	try {
		size_t	len	=		fEditor.GetLength ();
		Led_SmallStackBuffer<Led_tChar>	buf (len + 1);
		fEditor.CopyOut (0, len, buf);
		buf[len] = '\0';
		Led_SmallStackBuffer<Led_tChar>	buf2 (2*len + 1);
		len = Led_NLToNative (buf, len, buf2, 2*len + 1);
		buf2[len] = '\0';
		return CString (buf2).AllocSysString ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false); /*NOTREACHED*/ return NULL;
}

void	ActiveLedItControl::SetBufferTextCRLF (LPCTSTR text) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	SetBufferText (text);
}

BSTR	ActiveLedItControl::GetBufferTextAsRTF () 
{
	try {
		return CString (GetBufferTextAsRTF_ ().c_str ()).AllocSysString ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false); /*NOTREACHED*/ return NULL;
}

string	ActiveLedItControl::GetBufferTextAsRTF_ () 
{
	WordProcessor::WordProcessorTextIOSrcStream	source (&fEditor);
	StyledTextIOWriterSinkStream_Memory			sink;
	StyledTextIOWriter_RTF						textWriter (&source, &sink);
	textWriter.Write ();
	size_t	len	=	sink.GetLength ();
	Led_SmallStackBuffer<char>	buf (len + 1);
	memcpy (buf, sink.PeekAtData (), len);
	buf[len] = '\0';
	return string (static_cast<char*> (buf));
}

void	ActiveLedItControl::SetBufferTextAsRTF (LPCTSTR text) 
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		TextInteractor::TemporarilySetUpdateMode	tsum (fEditor, m_hWnd == NULL? TextInteractor::eNoUpdate: TextInteractor::eDefaultUpdate);
		fCommandHandler.Commit ();

		string	s	=	Led_SDKString2ANSI (text);
		StyledTextIOSrcStream_Memory					source (s.c_str (), s.length ());
		WordProcessor::WordProcessorTextIOSinkStream	sink (&fEditor);
		StyledTextIOReader_RTF							textReader (&source, &sink);
		fEditor.Replace (0, fEditor.GetEnd (), LED_TCHAR_OF (""), 0);		// clear out current text
		textReader.Read ();
		fEditor.SetEmptySelectionStyle ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

BSTR	ActiveLedItControl::GetBufferTextAsHTML () 
{
	try {
		WordProcessor::WordProcessorTextIOSrcStream	source (&fEditor);
		StyledTextIOWriterSinkStream_Memory			sink;
		StyledTextIOWriter_HTML						textWriter (&source, &sink);
		textWriter.Write ();
		size_t	len	=	sink.GetLength ();
		Led_SmallStackBuffer<char>	buf (len + 1);
		memcpy (buf, sink.PeekAtData (), len);
		buf[len] = '\0';
		return CString (buf).AllocSysString ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false); /*NOTREACHED*/ return NULL;
}

void	ActiveLedItControl::SetBufferTextAsHTML (LPCTSTR text) 
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		fCommandHandler.Commit ();
		string	s	=	Led_SDKString2ANSI (text);
		StyledTextIOSrcStream_Memory					source (s.c_str (), s.length ());
		WordProcessor::WordProcessorTextIOSinkStream	sink (&fEditor);
		StyledTextIOReader_HTML							textReader (&source, &sink);
		fEditor.Replace (0, fEditor.GetEnd (), LED_TCHAR_OF (""), 0);		// clear out current text
		textReader.Read ();
		fEditor.SetEmptySelectionStyle ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

	inline	void	PackBytesIntoVariantSAFEARRAY (const void* bytes, size_t nBytes, VARIANT* into)
		{
			// Note: we must use SafeArrayCreate/SafeArrayAllocData instead of SafeArrayCreateVector
			// due to bugs with the OLE32.DLL marshalling code (apparantly manually deletes both pointers and
			// doesnt call SafeArrayDelete...
			SAFEARRAYBOUND	rgsaBounds[1];
			rgsaBounds[0].lLbound = 0;
			rgsaBounds[0].cElements = nBytes;
			SAFEARRAY*	ar = ::SafeArrayCreate (VT_I1, 1, rgsaBounds);
			Led_ThrowIfNull (ar);
			Led_ThrowIfErrorHRESULT (::SafeArrayAllocData (ar));
			void*	ptr	=	NULL;
			Led_ThrowIfErrorHRESULT (::SafeArrayAccessData (ar, &ptr));
			(void)::memcpy (ptr, bytes, nBytes);	
			::SafeArrayUnaccessData (ar);
			::VariantInit (into);
			into->vt = VT_ARRAY | VT_I1;
			into->parray = ar;
		}


VARIANT	ActiveLedItControl::GetBufferTextAsDIB ()
{
	if (fEditor.m_hWnd == NULL) {
		VARIANT	v;
		::VariantInit (&v);
		return v;
	}

	CWindowDC				windowDC (&fEditor);
	Led_MFC_TabletFromCDC	tabFromDC (&windowDC);
	Led_Tablet	tablet	=	tabFromDC;


	Led_Distance	rhsMargin	=	0;
	{
		const	int	kRTF_SPEC_DefaultInches	=	6;	// HACK - see comments in SinkStreamDestination::SetRightMargin ()
		Led_TWIPS	rhsTWIPS	=	 Led_TWIPS (kRTF_SPEC_DefaultInches	* 1440);
		rhsMargin = Led_CvtScreenPixelsFromTWIPSH (rhsTWIPS);
	}


	size_t		savedScrollPos	=	fEditor.GetMarkerPositionOfStartOfWindow ();
	Led_Rect	savedWindowRect	=	fEditor.GetWindowRect ();
	Led_Rect	offscreenRect (0, 0, 1000, rhsMargin);								// height doesn't matter as long as more than one row - adjused below...

	Led_Bitmap	memoryBitmap;
	try {
		{
Again:
			unsigned	nTimes	=	0;
			fEditor.SetWindowRect (offscreenRect);
			offscreenRect.bottom = fEditor.GetHeightOfRows (0, fEditor.GetRowCount ());		// make sure extends far enuf down - and then reset the WindowRect accordingly...
			fEditor.SetWindowRect (offscreenRect);

			Led_Distance	rhsMargin	=	fEditor.CalculateFarthestRightMarginInWindow ();
			if (rhsMargin != offscreenRect.GetRight () and rhsMargin > 10) {
				offscreenRect.right = rhsMargin;
				if (++nTimes > 5) {
					// don't think this can ever happen - but in case...
					Led_Assert (false);
				}
				else {
					goto Again;
				}
			}
		}

		Led_Tablet_	memDC;
		Led_Verify (memDC.CreateCompatibleDC (tablet));
		Led_Verify (memoryBitmap.CreateCompatibleBitmap (tablet->m_hDC, offscreenRect.GetWidth (), offscreenRect.GetHeight ()));
		Led_Verify (memDC.SelectObject (memoryBitmap));
		(void)memDC.SetWindowOrg (offscreenRect.left, offscreenRect.top);

		LedItView::TemporarilyUseTablet	tmpUseTablet (fEditor, &memDC, LedItView::TemporarilyUseTablet::eDontDoTextMetricsChangedCall);
		fEditor.Draw (offscreenRect, false);

		fEditor.SetWindowRect (savedWindowRect);
		fEditor.SetTopRowInWindowByMarkerPosition (savedScrollPos);
	}
	catch (...) {
		fEditor.SetWindowRect (savedWindowRect);
		fEditor.SetTopRowInWindowByMarkerPosition (savedScrollPos);
		throw;
	}

	#define	qCopyDIBToClipToTest	0
//	#define	qCopyDIBToClipToTest	1
	#if		qCopyDIBToClipToTest
		{
			Led_Verify (::OpenClipboard (fEditor.m_hWnd));
			Led_Verify (::EmptyClipboard ());
			HGLOBAL	dataHandle = ::GlobalAlloc (GMEM_DDESHARE, dibRAMSize);
			Led_Verify (dataHandle);
			::memcpy (Led_StackBasedHandleLocker (dataHandle).GetPointer (), tmpDIB, dibRAMSize);
			Led_Verify (::SetClipboardData (CF_DIB, dataHandle));
			Led_Verify (::CloseClipboard ());
			::GlobalFree (dataHandle);// MAYTBE must do a ::GlobalFree (dataHandle) here?? DOCS on SetClipboardData() are ambiguous... - but robert complained of mem-leak and this could be it! - LGP 2000-06-28
		}
	#endif

	VARIANT	v;
	::VariantInit (&v);

	try {
		PICTDESC				pictDesc;
		::memset (&pictDesc, 0, sizeof (pictDesc));
		pictDesc.cbSizeofstruct = sizeof (pictDesc);
		pictDesc.picType = PICTYPE_BITMAP;
		pictDesc.bmp.hbitmap = memoryBitmap;
		pictDesc.bmp.hpal = NULL;				// NOT SURE THIS IS RIGHT???
		CComQIPtr<IDispatch>	result;
		Led_ThrowIfErrorHRESULT (::OleCreatePictureIndirect (&pictDesc, IID_IDispatch, true, (void**)&result));
		v.vt = VT_DISPATCH;
		v.pdispVal = result.Detach ();
	}
	catch (...) {
	}
	return v;
}

long	ActiveLedItControl::GetBufferLength ()
{
	return fEditor.GetLength ();
}

long	ActiveLedItControl::GetMaxLength ()
{
	return fEditor.GetMaxLength ();
}

void	ActiveLedItControl::SetMaxLength (long maxLength)
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	fEditor.SetMaxLength (maxLength < 0? -1: maxLength);
}

BOOL	ActiveLedItControl::GetSupportContextMenu () 
{
	return fEditor.GetSupportContextMenu ();
}

void	ActiveLedItControl::SetSupportContextMenu (BOOL bNewValue) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	if (bNewValue != GetSupportContextMenu ()) {
		fEditor.SetSupportContextMenu (bNewValue);
	}
}

BOOL	ActiveLedItControl::OLE_GetHideDisabledContextMenuItems () 
{
	return fEditor.GetHideDisabledContextMenuItems ();
}

void	ActiveLedItControl::OLE_SetHideDisabledContextMenuItems (BOOL bNewValue) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	if (bNewValue != OLE_GetHideDisabledContextMenuItems ()) {
		fEditor.SetHideDisabledContextMenuItems (bNewValue);
	}
}

BOOL	ActiveLedItControl::GetSmartCutAndPaste () 
{
	return fEditor.GetSmartCutAndPasteMode ();
}

void	ActiveLedItControl::SetSmartCutAndPaste (BOOL bNewValue) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	if (bNewValue != GetSmartCutAndPaste ()) {
		fEditor.SetSmartCutAndPasteMode (bNewValue);
	}
}

BOOL	ActiveLedItControl::OLE_GetSmartQuoteMode () 
{
	#if		qWideCharacters
		return fEditor.GetSmartQuoteMode ();
	#else
		return false;
	#endif
}

void	ActiveLedItControl::OLE_SetSmartQuoteMode (BOOL bNewValue) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	#if		qWideCharacters
	if (bNewValue != OLE_GetSmartQuoteMode ()) {
		fEditor.SetSmartQuoteMode (bNewValue);
	}
	#endif
}

BOOL	ActiveLedItControl::GetWrapToWindow () 
{
	return fEditor.GetWrapToWindow ();
}

void	ActiveLedItControl::SetWrapToWindow (BOOL bNewValue) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	if (bNewValue != GetWrapToWindow ()) {
		fEditor.SetWrapToWindow (bNewValue);
	}
}

BOOL	ActiveLedItControl::GetShowParagraphGlyphs () 
{
	return fEditor.GetShowParagraphGlyphs ();
}

void	ActiveLedItControl::SetShowParagraphGlyphs (BOOL bNewValue) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	if (bNewValue != GetShowParagraphGlyphs ()) {
		fEditor.SetShowParagraphGlyphs (bNewValue);
	}
}

BOOL	ActiveLedItControl::GetShowTabGlyphs () 
{
	return fEditor.GetShowTabGlyphs ();
}

void	ActiveLedItControl::SetShowTabGlyphs (BOOL bNewValue) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	if (bNewValue != GetShowTabGlyphs ()) {
		fEditor.SetShowTabGlyphs (bNewValue);
	}
}

BOOL	ActiveLedItControl::GetShowSpaceGlyphs () 
{
	return fEditor.GetShowSpaceGlyphs ();
}

void	ActiveLedItControl::SetShowSpaceGlyphs (BOOL bNewValue) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	if (bNewValue != GetShowSpaceGlyphs ()) {
		fEditor.SetShowSpaceGlyphs (bNewValue);
	}
}

BOOL	ActiveLedItControl::OLE_GetUseSelectEOLBOLRowHilightStyle () 
{
	return fEditor.GetUseSelectEOLBOLRowHilightStyle ();
}

void	ActiveLedItControl::OLE_SetUseSelectEOLBOLRowHilightStyle (BOOL bNewValue) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	if (bNewValue != OLE_GetUseSelectEOLBOLRowHilightStyle ()) {
		fEditor.SetUseSelectEOLBOLRowHilightStyle (bNewValue);
		fEditor.Refresh ();
	}
}

BOOL	ActiveLedItControl::OLE_GetShowSecondaryHilight () 
{
	return fEditor.GetUseSecondaryHilight ();
}

void	ActiveLedItControl::OLE_SetShowSecondaryHilight (BOOL bNewValue) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	if (bNewValue != OLE_GetShowSecondaryHilight ()) {
		fEditor.SetUseSecondaryHilight (bNewValue);
		fEditor.Refresh ();
	}
}

BOOL	ActiveLedItControl::OLE_GetShowHidableText () 
{
	ColoredUniformHidableTextMarkerOwner*	uhtmo	=	dynamic_cast<ColoredUniformHidableTextMarkerOwner*> (static_cast<HidableTextMarkerOwner*> (fEditor.GetHidableTextDatabase ()));
	Led_AssertNotNil (uhtmo);
	return !uhtmo->IsHidden ();
}

void	ActiveLedItControl::OLE_SetShowHidableText (BOOL bNewValue) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	IdleManager::NonIdleContext	nonIdleContext;
	if (bNewValue) {
		fEditor.GetHidableTextDatabase ()->ShowAll ();
	}
	else {
		fEditor.GetHidableTextDatabase ()->HideAll ();
	}
}

OLE_COLOR	ActiveLedItControl::OLE_GetHidableTextColor () 
{
	try {
		ColoredUniformHidableTextMarkerOwner*	uhtmo	=	dynamic_cast<ColoredUniformHidableTextMarkerOwner*> (static_cast<HidableTextMarkerOwner*> (fEditor.GetHidableTextDatabase ()));
		Led_AssertNotNil (uhtmo);
		return uhtmo->GetColor ().GetOSRep ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false);	return 0;	// NOTREACHED
}

void	ActiveLedItControl::OLE_SetHidableTextColor (OLE_COLOR color) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	try {
		ColoredUniformHidableTextMarkerOwner*	uhtmo	=	dynamic_cast<ColoredUniformHidableTextMarkerOwner*> (static_cast<HidableTextMarkerOwner*> (fEditor.GetHidableTextDatabase ()));
		Led_AssertNotNil (uhtmo);
		uhtmo->SetColor (Led_Color (TranslateColor (color)));
		fEditor.Refresh ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

BOOL	ActiveLedItControl::OLE_GetHidableTextColored () 
{
	try {
		ColoredUniformHidableTextMarkerOwner*	uhtmo	=	dynamic_cast<ColoredUniformHidableTextMarkerOwner*> (static_cast<HidableTextMarkerOwner*> (fEditor.GetHidableTextDatabase ()));
		Led_AssertNotNil (uhtmo);
		return uhtmo->GetColored ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false);	return 0;	// NOTREACHED
}

void	ActiveLedItControl::OLE_SetHidableTextColored (BOOL bNewValue) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	try {
		ColoredUniformHidableTextMarkerOwner*	uhtmo	=	dynamic_cast<ColoredUniformHidableTextMarkerOwner*> (static_cast<HidableTextMarkerOwner*> (fEditor.GetHidableTextDatabase ()));
		Led_AssertNotNil (uhtmo);
		uhtmo->SetColored (bNewValue);
		fEditor.Refresh ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

VARIANT	ActiveLedItControl::OLE_GetSpellChecker () 
{
	try {
		VARIANT	result;
		::VariantInit (&result);
		if (fSpellChecker != NULL) {
			result.vt = VT_DISPATCH; 
			result.pdispVal = fSpellChecker;
			result.pdispVal->AddRef ();
		}
		return result;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

void	ActiveLedItControl::OLE_SetSpellChecker (VARIANT& newValue) 
{
	// Note: the MSVC Class wizard for OLE / MFC in MSVC.Net 2003 creates the property setter as taking a 'VARIANT' argument. However
	// empirically - thats NOT what gets passed! This reference crap (or pointer) is necessary to get the right value assigned to
	// us - LGP 2003-06-11
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		if (fSpellChecker != NULL) {
			fSpellChecker->Release ();
			fSpellChecker = NULL;
			ChangedSpellCheckerCOMObject ();
		}

		Led_Assert (fSpellChecker == NULL);
		VARIANT	tmpV;
		::VariantInit (&tmpV);
		if (::VariantChangeType (&tmpV, &newValue, 0, VT_DISPATCH) == S_OK) {
			fSpellChecker = tmpV.pdispVal;
			if (fSpellChecker != NULL) {
				fSpellChecker->AddRef ();
			}
			::VariantClear (&tmpV);
			ChangedSpellCheckerCOMObject ();
		}
		else if (::VariantChangeType (&tmpV, &newValue, 0, VT_BSTR) == S_OK) {
			// must be a CLSID or a PROGID. Either way - try to create the object
			HRESULT	hr	=	S_OK;
			if (::SysStringLen (tmpV.bstrVal) != 0) {
				CLSID	theCLSID;
				if (SUCCEEDED (hr = ::CLSIDFromProgID (tmpV.bstrVal, &theCLSID)) or
					SUCCEEDED (hr = ::CLSIDFromString (tmpV.bstrVal, &theCLSID))
					) {
					hr	=	::CoCreateInstance (theCLSID, NULL, CLSCTX_ALL, IID_IDispatch, reinterpret_cast<LPVOID*> (&fSpellChecker));
					Led_Assert (SUCCEEDED (hr) == (fSpellChecker != NULL));
				}
			}
			::VariantClear (&tmpV);
			ChangedSpellCheckerCOMObject ();
			Led_ThrowIfErrorHRESULT (hr);
		}
		else {
			Led_ThrowIfErrorHRESULT (DISP_E_TYPEMISMATCH);
		}
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

void	ActiveLedItControl::ChangedSpellCheckerCOMObject ()
{
	fEditor.SetSpellCheckEngine (NULL);
	fLedSpellCheckWrapper = COMBased_SpellCheckEngine (fSpellChecker);
	if (fSpellChecker != NULL) {
		fEditor.SetSpellCheckEngine (&fLedSpellCheckWrapper);
	}
}

VARIANT	ActiveLedItControl::OLE_GetContextMenu () 
{
	try {
		VARIANT	result;
		::VariantInit (&result);
		if (fConextMenu.p != NULL) {
			result.vt = VT_DISPATCH; 
			result.pdispVal = fConextMenu;
			result.pdispVal->AddRef ();
		}
		return result;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

void	ActiveLedItControl::OLE_SetContextMenu (VARIANT& newValue) 
{
	// Note: the MSVC Class wizard for OLE / MFC in MSVC.Net 2003 creates the property setter as taking a 'VARIANT' argument. However
	// empirically - thats NOT what gets passed! This reference crap (or pointer) is necessary to get the right value assigned to
	// us - LGP 2003-06-11 (originally - but now copied for SetContextMenu - LGP 2004-01-14)
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	try {
		fConextMenu.Release ();
		VARIANT	tmpV;
		::VariantInit (&tmpV);
		if (::VariantChangeType (&tmpV, &newValue, 0, VT_DISPATCH) == S_OK) {
			fConextMenu = tmpV.pdispVal;
			::VariantClear (&tmpV);
		}
		else {
			Led_ThrowIfErrorHRESULT (DISP_E_TYPEMISMATCH);
		}
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

VARIANT	ActiveLedItControl::OLE_GetToolbarList () 
{
	try {
		VARIANT	result;
		::VariantInit (&result);
		if (fToolbarList.p != NULL) {
			result.vt = VT_DISPATCH; 
			result.pdispVal = fToolbarList;
			result.pdispVal->AddRef ();
		}
		return result;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

void	ActiveLedItControl::OLE_SetToolbarList (VARIANT& newValue) 
{
	// Note: the MSVC Class wizard for OLE / MFC in MSVC.Net 2003 creates the property setter as taking a 'VARIANT' argument. However
	// empirically - thats NOT what gets passed! This reference crap (or pointer) is necessary to get the right value assigned to
	// us - LGP 2003-06-11 (originally - but now copied for SetContextMenu - LGP 2004-01-14)
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		{
			CComQIPtr<IALToolbarList>	tbl	=	fToolbarList;
			if (tbl.p != NULL) {
				tbl->NotifyOfOwningActiveLedIt (NULL, NULL);
			}
		}
		fToolbarList.Release ();
		VARIANT	tmpV;
		::VariantInit (&tmpV);
		if (::VariantChangeType (&tmpV, &newValue, 0, VT_DISPATCH) == S_OK) {
			fToolbarList = tmpV.pdispVal;
			::VariantClear (&tmpV);
		}
		else {
			Led_ThrowIfErrorHRESULT (DISP_E_TYPEMISMATCH);
		}
		{
			CComQIPtr<IALToolbarList>	tbl	=	fToolbarList;
			if (tbl.p != NULL) {
				tbl->NotifyOfOwningActiveLedIt (CComQIPtr<IDispatch> (GetControllingUnknown ()), m_hWnd);
			}
		}
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

VARIANT	ActiveLedItControl::OLE_GetBuiltinCommands () 
{
	try {
		if (fBuiltinCommands.p == NULL) {
			fBuiltinCommands = GenerateBuiltinCommandsObject ();
		}
		VARIANT	result;
		::VariantInit (&result);
		{
			result.vt = VT_DISPATCH; 
			result.pdispVal = fBuiltinCommands;
			result.pdispVal->AddRef ();
		}
		return result;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

namespace	{
	CComPtr<IDispatch>	mkMenu_Select () 
		{
			try {
				CComObject<ActiveLedIt_MenuItemPopup>* o = NULL;
				Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_MenuItemPopup>::CreateInstance (&o));
				CComQIPtr<IDispatch>	result	=	o->GetUnknown ();

				o->put_Name (CComBSTR (L"Select"));
				o->put_InternalName (CComBSTR (kName_SelectMenu));

				o->AppendBuiltinCmd (kCmd_SelectWord);
				o->AppendBuiltinCmd (kCmd_SelectTextRow);
				o->AppendBuiltinCmd (kCmd_SelectParagraph);

				o->AppendBuiltinCmd (kCmd_Separator);

				o->AppendBuiltinCmd (kCmd_SelectTableIntraCellAll);
				o->AppendBuiltinCmd (kCmd_SelectTableCell);
				o->AppendBuiltinCmd (kCmd_SelectTableRow);
				o->AppendBuiltinCmd (kCmd_SelectTableColumn);
				o->AppendBuiltinCmd (kCmd_SelectTable);

				o->AppendBuiltinCmd (kCmd_Separator);

				o->AppendBuiltinCmd (kCmd_SelectAll);

				o->AppendBuiltinCmd (kCmd_Separator);

				o->AppendBuiltinCmd (kCmd_Find);
				o->AppendBuiltinCmd (kCmd_FindAgain);
				o->AppendBuiltinCmd (kCmd_EnterFindString);
				o->AppendBuiltinCmd (kCmd_Replace);
				o->AppendBuiltinCmd (kCmd_ReplaceAgain);

				o->AppendBuiltinCmd (kCmd_Separator);

				o->AppendBuiltinCmd (kCmd_CheckSpelling);
				return result;
			}
			CATCH_AND_HANDLE_EXCEPTIONS();
		}

	CComPtr<IDispatch>	mkMenu_Insert () 
		{
			try {
				CComObject<ActiveLedIt_MenuItemPopup>* o = NULL;
				Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_MenuItemPopup>::CreateInstance (&o));
				CComQIPtr<IDispatch>	result	=	o->GetUnknown ();

				o->put_Name (CComBSTR (L"Insert"));
				o->put_InternalName (CComBSTR (kName_InsertMenu));

				o->AppendBuiltinCmd (kCmd_InsertTable);
				o->AppendBuiltinCmd (kCmd_InsertTableRowAbove);
				o->AppendBuiltinCmd (kCmd_InsertTableRowBelow);
				o->AppendBuiltinCmd (kCmd_InsertTableColBefore);
				o->AppendBuiltinCmd (kCmd_InsertTableColAfter);

				o->AppendBuiltinCmd (kCmd_Separator);

				o->AppendBuiltinCmd (kCmd_InsertURL);
				o->AppendBuiltinCmd (kCmd_InsertSymbol);
				return result;
			}
			CATCH_AND_HANDLE_EXCEPTIONS();
		}

	CComPtr<IDispatch>	mkMenu_Font () 
		{
			try {
				CComObject<ActiveLedIt_MenuItemPopup>* o = NULL;
				Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_MenuItemPopup>::CreateInstance (&o));
				CComQIPtr<IDispatch>	result	=	o->GetUnknown ();


				o->put_Name (CComBSTR (L"Font"));
				o->put_InternalName (CComBSTR (kName_FontNameMenu));

				const vector<Led_SDK_String>&	fontNames	=	GetUsableFontNames ();
				Led_Assert (fontNames.size () <=  kLastFontNameCmd-kBaseFontNameCmd+1);
				for (size_t i = 0; i < fontNames.size (); i++) {
					int	cmdNum	=	kBaseFontNameCmd + i;
					if (cmdNum > kLastFontNameCmd) {
						break;		// asserted out before above - now just ignore extra font names...
					}
					ActiveLedIt_BuiltinCommand*	c	=	ActiveLedIt_BuiltinCommand::mk (BuiltinCmdSpec (cmdNum, mkFontNameCMDName (fontNames[i]).c_str ()));
					c->SetName (Led_SDKString2Wide (fontNames[i]));
					o->Insert (c);
				}

				return result;
			}
			CATCH_AND_HANDLE_EXCEPTIONS();
		}

	CComPtr<IDispatch>	mkMenu_FontStyle () 
		{
			try {
				CComObject<ActiveLedIt_MenuItemPopup>* o = NULL;
				Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_MenuItemPopup>::CreateInstance (&o));
				CComQIPtr<IDispatch>	result	=	o->GetUnknown ();

				o->put_Name (CComBSTR (L"Font Style"));
				o->put_InternalName (CComBSTR (kName_FontStyleMenu));

				o->AppendBuiltinCmd (kCmd_FontStylePlain);

				o->AppendBuiltinCmd (kCmd_Separator);

				o->AppendBuiltinCmd (kCmd_FontStyleBold);
				o->AppendBuiltinCmd (kCmd_FontStyleItalic);
				o->AppendBuiltinCmd (kCmd_FontStyleUnderline);
				o->AppendBuiltinCmd (kCmd_FontStyleStrikeout);
				o->AppendBuiltinCmd (kCmd_SubScript);
				o->AppendBuiltinCmd (kCmd_SuperScript);

				return result;
			}
			CATCH_AND_HANDLE_EXCEPTIONS();
		}

	CComPtr<IDispatch>	mkMenu_FontSize (bool forComboBox) 
		{
			try {
				CComObject<ActiveLedIt_MenuItemPopup>* o = NULL;
				Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_MenuItemPopup>::CreateInstance (&o));
				CComQIPtr<IDispatch>	result	=	o->GetUnknown ();

				o->put_Name (CComBSTR (L"Font Size"));
				o->put_InternalName (CComBSTR (kName_FontSizeMenu));

				o->AppendBuiltinCmd (kCmd_FontSize9);
				o->AppendBuiltinCmd (kCmd_FontSize10);
				o->AppendBuiltinCmd (kCmd_FontSize12);
				o->AppendBuiltinCmd (kCmd_FontSize14);
				o->AppendBuiltinCmd (kCmd_FontSize18);
				o->AppendBuiltinCmd (kCmd_FontSize24);
				o->AppendBuiltinCmd (kCmd_FontSize36);
				o->AppendBuiltinCmd (kCmd_FontSize48);
				o->AppendBuiltinCmd (kCmd_FontSize72);

				if (not forComboBox) {
					o->AppendBuiltinCmd (kCmd_Separator);

					o->AppendBuiltinCmd (kCmd_FontSizeSmaller);
					o->AppendBuiltinCmd (kCmd_FontSizeLarger);

					o->AppendBuiltinCmd (kCmd_Separator);
				}

				o->AppendBuiltinCmd (kCmd_FontSizeOther);

				return result;
			}
			CATCH_AND_HANDLE_EXCEPTIONS();
		}

	CComPtr<IDispatch>	mkMenu_FontColor () 
		{
			try {
				CComObject<ActiveLedIt_MenuItemPopup>* o = NULL;
				Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_MenuItemPopup>::CreateInstance (&o));
				CComQIPtr<IDispatch>	result	=	o->GetUnknown ();

				o->put_Name (CComBSTR (L"Font Color"));
				o->put_InternalName (CComBSTR (kName_FontColorMenu));

				o->AppendBuiltinCmd (kCmd_BlackColor);
				o->AppendBuiltinCmd (kCmd_MaroonColor);
				o->AppendBuiltinCmd (kCmd_GreenColor);
				o->AppendBuiltinCmd (kCmd_OliveColor);
				o->AppendBuiltinCmd (kCmd_NavyColor);
				o->AppendBuiltinCmd (kCmd_PurpleColor);
				o->AppendBuiltinCmd (kCmd_TealColor);
				o->AppendBuiltinCmd (kCmd_GrayColor);
				o->AppendBuiltinCmd (kCmd_SilverColor);
				o->AppendBuiltinCmd (kCmd_RedColor);
				o->AppendBuiltinCmd (kCmd_LimeColor);
				o->AppendBuiltinCmd (kCmd_YellowColor);
				o->AppendBuiltinCmd (kCmd_BlueColor);
				o->AppendBuiltinCmd (kCmd_FuchsiaColor);
				o->AppendBuiltinCmd (kCmd_AquaColor);
				o->AppendBuiltinCmd (kCmd_WhiteColor);

				o->AppendBuiltinCmd (kCmd_Separator);

				o->AppendBuiltinCmd (kCmd_OtherColor);

				return result;
			}
			CATCH_AND_HANDLE_EXCEPTIONS();
		}

	CComPtr<IDispatch>	mkMenu_ParagraphJustification () 
		{
			try {
				CComObject<ActiveLedIt_MenuItemPopup>* o = NULL;
				Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_MenuItemPopup>::CreateInstance (&o));
				CComQIPtr<IDispatch>	result	=	o->GetUnknown ();

				o->put_Name (CComBSTR (L"Paragraph Justification"));
				o->put_InternalName (CComBSTR (kName_ParagraphJustification));

				o->AppendBuiltinCmd (kCmd_JustifyLeft);
				o->AppendBuiltinCmd (kCmd_JustifyCenter);
				o->AppendBuiltinCmd (kCmd_JustifyRight);
				o->AppendBuiltinCmd (kCmd_JustifyFull);

				return result;
			}
			CATCH_AND_HANDLE_EXCEPTIONS();
		}

	CComPtr<IDispatch>	mkMenu_ListStyle () 
		{
			try {
				CComObject<ActiveLedIt_MenuItemPopup>* o = NULL;
				Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_MenuItemPopup>::CreateInstance (&o));
				CComQIPtr<IDispatch>	result	=	o->GetUnknown ();

				o->put_Name (CComBSTR (L"List Style"));
				o->put_InternalName (CComBSTR (kName_ListStyle));

				o->AppendBuiltinCmd (kCmd_ListStyle_None);
				o->AppendBuiltinCmd (kCmd_ListStyle_Bullet);

				return result;
			}
			CATCH_AND_HANDLE_EXCEPTIONS();
		}
}

VARIANT	ActiveLedItControl::OLE_GetPredefinedMenus () 
{
	try {
		if (fPredefinedMenus.p == NULL) {
			CComObject<ActiveLedIt_StaticCommandList>* o = NULL;
			Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_StaticCommandList>::CreateInstance (&o));
			CComPtr<IDispatch>	saved	=	o;	// so auto-deleted on exception

			o->Append (mkMenu_Select ());
			o->Append (mkMenu_Insert ());
			o->Append (mkMenu_Font ());
			o->Append (mkMenu_FontStyle ());
			o->Append (mkMenu_FontSize (false));
			o->Append (mkMenu_FontColor ());
			o->Append (mkMenu_ParagraphJustification ());
			o->Append (mkMenu_ListStyle ());

			fPredefinedMenus = o;
		}
		VARIANT	result;
		::VariantInit (&result);
		{
			result.vt = VT_DISPATCH; 
			result.pdispVal = fPredefinedMenus;
			result.pdispVal->AddRef ();
		}
		return result;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

VARIANT	ActiveLedItControl::OLE_GetAcceleratorTable () 
{
	try {
		VARIANT	result;
		::VariantInit (&result);
		if (fAcceleratorTable.p != NULL) {
			result.vt = VT_DISPATCH; 
			result.pdispVal = fAcceleratorTable;
			result.pdispVal->AddRef ();
		}
		return result;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

void	ActiveLedItControl::OLE_SetAcceleratorTable (VARIANT& newValue) 
{
	// Note: the MSVC Class wizard for OLE / MFC in MSVC.Net 2003 creates the property setter as taking a 'VARIANT' argument. However
	// empirically - thats NOT what gets passed! This reference crap (or pointer) is necessary to get the right value assigned to
	// us - LGP 2003-06-11 (originally - but now copied for SetContextMenu - LGP 2004-01-14)
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	try {
		fAcceleratorTable.Release ();
		VARIANT	tmpV;
		::VariantInit (&tmpV);
		if (::VariantChangeType (&tmpV, &newValue, 0, VT_DISPATCH) == S_OK) {
			fAcceleratorTable = tmpV.pdispVal;
			::VariantClear (&tmpV);
		}
		else {
			Led_ThrowIfErrorHRESULT (DISP_E_TYPEMISMATCH);
		}
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

HACCEL	ActiveLedItControl::GetCurrentWin32AccelTable ()
{
	const	float	kTimeBetweenRecomputes	=	10.0f;
	if (fWin32AccelTable == NULL or Led_GetTickCount () - fLastAccelTableUpdateAt > kTimeBetweenRecomputes) {
		{
			CComQIPtr<IALAcceleratorTable>	accelTable	=	fAcceleratorTable;
			if (accelTable.p != NULL) {
				HACCEL	maybeNewAccelTable	=	NULL;
				accelTable->GenerateWin32AcceleratorTable (&maybeNewAccelTable);
				fLastAccelTableUpdateAt = Led_GetTickCount ();

				// Now - to avoid causing problems with callers of GetCurrentWin32AccelTable () that might want to hang onto a
				// pointer (and wouldn't want it to get stale ;-)) - like OnGetControlInfo - try to avoid changing the handle
				// if not actually needed. See if the old and new handles are identical - and if so - delete the NEW one rather
				// than the old...
				bool	keepOld	=	false;
				if (fWin32AccelTable != NULL) {
					size_t	accelTableSize	=	::CopyAcceleratorTable (fWin32AccelTable, NULL, 0);
					if (accelTableSize == ::CopyAcceleratorTable (maybeNewAccelTable, NULL, 0)) {
						Led_SmallStackBuffer<ACCEL>	oldOne (accelTableSize);
						Led_Verify (::CopyAcceleratorTable (fWin32AccelTable, oldOne, accelTableSize) == accelTableSize);
						Led_SmallStackBuffer<ACCEL>	newOne (accelTableSize);
						Led_Verify (::CopyAcceleratorTable (maybeNewAccelTable, newOne, accelTableSize) == accelTableSize);
						if (::memcmp (oldOne, newOne, accelTableSize * sizeof (ACCEL)) == 0) {
							keepOld = true;
						}
					}
				}
				if (keepOld) {
					::DestroyAcceleratorTable (maybeNewAccelTable);
				}
				else {
					if (fWin32AccelTable != NULL) {
						::DestroyAcceleratorTable (fWin32AccelTable);
						fWin32AccelTable = NULL;
					}
					fWin32AccelTable = maybeNewAccelTable;
				}
			}
		}
	}
	return fWin32AccelTable;
}

IDispatch*	ActiveLedItControl::OLE_GetDefaultContextMenu () 
{
	try {
		CComObject<ActiveLedIt_UserCommandList>* o = NULL;
		Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_UserCommandList>::CreateInstance (&o));
		CComQIPtr<IDispatch>	saved	=	o->GetUnknown ();	// so deleted on exceptions during the subsequent add code

		o->AppendBuiltinCmd (kCmd_Undo);
		o->AppendBuiltinCmd (kCmd_Redo);
		o->AppendBuiltinCmd (kCmd_Separator);
		o->AppendBuiltinCmd (kCmd_Cut);
		o->AppendBuiltinCmd (kCmd_Copy);
		o->AppendBuiltinCmd (kCmd_Paste);
		o->AppendBuiltinCmd (kCmd_Clear);
		o->AppendBuiltinCmd (kCmd_Separator);

		o->Insert (mkMenu_Select ());
		o->Insert (mkMenu_Insert ());

		o->AppendBuiltinCmd (kCmd_Separator);
		o->AppendBuiltinCmd (kCmd_RemoveTableRows);
		o->AppendBuiltinCmd (kCmd_RemoveTableColumns);
		o->AppendBuiltinCmd (kCmd_Separator);

		o->Insert (mkMenu_Font ());
		o->Insert (mkMenu_FontStyle ());
		o->Insert (mkMenu_FontSize (false));
		o->Insert (mkMenu_FontColor ());

		o->AppendBuiltinCmd (kCmd_ChooseFontDialog);
		o->AppendBuiltinCmd (kCmd_Separator);

		o->Insert (mkMenu_ParagraphJustification ());

		o->AppendBuiltinCmd (kCmd_ParagraphIndents);
		o->AppendBuiltinCmd (kCmd_ParagraphSpacing);
		o->AppendBuiltinCmd (kCmd_Separator);

		o->Insert (mkMenu_ListStyle ());

		o->AppendBuiltinCmd (kCmd_IncreaseIndent);
		o->AppendBuiltinCmd (kCmd_DescreaseIndent);
		o->AppendBuiltinCmd (kCmd_Separator);
		o->AppendBuiltinCmd (kCmd_PropertiesForSelection);
		o->AppendBuiltinCmd (kCmd_OpenEmbedding);
		o->AppendBuiltinCmd (kCmd_Separator);

		o->AppendBuiltinCmd (kCmd_Print);
		o->AppendBuiltinCmd (kCmd_PrintSetup);

		o->AppendBuiltinCmd (kCmd_Separator);

		o->AppendBuiltinCmd (kCmd_BrowseOnlineHelp);
		o->AppendBuiltinCmd (kCmd_CheckForUpdatesOnWeb);

		o->AppendBuiltinCmd (kCmd_Separator);

		o->AppendBuiltinCmd (kCmd_About);

		o->AddRef ();	// cuz returning a pointer
		return saved;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

IDispatch*	ActiveLedItControl::OLE_GetDefaultAcceleratorTable () 
{
	try {
		CComObject<ActiveLedIt_AcceleratorTable>* o = NULL;
		Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_AcceleratorTable>::CreateInstance (&o));
		CComQIPtr<IDispatch>	saved	=	o->GetUnknown ();	// so deleted on exceptions during the subsequent add code

		o->AppendACCEL ("Undo", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'Z');
		o->AppendACCEL ("Undo", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eALT), VK_BACK);
		o->AppendACCEL ("Redo", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'Y');
		o->AppendACCEL ("Cut", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'X');
		o->AppendACCEL ("Cut", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eSHIFT), VK_DELETE);
		o->AppendACCEL ("Copy", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'C');
		o->AppendACCEL ("Copy", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), VK_INSERT);
		o->AppendACCEL ("Paste", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'V');
		o->AppendACCEL ("Paste", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eSHIFT), VK_INSERT);
		o->AppendACCEL ("SelectAll", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'A');
		o->AppendACCEL ("ChooseFontDialog", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'D');
		o->AppendACCEL ("Find", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'F');
		o->AppendACCEL ("FindAgain", static_cast<AcceleratorModifierFlag> (eVIRTKEY), VK_F3);
		o->AppendACCEL ("EnterFindString", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'E');
		o->AppendACCEL ("Replace", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'H');
		o->AppendACCEL ("ReplaceAgain", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'R');
		o->AppendACCEL ("CheckSpelling", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'L');
		o->AppendACCEL ("Plain", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'T');
		o->AppendACCEL ("Bold", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'B');
		o->AppendACCEL ("Print", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'P');
		o->AppendACCEL ("Italic", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'I');
		o->AppendACCEL ("Underline", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), 'U');
		o->AppendACCEL ("Smaller", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), VK_SUBTRACT);
		o->AppendACCEL ("Larger", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eCONTROL), VK_ADD);
		o->AppendACCEL ("Properties", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eALT), VK_RETURN);
		o->AppendACCEL ("IncreaseListIndent", static_cast<AcceleratorModifierFlag> (eVIRTKEY), VK_TAB);
		o->AppendACCEL ("DecreaseListIndent", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eSHIFT), VK_TAB);
		o->AppendACCEL ("BrowseOnlineHelp", static_cast<AcceleratorModifierFlag> (eVIRTKEY), VK_F1);
		#if 0
			o->AppendACCEL ("Help", static_cast<AcceleratorModifierFlag> (eVIRTKEY), VK_F1);
			o->AppendACCEL ("ContextHelp", static_cast<AcceleratorModifierFlag> (eVIRTKEY | eSHIFT), VK_F1);
		#endif
		o->AddRef ();	// cuz returning a pointer
		return saved;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

IDispatch*	ActiveLedItControl::OLE_MakeNewPopupMenuItem () 
{
	try {
		CComObject<ActiveLedIt_MenuItemPopup>* o = NULL;
		Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_MenuItemPopup>::CreateInstance (&o));
		o->AddRef ();	// cuz returning a pointer
		CComQIPtr<IDispatch>	result	=	o->GetUnknown ();
		return result;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

IDispatch*	ActiveLedItControl::OLE_MakeNewUserMenuItem () 
{
	try {
		CComObject<ActiveLedIt_UserCommand>* o = NULL;
		Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_UserCommand>::CreateInstance (&o));
		o->AddRef ();	// cuz returning a pointer
		CComQIPtr<IDispatch>	result	=	o->GetUnknown ();
		return result;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

IDispatch*	ActiveLedItControl::OLE_MakeNewAcceleratorElement () 
{
	try {
		CComObject<ActiveLedIt_AcceleratorElement>* o = NULL;
		Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_AcceleratorElement>::CreateInstance (&o));
		o->AddRef ();	// cuz returning a pointer
		CComQIPtr<IDispatch>	result	=	o->GetUnknown ();
		return result;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

void	ActiveLedItControl::OLE_InvokeCommand (const VARIANT& command) 
{
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		UINT32	cmdNum	=	CmdObjOrName2Num (command);
		if (cmdNum != 0) {
			(void)::SendMessage (fEditor.GetHWND (), WM_COMMAND, cmdNum, 0);
			return;
		}
		throw HRESULTErrorException (TYPE_E_ELEMENTNOTFOUND);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

BOOL	ActiveLedItControl::OLE_CommandEnabled (const VARIANT& command) 
{
	try {
		UINT32	cmdNum	=	CmdObjOrName2Num (command);

		struct	MyCmdUI	: CCmdUI {
			MyCmdUI ():
				CCmdUI (),
				fEnabled (false)
				{
				}
			override	void	Enable (BOOL bOn)
				{
					fEnabled = bOn;
					m_bEnableChanged = TRUE;
				}
			override	void	SetCheck (int /*nCheck*/)
				{
				}
			override	void	SetText (LPCTSTR /*lpszText*/)
				{
				}

			bool	fEnabled;
		};
		MyCmdUI	state;
		state.m_nID = cmdNum;
		state.DoUpdate (&fEditor, true);
		return state.fEnabled;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

BOOL	ActiveLedItControl::OLE_CommandChecked (const VARIANT& command) 
{
	try {
		UINT32	cmdNum	=	CmdObjOrName2Num (command);

		struct	MyCmdUI	: CCmdUI {
			MyCmdUI ():
				CCmdUI (),
				fChecked (false)
				{
				}
			override	void	Enable (BOOL bOn)
				{
					m_bEnableChanged = TRUE;
				}
			override	void	SetCheck (int nCheck)
				{
					fChecked = nCheck != 0;
				}
			override	void	SetText (LPCTSTR /*lpszText*/)
				{
				}

			bool	fChecked;
		};
		MyCmdUI	state;
		state.m_nID = cmdNum;
		state.DoUpdate (&fEditor, true);
		return state.fChecked;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

IDispatch*	ActiveLedItControl::OLE_MakeNewToolbarList () 
{
	try {
		CComObject<ActiveLedIt_ToolbarList>* o = NULL;
		Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_ToolbarList>::CreateInstance (&o));
		o->AddRef ();	// cuz returning a pointer
		CComQIPtr<IDispatch>	result	=	o->GetUnknown ();
		return result;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

CComPtr<IDispatch>	ActiveLedItControl::MakeNewToolbar ()
{
	CComPtr<IDispatch>	tmp;
	tmp.Attach (OLE_MakeNewToolbar ());
	return tmp;
}

IDispatch*	ActiveLedItControl::OLE_MakeNewToolbar () 
{
	try {
		CComObject<ActiveLedIt_Toolbar>* o = NULL;
		Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_Toolbar>::CreateInstance (&o));
		o->AddRef ();	// cuz returning a pointer
		CComQIPtr<IDispatch>	result	=	o->GetUnknown ();
		return result;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

IDispatch*	ActiveLedItControl::OLE_MakeIconButtonToolbarItem () 
{
	try {
		CComObject<ActiveLedIt_IconButtonToolbarElement>* o = NULL;
		Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_IconButtonToolbarElement>::CreateInstance (&o));
		o->AddRef ();	// cuz returning a pointer
		CComQIPtr<IDispatch>	result	=	o->GetUnknown ();
		return result;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

CComPtr<IDispatch>	ActiveLedItControl::MakeSeparatorToolbarItem ()
{
	CComPtr<IDispatch>	tmp;
	tmp.Attach (OLE_MakeSeparatorToolbarItem ());
	return tmp;
}

IDispatch*	ActiveLedItControl::OLE_MakeSeparatorToolbarItem () 
{
	try {
		CComObject<ActiveLedIt_SeparatorToolbarElement>* o = NULL;
		Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_SeparatorToolbarElement>::CreateInstance (&o));
		o->AddRef ();	// cuz returning a pointer
		CComQIPtr<IDispatch>	result	=	o->GetUnknown ();
		return result;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

CComPtr<IDispatch>	ActiveLedItControl::MakeBuiltinToolbar (LPCOLESTR builtinToolbarName)
{
	if (wstring (builtinToolbarName) == L"Standard") {
		CComPtr<IDispatch>		newTB	=	MakeNewToolbar ();
		CComQIPtr<IALToolbar>	tb		=	newTB;
		Led_ThrowIfErrorHRESULT (tb->MergeAdd (MakeBuiltinToolbar (L"EditBar")));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
		Led_ThrowIfErrorHRESULT (tb->MergeAdd (MakeBuiltinToolbar (L"FormatBar")));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Print"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("ActiveLedIt"))));
		return newTB;
	}

	if (wstring (builtinToolbarName) == L"StandardToolsOnly") {	// not including format bar - assumes will be added separately
		CComPtr<IDispatch>		newTB	=	MakeNewToolbar ();
		CComQIPtr<IALToolbar>	tb		=	newTB;
		Led_ThrowIfErrorHRESULT (tb->MergeAdd (MakeBuiltinToolbar (L"EditBar")));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
		Led_ThrowIfErrorHRESULT (tb->MergeAdd (MakeBuiltinToolbar (L"SelectBar")));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
		Led_ThrowIfErrorHRESULT (tb->MergeAdd (MakeBuiltinToolbar (L"InsertBar")));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Print"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("ActiveLedIt"))));
		return newTB;
	}

	if (wstring (builtinToolbarName) == L"FormatBar") {
		CComPtr<IDispatch>		newTB	=	MakeNewToolbar ();
		CComQIPtr<IALToolbar>	tb		=	newTB;
		Led_ThrowIfErrorHRESULT (tb->MergeAdd (MakeBuiltinToolbar (L"CharacterFormatBar")));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
		Led_ThrowIfErrorHRESULT (tb->MergeAdd (MakeBuiltinToolbar (L"ParagraphFormatBar")));
		return newTB;
	}

	if (wstring (builtinToolbarName) == L"EditBar") {
		CComPtr<IDispatch>		newTB	=	MakeNewToolbar ();
		CComQIPtr<IALToolbar>	tb		=	newTB;
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Undo"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Redo"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Cut"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Copy"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Paste"))));
		return newTB;
	}

	if (wstring (builtinToolbarName) == L"SelectBar") {
		CComPtr<IDispatch>		newTB	=	MakeNewToolbar ();
		CComQIPtr<IALToolbar>	tb		=	newTB;
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Find"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Replace"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("CheckSpelling"))));
		return newTB;
	}

	if (wstring (builtinToolbarName) == L"InsertBar") {
		CComPtr<IDispatch>		newTB	=	MakeNewToolbar ();
		CComQIPtr<IALToolbar>	tb		=	newTB;
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("InsertTable"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("InsertURL"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("InsertSymbol"))));
		return newTB;
	}

	if (wstring (builtinToolbarName) == L"CharacterFormatBar") {
		CComPtr<IDispatch>		newTB	=	MakeNewToolbar ();
		CComQIPtr<IALToolbar>	tb		=	newTB;
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("FontNameComboBox"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("FontSizeComboBox"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Bold"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Italic"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("Underline"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("FontColor"))));
		return newTB;
	}

	if (wstring (builtinToolbarName) == L"ParagraphFormatBar") {
		CComPtr<IDispatch>		newTB	=	MakeNewToolbar ();
		CComQIPtr<IALToolbar>	tb		=	newTB;
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("JustifyLeft"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("JustifyCenter"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("JustifyRight"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("JustifyFull"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("ParagraphIndents"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("ParagraphSpacing"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeSeparatorToolbarItem ()));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("ListStyle_None"))));
		Led_ThrowIfErrorHRESULT (tb->Add (MakeBuiltinToolbarItem (CComBSTR ("ListStyle_Bullet"))));
		return newTB;
	}

	// if name doesn't match - return NULL
	return CComPtr<IDispatch> ();
}

IDispatch*	ActiveLedItControl::OLE_MakeBuiltinToolbar (LPCOLESTR builtinToolbarName) 
{
	try {
		return MakeBuiltinToolbar (builtinToolbarName).Detach ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

CComPtr<IDispatch>	ActiveLedItControl::MakeBuiltinToolbarItem (LPCOLESTR builtinToolbarItemName)
{
	CComPtr<IDispatch>	tmp;
	tmp.Attach (OLE_MakeBuiltinToolbarItem (builtinToolbarItemName));
	return tmp;
}

IDispatch*	ActiveLedItControl::OLE_MakeBuiltinToolbarItem (LPCOLESTR builtinToolbarItemName) 
{
	const	ActiveLedItControl::ToolBarIconSpec	kToolBarIconSpecs[]	=	{
		{	_T("Undo"),				kToolbarButton_Undo_ResID,					_T("Undo"),				eIconButton_PushButton	},
		{	_T("Redo"),				kToolbarButton_Redo_ResID,					_T("Redo"),				eIconButton_PushButton	},
		{	_T("Cut"),				kToolbarButton_Cut_ResID,					_T("Cut"),				eIconButton_PushButton	},
		{	_T("Copy"),				kToolbarButton_Copy_ResID,					_T("Copy"),				eIconButton_PushButton	},
		{	_T("Paste"),			kToolbarButton_Paste_ResID,					_T("Paste"),			eIconButton_PushButton	},
		{	_T("Print"),			kToolbarButton_Print_ResID,					_T("Print"),			eIconButton_PushButton	},
		{	_T("Bold"),				kToolbarButton_Bold_ResID,					_T("Bold"),				eIconButton_Toggle		},
		{	_T("Italic"),			kToolbarButton_Italics_ResID,				_T("Italic"),			eIconButton_Toggle		},
		{	_T("Underline"),		kToolbarButton_Underline_ResID,				_T("Underline"),		eIconButton_Toggle		},
		{	_T("JustifyLeft"),		kToolbarButton_JustifyLeft_ResID,			_T("JustifyLeft"),		eIconButton_Sticky		},
		{	_T("JustifyCenter"),	kToolbarButton_JustifyCenter_ResID,			_T("JustifyCenter"),	eIconButton_Sticky		},
		{	_T("JustifyRight"),		kToolbarButton_JustifyRight_ResID,			_T("JustifyRight"),		eIconButton_Sticky		},
		{	_T("JustifyFull"),		kToolbarButton_JustifyFull_ResID,			_T("JustifyFull"),		eIconButton_Sticky		},
		{	_T("ParagraphIndents"),	kToolbarButton_ParagraphIndents_ResID,		_T("ParagraphIndents"),	eIconButton_PushButton	},
		{	_T("ParagraphSpacing"),	kToolbarButton_ParagraphSpacing_ResID,		_T("ParagraphSpacing"),	eIconButton_PushButton	},
		{	_T("ListStyle_None"),	kToolbarButton_ListStyle_None_ResID,		_T("ListStyle_None"),	eIconButton_Sticky		},
		{	_T("ListStyle_Bullet"),	kToolbarButton_ListStyle_Bullet_ResID,		_T("ListStyle_Bullet"),	eIconButton_Sticky		},
		{	_T("Find"),				kToolbarButton_Find_ResID,					_T("Find"),				eIconButton_PushButton	},
		{	_T("Replace"),			kToolbarButton_Replace_ResID,				_T("Replace"),			eIconButton_PushButton	},
		{	_T("CheckSpelling"),	kToolbarButton_CheckSpelling_ResID,			_T("CheckSpelling"),	eIconButton_PushButton	},
		{	_T("InsertTable"),		kToolbarButton_InsertTable_ResID,			_T("InsertTable"),		eIconButton_PushButton	},
		{	_T("InsertURL"),		kToolbarButton_InsertURL_ResID,				_T("InsertURL"),		eIconButton_PushButton	},
		{	_T("InsertSymbol"),		kToolbarButton_InsertSymbol_ResID,			_T("InsertSymbol"),		eIconButton_PushButton	},
	};

	try {
		for  (size_t i = 0; i < Led_NEltsOf (kToolBarIconSpecs); ++i) {
			if (CComBSTR (builtinToolbarItemName) == CComBSTR (kToolBarIconSpecs[i].fIconName)) {
				return mkIconElement (kToolBarIconSpecs[i]).Detach ();
			}
		}

		if (wstring (builtinToolbarItemName) == wstring (L"FontNameComboBox")) {
			return MakeBuiltinComboBoxToolbarItem (mkMenu_Font ()).Detach ();
		}
		else if (wstring (builtinToolbarItemName) == wstring (L"FontSizeComboBox")) {
			return MakeBuiltinComboBoxToolbarItem (mkMenu_FontSize (true)).Detach ();
		}
		else if (wstring (builtinToolbarItemName) == wstring (L"FontColor")) {
			return mkIconElement (kToolbarButton_FontColor_ResID, mkMenu_FontColor ()).Detach ();
		}
		else if (wstring (builtinToolbarItemName) == wstring (L"ActiveLedIt")) {
			CComPtr<IDispatch>	tmp;
			tmp.Attach (OLE_GetDefaultContextMenu ());
			return mkIconElement (kToolbarButton_ActiveLedIt_ResID, tmp).Detach ();
		}
		return NULL;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

CComPtr<IDispatch>	ActiveLedItControl::mkIconElement (int iconResID)
{
	CComPtr<IDispatch>						item;
	item.Attach (OLE_MakeIconButtonToolbarItem ());
	CComQIPtr<IALIconButtonToolbarElement>	newIconButton	=	item;

	CComPtr<IDispatch>	picture;
	{
		PICTDESC				pictDesc;
		::memset (&pictDesc, 0, sizeof (pictDesc));
		pictDesc.cbSizeofstruct = sizeof (pictDesc);
		pictDesc.picType = PICTYPE_ICON;
		pictDesc.icon.hicon = reinterpret_cast<HICON> (::LoadImage (AfxGetInstanceHandle (), MAKEINTRESOURCE (iconResID), IMAGE_ICON, 0, 0, 0));
		Led_ThrowIfErrorHRESULT (::OleCreatePictureIndirect (&pictDesc, IID_IDispatch, true, (void**)&picture));
	}

	Led_ThrowIfErrorHRESULT (newIconButton->put_ButtonImage (picture));
	return item;
}

CComPtr<IDispatch>	ActiveLedItControl::mkIconElement (const ToolBarIconSpec& s)
{
	CComPtr<IDispatch>						item		=	mkIconElement (s.fIconResId);
	CComQIPtr<IALIconButtonToolbarElement>	iconButton	=	item;

	Led_ThrowIfErrorHRESULT (iconButton->put_Command (CComVariant (s.fCmdName)));
	Led_ThrowIfErrorHRESULT (iconButton->put_ButtonStyle (s.fButtonStyle));
	return item;
}

CComPtr<IDispatch>	ActiveLedItControl::mkIconElement (int iconResID, CComPtr<IDispatch> cmdList)
{
	CComPtr<IDispatch>						item		=	mkIconElement (iconResID);
	CComQIPtr<IALIconButtonToolbarElement>	iconButton	=	item;

	Led_ThrowIfErrorHRESULT (iconButton->put_Command (CComVariant (cmdList)));
	return item;
}

CComPtr<IDispatch>	ActiveLedItControl::MakeBuiltinComboBoxToolbarItem (CComPtr<IDispatch> cmdList) 
{
	try {
		CComObject<ActiveLedIt_ComboBoxToolbarElement>* o = NULL;
		Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_ComboBoxToolbarElement>::CreateInstance (&o));
		CComQIPtr<IDispatch>	result	=	o->GetUnknown ();
		CComQIPtr<IALComboBoxToolbarElement>	alcomboBox	=	result;
		Led_ThrowIfErrorHRESULT (alcomboBox->put_CommandList (cmdList));
		return result;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

VARIANT		ActiveLedItControl::OLE_GetCurrentEventArguments ()
{
	VARIANT	result;
	::VariantInit (&result);
	{
		result.vt = VT_DISPATCH; 
		result.pdispVal = fCurrentEventArguments;
		if (result.pdispVal != NULL) {
			result.pdispVal->AddRef ();
		}
	}
	return result;
}

long	ActiveLedItControl::GetSelStart () 
{
	return fEditor.GetSelectionStart ();
}

void	ActiveLedItControl::SetSelStart (long start) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	size_t	s;
	size_t	e;
	fEditor.GetSelection (&s, &e);
	size_t	l	=	e-s;
	size_t	bufLen	=	fEditor.GetLength ();
	size_t	newStart = Led_Min (static_cast<size_t> (start), bufLen);
	size_t	newEnd = Led_Min (newStart + l, bufLen);
	fEditor.SetSelection (newStart, newEnd);
}

long	ActiveLedItControl::GetSelLength () 
{
	size_t	s;
	size_t	e;
	fEditor.GetSelection (&s, &e);
	return e-s;
}

void	ActiveLedItControl::SetSelLength (long length) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	if (length == -1) {
		length = INT_MAX;
	}
	if (length < 0) {
		length = 0;
	}
	size_t	s;
	size_t	e;
	fEditor.GetSelection (&s, &e);
	size_t	l	=	e-s;
	size_t	bufLen	=	fEditor.GetLength ();
	size_t	newEnd = Led_Min (s + length, bufLen);
	fEditor.SetSelection (s, newEnd);
}

BSTR	ActiveLedItControl::GetSelText ()
{
	try {
		size_t	s;
		size_t	e;
		fEditor.GetSelection (&s, &e);
		size_t	len	=		e-s;
		Led_SmallStackBuffer<Led_tChar>	buf (len + 1);
		fEditor.CopyOut (s, len, buf);
		buf[len] = '\0';
		return CString (buf).AllocSysString ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false); /*NOTREACHED*/ return NULL;
}

void	ActiveLedItControl::SetSelText (LPCTSTR text)
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	try {
		size_t	len	=	::_tcslen (text);
		Led_SmallStackBuffer<Led_tChar>	buf (len + 1);
		len = Led_NativeToNL (Led_SDKString2tString (text).c_str (), len, buf, len + 1);
		size_t	s;
		size_t	e;
		fEditor.GetSelection (&s, &e);
		fEditor.Replace (s, e, buf, len);
		if (s != e) {
			fEditor.SetSelection (s, s + len);
		}
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

BSTR	ActiveLedItControl::GetSelTextAsRTF () 
{
	try {
		WordProcessor::WordProcessorTextIOSrcStream	source (&fEditor, fEditor.GetSelectionStart (), fEditor.GetSelectionEnd ());
		StyledTextIOWriterSinkStream_Memory			sink;
		StyledTextIOWriter_RTF						textWriter (&source, &sink);
		textWriter.Write ();
		size_t	len	=	sink.GetLength ();
		Led_SmallStackBuffer<char>	buf (len + 1);
		::memcpy (buf, sink.PeekAtData (), len);
		buf[len] = '\0';
		return CString (buf).AllocSysString ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false); /*NOTREACHED*/ return NULL;
}

void	ActiveLedItControl::SetSelTextAsRTF (LPCTSTR text) 
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		fCommandHandler.Commit ();
		StyledTextIOSrcStream_Memory					source (text, text==NULL? 0: ::_tcslen (text));
		WordProcessor::WordProcessorTextIOSinkStream	sink (&fEditor, fEditor.GetSelectionStart ());
		StyledTextIOReader_RTF							textReader (&source, &sink);
		fEditor.Replace (fEditor.GetSelectionStart (), fEditor.GetSelectionEnd (), LED_TCHAR_OF (""), 0);
		textReader.Read ();
		fEditor.SetEmptySelectionStyle ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();

	// SHOULD FIX THIS CODE TO ASSURE OUTPUTTED TEXT REMAINS SELECTED!!!
}

BSTR	ActiveLedItControl::GetSelTextAsHTML () 
{
	try {
		WordProcessor::WordProcessorTextIOSrcStream	source (&fEditor, fEditor.GetSelectionStart (), fEditor.GetSelectionEnd ());
		StyledTextIOWriterSinkStream_Memory			sink;
		StyledTextIOWriter_HTML						textWriter (&source, &sink);
		textWriter.Write ();
		size_t	len	=	sink.GetLength ();
		Led_SmallStackBuffer<char>	buf (len + 1);
		::memcpy (buf, sink.PeekAtData (), len);
		buf[len] = '\0';
		return CString (buf).AllocSysString ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false);	return 0;	// NOTREACHED
}

void	ActiveLedItControl::SetSelTextAsHTML (LPCTSTR text) 
{
	CHECK_DEMO_AND_ALERT_AND_RETURN_NO_TIME_CHECK(fEditor.GetHWND ());
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		fCommandHandler.Commit ();
		StyledTextIOSrcStream_Memory					source (text, text==NULL? 0: ::_tcslen (text));
		WordProcessor::WordProcessorTextIOSinkStream	sink (&fEditor, fEditor.GetSelectionStart ());
		StyledTextIOReader_HTML							textReader (&source, &sink);
		fEditor.Replace (fEditor.GetSelectionStart (), fEditor.GetSelectionEnd (), LED_TCHAR_OF (""), 0);
		textReader.Read ();
		fEditor.SetEmptySelectionStyle ();
		// SHOULD FIX THIS CODE TO ASSURE OUTPUTTED TEXT REMAINS SELECTED!!!
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

OLE_COLOR	ActiveLedItControl::GetSelColor () 
{
	try {
		Led_Assert (fEditor.GetSelectionEnd () >= fEditor.GetSelectionStart ());
		size_t	selectionLength	=	fEditor.GetSelectionEnd () - fEditor.GetSelectionStart ();
		Led_IncrementalFontSpecification	fsp = fEditor.GetContinuousStyleInfo (fEditor.GetSelectionStart (), selectionLength);
	//HOW DO YOU SAY RETURNS ERROR??? LIKE YOU CAN WITH ATL???
		if (not fsp.GetTextColor_Valid ()) {
			//HACK!!!
			fsp = fEditor.GetContinuousStyleInfo (fEditor.GetSelectionStart (), 0);
			Led_Assert (fsp.GetTextColor_Valid ());
		}
		return fsp.GetTextColor ().GetOSRep ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false);	return 0;	// NOTREACHED
}

void	ActiveLedItControl::SetSelColor (OLE_COLOR color) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		fCommandHandler.Commit ();
		Led_IncrementalFontSpecification	applyFontSpec;
		applyFontSpec.SetTextColor (Led_Color (TranslateColor (color)));
		fEditor.InteractiveSetFont (applyFontSpec);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

BSTR	ActiveLedItControl::GetSelFontFace () 
{
	try {
		Led_Assert (fEditor.GetSelectionEnd () >= fEditor.GetSelectionStart ());
		size_t	selectionLength	=	fEditor.GetSelectionEnd () - fEditor.GetSelectionStart ();
		Led_IncrementalFontSpecification	fsp = fEditor.GetContinuousStyleInfo (fEditor.GetSelectionStart (), selectionLength);
		if (not fsp.GetFontNameSpecifier_Valid ()) {
			return CString ().AllocSysString ();
		}
		return CString (fsp.GetFontName ().c_str ()).AllocSysString ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false);	return 0;	// NOTREACHED
}

void	ActiveLedItControl::SetSelFontFace (LPCTSTR fontFace) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		fCommandHandler.Commit ();
		Led_IncrementalFontSpecification	applyFontSpec;
		applyFontSpec.SetFontName (fontFace);
		fEditor.InteractiveSetFont (applyFontSpec);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

long	ActiveLedItControl::GetSelFontSize () 
{
	try {
		Led_Assert (fEditor.GetSelectionEnd () >= fEditor.GetSelectionStart ());
		size_t	selectionLength	=	fEditor.GetSelectionEnd () - fEditor.GetSelectionStart ();
		Led_IncrementalFontSpecification	fsp = fEditor.GetContinuousStyleInfo (fEditor.GetSelectionStart (), selectionLength);
		if (not fsp.GetPointSize_Valid ()) {
			return 0;
		}
		return fsp.GetPointSize ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false);	return 0;	// NOTREACHED
}

void	ActiveLedItControl::SetSelFontSize (long size) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		fCommandHandler.Commit ();
		Led_IncrementalFontSpecification	applyFontSpec;
		if (size <= 0) {
			size = 2;	// minsize?
		}
		if (size >= 128) {
			size = 128;
		}
		applyFontSpec.SetPointSize (static_cast<unsigned short> (size));
		fEditor.InteractiveSetFont (applyFontSpec);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

long	ActiveLedItControl::GetSelBold () 
{
	try {
		size_t	selStart;
		size_t	selEnd;
		fEditor.GetSelection (&selStart, &selEnd);
		Led_Assert (selStart <= selEnd);
		size_t	selectionLength	=	selEnd - selStart;
		Led_IncrementalFontSpecification	fsp = fEditor.GetContinuousStyleInfo (selStart, selectionLength);
		if (not fsp.GetStyle_Bold_Valid ()) {
			return 2;
		}
		return fsp.GetStyle_Bold ()? 1: 0;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false);	return 0;	// NOTREACHED
}

void	ActiveLedItControl::SetSelBold (long bold) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		fCommandHandler.Commit ();
		Led_IncrementalFontSpecification	applyFontSpec;
		applyFontSpec.SetStyle_Bold (bold);
		fEditor.InteractiveSetFont (applyFontSpec);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

long	ActiveLedItControl::GetSelItalic () 
{
	try {
		size_t	selStart;
		size_t	selEnd;
		fEditor.GetSelection (&selStart, &selEnd);
		Led_Assert (selStart <= selEnd);
		size_t	selectionLength	=	selEnd - selStart;
		Led_IncrementalFontSpecification	fsp = fEditor.GetContinuousStyleInfo (selStart, selectionLength);
		if (not fsp.GetStyle_Italic_Valid ()) {
			return 2;
		}
		return fsp.GetStyle_Italic ()? 1: 0;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false);	return 0;	// NOTREACHED
}

void	ActiveLedItControl::SetSelItalic (long italic) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		fCommandHandler.Commit ();
		Led_IncrementalFontSpecification	applyFontSpec;
		applyFontSpec.SetStyle_Italic (italic);
		fEditor.InteractiveSetFont (applyFontSpec);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

long	ActiveLedItControl::GetSelStrikeThru () 
{
	try {
		size_t	selStart;
		size_t	selEnd;
		fEditor.GetSelection (&selStart, &selEnd);
		Led_Assert (selStart <= selEnd);
		size_t	selectionLength	=	selEnd - selStart;
		Led_IncrementalFontSpecification	fsp = fEditor.GetContinuousStyleInfo (selStart, selectionLength);
		if (not fsp.GetStyle_Strikeout_Valid ()) {
			return 2;
		}
		return fsp.GetStyle_Strikeout ()? 1: 0;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false);	return 0;	// NOTREACHED
}

void	ActiveLedItControl::SetSelStrikeThru (long strikeThru) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		fCommandHandler.Commit ();
		Led_IncrementalFontSpecification	applyFontSpec;
		applyFontSpec.SetStyle_Strikeout (strikeThru);
		fEditor.InteractiveSetFont (applyFontSpec);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

long	ActiveLedItControl::GetSelUnderline () 
{
	try {
		size_t	selStart;
		size_t	selEnd;
		fEditor.GetSelection (&selStart, &selEnd);
		Led_Assert (selStart <= selEnd);
		size_t	selectionLength	=	selEnd - selStart;
		Led_IncrementalFontSpecification	fsp = fEditor.GetContinuousStyleInfo (selStart, selectionLength);
		if (not fsp.GetStyle_Underline_Valid ()) {
			return 2;
		}
		return fsp.GetStyle_Underline ()? 1: 0;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false);	return 0;	// NOTREACHED
}

void	ActiveLedItControl::SetSelUnderline (long underline) 
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		fCommandHandler.Commit ();
		Led_IncrementalFontSpecification	applyFontSpec;
		applyFontSpec.SetStyle_Underline (underline);
		fEditor.InteractiveSetFont (applyFontSpec);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

UINT	ActiveLedItControl::OLE_GetSelJustification ()
{
	try {
		Led_Justification	justification	=	eLeftJustify;
		size_t	selStart;
		size_t	selEnd;
		fEditor.GetSelection (&selStart, &selEnd);
		Led_Assert (selStart <= selEnd);
		if (fEditor.GetJustification (selStart, selEnd, &justification)) {
			switch (justification) {
				case	eLeftJustify:	return eLeftJustification;
				case	eRightJustify:	return eRightJustification;
				case	eCenterJustify:	return eCenterJustification;
				case	eFullyJustify:	return eFullJustification;
				default:				return eLeftJustification;	// what should we return here?
			}
		}
		else {
			return eNoCommonJustification;
		}
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false);	return 0;	// NOTREACHED
}

void	ActiveLedItControl::OLE_SetSelJustification (UINT justification)
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		Led_Justification	lh	=	eLeftJustify;
		switch (justification) {
			case	eLeftJustification:		lh = eLeftJustify; break;
			case	eCenterJustification:	lh = eCenterJustify; break;
			case	eRightJustification:	lh = eRightJustify; break;
			case	eFullJustification:		lh = eFullyJustify; break;
			default:	return;	// should be an error - but no error reporting supported here- should return E_INVALIDARG;
		}
		fEditor.InteractiveSetJustification (lh);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

UINT	ActiveLedItControl::OLE_GetSelListStyle ()
{
	try {
		Led::ListStyle	listStyle	=	Led::eListStyle_None;
		size_t	selStart;
		size_t	selEnd;
		fEditor.GetSelection (&selStart, &selEnd);
		Led_Assert (selStart <= selEnd);
		if (fEditor.GetListStyle (selStart, selEnd, &listStyle)) {
			// take advantage of (assumption) that OLE list style enum is the same as the Led
			// one... NB: cast REALLY not necessary as these are all cast to 'int' - but done
			// for clarity that we are really intionally mixing enum types here - LGP 2003-06-03
			return static_cast<::ListStyle> (listStyle);
		}
		else {
			return eNoCommonListStyle;
		}
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	Led_Assert (false);	return 0;	// NOTREACHED
}

void	ActiveLedItControl::OLE_SetSelListStyle (UINT listStyle)
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		::ListStyle		ls1	=	static_cast<::ListStyle> (listStyle);
		Led::ListStyle	ls2	=	static_cast<Led::ListStyle> (ls1);		// take advantage of assumption that Led list style
																		// enum is idential to one used in OLE API
		fEditor.InteractiveSetListStyle (ls2);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

UINT	ActiveLedItControl::OLE_GetSelHidable ()
{
	try {
		size_t	selStart;
		size_t	selEnd;
		fEditor.GetSelection (&selStart, &selEnd);
		Led_Assert (selStart <= selEnd);
		if (fEditor.GetHidableTextDatabase ()->GetHidableRegionsContiguous (selStart, selEnd, true)) {
			return true;
		}
		else if (fEditor.GetHidableTextDatabase ()->GetHidableRegionsContiguous (selStart, selEnd, false)) {
			return false;
		}
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	return 2;
}

void	ActiveLedItControl::OLE_SetSelHidable (UINT hidable)
{
	CHECK_DEMO_AND_BEEP_AND_RETURN();
	try {
		IdleManager::NonIdleContext	nonIdleContext;
		size_t	selStart;
		size_t	selEnd;
		fEditor.GetSelection (&selStart, &selEnd);
		Led_Assert (selStart <= selEnd);
		if (hidable) {
			fEditor.GetHidableTextDatabase ()->MakeRegionHidable (selStart, selEnd);
		}
		else {
			fEditor.GetHidableTextDatabase ()->MakeRegionUnHidable (selStart, selEnd);
		}
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

