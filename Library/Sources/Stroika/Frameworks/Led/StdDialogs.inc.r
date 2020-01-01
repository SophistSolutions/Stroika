/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef	__Led_Std_Dialogs_r__
#define	__Led_Std_Dialogs_r__	1


#include	"StdDialogs.h"




	

#if		qSupportStdAboutBoxDlg
	#if		qPlatform_MacOS
		resource 'DLOG' (kLedStdDlg_AboutBoxID, purgeable) {
			{0, 0, 283, 425},
			dBoxProc,
			invisible,
			noGoAway,
			0x0,
			kLedStdDlg_AboutBoxID,
			"About XXX!",
			alertPositionMainScreen
		};
		resource 'dlgx' (kLedStdDlg_AboutBoxID, purgeable) {
			versionZero {kDialogFlagsHandleMovableModal }
		};
		resource 'DITL' (kLedStdDlg_AboutBoxID, purgeable) {
			{
				{133, 112, 133+14, 112+154},
				UserItem {
					enabled
				},
				{147, 17, 147+14, 17+160},
				UserItem {
					enabled
				},
				{0, 0, 283, 425},
				Picture {
					enabled,
					kLedStdDlg_AboutBoxID
				},
				{34, 80, 37+16, 80+284},
				StaticText {
					disabled,
					"VERS-GOES-HERE"
				},
			}
		};
		#if 0
		// These don't seem to work - at least under MacOS X - LGP 2003-01-22
		resource 'dctb' (kLedStdDlg_AboutBoxID, purgeable) {
			{
				wContentColor, 55000, 55000, 55000,
			}
		};	
		resource 'dftb' (kLedStdDlg_AboutBoxID, purgeable) {
			versionZero {
				{
					skipItem{},
					skipItem{},
					skipItem{},
					dataItem{kDialogFontUseJustMask|kDialogFontUseFaceMask|kDialogFontUseBackColorMask|kDialogFontUseForeColorMask|kDialogFontUseModeMask,0,0,0,1,2,0,0,0,3330,55000,55000,"Geneva"}
					//dataItem{kDialogFontUseJustMask|kDialogFontUseFaceMask,0,0,0,0,1,0,0,0,0,0,0,"Geneva"}
				}
			}
		};
		#endif
	#elif	qPlatform_Windows
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











#if		qSupportStdFindDlg
	#if		qPlatform_MacOS
		resource 'DLOG' (kLedStdDlg_FindBoxID) {
			{0, 0, 121, 428},
			movableDBoxProc,
			invisible,
			goAway,
			0x0,
			kLedStdDlg_FindBoxID,
			"Find",
			alertPositionParentWindow
		};
		resource 'dlgx' (kLedStdDlg_FindBoxID, purgeable) {
			versionZero {kDialogFlagsHandleMovableModal }
		};
		resource 'DITL' (kLedStdDlg_FindBoxID) {
			{
				{17, 70, 17+16, 304},
				EditText {
					disabled,
					""
				},
				{55, 35, 55+18, 35+120},
				CheckBox {
					enabled,
					"Wrap at End"
				},
				{80, 35, 80+18, 35+120},
				CheckBox {
					enabled,
					"Whole Word"
				},
				{55, 180, 55+18, 180+120},
				CheckBox {
					enabled,
					"Ignore case"
				},
				{17, 320, 17+20, 320+90},
				Button {
					enabled,
					"Find"
				},
				{50, 320, 50+20, 320+90},
				Button {
					enabled,
					"Don't Find"
				},
				{18, 22, 34, 66},
				StaticText {
					disabled,
					"Find:"
				}
			}
		};
	#elif	qPlatform_Windows
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







#if		qSupportStdReplaceDlg
	#if		qPlatform_MacOS
		resource 'DLOG' (kLedStdDlg_ReplaceBoxID) {
			{0, 0, 139, 544},
			movableDBoxProc,
			invisible,
			goAway,
			0x0,
			kLedStdDlg_ReplaceBoxID,
			"Find / Replace",
			alertPositionParentWindow
		};
		resource 'dlgx' (kLedStdDlg_ReplaceBoxID, purgeable) {
			versionZero {kDialogFlagsHandleMovableModal }
		};
		resource 'DITL' (kLedStdDlg_ReplaceBoxID) {
			{
				{17, 80, 17+16, 304},
				EditText {
					disabled,
					""
				},
				{43, 80, 43+16, 304},
				EditText {
					disabled,
					""
				},
				{79, 35, 79+18, 35+120},
				CheckBox {
					enabled,
					"Wrap at End"
				},
				{104, 35, 104+18, 35+120},
				CheckBox {
					enabled,
					"Whole Word"
				},
				{79, 180, 79+18, 180+120},
				CheckBox {
					enabled,
					"Ignore case"
				},
				{17, 320, 17+20, 320+90},
				Button {
					enabled,
					"Find"
				},
				{17, 426, 17+20, 426+90},
				Button {
					enabled,
					"Close"
				},
				{60, 320, 60+20, 320+90},
				Button {
					enabled,
					"Replace"
				},
				{60, 426, 60+20, 426+90},
				Button {
					enabled,
					"Replace All"
				},
				{93, 320, 93+20, 320+196},
				Button {
					enabled,
					"Replace All in Selection"
				},
				{18, 22, 34, 76},
				StaticText {
					disabled,
					"Find:"
				},
				{44, 22, 60, 76},
				StaticText {
					disabled,
					"Replace:"
				}
			}
		};
	#elif	qPlatform_Windows
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







#if		qSupportUpdateWin32FileAssocDlg
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









#if		qSupportParagraphIndentsDlg
	#if		qPlatform_MacOS
		resource 'DLOG' (kLedStdDlg_ParagraphIndentsID, purgeable) {
			{0, 0, 158, 292},
			movableDBoxProc,
			invisible,
			noGoAway,
			0x0,
			kLedStdDlg_ParagraphIndentsID,
			"Paragraph Indents",
			alertPositionParentWindow
		};
		resource 'dlgx' (kLedStdDlg_ParagraphIndentsID, purgeable) {
			versionZero {kDialogFlagsHandleMovableModal }
		};
		resource 'DITL' (kLedStdDlg_ParagraphIndentsID, purgeable) {
			{
					{120, 68, 120+20, 68+70},
					Button {
						enabled,
						"Cancel"
					},
					{120, 168, 120+20, 168+70},
					Button {
						enabled,
						"OK"
					},
					{24, 37, 24+17, 37+140},
					StaticText {
						disabled,
						"Left Margin (TWIPS):"
					},
					{24, 203, 24+18, 203+45},
					EditText {
						enabled,
						""
					},
					{56, 37, 56+17, 37+140},
					StaticText {
						disabled,
						"Right Margin (TWIPS):"
					},
					{56, 203, 56+18, 203+45},
					EditText {
						enabled,
						""
					},
					{85, 37, 85+17, 37+140},
					StaticText {
						disabled,
						"First Indent (TWIPS):"
					},
					{85, 203, 85+18, 203+45},
					EditText {
						enabled,
						""
					}
			}
		};
	#elif	qPlatform_Windows
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







#if		qSupportParagraphSpacingDlg
	#if		qPlatform_MacOS
		resource 'DLOG' (kLedStdDlg_ParagraphSpacingID, purgeable) {
			{0, 0, 163, 382},
			movableDBoxProc,
			invisible,
			noGoAway,
			0x0,
			kLedStdDlg_ParagraphSpacingID,
			"Paragraph Spacing",
			alertPositionParentWindow
		};
		resource 'dlgx' (kLedStdDlg_ParagraphSpacingID, purgeable) {
			versionZero {kDialogFlagsHandleMovableModal }
		};
		resource 'DITL' (kLedStdDlg_ParagraphSpacingID, purgeable) {
			{
				{124, 114, 124+20, 114+70},
				Button {
					enabled,
					"Cancel"
				},
				{124, 210, 124+20, 210+70},
				Button {
					enabled,
					"OK"
				},
				{28, 32, 28+17, 32+140},
				StaticText {
					disabled,
					"Space Before (TWIPS):"
				},
				{28, 195, 28+18, 195+45},
				EditText {
					enabled,
					""
				},
				{60, 32, 60+17, 32+140},
				StaticText {
					disabled,
					"Space After (TWIPS):"
				},
				{60, 195, 60+18, 195+45},
				EditText {
					enabled,
					""
				},
				{89, 30, 89+20, 30+260},
				Control {
					enabled,
					kParagraphSpacing_Dialog_LineSpaceCNTLID
				},
				{89, 305, 89+18, 305+45},
				EditText {
					enabled,
					""
				}
			}
		};
		resource 'MENU' (kParagraphSpacing_Dialog_LineSpaceMENUID) {
			kParagraphSpacing_Dialog_LineSpaceMENUID,
			textMenuProc,
			0xFFFFFFFF,
			enabled,
			"LineSpacing",
			{
			}
		};
		resource 'CNTL' (kParagraphSpacing_Dialog_LineSpaceCNTLID) {
			{89, 30, 89+20, 30+260},
			0,
			visible,
			160,
			kParagraphSpacing_Dialog_LineSpaceMENUID,
			popupMenuCDEFproc,
			0,
			"Line Spacing:"
		};
	#elif	qPlatform_Windows
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






#if		qSupportUnknownEmbeddingInfoDlg
	#if		qPlatform_MacOS
		resource 'DLOG' (kLedStdDlg_UnknownEmbeddingInfoBoxID, purgeable) {
			{0, 0, 123, 425},
			dBoxProc,
			invisible,
			noGoAway,
			0x0,
			kLedStdDlg_UnknownEmbeddingInfoBoxID,
			"Embedding Properties",
			alertPositionMainScreen
		};
		resource 'dlgx' (kLedStdDlg_UnknownEmbeddingInfoBoxID, purgeable) {
			versionZero {kDialogFlagsHandleMovableModal }
		};
		resource 'DITL' (kLedStdDlg_UnknownEmbeddingInfoBoxID, purgeable) {
			{
				{70, 320, 70+20, 320+60},
				Button {
					enabled,
					"OK"
				},
				{27, 80, 27+36, 80+230},
				StaticText {
					disabled,
					"The selected embedding is of type: '^0'."
				},
			}
		};
	#elif	qPlatform_Windows
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






#if		qSupportOtherFontSizeDlg
	#if		qPlatform_MacOS
		resource 'DLOG' (kLedStdDlg_OtherFontSizeID, purgeable) {
			{0, 0, 110, 212},
			movableDBoxProc,
			invisible,
			noGoAway,
			0x0,
			kLedStdDlg_OtherFontSizeID,
			"Font Size",
			alertPositionParentWindow
		};
		resource 'dlgx' (kLedStdDlg_OtherFontSizeID, purgeable) {
			versionZero {kDialogFlagsHandleMovableModal }
		};
		resource 'DITL' (kLedStdDlg_OtherFontSizeID, purgeable) {
			{
				{68, 30, 68+20, 30+65},
				Button {
					enabled,
					"Cancel"
				},
				{68, 115, 68+20, 115+65},
				Button {
					enabled,
					"OK"
				},
				{28, 37, 28+17, 37+80},
				StaticText {
					disabled,
					"Font Size:"
				},
				{28, 120, 28+16, 120+45},
				EditText {
					enabled,
					""
				}
			}
		};
	#elif	qPlatform_Windows
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







#if		qSupportURLXEmbeddingInfoDlg
	#if		qPlatform_MacOS
		resource 'DLOG' (kLedStdDlg_URLXEmbeddingInfoBoxID, purgeable) {
			{0, 0, 145, 369},
			movableDBoxProc,
			invisible,
			noGoAway,
			0x0,
			kLedStdDlg_URLXEmbeddingInfoBoxID,
			"URL Embedding Properties",
			alertPositionParentWindow
		};
		resource 'dlgx' (kLedStdDlg_URLXEmbeddingInfoBoxID, purgeable) {
			versionZero {kDialogFlagsHandleMovableModal }
		};
		resource 'DITL' (kLedStdDlg_URLXEmbeddingInfoBoxID, purgeable) {
			{
				{100, 245, 100+20, 245+90},
				Button {
					enabled,
					"Update"
				},
				{100, 135, 100+20, 135+90},
				Button {
					enabled,
					"No Change"
				},
				{27, 30, 27+16, 30+40},
				StaticText {
					disabled,
					"Title:"
				},
				{27, 30+40+10, 27+16, 30+40+10+250},
				EditText {
					enabled,
					""
				},
				{27+30, 30, 27+30+16, 30+50},
				StaticText {
					disabled,
					"URL:"
				},
				{27+30, 30+40+10, 27+30+16, 30+40+10+250},
				EditText {
					enabled,
					""
				}
			}
		};
	#elif	qPlatform_Windows
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







#if		qSupportURLXEmbeddingInfoDlg
	#if		qPlatform_MacOS
		resource 'DLOG' (kLedStdDlg_AddURLXEmbeddingInfoBoxID, purgeable) {
			{0, 0, 140, 375},
			movableDBoxProc,
			invisible,
			noGoAway,
			0x0,
			kLedStdDlg_AddURLXEmbeddingInfoBoxID,
			"Add URL",
			alertPositionParentWindow
		};
		resource 'dlgx' (kLedStdDlg_AddURLXEmbeddingInfoBoxID, purgeable) {
			versionZero {kDialogFlagsHandleMovableModal }
		};
		resource 'DITL' (kLedStdDlg_AddURLXEmbeddingInfoBoxID, purgeable) {
			{
				{100, 255, 100+20, 255+80},
				Button {
					enabled,
					"Add"
				},
				{100, 155, 100+20, 155+80},
				Button {
					enabled,
					"Cancel"
				},
				{27, 30, 27+16, 30+40},
				StaticText {
					disabled,
					"Title:"
				},
				{27, 30+40+10, 27+16, 30+40+10+250},
				EditText {
					enabled,
					""
				},
				{27+30, 30, 27+30+16, 30+40},
				StaticText {
					disabled,
					"URL:"
				},
				{27+30, 30+40+10, 27+30+16, 30+40+10+250},
				EditText {
					enabled,
					""
				}
			}
		};
	#elif	qPlatform_Windows
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








#if		qSupportAddNewTableDlg
	#if		qPlatform_MacOS
		resource 'DLOG' (kLedStdDlg_AddNewTableBoxID, purgeable) {
			{0, 0, 114, 313},
			movableDBoxProc,
			invisible,
			noGoAway,
			0x0,
			kLedStdDlg_AddNewTableBoxID,
			"Add New Table",
			alertPositionParentWindow
		};
		resource 'dlgx' (kLedStdDlg_AddNewTableBoxID, purgeable) {
			versionZero {kDialogFlagsHandleMovableModal }
		};
		resource 'DITL' (kLedStdDlg_AddNewTableBoxID, purgeable) {
			{
				{72, 206, 72+20, 206+80},
				Button {
					enabled,
					"OK"
				},
				{72, 113, 72+20, 113+80},
				Button {
					enabled,
					"Cancel"
				},
				{11, 18, 11+16, 18+140},
				StaticText {
					disabled,
					"Insert a new table:"
				},
				{38, 38, 38+16, 38+50},
				StaticText {
					disabled,
					"Rows:"
				},
				{38, 38+50, 38+16, 38+50+40},
				EditText {
					enabled,
					""
				},
				{38, 140, 38+16, 140+60},
				StaticText {
					disabled,
					"Columns:"
				},
				{38, 210, 38+16, 210+40},
				EditText {
					enabled,
					""
				}
			}
		};
	#elif	qPlatform_Windows
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







#if		qSupportEditTablePropertiesDlg
	#if		qPlatform_MacOS
		resource 'DLOG' (kLedStdDlg_EditTablePropertiesBoxID, purgeable) {
			{0, 0, 290, 466},
			movableDBoxProc,
			invisible,
			noGoAway,
			0x0,
			kLedStdDlg_EditTablePropertiesBoxID,
			"Edit Table Properties",
			alertPositionParentWindow
		};
		resource 'dlgx' (kLedStdDlg_EditTablePropertiesBoxID, purgeable) {
			versionZero {kDialogFlagsHandleMovableModal }
		};
		resource 'MENU' (kLedStdDlg_EditTablePropertiesBox_BorderColorCNTLID) {
			kLedStdDlg_EditTablePropertiesBox_BorderColorCNTLID,
			textMenuProc,
			0xFFFFFFFF,
			enabled,
			"Color",
			{
			}
		};
		resource 'CNTL' (kLedStdDlg_EditTablePropertiesBox_BorderColorCNTLID) {
			{0, 0, 20, 140},
			0,
			visible,
			60,
			kLedStdDlg_EditTablePropertiesBox_BorderColorCNTLID,
			popupMenuCDEFproc,
			0,
			"Color:"
		};
		resource 'MENU' (kLedStdDlg_EditTablePropertiesBox_CellBackgroundColorCNTLID) {
			kLedStdDlg_EditTablePropertiesBox_CellBackgroundColorCNTLID,
			textMenuProc,
			0xFFFFFFFF,
			enabled,
			"Color",
			{
			}
		};
		resource 'CNTL' (kLedStdDlg_EditTablePropertiesBox_CellBackgroundColorCNTLID) {
			{0, 0, 20, 205},
			0,
			visible,
			125,
			kLedStdDlg_EditTablePropertiesBox_CellBackgroundColorCNTLID,
			popupMenuCDEFproc,
			0,
			"Background Color:"
		};
		resource 'DITL' (kLedStdDlg_EditTablePropertiesBoxID, purgeable) {
			{
				{240, 365, 240+20, 365+80},
				Button {
					enabled,
					"OK"
				},
				{240, 265, 240+20, 265+80},
				Button {
					enabled,
					"Cancel"
				},


				{11, 18, 11+16, 18+140},
				StaticText {
					disabled,
					"Table"
				},

				{31, 28, 31+16, 28+140},
				StaticText {
					disabled,
					"Border"
				},
				{51, 48, 51+16, 48+50},
				StaticText {
					disabled,
					"Width:"
				},
				{51, 48+50, 51+16, 48+50+40},
				EditText {
					enabled,
					""
				},
				{51, 225, 51+16, 225+200},
				Control {
					enabled,
					kLedStdDlg_EditTablePropertiesBox_BorderColorCNTLID
				},


				{80, 28, 80+16, 28+140},
				StaticText {
					disabled,
					"Cell Margins"
				},
				{105, 38, 105+16, 38+50},
				StaticText {
					disabled,
					"Top:"
				},
				{105, 38+50, 105+16, 38+50+40},
				EditText {
					enabled,
					""
				},
				{105, 140, 105+16, 140+40},
				StaticText {
					disabled,
					"Left:"
				},
				{105, 140+40, 105+16, 140+40+40},
				EditText {
					enabled,
					""
				},
				{105, 238, 105+16, 238+60},
				StaticText {
					disabled,
					"Bottom:"
				},
				{105, 238+60, 105+16, 238+60+40},
				EditText {
					enabled,
					""
				},
				{105, 350, 105+16, 350+50},
				StaticText {
					disabled,
					"Right:"
				},
				{105, 350+50, 105+16, 350+50+40},
				EditText {
					enabled,
					""
				},

				{140, 28, 140+16, 28+100},
				StaticText {
					disabled,
					"Cell Spacing:"
				},
				{140, 28+100, 140+16, 28+100+40},
				EditText {
					enabled,
					""
				},


				{170, 18, 170+16, 18+140},
				StaticText {
					disabled,
					"Selected Columns"
				},
				{195, 38, 195+16, 38+50},
				StaticText {
					disabled,
					"Width:"
				},
				{195, 38+50, 195+16, 38+50+40},
				EditText {
					enabled,
					""
				},

				{170, 225, 170+16, 225+140},
				StaticText {
					disabled,
					"Selected Cells"
				},
				{195, 235, 195+16, 235+210},
				Control {
					enabled,
					kLedStdDlg_EditTablePropertiesBox_CellBackgroundColorCNTLID
				},

				{240, 25, 240+16, 25+200},
				StaticText {
					disabled,
					"(measurements in TWIPS)"
				},
			}
		};
	#elif	qPlatform_Windows
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







#if		qSupportStdSpellCheckDlg
	#if		qPlatform_MacOS
		resource 'DLOG' (kLedStdDlg_SpellCheckBoxID) {
			{0, 0, 194, 520},
			movableDBoxProc,
			invisible,
			goAway,
			0x0,
			kLedStdDlg_SpellCheckBoxID,
			"Check Spelling",
			alertPositionParentWindow
		};
		resource 'dlgx' (kLedStdDlg_SpellCheckBoxID, purgeable) {
			versionZero {kDialogFlagsHandleMovableModal }
		};
		resource 'CNTL' (kLedStdDlg_SpellCheckBox_SuggestionsCNTLID) {
			{0, 0, 20, 140},
			0,
			visible,
			60,
			kLedStdDlg_SpellCheckBox_SuggestionsCNTLID,
			popupMenuCDEFproc,
			0,
			"Color:"
		};
		resource 'DITL' (kLedStdDlg_SpellCheckBoxID) {
			{
				{17, 130, 17+16, 304},
				EditText {
					disabled,
					""
				},
				{43, 130, 43+16, 304},
				EditText {
					disabled,
					""
				},
				{70, 130, 70+80, 304},
				Control {
					enabled,
					kLedStdDlg_SpellCheckBox_SuggestionsCNTLID
				},
				{17, 320, 17+20, 320+90},
				Button {
					enabled,
					"Ignore"
				},
				{17, 420, 17+20, 420+90},
				Button {
					enabled,
					"Ignore All"
				},
				{45, 320, 45+20, 320+90},
				Button {
					enabled,
					"Change"
				},
				{45, 420, 45+20, 420+90},
				Button {
					enabled,
					"Change All"
				},
				{75, 345, 75+20, 360+120},
				Button {
					enabled,
					"Add to Dictionary"
				},
				{103, 345, 103+20, 360+120},
				Button {
					enabled,
					"Lookup on Web"
				},
				{131, 345, 131+20, 360+120},
				Button {
					enabled,
					"Options�"
				},
				{159, 345, 159+20, 360+120},
				Button {
					enabled,
					"Close"
				},
				{18, 22, 34, 120},
				StaticText {
					disabled,
					"Unknown Word:"
				},
				{44, 22, 60, 120},
				StaticText {
					disabled,
					"Change To:"
				},
				{70, 22, 86, 120},
				StaticText {
					disabled,
					"Suggestions:"
				},
			}
		};
	#elif	qPlatform_Windows
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
