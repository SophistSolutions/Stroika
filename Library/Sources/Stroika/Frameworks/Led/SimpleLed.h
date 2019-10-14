/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_SimpleLed_h_
#define _Stroika_Frameworks_Led_SimpleLed_h_ 1

/*
@MODULE:    SimpleLed
@DESCRIPTION:
        <p>Simple wrappers on the word processing classes and line editing classes to make it a bit easier to
    instantiate a Led editor. Using these has some costs - however - as it removes some opportunities to customize exactly
    what you build into your Led-based editor. Still - its a very helpful way to get started using Led.</p>
 */

#if !defined(__PowerPlant__) && !defined(_MFC_VER) && !defined(_WIN32)
#error "This file provides a simple to use wrapper on one of the above class libraries"
#error "But you don't seem to be using any of them. Be sure to set the proper defines"
#error "if you ARE in fact using one of the supported class libraries."
#endif

#include "../StroikaPreComp.h"

#include "ChunkedArrayTextStore.h"
#include "Command.h"
#include "SimpleTextInteractor.h"
#include "WordProcessor.h"

#if defined(__PowerPlant__)
#include "Led_PP.h"
#elif defined(_MFC_VER)
#include "Led_MFC.h"
#include "Led_MFC_WordProcessor.h"
#elif defined(_WIN32)
#include "Platform/Windows.h"
#endif

#include "WordProcessor.h"

namespace Stroika::Frameworks::Led {

    DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning
    /*
    @CLASS:         SimpleLedWordProcessor
    @BASES:         @'Led_MFC', @'Led_PP', @'WordProcessor'
    @DESCRIPTION:   <p>Led is a concrete editor class. You can directly instantiate and use this one.
                Or mix and match various component classes to make your own editor with the features
                you need. This is really only intended for use as a 'quick start' to using Led. You will probably
                want to directly use the base classes of Led, or various other components directly, once you get
                the hang of using Led.</p>
                    <P>NB: if you are using PowerPlant, be sure to somehow arrange for "PP_Prefix.h" to be included before
                this file is (since thats where __PowerPlant__ is defined). Typically this will be done via the precompiled
                header prefix file.</p>
        */
    class SimpleLedWordProcessor
#if defined(__PowerPlant__)
        : public Led_PPView_X<WordProcessor, Led_PPView_Traits_Default>
#elif defined(_MFC_VER)
        : public Led_MFC_X<WordProcessor>
#elif defined(_WIN32)
        : public Platform::Led_Win32_SimpleWndProc_Helper<Platform::Led_Win32_Helper<WordProcessor>>
#endif
    {
    private:
#if defined(__PowerPlant__)
        using inherited = Led_PPView_X<WordProcessor, Led_PPView_Traits_Default>;
#elif defined(_MFC_VER)
        using inherited = Led_MFC_X<WordProcessor>;
#elif defined(_WIN32)
        using inherited = Platform::Led_Win32_SimpleWndProc_Helper<Platform::Led_Win32_Helper<WordProcessor>>;
#endif
    public:
        SimpleLedWordProcessor ();
        ~SimpleLedWordProcessor ();

#if qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR
    public:
#if defined(_MFC_VER)
        virtual void OnInitialUpdate () override;
        virtual void PostNcDestroy () override;
#elif defined(_WIN32)
        virtual LRESULT OnCreate_Msg (LPCREATESTRUCT createStruct) override;
        virtual void    OnNCDestroy_Msg () override;
#endif
#endif

    private:
        enum { kMaxUndoLevels = 5 };
        ChunkedArrayTextStore        fTextStore;
        MultiLevelUndoCommandHandler fCommandHandler;

#if defined(_MFC_VER)
    protected:
        DECLARE_MESSAGE_MAP ()
        DECLARE_DYNCREATE (SimpleLedWordProcessor)
#endif
    };

    DISABLE_COMPILER_MSC_WARNING_END (4250)

    DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning
    /*
    @CLASS:         SimpleLedLineEditor
    @BASES:         @'Led_MFC', @'Led_PP', @'SimpleTextInteractor'
    @DESCRIPTION:   <p>Led is a concrete editor class. You can directly instantiate and use this one.
                Or mix and match various component classes to make your own editor with the features
                you need. This is really only intended for use as a 'quick start' to using Led. You will probably
                want to directly use the base classes of Led, or various other components directly, once you get
                the hang of using Led.</p>
                    <P>NB: if you are using PowerPlant, be sure to somehow arrange for "PP_Prefix.h" to be included before
                this file is (since thats where __PowerPlant__ is defined). Typically this will be done via the precompiled
                header prefix file.</p>
        */
    class SimpleLedLineEditor
#if defined(__PowerPlant__)
        : public Led_PPView_X<SimpleTextInteractor, Led_PPView_Traits_Default>
#elif defined(_MFC_VER)
        : public Led_MFC_X<SimpleTextInteractor>
#elif defined(_WIN32)
        : public Platform::Led_Win32_SimpleWndProc_Helper<Platform::Led_Win32_Helper<SimpleTextInteractor>>
#endif
    {
    private:
#if defined(__PowerPlant__)
        using inherited = Led_PPView_X<SimpleTextInteractor, Led_PPView_Traits_Default>;
#elif defined(_MFC_VER)
        using inherited = Led_MFC_X<SimpleTextInteractor>;
#elif defined(_WIN32)
        using inherited = Platform::Led_Win32_SimpleWndProc_Helper<Platform::Led_Win32_Helper<SimpleTextInteractor>>;
#endif
    public:
        SimpleLedLineEditor ();
        ~SimpleLedLineEditor ();

#if qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR
    public:
#if defined(_MFC_VER)
        virtual void OnInitialUpdate () override;
        virtual void PostNcDestroy () override;
#elif defined(_WIN32)
        virtual LRESULT OnCreate_Msg (LPCREATESTRUCT createStruct) override;
        virtual void    OnNCDestroy_Msg () override;
#endif
#endif

    private:
        enum { kMaxUndoLevels = 5 };
        ChunkedArrayTextStore        fTextStore;
        MultiLevelUndoCommandHandler fCommandHandler;

#if defined(_MFC_VER)
    protected:
        DECLARE_MESSAGE_MAP ()
        DECLARE_DYNCREATE (SimpleLedLineEditor)
#endif
    };
    DISABLE_COMPILER_MSC_WARNING_END (4250)

    DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning
    /*
    @CLASS:         LedDialogText
    @BASES:         @'SimpleLedWordProcessor'
    @DESCRIPTION:   <p>Very similar to class @'Led'. But with a few class-library-specific twists to make it easier
                to embed the Led view into a dialog. Not really needed. But if you are having trouble getting your Led-based
                view working in a dialog, you may want to try, or at least look at this code.</p>
                    <p>PROBABLY SHOULD RE-IMPLEMENT THIS to be more like a textedit box. Make WordWrap optional. For MFC - use
                CWND instead of CView. Actually TEST it and make clear where/why its useful (UNICODE for one thing).
                </p>
        */
    class LedDialogText : public SimpleLedWordProcessor {
    private:
        using inherited = SimpleLedWordProcessor;

    public:
        LedDialogText ();

#if qPlatform_Windows && defined(_MFC_VER)
    public:
        virtual void PostNcDestroy () override;
        virtual int  OnMouseActivate (CWnd* pDesktopWnd, UINT nHitTest, UINT message) override;
#endif

#if qPlatform_Windows && defined(_MFC_VER)
    protected:
        DECLARE_MESSAGE_MAP ()
        DECLARE_DYNCREATE (LedDialogText)
#endif
    };
    DISABLE_COMPILER_MSC_WARNING_END (4250)

}

#endif /*_Stroika_Frameworks_Led_SimpleLed_h_*/
