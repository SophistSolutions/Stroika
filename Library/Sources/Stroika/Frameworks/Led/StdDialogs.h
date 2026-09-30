/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef __Led_StdDialogs__h__
#define __Led_StdDialogs__h__ 1

/*
@MODULE:    LedStdDialogs
@DESCRIPTION:
        <p>Pre-canned (though subclassable) dialogs which tend to come in handy in Led-based applications.
    These are not always available for all platforms. Also - some minimal effort is made to make it a bit
    easier to do somewhat platoform-independent dialog code (very minimal support for this as of Led 3.0).
    </p>
 */

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Frameworks/Led/Config.h"

#if defined(__cplusplus)
#include "Stroika/Frameworks/Led/GDI.h"
#include "Stroika/Frameworks/Led/Support.h"
#endif

/*
 *  The Windows resource compiler (rc.exe) also reads this header (for the dialog templates in StdDialogs.inc.r), and it
 *  truncates identifiers to 31 characters - so the #if tests here use the qStroika_Platform_ names, which are short
 *  enough for it (see Design-Overview.md "Macro names").
 */

// MFC must define something like this someplace, but I haven't found where....
// Use this for now, so I can update things more easily when I find the MFC definition...
#ifndef kLedStdDlgCommandBase
#if qStroika_Platform_Windows
#define kLedStdDlgCommandBase 0x1000
#endif
#endif

// NOTE - due to quirks of MSDEV .rc compiler - you CANNOT change this #. If you do - it will be out of sync with
// kLedStdDlg_AboutBoxID etc below. You cannot make kLedStdDlg_AboutBoxID etc below be based on this - or the .rc compiler
// will silently - but surely - generate the wrong resids. Amazing but true stories -- LGP 2000-10-16
#define kLedStdDlgIDBase 0x1000

#if qStroika_Platform_Windows && defined(__cplusplus)
#include "Stroika/Frameworks/Led/Platform/Windows.h"
#include "Stroika/Frameworks/Led/SimpleTextInteractor.h"
#include "Stroika/Frameworks/Led/SimpleTextStore.h"
#endif

#if defined(__cplusplus)
namespace Stroika::Frameworks::Led {
#endif

#if defined(__cplusplus)
    class StdColorPopupHelper {
    public:
        StdColorPopupHelper (bool allowNone);
        ~StdColorPopupHelper ();

    public:
        nonvirtual bool GetSelectedColor (Color* c) const;
        nonvirtual void SetSelectedColor (const Color& c);
        nonvirtual void SetNoSelectedColor ();

    private:
        bool  fIsSelectedColor;
        Color fSelectedColor{Color::kBlack};

    private:
        nonvirtual size_t MapColorIdx (const Color& c) const;
        nonvirtual Color  MapColorIdx (size_t i) const;

    private:
        bool fAllowNone;

#if qStroika_Platform_Windows
    public:
        nonvirtual void Attach (HWND popup);

    private:
        HWND fHWnd;
#endif

#if qStroika_Platform_Windows
    public:
        nonvirtual void OnSelChange ();
#endif
#if qStroika_Platform_Windows
    private:
        nonvirtual void DoMenuAppends ();
        nonvirtual void AppendMenuString (const SDKString& s);
#endif
    };
#endif

#if qStroika_Platform_Windows && defined(__cplusplus)

    namespace LedDialogWidget_Private {
        using LedDialogWidget_BASE = Platform::Led_Win32_SimpleWndProc_HelperWithSDKMessages<Platform::Led_Win32_Helper<SimpleTextInteractor>>;
    }

    DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning
    class LedDialogWidget : public LedDialogWidget_Private::LedDialogWidget_BASE {
    private:
        using inherited = LedDialogWidget_Private::LedDialogWidget_BASE;

    public:
        LedDialogWidget ();

    protected:
        /**
         *  Like the default CTOR, but does not SpecifyTextStore (&fTextStore) - for a subclass that specifies its own.
         */
        enum DerivedClassSpecifiesTextStore {
            eDerivedClassSpecifiesTextStore
        };
        LedDialogWidget (DerivedClassSpecifiesTextStore);

    public:
        virtual ~LedDialogWidget ();

    public:
        virtual void OnBadUserInput () override;

    public:
        virtual void OnTypedNormalCharacter (Led_tChar theChar, bool /*optionPressed*/, bool /*shiftPressed*/, bool /*commandPressed*/,
                                             bool      controlPressed, bool /*altKeyPressed*/) override;

    protected:
        virtual CommandNumber CharToCommand (Led_tChar theChar) const;

    public:
        nonvirtual Led_tString GetText () const;
        nonvirtual void        SetText (const Led_tString& t);

    public:
        SimpleTextStore          fTextStore;
        SingleUndoCommandHandler fCommandHandler;
    };
    DISABLE_COMPILER_MSC_WARNING_END (4250)

#endif

#if qStroika_Platform_Windows && defined(__cplusplus)
    /*
            @CLASS:         LedComboBoxWidget
            @DESCRIPTION:   <p></p>
            */
    class LedComboBoxWidget
#if qStroika_Platform_Windows
        : public Platform::SimpleWin32WndProcHelper
#endif
    {
#if qStroika_Platform_Windows
    private:
        using inherited = Platform::SimpleWin32WndProcHelper;
#endif

    public:
        LedComboBoxWidget ();
        virtual ~LedComboBoxWidget ();

#if qStroika_Platform_Windows
    public:
        nonvirtual bool ReplaceWindow (HWND hWnd);
#endif

    public:
        nonvirtual Led_tString GetText () const;
        nonvirtual void        SetText (const Led_tString& t);

    public:
        nonvirtual vector<Led_tString> GetPopupItems () const;
        nonvirtual void                SetPopupItems (const vector<Led_tString>& pi);

    private:
        vector<Led_tString> fPopupItems;

#if qStroika_Platform_Windows
    public:
        virtual LRESULT WndProc (UINT message, WPARAM wParam, LPARAM lParam) override;

    protected:
        virtual LRESULT OnCreate_Msg (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnSize_Msg (WPARAM wParam, LPARAM lParam);
#endif

#if qStroika_Platform_Windows
    protected:
        struct MyButton : public SimpleWin32WndProcHelper {
            using inherited = SimpleWin32WndProcHelper;

            MyButton ();
            virtual LRESULT WndProc (UINT message, WPARAM wParam, LPARAM lParam) override;

            LedComboBoxWidget* fComboBox;
            Bitmap             fDropDownArrow;
        };
        friend struct MyButton;
#endif

#if qStroika_Platform_Windows
    protected:
        struct MyComboListBoxPopup : public SimpleWin32WndProcHelper {
            using inherited = SimpleWin32WndProcHelper;

            MyComboListBoxPopup ();
            virtual LRESULT WndProc (UINT message, WPARAM wParam, LPARAM lParam) override;
            nonvirtual void UpdatePopupItems ();
            nonvirtual void MadeSelection ();
            nonvirtual void ComputePreferedHeight (DistanceType* prefHeight, size_t* nEltsShown) const;

            LedComboBoxWidget* fComboBox;
        };
        friend struct MyComboListBoxPopup;
#endif

#if qStroika_Platform_Windows
    protected:
        DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning
        struct MyTextWidget : public LedDialogWidget {
            using inherited = LedDialogWidget;

            MyTextWidget ();
            ~MyTextWidget ();
            virtual LRESULT WndProc (UINT message, WPARAM wParam, LPARAM lParam) override;

            LedComboBoxWidget* fComboBox;
        };
        DISABLE_COMPILER_MSC_WARNING_END (4250)
        friend struct MyTextWidget;
#endif

    protected:
        nonvirtual void ShowPopup ();
        nonvirtual void HidePopup ();
        nonvirtual bool PopupShown () const;
        nonvirtual void TogglePopupShown ();

    private:
#if qStroika_Platform_Windows
        MyButton            fPopupButton;
        MyComboListBoxPopup fComboListBoxPopup;
        MyTextWidget        fTextWidget;
        FontObject*         fUseWidgetFont;
#endif
    };
#endif

#if defined(__cplusplus)
    /*
            @CLASS:         Led_StdDialogHelper
            @DESCRIPTION:   <p>A very minimalistic attempt at providing cross-platform dialog support.</p>
            */
    class Led_StdDialogHelper {
    public:
#if qStroika_Platform_Windows
        Led_StdDialogHelper (HINSTANCE hInstance, const SDKChar* resID, HWND parentWnd);
#endif

    public:
        virtual ~Led_StdDialogHelper ();

    public:
        virtual bool DoModal ();

    public:
        nonvirtual void ReplaceAllTokens (SDKString* m, const SDKString& token, const SDKString& with);

    protected:
        virtual void PreDoModalHook ();

    private:
#if qStroika_Platform_Windows
        HINSTANCE      fHINSTANCE;
        const SDKChar* fResID; // not a REAL string  - fake one for MAKEINTRESOURCE - which is why we don't copy with 'string' class
        HWND           fParentWnd;
#endif

    public:
        virtual void OnOK ();
        virtual void OnCancel ();

    public:
        nonvirtual bool GetWasOK () const;

    private:
        bool fWasOK;

        /*
                 *  Some cross-platform portability helper functions.
                 */
    public:
#if qStroika_Platform_Windows
        using DialogItemID = int;
#endif
    public:
#if qStroika_Platform_Windows
        nonvirtual SDKString GetItemText (DialogItemID itemID) const;
        nonvirtual void      SetItemText (DialogItemID itemID, const SDKString& text);
        nonvirtual void      SelectItemText (DialogItemID itemID, size_t from = 0, size_t to = static_cast<size_t> (-1));
        nonvirtual bool      GetItemChecked (DialogItemID itemID) const;
        nonvirtual void      SetItemChecked (DialogItemID itemID, bool checked);
        nonvirtual bool      GetItemEnabled (DialogItemID itemID) const;
        nonvirtual void      SetItemEnabled (DialogItemID itemID, bool enabled);
        nonvirtual void      SetFocusedItem (DialogItemID itemID);
#endif

    private:
#if qStroika_Platform_Windows
        bool fSetFocusItemCalled;
#endif

#if qStroika_Platform_Windows
    public:
        nonvirtual HWND GetHWND () const;
        nonvirtual void SetHWND (HWND hWnd);

    private:
        HWND fHWnd;
#endif

#if qStroika_Platform_Windows
    protected:
        virtual BOOL OnInitDialog ();
#endif

#if qStroika_Platform_Windows
    public:
        static BOOL CALLBACK StaticDialogProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    protected:
        virtual BOOL DialogProc (UINT message, WPARAM wParam, LPARAM lParam);
#endif
    };
#endif

#if qStroika_Platform_Windows
#define kLedStdDlg_AboutBoxID 0x1001
#define kLedStdDlg_AboutBox_InfoLedFieldID (kLedStdDlgCommandBase + 1)
#define kLedStdDlg_AboutBox_LedWebPageFieldID (kLedStdDlgCommandBase + 2)
#define kLedStdDlg_AboutBox_BigPictureFieldID (kLedStdDlgCommandBase + 3)
#define kLedStdDlg_AboutBox_VersionFieldID (kLedStdDlgCommandBase + 4)

#if defined(__cplusplus)
    class Led_StdDialogHelper_AboutBox : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qStroika_Platform_Windows
        Led_StdDialogHelper_AboutBox (HINSTANCE hInstance, HWND parentWnd, const SDKChar* resID = MAKEINTRESOURCE (kLedStdDlg_AboutBoxID));
#endif

    protected:
#if qStroika_Platform_Windows
        virtual BOOL DialogProc (UINT message, WPARAM wParam, LPARAM lParam) override;
#endif

    public:
        virtual void OnClickInInfoField ();
        virtual void OnClickInLedWebPageField ();
    };
#endif

#endif

#if qStroika_Platform_Windows
#define kLedStdDlg_FindBoxID 0x1002
#define kLedStdDlg_FindBox_FindText (kLedStdDlgCommandBase + 1)
#define kLedStdDlg_FindBox_WrapAtEndOfDoc (kLedStdDlgCommandBase + 2)
#define kLedStdDlg_FindBox_WholeWord (kLedStdDlgCommandBase + 3)
#define kLedStdDlg_FindBox_IgnoreCase (kLedStdDlgCommandBase + 4)
#define kLedStdDlg_FindBox_Find (kLedStdDlgCommandBase + 5)
#define kLedStdDlg_FindBox_Cancel (kLedStdDlgCommandBase + 6)

#if defined(__cplusplus)
    class Led_StdDialogHelper_FindDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qStroika_Platform_Windows
        Led_StdDialogHelper_FindDialog (HINSTANCE hInstance, HWND parentWnd, const SDKChar* resID = MAKEINTRESOURCE (kLedStdDlg_FindBoxID));
#endif

    protected:
#if qStroika_Platform_Windows
        virtual BOOL DialogProc (UINT message, WPARAM wParam, LPARAM lParam) override;
#endif

    public:
        Led_tString         fFindText;
        vector<Led_tString> fRecentFindTextStrings;
        bool                fWrapSearch;
        bool                fWholeWordSearch;
        bool                fCaseSensativeSearch;
        bool                fPressedOK;

    protected:
#if qStroika_Platform_Windows
        LedComboBoxWidget fFindTextWidget;
#endif

    protected:
        virtual void PreDoModalHook () override;

    public:
        virtual void OnFindButton ();
        virtual void OnDontFindButton ();
    };
#endif
#endif

#if qStroika_Platform_Windows
#define kLedStdDlg_ReplaceBoxID 0x1003
#define kLedStdDlg_ReplaceBox_FindText (kLedStdDlgCommandBase + 1)
#define kLedStdDlg_ReplaceBox_ReplaceText (kLedStdDlgCommandBase + 2)
#define kLedStdDlg_ReplaceBox_WrapAtEndOfDoc (kLedStdDlgCommandBase + 3)
#define kLedStdDlg_ReplaceBox_WholeWord (kLedStdDlgCommandBase + 4)
#define kLedStdDlg_ReplaceBox_IgnoreCase (kLedStdDlgCommandBase + 5)
#define kLedStdDlg_ReplaceBox_Find (kLedStdDlgCommandBase + 6)
#define kLedStdDlg_ReplaceBox_Cancel (kLedStdDlgCommandBase + 7)
#define kLedStdDlg_ReplaceBox_Replace (kLedStdDlgCommandBase + 8)
#define kLedStdDlg_ReplaceBox_ReplaceAll (kLedStdDlgCommandBase + 9)
#define kLedStdDlg_ReplaceBox_ReplaceAllInSelection (kLedStdDlgCommandBase + 10)

#if defined(__cplusplus)
    class Led_StdDialogHelper_ReplaceDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qStroika_Platform_Windows
        Led_StdDialogHelper_ReplaceDialog (HINSTANCE hInstance, HWND parentWnd, const SDKChar* resID = MAKEINTRESOURCE (kLedStdDlg_ReplaceBoxID));
#endif

    protected:
#if qStroika_Platform_Windows
        virtual BOOL DialogProc (UINT message, WPARAM wParam, LPARAM lParam) override;
#endif

    public:
        Led_tString         fFindText;
        vector<Led_tString> fRecentFindTextStrings;
        Led_tString         fReplaceText;
        bool                fWrapSearch;
        bool                fWholeWordSearch;
        bool                fCaseSensativeSearch;

    public:
        enum ButtonPressed {
            eCancel,
            eFind,
            eReplace,
            eReplaceAll,
            eReplaceAllInSelection
        };
        ButtonPressed fPressed;

    protected:
#if qStroika_Platform_Windows
        LedComboBoxWidget fFindTextWidget;
        LedDialogWidget   fReplaceTextWidget;
#endif

    protected:
        virtual void PreDoModalHook () override;

    public:
        virtual void OnFindButton ();
        virtual void OnReplaceButton ();
        virtual void OnReplaceAllButton ();
        virtual void OnReplaceAllInSelectionButton ();
        virtual void OnDontFindButton ();

    protected:
        virtual void SaveItems ();
    };
#endif
#endif

#if qStroika_Platform_Windows && defined(__cplusplus)
    /*
            @CLASS:         StdColorPickBox
            @DESCRIPTION:   <p>This doesn't even use @'Led_StdDialogHelper' - but
                        instead - uses other SDK calls to display the dialog.</p>
                            <p>You still invoke it with a DoModal () call and consider that OK was pressed and
                        the fColor field filled in if DoModal returns true.
                        </p>
            */
    class StdColorPickBox {
    public:
#if qStroika_Platform_Windows
        StdColorPickBox (const Color& initialColor);
        StdColorPickBox (HINSTANCE hInstance, HWND parentWnd, const Color& initialColor);
#endif

#if qStroika_Platform_Windows
    private:
        HWND fParentWnd;
#endif

#if qStroika_Platform_Windows
    public:
        virtual bool DoModal ();
#endif

#if qStroika_Platform_Windows
    private:
        static UINT_PTR CALLBACK ColorPickerINITPROC (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

    public:
        Color fColor;
    };
#endif

#if qStroika_Platform_Windows
#define kLedStdDlg_UpdateWin32FileAssocsDialogID 0x1004
#define kLedStdDlg_UpdateWin32FileAssocsDialog_Msg (kLedStdDlgCommandBase + 1)
#define kLedStdDlg_UpdateWin32FileAssocsDialog_KeepCheckingCheckboxMsg (kLedStdDlgCommandBase + 2)

#if defined(__cplusplus)
    /*
            @CLASS:         Led_StdDialogHelper_UpdateWin32FileAssocsDialog
            @DESCRIPTION:   <p>Windows only.</p>
            */
    class Led_StdDialogHelper_UpdateWin32FileAssocsDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
        Led_StdDialogHelper_UpdateWin32FileAssocsDialog (HINSTANCE hInstance, HWND parentWnd,
                                                         const SDKChar* resID = MAKEINTRESOURCE (kLedStdDlg_UpdateWin32FileAssocsDialogID));

    public:
        SDKString fAppName;
        SDKString fTypeList;
        bool      fKeepChecking;

    protected:
        virtual void PreDoModalHook () override;

    public:
        virtual void OnOK () override;
        virtual void OnCancel () override;
    };
#endif
#endif

#if qStroika_Platform_Windows
#define kLedStdDlg_ParagraphIndentsID 0x1005
#define kLedStdDlg_ParagraphIndents_LeftMarginFieldID (kLedStdDlgCommandBase + 4)
#define kLedStdDlg_ParagraphIndents_RightMarginFieldID (kLedStdDlgCommandBase + 6)
#define kLedStdDlg_ParagraphIndents_FirstIndentFieldID (kLedStdDlgCommandBase + 8)

#if defined(__cplusplus)
    class Led_StdDialogHelper_ParagraphIndentsDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qStroika_Platform_Windows
        Led_StdDialogHelper_ParagraphIndentsDialog (HINSTANCE hInstance, HWND parentWnd,
                                                    const SDKChar* resID = MAKEINTRESOURCE (kLedStdDlg_ParagraphIndentsID));
#endif

    public:
        virtual void InitValues (TWIPS leftMargin, bool leftMarginValid, TWIPS rightMargin, bool rightMarginValid, TWIPS firstIndent, bool firstIndentValid);

    protected:
        virtual void PreDoModalHook () override;

    public:
        virtual void OnOK () override;

    public:
        bool  fLeftMargin_Valid;
        TWIPS fLeftMargin_Orig;
        TWIPS fLeftMargin_Result;
        bool  fRightMargin_Valid;
        TWIPS fRightMargin_Orig;
        TWIPS fRightMargin_Result;
        bool  fFirstIndent_Valid;
        TWIPS fFirstIndent_Orig;
        TWIPS fFirstIndent_Result;
    };
#endif
#endif

#if qStroika_Platform_Windows
#define kLedStdDlg_ParagraphSpacingID 0x1006
#define kParagraphSpacing_Dialog_SpaceBeforeFieldID (kLedStdDlgCommandBase + 4)
#define kParagraphSpacing_Dialog_SpaceAfterFieldID (kLedStdDlgCommandBase + 6)
#define kParagraphSpacing_Dialog_LineSpaceModeFieldID (kLedStdDlgCommandBase + 7)
#define kParagraphSpacing_Dialog_LineSpaceArgFieldID (kLedStdDlgCommandBase + 8)

#if defined(__cplusplus)
    class Led_StdDialogHelper_ParagraphSpacingDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qStroika_Platform_Windows
        Led_StdDialogHelper_ParagraphSpacingDialog (HINSTANCE hInstance, HWND parentWnd,
                                                    const SDKChar* resID = MAKEINTRESOURCE (kLedStdDlg_ParagraphSpacingID));
#endif

    public:
        virtual void InitValues (TWIPS spaceBefore, bool spaceBeforeValid, TWIPS spaceAfter, bool spaceAfterValid, LineSpacing lineSpacing,
                                 bool lineSpacingValid);

    protected:
        virtual void PreDoModalHook () override;

    public:
        virtual void OnOK () override;

    public:
        bool        fSpaceBefore_Valid;
        TWIPS       fSpaceBefore_Orig;
        TWIPS       fSpaceBefore_Result;
        bool        fSpaceAfter_Valid;
        TWIPS       fSpaceAfter_Orig;
        TWIPS       fSpaceAfter_Result;
        bool        fLineSpacing_Valid;
        LineSpacing fLineSpacing_Orig;
        LineSpacing fLineSpacing_Result;
    };
#endif
#endif

#if qStroika_Platform_Windows
#define kLedStdDlg_OtherFontSizeID 0x1007
#define kOtherFontSize_Dialog_FontSizeEditFieldID (kLedStdDlgCommandBase + 4)

#if defined(__cplusplus)
    class Led_StdDialogHelper_OtherFontSizeDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qStroika_Platform_Windows
        Led_StdDialogHelper_OtherFontSizeDialog (HINSTANCE hInstance, HWND parentWnd, const SDKChar* resID = MAKEINTRESOURCE (kLedStdDlg_OtherFontSizeID));
#endif

    public:
        virtual void InitValues (DistanceType origFontSize);

    protected:
        virtual void PreDoModalHook () override;

    public:
        virtual void OnOK () override;

    public:
        DistanceType fFontSize_Orig;
        DistanceType fFontSize_Result;
    };
#endif
#endif

#if qStroika_Platform_Windows
#define kLedStdDlg_UnknownEmbeddingInfoBoxID 0x1008
#define kLedStdDlg_UnknownEmbeddingInfoBox_TypeTextMsg (kLedStdDlgCommandBase + 1)

#if defined(__cplusplus)
    class Led_StdDialogHelper_UnknownEmbeddingInfoDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qStroika_Platform_Windows
        Led_StdDialogHelper_UnknownEmbeddingInfoDialog (HINSTANCE hInstance, HWND parentWnd,
                                                        const SDKChar* resID = MAKEINTRESOURCE (kLedStdDlg_UnknownEmbeddingInfoBoxID));
#endif

    public:
        SDKString fEmbeddingTypeName;

    protected:
        virtual void PreDoModalHook () override;
    };
#endif
#endif

#if qStroika_Platform_Windows
#define kLedStdDlg_URLXEmbeddingInfoBoxID 0x1009
#define kLedStdDlg_URLXEmbeddingInfoBox_TypeTextMsg (kLedStdDlgCommandBase + 1)
#define kLedStdDlg_URLXEmbeddingInfoBox_TitleText (kLedStdDlgCommandBase + 4)
#define kLedStdDlg_URLXEmbeddingInfoBox_URLText (kLedStdDlgCommandBase + 6)

#if defined(__cplusplus)
    class Led_StdDialogHelper_URLXEmbeddingInfoDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qStroika_Platform_Windows
        Led_StdDialogHelper_URLXEmbeddingInfoDialog (HINSTANCE hInstance, HWND parentWnd,
                                                     const SDKChar* resID = MAKEINTRESOURCE (kLedStdDlg_URLXEmbeddingInfoBoxID));
#endif

    public:
        SDKString fEmbeddingTypeName;
        SDKString fTitleText;
        SDKString fURLText;

    protected:
        virtual void PreDoModalHook () override;

    public:
        virtual void OnOK () override;
    };
#endif

#define kLedStdDlg_AddURLXEmbeddingInfoBoxID 0x100a
#define kLedStdDlg_AddURLXEmbeddingInfoBox_TitleText (kLedStdDlgCommandBase + 4)
#define kLedStdDlg_AddURLXEmbeddingInfoBox_URLText (kLedStdDlgCommandBase + 6)

#if defined(__cplusplus)
    class Led_StdDialogHelper_AddURLXEmbeddingInfoDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qStroika_Platform_Windows
        Led_StdDialogHelper_AddURLXEmbeddingInfoDialog (HINSTANCE hInstance, HWND parentWnd,
                                                        const SDKChar* resID = MAKEINTRESOURCE (kLedStdDlg_AddURLXEmbeddingInfoBoxID));
#endif

    public:
        SDKString fTitleText;
        SDKString fURLText;

    protected:
        virtual void PreDoModalHook () override;

    public:
        virtual void OnOK () override;
    };
#endif
#endif

#if qStroika_Platform_Windows
#define kLedStdDlg_AddNewTableBoxID 0x100b
#define kLedStdDlg_AddNewTableBox_RowCount (kLedStdDlgCommandBase + 5)
#define kLedStdDlg_AddNewTableBox_ColCount (kLedStdDlgCommandBase + 7)

#if defined(__cplusplus)
    class Led_StdDialogHelper_AddNewTableDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qStroika_Platform_Windows
        Led_StdDialogHelper_AddNewTableDialog (HINSTANCE hInstance, HWND parentWnd, const SDKChar* resID = MAKEINTRESOURCE (kLedStdDlg_AddNewTableBoxID));
#endif

    public:
        size_t fRows;
        size_t fColumns;

    protected:
        virtual void PreDoModalHook () override;

    public:
        virtual void OnOK () override;
    };
#endif
#endif

#if qStroika_Platform_Windows
#define kLedStdDlg_EditTablePropertiesBoxID 0x100c
#define kLedStdDlg_EditTablePropertiesBox_CellMarginTop (kLedStdDlgCommandBase + 10)
#define kLedStdDlg_EditTablePropertiesBox_CellMarginLeft (kLedStdDlgCommandBase + 12)
#define kLedStdDlg_EditTablePropertiesBox_CellMarginBottom (kLedStdDlgCommandBase + 14)
#define kLedStdDlg_EditTablePropertiesBox_CellMarginRight (kLedStdDlgCommandBase + 16)

#define kLedStdDlg_EditTablePropertiesBox_DefaultCellSpacing (kLedStdDlgCommandBase + 18)

#define kLedStdDlg_EditTablePropertiesBox_CellBackgroundColor (kLedStdDlgCommandBase + 23)

#define kLedStdDlg_EditTablePropertiesBox_ColumnWidth (kLedStdDlgCommandBase + 21)

#define kLedStdDlg_EditTablePropertiesBox_BorderWidth (kLedStdDlgCommandBase + 6)
#define kLedStdDlg_EditTablePropertiesBox_BorderColor (kLedStdDlgCommandBase + 7)

#if defined(__cplusplus)
    class Led_StdDialogHelper_EditTablePropertiesDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qStroika_Platform_Windows
        Led_StdDialogHelper_EditTablePropertiesDialog (HINSTANCE hInstance, HWND parentWnd,
                                                       const SDKChar* resID = MAKEINTRESOURCE (kLedStdDlg_EditTablePropertiesBoxID));
#endif

    public:
        struct Info {
            Info ();

            TWIPS fTableBorderWidth;
            Color fTableBorderColor;

            TWIPS_Rect fDefaultCellMargins;
            TWIPS      fCellSpacing;

            bool  fCellWidth_Common;
            TWIPS fCellWidth;

            bool  fCellBackgroundColor_Common;
            Color fCellBackgroundColor;
        };
        Info fInfo;

    protected:
        virtual void PreDoModalHook () override;

    protected:
#if qStroika_Platform_Windows
        virtual BOOL DialogProc (UINT message, WPARAM wParam, LPARAM lParam) override;
#endif

    public:
        virtual void OnOK () override;

    protected:
        StdColorPopupHelper fBorderColorPopup;
        StdColorPopupHelper fCellBackgroundColorPopup;

    public:
        template <typename T1, typename T2>
        static void cvt (T1* o, const T2& i);
    };

    template <typename T1, typename T2>
    void Led_StdDialogHelper_EditTablePropertiesDialog::cvt (T1* o, const T2& i)
    {
        o->fTableBorderWidth           = i.fTableBorderWidth;
        o->fTableBorderColor           = i.fTableBorderColor;
        o->fDefaultCellMargins         = i.fDefaultCellMargins;
        o->fCellSpacing                = i.fCellSpacing;
        o->fCellWidth_Common           = i.fCellWidth_Common;
        o->fCellWidth                  = i.fCellWidth;
        o->fCellBackgroundColor_Common = i.fCellBackgroundColor_Common;
        o->fCellBackgroundColor        = i.fCellBackgroundColor;
    }
    inline Led_StdDialogHelper_EditTablePropertiesDialog::Info::Info ()
        : fTableBorderWidth (TWIPS{0})
        , fTableBorderColor (Color::kWhite)
        , fDefaultCellMargins ()
        , fCellSpacing (TWIPS{0})
        , fCellWidth_Common (false)
        , fCellWidth (TWIPS{0})
        , fCellBackgroundColor_Common (false)
        , fCellBackgroundColor (Color::kWhite)
    {
    }
#endif
#endif

#if qStroika_Platform_Windows
#define kLedStdDlg_SpellCheckBoxID 0x100d
#define kLedStdDlg_SpellCheckBox_UnknownWordText (kLedStdDlgCommandBase + 1)
#define kLedStdDlg_SpellCheckBox_ChangeText (kLedStdDlgCommandBase + 2)
#define kLedStdDlg_SpellCheckBox_SuggestedList (kLedStdDlgCommandBase + 3)
#define kLedStdDlg_SpellCheckBox_IgnoreOnce (kLedStdDlgCommandBase + 4)
#define kLedStdDlg_SpellCheckBox_IgnoreAll (kLedStdDlgCommandBase + 5)
#define kLedStdDlg_SpellCheckBox_ChangeOnce (kLedStdDlgCommandBase + 6)
#define kLedStdDlg_SpellCheckBox_ChangeAll (kLedStdDlgCommandBase + 7)
#define kLedStdDlg_SpellCheckBox_AddDictionary (kLedStdDlgCommandBase + 8)
#define kLedStdDlg_SpellCheckBox_LookupOnWeb (kLedStdDlgCommandBase + 9)
#define kLedStdDlg_SpellCheckBox_Options (kLedStdDlgCommandBase + 10)
#define kLedStdDlg_SpellCheckBox_Close (kLedStdDlgCommandBase + 11)

#if defined(__cplusplus)
    class Led_StdDialogHelper_SpellCheckDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
        class SpellCheckDialogCallback;
        struct MisspellingInfo;

    public:
#if qStroika_Platform_Windows
        Led_StdDialogHelper_SpellCheckDialog (SpellCheckDialogCallback& callback, HINSTANCE hInstance, HWND parentWnd,
                                              const SDKChar* resID = MAKEINTRESOURCE (kLedStdDlg_SpellCheckBoxID));
#endif
        ~Led_StdDialogHelper_SpellCheckDialog ();

    protected:
        SpellCheckDialogCallback& fCallback;

    protected:
        MisspellingInfo* fCurrentMisspellInfo;

    protected:
#if qStroika_Platform_Windows
        virtual BOOL DialogProc (UINT message, WPARAM wParam, LPARAM lParam) override;
#endif

    protected:
#if qStroika_Platform_Windows
        LedDialogWidget fUndefinedWordWidget;
        LedDialogWidget fChangeTextWidget;
#endif

    protected:
        virtual void PreDoModalHook () override;

    public:
        virtual void OnIgnoreButton ();
        virtual void OnIgnoreAllButton ();
        virtual void OnChangeButton ();
        virtual void OnChangeAllButton ();
        virtual void OnAddToDictionaryButton ();
        virtual void OnLookupOnWebButton ();
        virtual void OnOptionsDialogButton ();
        virtual void OnCloseButton ();
        virtual void OnSuggestionListChangeSelection ();
        virtual void OnSuggestionListDoubleClick ();

    protected:
        virtual void DoFindNextCall ();

    public:
        template <typename DEL>
        class CallbackDelegator;
    };

    struct Led_StdDialogHelper_SpellCheckDialog::MisspellingInfo {
        Led_tString         fUndefinedWord;
        vector<Led_tString> fSuggestions;
    };

    /*
            @CLASS:         Led_StdDialogHelper_SpellCheckDialog::SpellCheckDialogCallback
            @DESCRIPTION:   <p>This interface is called by the actual spellcheck dialog implematation back to the implementer
                        of the real spellchecking functionality.</p>
            */
    class Led_StdDialogHelper_SpellCheckDialog::SpellCheckDialogCallback {
    public:
        using MisspellingInfo = Led_StdDialogHelper_SpellCheckDialog::MisspellingInfo;

    public:
        virtual MisspellingInfo* GetNextMisspelling () = 0;

    public:
        virtual void DoIgnore ()                                  = 0;
        virtual void DoIgnoreAll ()                               = 0;
        virtual void DoChange (const Led_tString& changeTo)       = 0;
        virtual void DoChangeAll (const Led_tString& changeTo)    = 0;
        virtual bool AddToDictionaryEnabled () const              = 0;
        virtual void AddToDictionary (const Led_tString& newWord) = 0;
        virtual void LookupOnWeb (const Led_tString& word)        = 0;
        virtual bool OptionsDialogEnabled () const                = 0;
        virtual void OptionsDialog ()                             = 0;
    };

    template <typename DEL>
    class Led_StdDialogHelper_SpellCheckDialog::CallbackDelegator : public Led_StdDialogHelper_SpellCheckDialog::SpellCheckDialogCallback {
    private:
        using inherited = SpellCheckDialogCallback;

    public:
        CallbackDelegator (DEL& del)
            : inherited ()
            , fDelegate (del)
        {
        }

    public:
        virtual MisspellingInfo* GetNextMisspelling () override
        {
            typename DEL::MisspellingInfo* delResult = fDelegate.GetNextMisspelling ();
            if (delResult != NULL) {
                MisspellingInfo* result = new MisspellingInfo;
                result->fUndefinedWord  = delResult->fUndefinedWord;
                result->fSuggestions    = delResult->fSuggestions;
                delete delResult;
                return result;
            }
            return NULL;
        }

    public:
        virtual void DoIgnore () override
        {
            fDelegate.DoIgnore ();
        }
        virtual void DoIgnoreAll () override
        {
            fDelegate.DoIgnoreAll ();
        }
        virtual void DoChange (const Led_tString& changeTo) override
        {
            fDelegate.DoChange (changeTo);
        }
        virtual void DoChangeAll (const Led_tString& changeTo) override
        {
            fDelegate.DoChangeAll (changeTo);
        }
        virtual bool AddToDictionaryEnabled () const override
        {
            return fDelegate.AddToDictionaryEnabled ();
        }
        virtual void AddToDictionary (const Led_tString& newWord) override
        {
            fDelegate.AddToDictionary (newWord);
        }
        virtual void LookupOnWeb (const Led_tString& word) override
        {
            fDelegate.LookupOnWeb (word);
        }
        virtual bool OptionsDialogEnabled () const override
        {
            return fDelegate.OptionsDialogEnabled ();
        }
        virtual void OptionsDialog () override
        {
            fDelegate.OptionsDialog ();
        }

    private:
        DEL& fDelegate;
    };
#endif
#endif
#if defined(__cplusplus)
}
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StdDialogs.inl"

#endif /*__Led_StdDialogs__h__*/
