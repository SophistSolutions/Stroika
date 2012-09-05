/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__DispIDs_h__
#define	__DispIDs_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveLedIt/Headers/DispIDs.h,v 1.43 2004/02/03 22:29:28 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: DispIDs.h,v $
 *	Revision 1.43  2004/02/03 22:29:28  lewis
 *	SPR#1618: add ToggleButton property to IconButtonToolbarItem, and CommandChecked () method to ActiveLedIt - to support checked buttons - like BOLD, ITALIC, and UNDERLINE
 *	
 *	Revision 1.42  2004/02/03 02:23:03  lewis
 *	SPR#1618: Added undo/redo buttons, and separator support
 *	
 *	Revision 1.41  2004/02/02 23:02:35  lewis
 *	SPR#1618: Started CommandEnabled support - and got associated command (to toolbar icon) code working - so for simple cases - at least - it invokes the command)
 *	
 *	Revision 1.40  2004/02/02 21:15:08  lewis
 *	SPR#1618: Added preliminary toolbar support. A small toolbar with a few icons appears - but no DO command or UPDATE_ENABLE_DISABLE CMD support yet (and just plain icon cmd items)
 *	
 *	Revision 1.39  2004/01/30 04:36:28  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.38  2004/01/25 16:19:21  lewis
 *	SPR#1617: lose old resource-based contextmenu/accelerators, and use new COM-based
 *	accelerator mechanism (part of new COM-based command support)
 *	
 *	Revision 1.37  2004/01/24 03:50:33  lewis
 *	InvokeCommand () functionality implemented
 *	
 *	Revision 1.36  2004/01/23 23:11:15  lewis
 *	SPR#1617: Added preliminary (largely functional) implementation of OLE automation-based
 *	context menus. Tons of new OLE APIs. Can create context menus from scratch, or built from
 *	building blocks of existing submenus (like font menu).
 *	
 *	Revision 1.35  2003/06/12 17:15:44  lewis
 *	SPR#1526: Added COM-based spellchecker support to ActiveLedIt. Now you can access SpellCheck
 *	menu command if there is a COM-spellchecker associated with ActiveLedIt. New property 'SpellChecker' you
 *	can get/set to specify the spell checker. Seems to work as well as with LedIt!
 *	
 *	Revision 1.34  2003/06/03 14:42:12  lewis
 *	SPR#1521: added 'SelListStyle' support. Somewhat related - also changed build of ActiveLedIt to include
 *	the _h.h file generated from the IDL for access to enums rather than redefining them in the
 *	ActiveLedItControl sources.
 *	
 *	Revision 1.33  2003/05/30 14:26:14  lewis
 *	SPR#1517: Added find/replace dialog and OLE_FindReplace and OLE_LaunchReplaceDialog new OLE AUTOMATION methods
 *	
 *	Revision 1.32  2003/03/26 15:47:32  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.31  2003/03/05 22:39:29  lewis
 *	SPR#1320 - Added stock Appearance and BorderStyle property support. Also, changed my existing HWND
 *	property to using the stock HWND property code.
 *	
 *	Revision 1.30  2003/02/12 15:59:48  lewis
 *	SPR#1298- added ShowSecondaryHilight property to the control, and default to TRUE
 *	
 *	Revision 1.29  2003/01/15 18:57:29  lewis
 *	SPR#1221- added SmartQuoteMode OLE automation
 *	
 *	Revision 1.28  2003/01/09 14:09:22  lewis
 *	SPR#1227- added hWnd read-only property to ActiveLedIt control
 *	
 *	Revision 1.27  2002/12/16 19:23:48  lewis
 *	SPR#1208- added UseSelectEOLBOLRowHilightStyle property
 *	
 *	Revision 1.26  2002/10/30 16:55:36  lewis
 *	SPR#1153- added PrintMargins OLE AUT interface to ActiveLedIt
 *	
 *	Revision 1.25  2002/10/28 21:47:09  lewis
 *	SPR#0678 - Added OLE AUT methods for Get/Set DefaultWindowMargins
 *	
 *	Revision 1.24  2002/05/06 21:34:19  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 1.23  2001/11/27 00:32:28  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.22  2001/08/30 00:35:56  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.21  2001/08/23 20:47:01  lewis
 *	SPR#0972- new EnableAutoChangesBackgroundColor feature
 *	
 *	Revision 1.20  2001/08/01 23:51:42  lewis
 *	use a few more STOCK_PROPRETY DISPIDs. And SPR# 0964 - ReadOnly/Enabled property supported
 *	
 *	Revision 1.19  2001/05/30 16:23:03  lewis
 *	SPR#0945- new VersionNumber/ShortVersionString OLEAUT APIs
 *	
 *	Revision 1.18  2001/04/17 16:50:28  lewis
 *	SPR#0610- Added PrinterSetupDialog, PrintDialog and PrintOnce OLE AUT methods.
 *	Also made plain Added PrinterSetupDialog, PrintDialog and PrintOnce OLE AUT methods.
 *	Also made plain
 *	
 *	Revision 1.17  2001/04/09 20:43:33  lewis
 *	SPR#0859- Added LaunchFontSettingsDialog/LaunchParagraphSettingsDialog methods.
 *	
 *	Revision 1.16  2001/02/12 15:02:33  lewis
 *	SPR#0854- added OLE Automation GetHeight method - to compute pixelheight of a
 *	given range of text
 *	
 *	Revision 1.15  2000/08/28 21:51:11  lewis
 *	SPR#0830- added TextColored IDL proprty - so you can use the old or new display
 *	algorith for hidden text. Defaults to MSWord style display.
 *	
 *	Revision 1.14  2000/04/10 16:26:52  lewis
 *	SPR#0735- added SelHidable OLE AUT property
 *	
 *	Revision 1.13  2000/04/03 15:39:56  lewis
 *	SPR#0728- Incorporate HiddenText OLEAUT support into the OCX
 *	
 *	Revision 1.12  2000/04/03 03:44:22  lewis
 *	SPR#0716- added FindUI and Find(noUI) OLEAutomation methods
 *	
 *	Revision 1.11  2000/03/15 21:27:52  lewis
 *	SPR#0715- added a read/write SelJustification property
 *	
 *	Revision 1.10  2000/03/15 19:20:25  lewis
 *	SPR#0714: support IDL dirty flag. Also - change default size of new ActiveLedIt! control
 *	
 *	Revision 1.9  2000/03/14 00:32:13  lewis
 *	started adding UNDO support to IDL - SPR#0712
 *	
 *	Revision 1.8  1999/12/27 20:16:26  lewis
 *	SPR#0669- partial - now support these 3 properties for show/hide space/paragraphs etc
 *	
 *	Revision 1.7  1999/12/21 20:51:09  lewis
 *	SPR#0672- AsTextBitmap() support. Plus a few small tweeks.
 *	
 *	Revision 1.6  1999/11/13 16:53:54  lewis
 *	Fixed spr#0623 - added ScrollToSelection IDL method
 *	
 *	Revision 1.5  1999/06/26 20:44:58  lewis
 *	add named defines for a few event DISPIDs (relating to spr#0528)
 *	
 *	Revision 1.4  1999/06/25 14:08:52  lewis
 *	make IDL name MaxInputLength -> MaxLength to match MS RichTextControl 6.0
 *	
 *	Revision 1.3  1999/06/25 00:50:36  lewis
 *	spr#0595- add 'MaximumInputLength property to control. Most of this was trivial. We do a
 *	throw from AboutToUpdateText() method if too much text entered. Easy. But the trick is
 *	that for an OCX - there is no top level catcher to handle these excptions. They go
 *	unhandled. To address this, the Led_MFC_ExceptionHandlerHelper<> template as added
 *	
 *	Revision 1.2  1999/06/23 21:57:40  lewis
 *	add a HorzSBar and WrapToWindow toggle button
 *	
 *	Revision 1.1  1998/03/04 20:43:51  lewis
 *	Initial revision
 *	
 *
 *
 *
 *
 */


#define	DISPID_VersionNumber					1001
#define	DISPID_ShortVersionString				1002
#define	DISPID_HasVerticalScrollBar				1003
#define	DISPID_HasHorizontalScrollBar			1004
#define	DISPID_Text								1005
#define	DISPID_TextCRLF							1006
#define	DISPID_TextRTF							1007
#define	DISPID_TextHTML							1008
#define	DISPID_TextBitmap						1009
#define	DISPID_Length							1010
#define	DISPID_MaxLength						1011
#define	DISPID_SupportContextMenu				1012
#define	DISPID_SmartCutAndPaste					1013
#define	DISPID_WrapToWindow						1014
#define	DISPID_ShowParagraphGlyphs				1015
#define	DISPID_ShowTabGlyphs					1016
#define	DISPID_ShowSpaceGlyphs					1017
#define	DISPID_ShowHidableText					1018
#define	DISPID_HidableTextColor					1019
#define	DISPID_HidableTextColored				1020
#define	DISPID_SpellChecker						1066
//#define	DISPID_SelStart						1021
//#define	DISPID_SelLength					1022
//#define	DISPID_SelText						1023
#define	DISPID_SelTextRTF						1024
#define	DISPID_SelTextHTML						1025
#define	DISPID_SelColor							1026
#define	DISPID_SelFontFace						1027
#define	DISPID_SelFontSize						1028
#define	DISPID_SelBold							1029
#define	DISPID_SelItalic						1030
#define	DISPID_SelStrikeThru					1031
#define	DISPID_SelUnderline						1032
#define	DISPID_SelJustification					1033
#define	DISPID_SelListStyle						1065
#define	DISPID_SelHidable						1034
#define	DISPID_LoadFile							1035
#define	DISPID_SaveFile							1036
#define	DISPID_SaveFileCRLF						1037
#define	DISPID_SaveFileRTF						1038
#define	DISPID_SaveFileHTML						1039
//#define	DISPID_Refresh						1040
#define	DISPID_ScrollToSelection				1041
#define	DISPID_MaxUndoLevel						1042
#define	DISPID_CanUndo							1043
#define	DISPID_CanRedo							1044
#define	DISPID_Undo								1045
#define	DISPID_Redo								1046
#define	DISPID_CommitUndo						1047
#define	DISPID_Dirty							1048
#define	DISPID_ReadOnly							1071
#define	DISPID_EnableAutoChangesBackgroundColor	1072

#define	DISPID_LaunchFontSettingsDialog			1049
#define	DISPID_LaunchParagraphSettingsDialog	1050

#define	DISPID_LaunchFindDialog					1051
#define	DISPID_Find								1052
#define	DISPID_LaunchReplaceDialog				1079
#define	DISPID_FindReplace						1080

#define	DISPID_PrinterSetupDialog				1053
#define	DISPID_PrintDialog						1054
#define	DISPID_PrintOnce						1055

#define	DISPID_GetHeight						1056

#define	DISPID_WindowMarginTop					1057
#define	DISPID_WindowMarginLeft					1058
#define	DISPID_WindowMarginBottom				1059
#define	DISPID_WindowMarginRight				1060

#define	DISPID_PrintMarginTop					1061
#define	DISPID_PrintMarginLeft					1062
#define	DISPID_PrintMarginBottom				1063
#define	DISPID_PrintMarginRight					1064

#define	DISPID_UseSelectEOLBOLRowHilightStyle	1075

//#define	DISPID_hWnd								1076
#define	DISPID_SmartQuoteMode					1077

#define	DISPID_ShowSecondaryHilight				1078

#define	DISPID_ContextMenu						1081
#define	DISPID_HideDisabledContextMenuItems		1086
#define	DISPID_GetDefaultContextMenu			1082
#define	DISPID_BuiltinCommands					1083
#define	DISPID_MakeNewPopupMenuItem				1084
#define	DISPID_MakeNewUserMenuItem				1085

#define	DISPID_CurrentEventArguments			1087

#define	DISPID_PredefinedMenus					1088
#define	DISPID_InvokeCommand					1089
#define	DISPID_CommandEnabled					1101
#define	DISPID_CommandChecked					1102
#define	DISPID_AcceleratorTable					1090
#define	DISPID_GetDefaultAcceleratorTable		1091
#define	DISPID_MakeNewAcceleratorElement		1092

#define	DISPID_ToolbarList						1093
#define	DISPID_MakeNewToolbarList				1094
#define	DISPID_MakeNewToolbar					1095
#define	DISPID_MakeIconButtonToolbarItem		1096
#define	DISPID_MakeSeparatorToolbarItem			1097
#define	DISPID_MakeBuiltinToolbar				1098
#define	DISPID_MakeBuiltinToolbarItem			1099
#define	DISPID_InvalidateLayout					1100

//EVENT dispids

#define	DISPID_Change					0x01
#define	DISPID_SelChange				0x02
#define	DISPID_UpdateUserCommand		0x03
#define	DISPID_UserCommand				0x04





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


#endif	/*__DispIDs_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

