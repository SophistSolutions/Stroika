/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef	__Led_Std_Dialogs_r__
#define	__Led_Std_Dialogs_r__	1


#include	"Stroika/Frameworks/Led/StdDialogs.h"




	

#if		qStroika_Platform_Windows
	#if		qStroika_Platform_Windows
		kLedStdDlg_AboutBoxID	DIALOG DISCARDABLE  34, 22, 250, 110
		CAPTION "About XXXX!"
		STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
		FONT 8, "MS Shell Dlg"
		Begin
			// Don't sweat positions here, cuz they are all patched in the OnInitDialog() cuz
			// they must be BITMAP relative...
			CONTROL			"",					kLedStdDlg_AboutBox_InfoLedFieldID,		"Button",		BS_OWNERDRAW | WS_VISIBLE,
																						20,20,50,20
			CONTROL			"",					kLedStdDlg_AboutBox_LedWebPageFieldID, "Button",		BS_OWNERDRAW | WS_VISIBLE,
																						20,20,50,20
			CONTROL			"AboutBoxImage",	kLedStdDlg_AboutBox_BigPictureFieldID, "Static",		SS_BITMAP | WS_VISIBLE,
																						0,0,200,200
			CTEXT           "VERSION\n",		kLedStdDlg_AboutBox_VersionFieldID,		45,10,150,8
			DEFPUSHBUTTON   "OK",				IDOK,									196,6,32,14,	WS_GROUP
		End
	#endif
#endif











#if		qStroika_Platform_Windows
	#if		qStroika_Platform_Windows
		kLedStdDlg_FindBoxID DIALOG DISCARDABLE  34, 22, 262, 60
		CAPTION "Find"
		STYLE DS_MODALFRAME | WS_CAPTION | WS_SYSMENU
		FONT 8, "MS Shell Dlg"
		Begin
			LTEXT           "Find:",			0,									10, 10,	25, 8
			EditText							kLedStdDlg_FindBox_FindText,		33, 8,	164, 12,	WS_TABSTOP | ES_AUTOHSCROLL
			AutoCheckBox	"Word &Wrap at End",kLedStdDlg_FindBox_WrapAtEndOfDoc,	10, 29, 80, 10,		WS_TABSTOP
			AutoCheckBox	"Match W&hole Words",kLedStdDlg_FindBox_WholeWord,		10, 41, 80, 10,		WS_TABSTOP
			AutoCheckBox	"&Ignore Case",		kLedStdDlg_FindBox_IgnoreCase,		110, 29, 80, 10,	WS_TABSTOP
			DefPushButton   "&Find",			kLedStdDlg_FindBox_Find,			210,6,42,14,		WS_TABSTOP
			PushButton		"Don't Find",		kLedStdDlg_FindBox_Cancel,			210,26,42,14,		WS_TABSTOP
		End
	#endif
#endif







#if		qStroika_Platform_Windows
	#if		qStroika_Platform_Windows
		kLedStdDlg_ReplaceBoxID DIALOG DISCARDABLE  34, 22, 314, 72
		CAPTION "Find / Replace"
		STYLE DS_MODALFRAME | WS_CAPTION | WS_SYSMENU
		FONT 8, "MS Shell Dlg"
		Begin
			LTEXT           "Find:",			-1,										10, 10,	33, 8
			EditText							kLedStdDlg_ReplaceBox_FindText,			43, 8,	154, 12,	WS_TABSTOP | ES_AUTOHSCROLL
			LTEXT           "Replace:",			-1,										10, 26,	33, 8
			EditText							kLedStdDlg_ReplaceBox_ReplaceText,		43, 24,	154, 12,	WS_TABSTOP | ES_AUTOHSCROLL
			AutoCheckBox	"Word &Wrap at End",kLedStdDlg_ReplaceBox_WrapAtEndOfDoc,	10, 43, 80, 10,		WS_TABSTOP
			AutoCheckBox	"Match W&hole Words",kLedStdDlg_ReplaceBox_WholeWord,		10, 55, 80, 10,		WS_TABSTOP
			AutoCheckBox	"&Ignore Case",		kLedStdDlg_ReplaceBox_IgnoreCase,		110, 43, 80, 10,	WS_TABSTOP
			PushButton		"&Find",			kLedStdDlg_ReplaceBox_Find,				210,8,42,14,		WS_TABSTOP
			PushButton		"Close",			kLedStdDlg_ReplaceBox_Cancel,			260,8,42,14,		WS_TABSTOP
			DefPushButton	"&Replace",			kLedStdDlg_ReplaceBox_Replace,			210,31,42,14,		WS_TABSTOP
			PushButton		"Replace &All",		kLedStdDlg_ReplaceBox_ReplaceAll,		260,31,42,14,		WS_TABSTOP
			PushButton		"Replace All in &Selection",
												kLedStdDlg_ReplaceBox_ReplaceAllInSelection,
																						210,49,92,14,		WS_TABSTOP
		End
	#endif
#endif







#if		qStroika_Platform_Windows
	kLedStdDlg_UpdateWin32FileAssocsDialogID	DIALOG DISCARDABLE  34, 22, 250, 82
	CAPTION "Update file associations"
	STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
	FONT 8, "MS Shell Dlg"
	Begin
		LTEXT           "%0 is not associated with certain file types (%1). Would you like %0 to automatically associate itself with those file types?",
																		kLedStdDlg_UpdateWin32FileAssocsDialog_Msg,		25,10,200,24
		AutoCheckBox	"Perform this check each time %0 starts",		kLedStdDlg_UpdateWin32FileAssocsDialog_KeepCheckingCheckboxMsg,		25, 40, 200, 10,	WS_TABSTOP
		PUSHBUTTON		"Don't",										IDCANCEL,															36,62,86,14,		WS_TABSTOP | WS_GROUP
		DEFPUSHBUTTON   "Update File Associations",						IDOK,																136,62,86,14,		WS_TABSTOP | WS_GROUP
	End
#endif









#if		qStroika_Platform_Windows
	#if		qStroika_Platform_Windows
		kLedStdDlg_ParagraphIndentsID DIALOG DISCARDABLE  0, 0, 136, 86
		CAPTION "Paragraph Indents"
		STYLE DS_MODALFRAME | WS_CAPTION | WS_SYSMENU
		FONT 8, "MS Shell Dlg"
		Begin
			DEFPUSHBUTTON   "OK",				IDOK,							78,65,40,14,		WS_TABSTOP
			PushButton		"Cancel",			IDCANCEL,						18,65,40,14,		WS_TABSTOP
			LText           "Left Margin (TWIPS):",	0,							15,10,80,8
			EditText							kLedStdDlg_ParagraphIndents_LeftMarginFieldID,
																				95,8,26,12,			WS_TABSTOP|ES_NUMBER
			LText           "Right Margin (TWIPS):",	666,					15,28,80,8
			EditText							kLedStdDlg_ParagraphIndents_RightMarginFieldID,
																				95,26,26,12,		WS_TABSTOP|ES_NUMBER
			LText           "First Indent (TWIPS):",	667,					15,46,80,8
			EditText							kLedStdDlg_ParagraphIndents_FirstIndentFieldID,
																				95,44,26,12,		WS_TABSTOP|ES_NUMBER
		End
	#endif
#endif







#if		qStroika_Platform_Windows
	#if		qStroika_Platform_Windows
		kLedStdDlg_ParagraphSpacingID DIALOG DISCARDABLE  0, 0, 221, 87
		CAPTION "Paragraph Spacing"
		STYLE DS_MODALFRAME | WS_CAPTION | WS_SYSMENU
		FONT 8, "MS Shell Dlg"
		Begin
			DEFPUSHBUTTON   "OK",				IDOK,							122,66,40,14,		WS_TABSTOP
			PushButton		"Cancel",			IDCANCEL,						63,66,40,14,		WS_TABSTOP
			LText           "Space Before (TWIPS):",	0,						15,10,80,8
			EditText							kParagraphSpacing_Dialog_SpaceBeforeFieldID,
																				100,8,26,12,		WS_TABSTOP|ES_NUMBER
			LText           "Space After (TWIPS):",	666,						15,28,80,8
			EditText							kParagraphSpacing_Dialog_SpaceAfterFieldID,
																				100,26,26,12,		WS_TABSTOP|ES_NUMBER
			LText           "Line spacing:",	667,							15,46,80,8
			ComboBox		kParagraphSpacing_Dialog_LineSpaceModeFieldID,		100,46,70,100,		CBS_DROPDOWNLIST | WS_TABSTOP
			EditText							kParagraphSpacing_Dialog_LineSpaceArgFieldID,
																				180,46,26,12,		WS_TABSTOP|ES_NUMBER
		End
	#endif
#endif






#if		qStroika_Platform_Windows
	#if		qStroika_Platform_Windows
		kLedStdDlg_UnknownEmbeddingInfoBoxID	DIALOG DISCARDABLE  34, 22, 250, 60
		CAPTION "Embedding Properties"
		STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
		FONT 8, "MS Shell Dlg"
		Begin
			// Don't sweat positions here, cuz they are all patched in the OnInitDialog() cuz
			// they must be BITMAP relative...
			LTEXT           "The selected embedding is of type: '%0'",		kLedStdDlg_UnknownEmbeddingInfoBox_TypeTextMsg,		25,10,200,8
			DEFPUSHBUTTON   "OK",											IDOK,												196,36,32,14,	WS_GROUP
		End
	#endif
#endif






#if		qStroika_Platform_Windows
	#if		qStroika_Platform_Windows
		kLedStdDlg_OtherFontSizeID DIALOG DISCARDABLE  0, 0, 122, 49
		CAPTION "Font Size"
		STYLE DS_MODALFRAME | WS_CAPTION | WS_SYSMENU
		FONT 8, "MS Shell Dlg"
		Begin
			DEFPUSHBUTTON   "OK",				IDOK,							67,29,40,14,		WS_TABSTOP
			PushButton		"Cancel",			IDCANCEL,						14,29,40,14,		WS_TABSTOP
			LText           "Font size:",		0,								20,10,51,8
			EditText							kOtherFontSize_Dialog_FontSizeEditFieldID,
																				75,8,24,12,			WS_TABSTOP|ES_NUMBER
		End
	#endif
#endif







#if		qStroika_Platform_Windows
	#if		qStroika_Platform_Windows
		kLedStdDlg_URLXEmbeddingInfoBoxID	DIALOG DISCARDABLE  0, 0, 235, 90
		CAPTION "URL Embedding Properties"
		STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
		FONT 8, "MS Shell Dlg"
		Begin
			LTEXT           "The selected embedding is of type: '%0'",
												kLedStdDlg_URLXEmbeddingInfoBox_TypeTextMsg,		15,10,205,8
			LTEXT           "Title:",		-1,														15, 28,	25, 8
			EditText							kLedStdDlg_URLXEmbeddingInfoBox_TitleText,			40, 28,	180, 12,	WS_TABSTOP | ES_AUTOHSCROLL
			LTEXT           "URL:",			-1,														15, 45,	25, 8
			EditText							kLedStdDlg_URLXEmbeddingInfoBox_URLText,			40, 45,	180, 12,	WS_TABSTOP | ES_AUTOHSCROLL
			PUSHBUTTON		"No Change",		IDCANCEL,											105,66,52,14,		WS_GROUP
			DEFPUSHBUTTON   "Update",			IDOK,												168,66,52,14,		WS_GROUP
		End
	#endif
#endif







#if		qStroika_Platform_Windows
	#if		qStroika_Platform_Windows
		kLedStdDlg_AddURLXEmbeddingInfoBoxID	DIALOG DISCARDABLE  0, 0, 230, 68
		CAPTION "Add URL"
		STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
		FONT 8, "MS Shell Dlg"
		Begin
			LTEXT           "Title:",		-1,														15, 10,	25, 8
			EditText							kLedStdDlg_AddURLXEmbeddingInfoBox_TitleText,		35, 8,	180, 12,	WS_TABSTOP | ES_AUTOHSCROLL
			LTEXT           "URL:",			-1,														15, 28,	25, 8
			EditText							kLedStdDlg_AddURLXEmbeddingInfoBox_URLText,			35, 26,	180, 12,	WS_TABSTOP | ES_AUTOHSCROLL
			PUSHBUTTON		"Cancel",			IDCANCEL,											98,46,52,14,		WS_GROUP
			DEFPUSHBUTTON   "Add URL",			IDOK,												163,46,52,14,		WS_GROUP
		End
	#endif
#endif








#if		qStroika_Platform_Windows
	#if		qStroika_Platform_Windows
		kLedStdDlg_AddNewTableBoxID	DIALOG DISCARDABLE  34, 22, 180, 68
		CAPTION "Add New Table"
		STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
		FONT 8, "MS Shell Dlg"
		Begin
			LTEXT           "Insert a new table:",
											-1,													15,10,200,8

			LTEXT           "Rows:",		-1,													30, 27,	26, 8
			EditText						kLedStdDlg_AddNewTableBox_RowCount,					56, 25,	25, 12,		WS_TABSTOP | ES_AUTOHSCROLL
			LTEXT           "Columns:",		-1,													90, 27,	30, 8
			EditText						kLedStdDlg_AddNewTableBox_ColCount,					125,25,	25, 12,		WS_TABSTOP | ES_AUTOHSCROLL

			PUSHBUTTON		"Cancel",		IDCANCEL,											92,46,32,14,		WS_GROUP
			DEFPUSHBUTTON   "OK",			IDOK,												132,46,32,14,		WS_GROUP
		End
	#endif
#endif







#if		qStroika_Platform_Windows
	#if		qStroika_Platform_Windows
		kLedStdDlg_EditTablePropertiesBoxID	DIALOG DISCARDABLE  0, 0, 302, 211
		CAPTION "Edit Table Properties"
		STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
		FONT 8, "MS Shell Dlg"
		Begin
			GROUPBOX        "Table",						IDC_STATIC,					17,15,270,116

			GROUPBOX        "Border",						IDC_STATIC,					27,28,250,30
			LTEXT           "Width:",						IDC_STATIC,					45,42,57,11
			EDITTEXT										kLedStdDlg_EditTablePropertiesBox_BorderWidth,
																						89,39,24,12,ES_NUMBER
			LTEXT           "Color:",						IDC_STATIC,					152,41,30,11
			COMBOBOX										kLedStdDlg_EditTablePropertiesBox_BorderColor,
																						183,39,45,175,CBS_DROPDOWNLIST | WS_TABSTOP

			GROUPBOX        "Cell Margins",					IDC_STATIC,					28,63,250,35
			LTEXT           "Top:",							IDC_STATIC,					42,77,15,10
			EDITTEXT										kLedStdDlg_EditTablePropertiesBox_CellMarginTop,
																						62,76,24,12,ES_NUMBER
			LTEXT           "Left:",						IDC_STATIC,					98,77,15,10
			EDITTEXT										kLedStdDlg_EditTablePropertiesBox_CellMarginLeft,			
																						118,76,24,12,ES_NUMBER
			LTEXT           "Bottom:",						IDC_STATIC,					152,77,25,10
			EDITTEXT										kLedStdDlg_EditTablePropertiesBox_CellMarginBottom,			
																						184,76,24,12,ES_NUMBER
			LTEXT           "Right:",						IDC_STATIC,					216,77,19,10
			EDITTEXT										kLedStdDlg_EditTablePropertiesBox_CellMarginRight,			
																						238,76,24,12,ES_NUMBER

			LTEXT           "Cell Spacing:",				IDC_STATIC,					27,109,60,10
			EDITTEXT										kLedStdDlg_EditTablePropertiesBox_DefaultCellSpacing,		
																						87,108,24,12,ES_NUMBER

			GROUPBOX        "Selected Columns",				IDC_STATIC,					17,139,120,30
			LTEXT           "Width:",						IDC_STATIC,					42,153,25,9
			EDITTEXT										kLedStdDlg_EditTablePropertiesBox_ColumnWidth,
																						71,152,24,12,ES_NUMBER

			GROUPBOX        "Selected Cells",				IDC_STATIC,					145,139,142,30
			LTEXT           "Background Color:",			IDC_STATIC,					160,153,60,11
			COMBOBOX										kLedStdDlg_EditTablePropertiesBox_CellBackgroundColor,
																						230,152,45,175, CBS_DROPDOWNLIST | WS_TABSTOP

			LTEXT           "(measurements in TWIPS)",		IDC_STATIC,					25,184,100,14

			PUSHBUTTON      "Cancel",						IDCANCEL,					167,184,50,14
			DEFPUSHBUTTON   "OK",							IDOK,						234,184,50,14
		End
	#endif
#endif







#if		qStroika_Platform_Windows
	#if		qStroika_Platform_Windows
		kLedStdDlg_SpellCheckBoxID DIALOG DISCARDABLE  34, 22, 360, 120
		CAPTION "Check Spelling"
		STYLE DS_MODALFRAME | WS_CAPTION | WS_SYSMENU
		FONT 8, "MS Shell Dlg"
		Begin
			RTEXT           "Unknown Word:",	-1,											10, 10,	55, 8
			EditText							kLedStdDlg_SpellCheckBox_UnknownWordText,	70, 8,	140, 12,	WS_TABSTOP | ES_AUTOHSCROLL | ES_READONLY
			RTEXT           "Change To:",		-1,											10, 26,	55, 8
			EditText							kLedStdDlg_SpellCheckBox_ChangeText,		70, 24,	140, 12,	WS_TABSTOP | ES_AUTOHSCROLL
			RTEXT           "Suggestions:",		-1,											10, 42,	55, 8
			ListBox								kLedStdDlg_SpellCheckBox_SuggestedList,		70, 44, 140, 80,	WS_VSCROLL

			PushButton		"&Ignore Once",		kLedStdDlg_SpellCheckBox_IgnoreOnce,		220, 6,60,14,		WS_TABSTOP
			PushButton		"I&gnore All",		kLedStdDlg_SpellCheckBox_IgnoreAll,			290, 6,60,14,		WS_TABSTOP
			DefPushButton	"&Change",			kLedStdDlg_SpellCheckBox_ChangeOnce,		220,22,60,14,		WS_TABSTOP
			PushButton		"C&hange All",		kLedStdDlg_SpellCheckBox_ChangeAll,			290,22,60,14,		WS_TABSTOP
			PushButton		"Add to &Dictionary",kLedStdDlg_SpellCheckBox_AddDictionary,	255,44,60,14,		WS_TABSTOP
			PushButton		"&Lookup on Web",	kLedStdDlg_SpellCheckBox_LookupOnWeb,		255,62,60,14,		WS_TABSTOP
			PushButton		"&Options...",		kLedStdDlg_SpellCheckBox_Options,			255,80,60,14,		WS_TABSTOP
			PushButton		"&Close",			kLedStdDlg_SpellCheckBox_Close,				255,98,60,14,		WS_TABSTOP
		End
	#endif
#endif









#endif	/*__Led_Std_Dialogs_r__*/
