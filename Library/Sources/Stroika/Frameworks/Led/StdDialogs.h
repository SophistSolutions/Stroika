/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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

#include "../../Foundation/StroikaPreComp.h"

#include "Config.h"

#if defined(__cplusplus)
#include "GDI.h"
#include "Support.h"
#endif

#if qPlatform_MacOS && defined(__cplusplus)
#include <Controls.h>
#endif

/*
@CONFIGVAR:     qUseGTKForLedStandardDialogs
@DESCRIPTION:   <p>Generally we try to keep toolkit dependencies restricted to the Led_TOOLKIT module(s), such as Led_Gtk.
            Thats pretty well doable where there is decent native dialog support, such as with Win32 and MacOS. But this isn't
            very practical for X-Windows. So - I've added a Gtk-dependency to this module as well - just for implementing
            these standard dialogs.</p>
                <p>This flag defaults to true, iff qXWindows is true.</p>
 */
#ifndef qUseGTKForLedStandardDialogs
#define qUseGTKForLedStandardDialogs qXWindows
#endif

#if qUseGTKForLedStandardDialogs
#include <gtk/gtk.h>
#endif

// MFC must define something like this someplace, but I haven't found where....
// Use this for now, so I can update things more easily when I find the MFC definition...
#ifndef kLedStdDlgCommandBase
#if qPlatform_MacOS
#define kLedStdDlgCommandBase 0x0
#elif qPlatform_Windows
#define kLedStdDlgCommandBase 0x1000
#endif
#endif

// NOTE - due to quirks of MSDEV .rc compiler - you CANNOT change this #. If you do - it will be out of sync with
// kLedStdDlg_AboutBoxID etc below. You cannot make kLedStdDlg_AboutBoxID etc below be based on this - or the .rc compiler
// will silently - but surely - generate the wrong resids. Amazing but true stories -- LGP 2000-10-16
#define kLedStdDlgIDBase 0x1000

/*
@CONFIGVAR:     qSupportLedDialogWidgets
@DESCRIPTION:   <p>Support the @'LedDialogWidget' class. This requires you link with SimpleTextStore.cpp,
            SimpleTextImager.cpp, and SimpleTextInteractor.cpp.</p>
                <p>This flag defaults to true, iff qPlatform_Windows is true.</p>
 */
#ifndef qSupportLedDialogWidgets
#define qSupportLedDialogWidgets (qPlatform_Windows)
#endif

#if qSupportLedDialogWidgets && qPlatform_Windows && defined(__cplusplus)
#include "Platform/Windows.h"
#include "SimpleTextInteractor.h"
#include "SimpleTextStore.h"
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
        nonvirtual bool GetSelectedColor (Led_Color* c) const;
        nonvirtual void SetSelectedColor (const Led_Color& c);
        nonvirtual void SetNoSelectedColor ();

    private:
        bool      fIsSelectedColor;
        Led_Color fSelectedColor;

    private:
        nonvirtual size_t MapColorIdx (const Led_Color& c) const;
        nonvirtual Led_Color MapColorIdx (size_t i) const;

    private:
        bool fAllowNone;

#if qPlatform_MacOS
    public:
        nonvirtual void Attach (ControlRef popup);

    private:
        ControlRef fControl;
#elif qPlatform_Windows
    public:
        nonvirtual void Attach (HWND popup);

    private:
        HWND fHWnd;
#endif

#if qPlatform_MacOS || qPlatform_Windows
    public:
        nonvirtual void OnSelChange ();
#endif
#if qPlatform_MacOS || qPlatform_Windows
    private:
        nonvirtual void DoMenuAppends ();
        nonvirtual void AppendMenuString (const Led_SDK_String& s);
#endif
    };
#endif

#if qSupportLedDialogWidgets && defined(__cplusplus)

    namespace LedDialogWidget_Private {
        using LedDialogWidget_BASE = Platform::Led_Win32_SimpleWndProc_HelperWithSDKMessages<Platform::Led_Win32_Helper<SimpleTextInteractor>>;
    }

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250) // inherits via dominance warning
#endif
    class LedDialogWidget : public LedDialogWidget_Private::LedDialogWidget_BASE {
    private:
        using inherited = LedDialogWidget_Private::LedDialogWidget_BASE;

    public:
        LedDialogWidget ();

    protected:
        enum TS_SET_OUTSIDE_BWA { eTS_SET_OUTSIDE_BWA }; /// FILL IN RIGHT LED_CONFIG BWA DEFINE FOR THIS CRAPOLA
        LedDialogWidget (TS_SET_OUTSIDE_BWA);

    public:
        virtual ~LedDialogWidget ();

    public:
        virtual void OnBadUserInput () override;

    public:
        virtual void OnTypedNormalCharacter (Led_tChar theChar, bool /*optionPressed*/, bool /*shiftPressed*/, bool /*commandPressed*/, bool controlPressed, bool /*altKeyPressed*/) override;

    protected:
        virtual CommandNumber CharToCommand (Led_tChar theChar) const;

    public:
        nonvirtual Led_tString GetText () const;
        nonvirtual void        SetText (const Led_tString& t);

    public:
        SimpleTextStore          fTextStore;
        SingleUndoCommandHandler fCommandHandler;
    };
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif

#endif

#if qSupportLedDialogWidgets && defined(__cplusplus)
    /*
            @CLASS:         LedComboBoxWidget
            @DESCRIPTION:   <p></p>
            */
    class LedComboBoxWidget
#if qPlatform_Windows
        : public Platform::SimpleWin32WndProcHelper
#endif
    {
#if qPlatform_Windows
    private:
        using inherited = Platform::SimpleWin32WndProcHelper;
#endif

    public:
        LedComboBoxWidget ();
        virtual ~LedComboBoxWidget ();

#if qPlatform_Windows
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

#if qPlatform_Windows
    public:
        virtual LRESULT WndProc (UINT message, WPARAM wParam, LPARAM lParam) override;

    protected:
        virtual LRESULT OnCreate_Msg (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnSize_Msg (WPARAM wParam, LPARAM lParam);
#endif

#if qPlatform_Windows
    protected:
        struct MyButton : public SimpleWin32WndProcHelper {
            using inherited = SimpleWin32WndProcHelper;

            MyButton ();
            virtual LRESULT WndProc (UINT message, WPARAM wParam, LPARAM lParam) override;

            LedComboBoxWidget* fComboBox;
            Led_Bitmap         fDropDownArrow;
        };
        friend struct MyButton;
#endif

#if qPlatform_Windows
    protected:
        struct MyComboListBoxPopup : public SimpleWin32WndProcHelper {
            using inherited = SimpleWin32WndProcHelper;

            MyComboListBoxPopup ();
            virtual LRESULT WndProc (UINT message, WPARAM wParam, LPARAM lParam) override;
            nonvirtual void UpdatePopupItems ();
            nonvirtual void MadeSelection ();
            nonvirtual void ComputePreferedHeight (Led_Distance* prefHeight, size_t* nEltsShown) const;

            LedComboBoxWidget* fComboBox;
        };
        friend struct MyComboListBoxPopup;
#endif

#if qPlatform_Windows
    protected:
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250) // inherits via dominance warning
#endif
        struct MyTextWidget : public LedDialogWidget {
            using inherited = LedDialogWidget;

            MyTextWidget ();
            ~MyTextWidget ();
            virtual LRESULT WndProc (UINT message, WPARAM wParam, LPARAM lParam) override;

            LedComboBoxWidget* fComboBox;
        };
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif
        friend struct MyTextWidget;
#endif

    protected:
        nonvirtual void ShowPopup ();
        nonvirtual void HidePopup ();
        nonvirtual bool PopupShown () const;
        nonvirtual void TogglePopupShown ();

    private:
#if qPlatform_Windows
        MyButton            fPopupButton;
        MyComboListBoxPopup fComboListBoxPopup;
        MyTextWidget        fTextWidget;
        Led_FontObject*     fUseWidgetFont;
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
#if qPlatform_MacOS
        Led_StdDialogHelper (int resID);
#elif qPlatform_Windows
        Led_StdDialogHelper (HINSTANCE hInstance, const Led_SDK_Char* resID, HWND parentWnd);
#elif (qXWindows && qUseGTKForLedStandardDialogs)
        Led_StdDialogHelper (GtkWindow* parentWindow);
#endif

    public:
        virtual ~Led_StdDialogHelper ();

    public:
        virtual bool DoModal ();

    public:
        nonvirtual void ReplaceAllTokens (Led_SDK_String* m, const Led_SDK_String& token, const Led_SDK_String& with);

    protected:
#if (qXWindows && qUseGTKForLedStandardDialogs)
        virtual GtkWidget* MakeWindow ();
#endif
        virtual void PreDoModalHook ();

#if qPlatform_MacOS
    protected:
        virtual bool HandleCommandClick (int itemNum);
#endif

    public:
#if qPlatform_MacOS
        nonvirtual int GetResID () const
        {
            return fResID;
        }
#endif

    private:
#if qPlatform_MacOS
        int  fResID;
        bool fDialogClosing;
#elif qPlatform_Windows
        HINSTANCE           fHINSTANCE;
        const Led_SDK_Char* fResID; // not a REAL string  - fake one for MAKEINTRESOURCE - which is why we don't copy with 'string' class
        HWND                fParentWnd;
#elif qXWindows && qUseGTKForLedStandardDialogs
        GtkWindow* fParentWindow;
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
#if qPlatform_MacOS || qPlatform_Windows
        using DialogItemID = int;
#elif qXWindows && qUseGTKForLedStandardDialogs
        using DialogItemID = GtkWidget*;
#endif
    public:
#if qPlatform_MacOS || qPlatform_Windows || (qXWindows && qUseGTKForLedStandardDialogs)
        nonvirtual Led_SDK_String GetItemText (DialogItemID itemID) const;
        nonvirtual void           SetItemText (DialogItemID itemID, const Led_SDK_String& text);
        nonvirtual void           SelectItemText (DialogItemID itemID, size_t from = 0, size_t to = static_cast<size_t> (-1));
        nonvirtual bool           GetItemChecked (DialogItemID itemID) const;
        nonvirtual void           SetItemChecked (DialogItemID itemID, bool checked);
        nonvirtual bool           GetItemEnabled (DialogItemID itemID) const;
        nonvirtual void           SetItemEnabled (DialogItemID itemID, bool enabled);
        nonvirtual void           SetFocusedItem (DialogItemID itemID);
#endif

    private:
#if qPlatform_Windows
        bool fSetFocusItemCalled;
#endif

#if qPlatform_MacOS
    public:
        nonvirtual DialogPtr GetDialogPtr () const;
        nonvirtual void      SetDialogPtr (DialogPtr d);

    private:
        DialogPtr fDialogPtr;
#elif qPlatform_Windows
    public:
        nonvirtual HWND GetHWND () const;
        nonvirtual void SetHWND (HWND hWnd);

    private:
        HWND fHWnd;
#elif qXWindows && qUseGTKForLedStandardDialogs
    public:
        nonvirtual GtkWidget* GetWindow () const;
        nonvirtual void       SetWindow (GtkWidget* w);

    private:
        GtkWidget* fWindow;

    public:
        nonvirtual GtkWidget* GetOKButton () const;
        nonvirtual void       SetOKButton (GtkWidget* okButton);

    private:
        GtkWidget* fOKButton;

    public:
        nonvirtual GtkWidget* GetCancelButton () const;
        nonvirtual void       SetCancelButton (GtkWidget* cancelButton);

    private:
        GtkWidget* fCancelButton;
#endif

#if qPlatform_MacOS
    protected:
        static pascal Boolean StaticEventFilter (DialogPtr dialog, EventRecord* eventRecord, short* itemHit);
        virtual bool          EventFilter (DialogPtr dialog, EventRecord* eventRecord, short* itemHit);
#endif

#if qPlatform_Windows
    protected:
        virtual BOOL OnInitDialog ();
#endif

#if qPlatform_Windows
    public:
        static BOOL CALLBACK StaticDialogProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    protected:
        virtual BOOL DialogProc (UINT message, WPARAM wParam, LPARAM lParam);
#endif

#if qXWindows && qUseGTKForLedStandardDialogs
    public:
        static void Static_OnOKButtonClick (GtkWidget* widget, gpointer data);
        static void Static_OnCancelButtonClick (GtkWidget* widget, gpointer data);
        static void Static_OnWindowDeleteRequest (GtkWidget* widget);
#endif
    };
#endif

#if qPlatform_MacOS
#if defined(__cplusplus)
    /*
            @CLASS:         Led_StdAlertHelper
            @DESCRIPTION:   <p>MacOS only</p>
            */
    class Led_StdAlertHelper : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
        Led_StdAlertHelper (int resID);

    public:
        virtual bool DoModal () override;
    };
#endif

#endif

#if qPlatform_MacOS
// Some dialogs require their own menus and CNTL resources
#ifndef kLedStdDlgMENUBase
#define kLedStdDlgMENUBase kLedStdDlgIDBase
#endif
#ifndef kLedStdDlgCNTLBase
#define kLedStdDlgCNTLBase kLedStdDlgIDBase
#endif
#endif

#ifndef qSupportStdAboutBoxDlg
#define qSupportStdAboutBoxDlg (qPlatform_MacOS || qPlatform_Windows || qUseGTKForLedStandardDialogs)
#endif

#if qSupportStdAboutBoxDlg
#define kLedStdDlg_AboutBoxID 0x1001
//NB: order of these must track declared order in Mac .r file!
#define kLedStdDlg_AboutBox_InfoLedFieldID (kLedStdDlgCommandBase + 1)
#define kLedStdDlg_AboutBox_LedWebPageFieldID (kLedStdDlgCommandBase + 2)
#define kLedStdDlg_AboutBox_BigPictureFieldID (kLedStdDlgCommandBase + 3)
#define kLedStdDlg_AboutBox_VersionFieldID (kLedStdDlgCommandBase + 4)

#if defined(__cplusplus)
    /*
            @CLASS:         Led_StdDialogHelper_AboutBox
            @DESCRIPTION:   <p>You can define qSupportStdAboutBoxDlg to 0 to disable inclusion of this resource/code (for size reasons).</p>
            */
    class Led_StdDialogHelper_AboutBox : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qPlatform_MacOS
        Led_StdDialogHelper_AboutBox (int resID = kLedStdDlg_AboutBoxID);
#elif qPlatform_Windows
        Led_StdDialogHelper_AboutBox (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_AboutBoxID));
#elif qXWindows && qUseGTKForLedStandardDialogs
        Led_StdDialogHelper_AboutBox (GtkWindow* parentWindow);
#endif

#if qPlatform_MacOS
    protected:
        virtual void PreDoModalHook () override;

        nonvirtual void SimpleLayoutHelper (short pictHeight, short pictWidth, Led_Rect infoField, Led_Rect webPageField, const Led_SDK_String versionStr);
#endif

#if qXWindows
        virtual GtkWidget* MakeWindow () override;
#endif

#if qPlatform_MacOS
    protected:
        virtual bool EventFilter (DialogPtr dialog, EventRecord* eventRecord, short* itemHit) override;
#endif

    protected:
#if qPlatform_MacOS
        virtual bool HandleCommandClick (int itemNum) override;
#elif qPlatform_Windows
        virtual BOOL DialogProc (UINT message, WPARAM wParam, LPARAM lParam) override;
#endif

    public:
        virtual void OnClickInInfoField ();
        virtual void OnClickInLedWebPageField ();
    };
#endif

#endif

#ifndef qSupportStdFindDlg
#define qSupportStdFindDlg qPlatform_MacOS || qPlatform_Windows || qXWindows
#endif

#if qSupportStdFindDlg
#define kLedStdDlg_FindBoxID 0x1002
#define kLedStdDlg_FindBox_FindText (kLedStdDlgCommandBase + 1)
#define kLedStdDlg_FindBox_WrapAtEndOfDoc (kLedStdDlgCommandBase + 2)
#define kLedStdDlg_FindBox_WholeWord (kLedStdDlgCommandBase + 3)
#define kLedStdDlg_FindBox_IgnoreCase (kLedStdDlgCommandBase + 4)
#define kLedStdDlg_FindBox_Find (kLedStdDlgCommandBase + 5)
#define kLedStdDlg_FindBox_Cancel (kLedStdDlgCommandBase + 6)

#if defined(__cplusplus)
    /*
            @CLASS:         Led_StdDialogHelper_FindDialog
            @DESCRIPTION:   <p>You can define qSupportStdFindDlg to 0 to disable inclusion of this resource/code (for size reasons).</p>
            */
    class Led_StdDialogHelper_FindDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qPlatform_MacOS
        Led_StdDialogHelper_FindDialog (int resID = kLedStdDlg_FindBoxID);
#elif qPlatform_Windows
        Led_StdDialogHelper_FindDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_FindBoxID));
#elif qXWindows && qUseGTKForLedStandardDialogs
        Led_StdDialogHelper_FindDialog (GtkWindow* parentWindow);
#endif

    protected:
#if qPlatform_MacOS
        virtual bool HandleCommandClick (int itemNum) override;
#elif qPlatform_Windows
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
#if qSupportLedDialogWidgets
        LedComboBoxWidget fFindTextWidget;
#endif

    protected:
        virtual void PreDoModalHook () override;

    public:
        virtual void OnFindButton ();
        virtual void OnDontFindButton ();

#if qXWindows && qUseGTKForLedStandardDialogs
    private:
        GtkWidget* fLookupTextWidget;

    private:
        static void Static_OnFindButtonClick (GtkWidget* widget, gpointer data);
        static void Static_OnDontFindButtonClick (GtkWidget* widget, gpointer data);
#endif
    };
#endif
#endif

#ifndef qSupportStdReplaceDlg
#define qSupportStdReplaceDlg qPlatform_MacOS || qPlatform_Windows || qXWindows
#endif

#if qSupportStdReplaceDlg
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
    /*
            @CLASS:         Led_StdDialogHelper_ReplaceDialog
            @DESCRIPTION:   <p>You can define qSupportStdReplaceDlg to 0 to disable inclusion of this resource/code (for size reasons).</p>
            */
    class Led_StdDialogHelper_ReplaceDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qPlatform_MacOS
        Led_StdDialogHelper_ReplaceDialog (int resID = kLedStdDlg_ReplaceBoxID);
#elif qPlatform_Windows
        Led_StdDialogHelper_ReplaceDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_ReplaceBoxID));
#elif qXWindows && qUseGTKForLedStandardDialogs
        Led_StdDialogHelper_ReplaceDialog (GtkWindow* parentWindow);
#endif

    protected:
#if qPlatform_MacOS
        virtual bool HandleCommandClick (int itemNum) override;
#elif qPlatform_Windows
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
        enum ButtonPressed { eCancel,
                             eFind,
                             eReplace,
                             eReplaceAll,
                             eReplaceAllInSelection };
        ButtonPressed fPressed;

    protected:
#if qSupportLedDialogWidgets
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

#if qXWindows && qUseGTKForLedStandardDialogs
    private:
        GtkWidget* fLookupTextWidget;
        GtkWidget* fReplaceTextWidget;

    private:
        static void Static_OnFindButtonClick (GtkWidget* widget, gpointer data);
        static void Static_OnDontFindButtonClick (GtkWidget* widget, gpointer data);
        static void Static_OnReplaceButtonClick (GtkWidget* widget, gpointer data);
        static void Static_OnReplaceAllButtonClick (GtkWidget* widget, gpointer data);
        static void Static_OnReplaceAllInSelectionButtonClick (GtkWidget* widget, gpointer data);
#endif
    };
#endif
#endif

#if qUseGTKForLedStandardDialogs && qXWindows && defined(__cplusplus)
    /*
            @CLASS:         StdFontPickBox
            @DESCRIPTION:   <p>XWindows only. You can define qUseGTKForLedStandardDialogs to 0 to disable inclusion of this resource/code (for size reasons).</p>
            */
    class StdFontPickBox : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
        StdFontPickBox (GtkWindow* modalParentWindow, const Led_FontSpecification& initialFont);

    public:
        virtual GtkWidget* MakeWindow () override;
        virtual void       PreDoModalHook () override;

    public:
        virtual void OnOK () override;

    public:
        Led_FontSpecification fFont;
    };
#endif

#ifndef qSupportStdColorPickBox
#define qSupportStdColorPickBox (qPlatform_MacOS || qPlatform_Windows || (qUseGTKForLedStandardDialogs && qXWindows))
#endif

#if qSupportStdColorPickBox && defined(__cplusplus)
    /*
            @CLASS:         StdColorPickBox
            @DESCRIPTION:   <p>Only defined if @'qSupportStdColorPickBox' defined. For XWindows, we do our own dialog box using Gtk, but
                        for MacOS, and Win32, this doesn't even use @'Led_StdDialogHelper' - but instead - uses other SDK calls to
                        display the dialog.</p>
                            <p>Either way - you still invoke it with a DoModal () call and consider that OK was pressed and
                        the fColor field filled in if DoModal returns true.
                        </p>
            */
    class StdColorPickBox
#if qUseGTKForLedStandardDialogs && qXWindows
        : public Led_StdDialogHelper
#endif
    {
#if qUseGTKForLedStandardDialogs && qXWindows
    private:
        using inherited = Led_StdDialogHelper;
#endif

    public:
#if qPlatform_MacOS
        StdColorPickBox (const Led_Color& initialColor);
#elif qPlatform_Windows
        StdColorPickBox (const Led_Color& initialColor);
        StdColorPickBox (HINSTANCE hInstance, HWND parentWnd, const Led_Color& initialColor);
#elif qXWindows && qUseGTKForLedStandardDialogs
        StdColorPickBox (GtkWindow* modalParentWindow, const Led_Color& initialColor);
#endif

#if qPlatform_Windows
    private:
        HWND fParentWnd;
#endif

#if qPlatform_MacOS || qPlatform_Windows
    public:
        virtual bool DoModal ();
#endif

#if qUseGTKForLedStandardDialogs && qXWindows
    public:
        virtual GtkWidget* MakeWindow () override;
        virtual void       PreDoModalHook () override;

    public:
        virtual void OnOK () override;
#endif

#if qPlatform_Windows
    private:
        static UINT_PTR CALLBACK ColorPickerINITPROC (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

    public:
        Led_Color fColor;
    };
#endif

#ifndef qSupportStdFileDlg
#define qSupportStdFileDlg qXWindows
#endif

#if qSupportStdFileDlg && defined(__cplusplus)
    /*
            @CLASS:         StdFilePickBox
            @DESCRIPTION:   <p>XWindows only.</p>
            */
    class StdFilePickBox : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
        StdFilePickBox (GtkWindow* modalParentWindow, const Led_SDK_String& title, bool saveDialog, const Led_SDK_String& fileName);

    public:
        virtual GtkWidget* MakeWindow () override;
        virtual void       PreDoModalHook () override;

    public:
        virtual void OnOK () override;

    public:
        nonvirtual Led_SDK_String GetFileName () const;

    protected:
        Led_SDK_String fTitle;
        bool           fSaveDialog;
        Led_SDK_String fFileName;
    };
#endif

#ifndef qSupportUpdateWin32FileAssocDlg
#define qSupportUpdateWin32FileAssocDlg qPlatform_Windows
#endif

#if qSupportUpdateWin32FileAssocDlg
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
        Led_StdDialogHelper_UpdateWin32FileAssocsDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_UpdateWin32FileAssocsDialogID));

    public:
        Led_SDK_String fAppName;
        Led_SDK_String fTypeList;
        bool           fKeepChecking;

    protected:
        virtual void PreDoModalHook () override;

    public:
        virtual void OnOK () override;
        virtual void OnCancel () override;
    };
#endif
#endif

#ifndef qSupportParagraphIndentsDlg
#define qSupportParagraphIndentsDlg qPlatform_MacOS || qPlatform_Windows
#endif

#if qSupportParagraphIndentsDlg
#define kLedStdDlg_ParagraphIndentsID 0x1005
#define kLedStdDlg_ParagraphIndents_LeftMarginFieldID (kLedStdDlgCommandBase + 4)
#define kLedStdDlg_ParagraphIndents_RightMarginFieldID (kLedStdDlgCommandBase + 6)
#define kLedStdDlg_ParagraphIndents_FirstIndentFieldID (kLedStdDlgCommandBase + 8)

#if defined(__cplusplus)
    /*
            @CLASS:         Led_StdDialogHelper_ParagraphIndentsDialog
            @DESCRIPTION:   <p>You can define qSupportParagraphIndentsDlg to exlude this from your build (for size reasons)</p>
            */
    class Led_StdDialogHelper_ParagraphIndentsDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qPlatform_MacOS
        Led_StdDialogHelper_ParagraphIndentsDialog (int resID = kLedStdDlg_ParagraphIndentsID);
#elif qPlatform_Windows
        Led_StdDialogHelper_ParagraphIndentsDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_ParagraphIndentsID));
#endif

    public:
        virtual void InitValues (Led_TWIPS leftMargin, bool leftMarginValid, Led_TWIPS rightMargin, bool rightMarginValid, Led_TWIPS firstIndent, bool firstIndentValid);

    protected:
        virtual void PreDoModalHook () override;

    public:
        virtual void OnOK () override;

    public:
        bool      fLeftMargin_Valid;
        Led_TWIPS fLeftMargin_Orig;
        Led_TWIPS fLeftMargin_Result;
        bool      fRightMargin_Valid;
        Led_TWIPS fRightMargin_Orig;
        Led_TWIPS fRightMargin_Result;
        bool      fFirstIndent_Valid;
        Led_TWIPS fFirstIndent_Orig;
        Led_TWIPS fFirstIndent_Result;
    };
#endif
#endif

#ifndef qSupportParagraphSpacingDlg
#define qSupportParagraphSpacingDlg qPlatform_MacOS || qPlatform_Windows
#endif

#if qSupportParagraphSpacingDlg
#define kLedStdDlg_ParagraphSpacingID 0x1006
#define kParagraphSpacing_Dialog_SpaceBeforeFieldID (kLedStdDlgCommandBase + 4)
#define kParagraphSpacing_Dialog_SpaceAfterFieldID (kLedStdDlgCommandBase + 6)
#define kParagraphSpacing_Dialog_LineSpaceModeFieldID (kLedStdDlgCommandBase + 7)
#define kParagraphSpacing_Dialog_LineSpaceArgFieldID (kLedStdDlgCommandBase + 8)

#if qPlatform_MacOS
#define kParagraphSpacing_Dialog_LineSpaceMENUID (kLedStdDlgMENUBase + 1)
#define kParagraphSpacing_Dialog_LineSpaceCNTLID (kLedStdDlgCNTLBase + 1)
#endif

#if defined(__cplusplus)
    /*
            @CLASS:         Led_StdDialogHelper_ParagraphSpacingDialog
            @DESCRIPTION:   <p>You can define qSupportParagraphSpacingDlg to exlude this from your build (for size reasons)</p>
            */
    class Led_StdDialogHelper_ParagraphSpacingDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qPlatform_MacOS
        Led_StdDialogHelper_ParagraphSpacingDialog (int resID = kLedStdDlg_ParagraphSpacingID);
#elif qPlatform_Windows
        Led_StdDialogHelper_ParagraphSpacingDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_ParagraphSpacingID));
#endif

    public:
        virtual void InitValues (Led_TWIPS spaceBefore, bool spaceBeforeValid, Led_TWIPS spaceAfter, bool spaceAfterValid, Led_LineSpacing lineSpacing, bool lineSpacingValid);

    protected:
        virtual void PreDoModalHook () override;

    public:
        virtual void OnOK () override;

    public:
        bool            fSpaceBefore_Valid;
        Led_TWIPS       fSpaceBefore_Orig;
        Led_TWIPS       fSpaceBefore_Result;
        bool            fSpaceAfter_Valid;
        Led_TWIPS       fSpaceAfter_Orig;
        Led_TWIPS       fSpaceAfter_Result;
        bool            fLineSpacing_Valid;
        Led_LineSpacing fLineSpacing_Orig;
        Led_LineSpacing fLineSpacing_Result;
    };
#endif
#endif

#ifndef qSupportOtherFontSizeDlg
#define qSupportOtherFontSizeDlg qPlatform_MacOS || qPlatform_Windows
#endif

#if qSupportOtherFontSizeDlg
#define kLedStdDlg_OtherFontSizeID 0x1007
#define kOtherFontSize_Dialog_FontSizeEditFieldID (kLedStdDlgCommandBase + 4)

#if defined(__cplusplus)
    /*
            @CLASS:         Led_StdDialogHelper_OtherFontSizeDialog
            @DESCRIPTION:   <p>You can define qSupportOtherFontSizeDlg to exlude this from your build (for size reasons)</p>
            */
    class Led_StdDialogHelper_OtherFontSizeDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qPlatform_MacOS
        Led_StdDialogHelper_OtherFontSizeDialog (int resID = kLedStdDlg_OtherFontSizeID);
#elif qPlatform_Windows
        Led_StdDialogHelper_OtherFontSizeDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_OtherFontSizeID));
#endif

    public:
        virtual void InitValues (Led_Distance origFontSize);

    protected:
        virtual void PreDoModalHook () override;

    public:
        virtual void OnOK () override;

    public:
        Led_Distance fFontSize_Orig;
        Led_Distance fFontSize_Result;
    };
#endif
#endif

#ifndef qSupportUnknownEmbeddingInfoDlg
#define qSupportUnknownEmbeddingInfoDlg qPlatform_MacOS || qPlatform_Windows
#endif

#if qSupportUnknownEmbeddingInfoDlg
#define kLedStdDlg_UnknownEmbeddingInfoBoxID 0x1008
#define kLedStdDlg_UnknownEmbeddingInfoBox_TypeTextMsg (kLedStdDlgCommandBase + 1)

#if defined(__cplusplus)
    /*
            @CLASS:         Led_StdDialogHelper_UnknownEmbeddingInfoDialog
            @DESCRIPTION:   <p>You can define qSupportUnknownEmbeddingInfoDlg to exlude this from your build (for size reasons)</p>
            */
    class Led_StdDialogHelper_UnknownEmbeddingInfoDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qPlatform_MacOS
        Led_StdDialogHelper_UnknownEmbeddingInfoDialog (int resID = kLedStdDlg_UnknownEmbeddingInfoBoxID);
#elif qPlatform_Windows
        Led_StdDialogHelper_UnknownEmbeddingInfoDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_UnknownEmbeddingInfoBoxID));
#elif qXWindows && qUseGTKForLedStandardDialogs
        Led_StdDialogHelper_UnknownEmbeddingInfoDialog (GtkWindow* parentWindow);
#endif

    public:
        Led_SDK_String fEmbeddingTypeName;

    protected:
        virtual void PreDoModalHook () override;
    };
#endif
#endif

#ifndef qSupportURLXEmbeddingInfoDlg
#define qSupportURLXEmbeddingInfoDlg qPlatform_MacOS || qPlatform_Windows
#endif

#if qSupportURLXEmbeddingInfoDlg
#define kLedStdDlg_URLXEmbeddingInfoBoxID 0x1009
#define kLedStdDlg_URLXEmbeddingInfoBox_TypeTextMsg (kLedStdDlgCommandBase + 1)
#define kLedStdDlg_URLXEmbeddingInfoBox_TitleText (kLedStdDlgCommandBase + 4)
#define kLedStdDlg_URLXEmbeddingInfoBox_URLText (kLedStdDlgCommandBase + 6)

#if defined(__cplusplus)
    /*
            @CLASS:         Led_StdDialogHelper_URLXEmbeddingInfoDialog
            @DESCRIPTION:   <p>You can define qSupportURLXEmbeddingInfoDlg to exlude this from your build (for size reasons)</p>
            */
    class Led_StdDialogHelper_URLXEmbeddingInfoDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qPlatform_MacOS
        Led_StdDialogHelper_URLXEmbeddingInfoDialog (int resID = kLedStdDlg_URLXEmbeddingInfoBoxID);
#elif qPlatform_Windows
        Led_StdDialogHelper_URLXEmbeddingInfoDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_URLXEmbeddingInfoBoxID));
#elif qXWindows && qUseGTKForLedStandardDialogs
        Led_StdDialogHelper_URLXEmbeddingInfoDialog (GtkWindow* parentWindow);
#endif

    public:
        Led_SDK_String fEmbeddingTypeName;
        Led_SDK_String fTitleText;
        Led_SDK_String fURLText;

    protected:
        virtual void PreDoModalHook () override;

    public:
        virtual void OnOK () override;

#if qXWindows && qUseGTKForLedStandardDialogs
    private:
        GtkWidget* fTitleTextWidget;
        GtkWidget* fURLTextWidget;
#endif
    };
#endif

#define kLedStdDlg_AddURLXEmbeddingInfoBoxID 0x100a
#define kLedStdDlg_AddURLXEmbeddingInfoBox_TitleText (kLedStdDlgCommandBase + 4)
#define kLedStdDlg_AddURLXEmbeddingInfoBox_URLText (kLedStdDlgCommandBase + 6)

#if defined(__cplusplus)
    /*
            @CLASS:         Led_StdDialogHelper_AddURLXEmbeddingInfoDialog
            @DESCRIPTION:   <p>You can define qSupportURLXEmbeddingInfoDlg to exlude this from your build (for size reasons)</p>
            */
    class Led_StdDialogHelper_AddURLXEmbeddingInfoDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qPlatform_MacOS
        Led_StdDialogHelper_AddURLXEmbeddingInfoDialog (int resID = kLedStdDlg_AddURLXEmbeddingInfoBoxID);
#elif qPlatform_Windows
        Led_StdDialogHelper_AddURLXEmbeddingInfoDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_AddURLXEmbeddingInfoBoxID));
#elif qXWindows && qUseGTKForLedStandardDialogs
        Led_StdDialogHelper_AddURLXEmbeddingInfoDialog (GtkWindow* parentWindow);
#endif

    public:
        Led_SDK_String fTitleText;
        Led_SDK_String fURLText;

    protected:
        virtual void PreDoModalHook () override;

    public:
        virtual void OnOK () override;

#if qXWindows && qUseGTKForLedStandardDialogs
    private:
        GtkWidget* fTitleTextWidget;
        GtkWidget* fURLTextWidget;
#endif
    };
#endif
#endif

#ifndef qSupportAddNewTableDlg
#define qSupportAddNewTableDlg qPlatform_Windows || qPlatform_MacOS
#endif

#if qSupportAddNewTableDlg
#define kLedStdDlg_AddNewTableBoxID 0x100b
#define kLedStdDlg_AddNewTableBox_RowCount (kLedStdDlgCommandBase + 5)
#define kLedStdDlg_AddNewTableBox_ColCount (kLedStdDlgCommandBase + 7)

#if defined(__cplusplus)
    /*
            @CLASS:         Led_StdDialogHelper_AddNewTableDialog
            @DESCRIPTION:   <p>You can define qSupportAddNewTableDlg to exlude this from your build (for size reasons)</p>
            */
    class Led_StdDialogHelper_AddNewTableDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qPlatform_MacOS
        Led_StdDialogHelper_AddNewTableDialog (int resID = kLedStdDlg_AddNewTableBoxID);
#elif qPlatform_Windows
        Led_StdDialogHelper_AddNewTableDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_AddNewTableBoxID));
#elif qXWindows && qUseGTKForLedStandardDialogs
        Led_StdDialogHelper_AddNewTableDialog (GtkWindow* parentWindow);
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

#ifndef qSupportEditTablePropertiesDlg
#define qSupportEditTablePropertiesDlg qPlatform_Windows || qPlatform_MacOS
#endif

#if qSupportEditTablePropertiesDlg
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

#if qPlatform_MacOS
#define kLedStdDlg_EditTablePropertiesBox_BorderColorMENUID (kLedStdDlgMENUBase + 2)
#define kLedStdDlg_EditTablePropertiesBox_BorderColorCNTLID (kLedStdDlgCNTLBase + 2)

#define kLedStdDlg_EditTablePropertiesBox_CellBackgroundColorMENUID (kLedStdDlgMENUBase + 3)
#define kLedStdDlg_EditTablePropertiesBox_CellBackgroundColorCNTLID (kLedStdDlgCNTLBase + 3)
#endif

#if defined(__cplusplus)
    /*
            @CLASS:         Led_StdDialogHelper_EditTablePropertiesDialog
            @DESCRIPTION:   <p>You can define qSupportEditTablePropertiesDlg to exlude this from your build (for size reasons)</p>
            */
    class Led_StdDialogHelper_EditTablePropertiesDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
#if qPlatform_MacOS
        Led_StdDialogHelper_EditTablePropertiesDialog (int resID = kLedStdDlg_EditTablePropertiesBoxID);
#elif qPlatform_Windows
        Led_StdDialogHelper_EditTablePropertiesDialog (HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_EditTablePropertiesBoxID));
#elif qXWindows && qUseGTKForLedStandardDialogs
        Led_StdDialogHelper_EditTablePropertiesDialog (GtkWindow* parentWindow);
#endif

    public:
        struct Info {
            Info ();

            Led_TWIPS fTableBorderWidth;
            Led_Color fTableBorderColor;

            Led_TWIPS_Rect fDefaultCellMargins;
            Led_TWIPS      fCellSpacing;

            bool      fCellWidth_Common;
            Led_TWIPS fCellWidth;

            bool      fCellBackgroundColor_Common;
            Led_Color fCellBackgroundColor;
        };
        Info fInfo;

    protected:
        virtual void PreDoModalHook () override;

    protected:
#if qPlatform_MacOS
        virtual bool HandleCommandClick (int itemNum) override;
#elif qPlatform_Windows
        virtual BOOL DialogProc (UINT message, WPARAM wParam, LPARAM lParam) override;
#endif

    public:
        virtual void OnOK () override;

    protected:
        StdColorPopupHelper fBorderColorPopup;
        StdColorPopupHelper fCellBackgroundColorPopup;

    public:
        template <typename T1, typename T2>
        static void cvt (typename T1* o, const typename T2& i);
    };

    template <typename T1, typename T2>
    void Led_StdDialogHelper_EditTablePropertiesDialog::cvt (typename T1* o, const typename T2& i)
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
#if qTemplatedMemberFunctionsFailBug
    template <typename T1, typename T2>
    void Led_StdDialogHelper_EditTablePropertiesDialog_cvt (typename T1* o, const typename T2& i)
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
#endif
    inline Led_StdDialogHelper_EditTablePropertiesDialog::Info::Info ()
        : fTableBorderWidth (Led_TWIPS (0))
        , fTableBorderColor (Led_Color::kWhite)
        , fDefaultCellMargins ()
        , fCellSpacing (Led_TWIPS (0))
        , fCellWidth_Common (false)
        , fCellWidth (Led_TWIPS (0))
        , fCellBackgroundColor_Common (false)
        , fCellBackgroundColor (Led_Color::kWhite)
    {
    }
#endif
#endif

#ifndef qSupportStdSpellCheckDlg
#define qSupportStdSpellCheckDlg qPlatform_MacOS || qPlatform_Windows || qXWindows
#endif

#if qSupportStdSpellCheckDlg
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

#if qPlatform_MacOS
#define kLedStdDlg_SpellCheckBox_SuggestionsCNTLID (kLedStdDlgCNTLBase + 4)
#endif

#if defined(__cplusplus)
    /*
            @CLASS:         Led_StdDialogHelper_SpellCheckDialog
            @DESCRIPTION:   <p>You can define qSupportStdSpellCheckDlg to 0 to disable inclusion of this resource/code (for size reasons).</p>
            */
    class Led_StdDialogHelper_SpellCheckDialog : public Led_StdDialogHelper {
    private:
        using inherited = Led_StdDialogHelper;

    public:
        class SpellCheckDialogCallback;
        struct MisspellingInfo;

    public:
#if qPlatform_MacOS
        Led_StdDialogHelper_SpellCheckDialog (SpellCheckDialogCallback& callback, int resID = kLedStdDlg_SpellCheckBoxID);
#elif qPlatform_Windows
        Led_StdDialogHelper_SpellCheckDialog (SpellCheckDialogCallback& callback, HINSTANCE hInstance, HWND parentWnd, const Led_SDK_Char* resID = MAKEINTRESOURCE (kLedStdDlg_SpellCheckBoxID));
#elif qXWindows && qUseGTKForLedStandardDialogs
        Led_StdDialogHelper_SpellCheckDialog (SpellCheckDialogCallback& callback, GtkWindow* parentWindow);
#endif
        ~Led_StdDialogHelper_SpellCheckDialog ();

    protected:
        SpellCheckDialogCallback& fCallback;

    protected:
        MisspellingInfo* fCurrentMisspellInfo;

    protected:
#if qPlatform_MacOS
        virtual bool HandleCommandClick (int itemNum) override;
#elif qPlatform_Windows
        virtual BOOL DialogProc (UINT message, WPARAM wParam, LPARAM lParam) override;
#endif

    protected:
#if qSupportLedDialogWidgets
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

#if qXWindows && qUseGTKForLedStandardDialogs
    private:
        GtkWidget* fLookupTextWidget;
        GtkWidget* fChangeTextWidget;

    private:
        static void Static_OnIgnoreButtonClick (GtkWidget* widget, gpointer data);
        static void Static_OnIgnoreAllButtonClick (GtkWidget* widget, gpointer data);
        static void Static_OnChangeButtonClick (GtkWidget* widget, gpointer data);
        static void Static_OnChangeAllButtonClick (GtkWidget* widget, gpointer data);
        static void Static_OnAddToDictionaryButtonClick (GtkWidget* widget, gpointer data);
        static void Static_OnLookupOnWebButtonClick (GtkWidget* widget, gpointer data);
        static void Static_OnOptionsDialogButtonClick (GtkWidget* widget, gpointer data);
        static void Static_OnCloseButtonClick (GtkWidget* widget, gpointer data);
#endif

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
        virtual void DoIgnore () override { fDelegate.DoIgnore (); }
        virtual void DoIgnoreAll () override { fDelegate.DoIgnoreAll (); }
        virtual void DoChange (const Led_tString& changeTo) override { fDelegate.DoChange (changeTo); }
        virtual void DoChangeAll (const Led_tString& changeTo) override { fDelegate.DoChangeAll (changeTo); }
        virtual bool AddToDictionaryEnabled () const override { return fDelegate.AddToDictionaryEnabled (); }
        virtual void AddToDictionary (const Led_tString& newWord) override { fDelegate.AddToDictionary (newWord); }
        virtual void LookupOnWeb (const Led_tString& word) override { fDelegate.LookupOnWeb (word); }
        virtual bool OptionsDialogEnabled () const override { return fDelegate.OptionsDialogEnabled (); }
        virtual void OptionsDialog () override { fDelegate.OptionsDialog (); }

    private:
        DEL& fDelegate;
    };
#endif
#endif

    /*
             ********************************************************************************
             ***************************** Implementation Details ***************************
             ********************************************************************************
             */

#if qSupportLedDialogWidgets && defined(__cplusplus)
    //  class   LedComboBoxWidget
    inline vector<Led_tString> LedComboBoxWidget::GetPopupItems () const
    {
        return fPopupItems;
    }
#endif

#if defined(__cplusplus)
}
#endif

#endif /*__Led_StdDialogs__h__*/
