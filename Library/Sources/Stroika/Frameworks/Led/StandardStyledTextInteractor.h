/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_StandardStyledTextInteractor_h_
#define _Stroika_Frameworks_Led_StandardStyledTextInteractor_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

/*
@MODULE:    StandardStyledTextInteractor
@DESCRIPTION:
        <p>Combine the functionality of @'TextInteractor', and @'StandardStyledTextImager', and add a bunch of
    related functionality (which only makes sense when you have both of these features).</p>

 */

#include "StandardStyledTextImager.h"
#include "TextInteractor.h"

#include "StyledTextIO/StyledTextIO.h"

namespace Stroika::Frameworks::Led {

    /*
    @CONFIGVAR:     qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor
    @DESCRIPTION:   <p>This format isn't terribly useful right now. It may become much more useful in future versions, if I decide
                to rewrite it, to make it much faster than RTF. Then I may use it internally more.</p>
                    <p>It should cause little harm being turned on, but some people (SPR#0810) have requested the ability to
                have this code stripped out.</p>
                    <p>Turn ON by default.</p>
        */
#define qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor 0
#ifndef qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor
#define qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor 1
#endif

#if qStroika_FeatureSupported_XWindows
    extern Led_ClipFormat kLedPrivateClipFormat;
    extern Led_ClipFormat kRTFClipFormat;
    extern Led_ClipFormat kHTMLClipFormat;
#else
    extern const Led_ClipFormat kLedPrivateClipFormat;
    extern const Led_ClipFormat kRTFClipFormat;
    extern const Led_ClipFormat kHTMLClipFormat;
#endif

    /**
     *   <p>This is a writer source stream which talks to a StandardStyledTextImager and/or WordProcessor
     *   class. It knows about StyleDatabases, and ParagraphDatabases, and gets content from them for the
     *   output writer class.</p>
     */
    class StandardStyledTextIOSrcStream : public virtual StyledTextIO::StyledTextIOWriter::SrcStream {
    private:
        using inherited = StyledTextIO::StyledTextIOWriter::SrcStream;

    public:
        StandardStyledTextIOSrcStream (TextStore* textStore, const shared_ptr<AbstractStyleDatabaseRep>& textStyleDatabase,
                                       size_t selectionStart = 0, size_t selectionEnd = kBadIndex);
#if qStroika_Frameworks_Led_SupportGDI
        StandardStyledTextIOSrcStream (StandardStyledTextImager* textImager, size_t selectionStart = 0, size_t selectionEnd = kBadIndex);
#endif

    public:
        virtual size_t                          readNTChars (Led_tChar* intoBuf, size_t maxTChars) override;
        virtual size_t                          current_offset () const override;
        virtual void                            seek_to (size_t to) override;
        virtual size_t                          GetTotalTextLength () const override;
        virtual vector<StyledInfoSummaryRecord> GetStyleInfo (size_t from, size_t len) const override;
#if qStroika_Frameworks_Led_SupportGDI
        virtual vector<SimpleEmbeddedObjectStyleMarker*> CollectAllEmbeddingMarkersInRange (size_t from, size_t to) const override;
#endif
        virtual Table* GetTableAt (size_t at) const override;
        virtual void   SummarizeFontAndColorTable (set<SDKString>* fontNames, set<Color>* colorsUsed) const override;
        virtual size_t GetEmbeddingMarkerPosOffset () const override;

    public:
        nonvirtual size_t GetCurOffset () const;
        nonvirtual size_t GetSelStart () const;
        nonvirtual size_t GetSelEnd () const;

    private:
        TextStore*                           fTextStore;
        shared_ptr<AbstractStyleDatabaseRep> fStyleRunDatabase;
        size_t                               fCurOffset;
        size_t                               fSelStart;
        size_t                               fSelEnd;
    };

    /**
     *      <p>This is a writer sink stream which talks to a StandardStyledTextImager and/or WordProcessor
     *  class. It knows about StyleDatabases, and ParagraphDatabases, and writes content to them from the
     *  input reader class.</p>
     */
    class StandardStyledTextIOSinkStream : public virtual StyledTextIO::StyledTextIOReader::SinkStream {
    private:
        using inherited = StyledTextIO::StyledTextIOReader::SinkStream;

    public:
        StandardStyledTextIOSinkStream (TextStore* textStore, const shared_ptr<AbstractStyleDatabaseRep>& textStyleDatabase, size_t insertionStart = 0);
        ~StandardStyledTextIOSinkStream ();

    public:
        // if fontSpec is nullptr, use default. Probably later we will return and update the fontspec with
        // ApplyStyle
        virtual size_t            current_offset () const override;
        virtual void              AppendText (const Led_tChar* text, size_t nTChars, const FontSpecification* fontSpec) override;
        virtual void              ApplyStyle (size_t from, size_t to, const vector<StyledInfoSummaryRecord>& styleRuns) override;
        virtual FontSpecification GetDefaultFontSpec () const override;
#if qStroika_Frameworks_Led_SupportGDI
        virtual void InsertEmbeddingForExistingSentinel (SimpleEmbeddedObjectStyleMarker* embedding, size_t at) override;
        virtual void AppendEmbedding (SimpleEmbeddedObjectStyleMarker* embedding) override;
#endif
        virtual void AppendSoftLineBreak () override;
        virtual void InsertMarker (Marker* m, size_t at, size_t length, MarkerOwner* markerOwner) override;
        virtual void Flush () override;

    public:
        nonvirtual size_t GetInsertionStart () const;
        nonvirtual void   SetInsertionStart (size_t insertionStart);
        nonvirtual size_t GetOriginalStart () const;
        nonvirtual size_t GetCachedTextSize () const;

    protected:
        nonvirtual const vector<Led_tChar>& GetCachedText () const;

    protected:
        nonvirtual TextStore& GetTextStore () const;
        nonvirtual shared_ptr<AbstractStyleDatabaseRep> GetStyleDatabase () const;

    protected:
        nonvirtual void PushContext (TextStore* ts, const shared_ptr<AbstractStyleDatabaseRep>& textStyleDatabase, size_t insertionStart);
        nonvirtual void PopContext ();

    private:
        struct Context {
            TextStore*                           fTextStore;
            shared_ptr<AbstractStyleDatabaseRep> fStyleRunDatabase;
            size_t                               fOriginalStart;
            size_t                               fInsertionStart;
        };
        vector<Context> fSavedContexts;

    private:
        TextStore*                           fTextStore{nullptr};
        shared_ptr<AbstractStyleDatabaseRep> fStyleRunDatabase{nullptr};
        size_t                               fOriginalStart{0};
        size_t                               fInsertionStart{0};
        vector<StyledInfoSummaryRecord>      fSavedStyleInfo{};
        vector<Led_tChar>                    fCachedText{};
    };

#if qStroika_Frameworks_Led_SupportGDI
    class SimpleEmbeddedObjectStyleMarker;

    DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning
    /*
    @CLASS:         StandardStyledTextInteractor
    @BASES:         virtual @'TextInteractor', @'StandardStyledTextImager'
    @DESCRIPTION:
            <p>Simple mixin of StandardStyledTextImager and TextInteractor.</p>
            <p>Since this class is designed to make easier implementing a standard font/stylerun
        type editor, we will OVERRIDE SetDefaultFont() and re-interpret it to mean
        setting the font for the current selection of text, or for the nullptr selection,
        just as with Apples TE.</p>
            <p>NB:  We considered using @'InteractorImagerMixinHelper<IMAGER>' template as base-class helper.
        But it didn't help. We do too many overrides it doesn't, so it doesn't help much. And then
        later mixing together two subclasses of InteractorImagerMixinHelper (like for word processor)
        requires disambiguating stuff in InteractorImagerMixinHelper itself (like Draw).
        Just not worth much. -- LGP 970707.</p>
    */
    class StandardStyledTextInteractor : public virtual TextInteractor, public StandardStyledTextImager {
    private:
        using inherited = void*; // prevent accidental references to this name in subclasses to base class name
    protected:
        StandardStyledTextInteractor ();

    public:
        virtual ~StandardStyledTextInteractor () = default;

    public:
        struct CommandNames;

        // This class builds commands with command names. The UI may wish to change these
        // names (eg. to customize for particular languages, etc)
        // Just patch these strings here, and commands will be created with these names.
        // (These names appear in text of undo menu item)
    public:
        static const CommandNames& GetCommandNames ();
        static void                SetCommandNames (const CommandNames& cmdNames);
        static CommandNames        MakeDefaultCommandNames ();

    private:
        static CommandNames sCommandNames;

    protected:
        virtual void    HookLosingTextStore () override;
        nonvirtual void HookLosingTextStore_ ();
        virtual void    HookGainedNewTextStore () override;
        nonvirtual void HookGainedNewTextStore_ ();

    public:
        virtual void SetDefaultFont (const IncrementalFontSpecification& defaultFont) override;

    public:
        virtual void InteractiveSetFont (const IncrementalFontSpecification& defaultFont);

    public:
        virtual IncrementalFontSpecification GetContinuousStyleInfo (size_t from, size_t nTChars) const override;

    public:
        virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

    public:
        virtual bool ShouldEnablePasteCommand () const override;
        virtual bool CanAcceptFlavor (Led_ClipFormat clipFormat) const override;

    public:
        class StyledTextFlavorPackageInternalizer;
        class StyledTextFlavorPackageExternalizer;

    protected:
        virtual void HookStyleDatabaseChanged () override;

    protected:
        virtual shared_ptr<FlavorPackageInternalizer> MakeDefaultInternalizer () override;
        virtual shared_ptr<FlavorPackageExternalizer> MakeDefaultExternalizer () override;

    protected:
        virtual bool ProcessSimpleClick (Led_Point clickedAt, unsigned clickCount, bool extendSelection, size_t* dragAnchor) override;
        virtual void WhileSimpleMouseTracking (Led_Point newMousePos, size_t dragAnchor) override;

    public:
        virtual void InteractiveReplace (const Led_tChar* withWhat, size_t withWhatCharCount, UpdateMode updateMode = eDefaultUpdate) override;
        virtual void SetSelection (size_t start, size_t end) override;
#if qUsingMemberNameToOverloadInWithUsingBug
        void SetSelection (size_t start, size_t end, TextInteractor::UpdateMode updateMode)
        {
            TextInteractor::SetSelection (start, end, updateMode);
        }
#else
        using TextInteractor::SetSelection;
#endif
    protected:
        virtual bool    InteractiveReplaceEarlyPostReplaceHook (size_t withWhatCharCount) override;
        nonvirtual void SetSelection_ (size_t start, size_t end); // simply refills fEmptySelectionStyle
    private:
        bool              fEmptySelectionStyleSuppressMode;
        FontSpecification fEmptySelectionStyle;

    public:
        nonvirtual FontSpecification GetEmptySelectionStyle () const;
        nonvirtual void              SetEmptySelectionStyle ();
        nonvirtual void              SetEmptySelectionStyle (FontSpecification newEmptyFontSpec);

    public:
        nonvirtual vector<SimpleEmbeddedObjectStyleMarker*> CollectAllEmbeddingMarkersInRange (size_t from, size_t to) const;

    protected:
        virtual InteractiveReplaceCommand::SavedTextRep* InteractiveUndoHelperMakeTextRep (size_t regionStart, size_t regionEnd,
                                                                                           size_t selStart, size_t selEnd) override;

    public:
        class EmptySelStyleTextRep;
        friend class EmptySelStyleTextRep;
    };
    DISABLE_COMPILER_MSC_WARNING_END (4250) // inherits via dominance warning

    /*
    @CLASS:         StandardStyledTextInteractor::CommandNames
    @DESCRIPTION:   <p>Command names for each of the user-visible commands produced by the @'TextInteractor' module.
                This name is used used in the constructed Undo command name, as
                in, "Undo Change Font". You can replace this name with whatever you like.
                You change this value with @'StandardStyledTextInteractor::SetCommandNames'.</p>
                    <p> The point of this is to allow for different UI-language localizations,
                without having to change Led itself.</p>
                    <p>See also @'TextInteractor::GetCommandNames'.</p>
                    <p>See also @'TextInteractor::CommandNames'.</p>
    */
    struct StandardStyledTextInteractor::CommandNames {
        SDKString fFontChangeCommandName;
    };

    /*
    @CLASS:         StandardStyledTextInteractor::StyledTextFlavorPackageInternalizer
    @BASES:         virtual @'FlavorPackageInternalizer'
    @DESCRIPTION:   <p>Add RTF, Led_Native, and SingleSelectedEmbedding support.</p>
    */
    class StandardStyledTextInteractor::StyledTextFlavorPackageInternalizer : public virtual FlavorPackageInternalizer {
    private:
        using inherited = FlavorPackageInternalizer;

    public:
        StyledTextFlavorPackageInternalizer (TextStore& ts, const shared_ptr<AbstractStyleDatabaseRep>& styleDatabase);

    public:
        virtual bool InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage, size_t from, size_t to) override;

    public:
        virtual void InternalizeFlavor_FILEGuessFormatsFromName (filesystem::path fileName, Led_ClipFormat* suggestedClipFormat,
                                                                 optional<CodePage> suggestedCodePage) override;
        virtual void InternalizeFlavor_FILEGuessFormatsFromStartOfData (Led_ClipFormat* suggestedClipFormat, optional<CodePage> suggestedCodePage,
                                                                        const byte* fileStart, const byte* fileEnd) override;

    public:
#if qPlatform_MacOS
        virtual bool InternalizeFlavor_STYLAndTEXT (ReaderFlavorPackage& flavorPackage, size_t from, size_t to);
#endif
#if qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor
        virtual bool InternalizeFlavor_Native (ReaderFlavorPackage& flavorPackage, size_t from, size_t to);
#endif
        virtual bool InternalizeFlavor_RTF (ReaderFlavorPackage& flavorPackage, size_t from, size_t to);
        virtual bool InternalizeFlavor_HTML (ReaderFlavorPackage& flavorPackage, size_t from, size_t to);
        virtual bool InternalizeFlavor_OtherRegisteredEmbedding (ReaderFlavorPackage& flavorPackage, size_t from, size_t to);

    public:
        virtual StandardStyledTextIOSinkStream* mkStandardStyledTextIOSinkStream (size_t insertionStart);

    protected:
        shared_ptr<AbstractStyleDatabaseRep> fStyleDatabase;
    };

    /*
    @CLASS:         StandardStyledTextInteractor::StyledTextFlavorPackageExternalizer
    @BASES:         virtual @'FlavorPackageExternalizer'
    @DESCRIPTION:   <p>Add RTF, Led_Native, and SingleSelectedEmbedding support.</p>
    */
    class StandardStyledTextInteractor::StyledTextFlavorPackageExternalizer : public virtual FlavorPackageExternalizer {
    private:
        using inherited = FlavorPackageExternalizer;

    public:
        StyledTextFlavorPackageExternalizer (TextStore& ts, const shared_ptr<AbstractStyleDatabaseRep>& styleDatabase);

    public:
        virtual void ExternalizeFlavors (WriterFlavorPackage& flavorPackage, size_t from, size_t to) override;
        virtual void ExternalizeBestFlavor (WriterFlavorPackage& flavorPackage, size_t from, size_t to) override;

    public:
#if qPlatform_MacOS
        nonvirtual void ExternalizeFlavor_STYL (WriterFlavorPackage& flavorPackage, size_t from, size_t to);
#endif
#if qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor
        nonvirtual void ExternalizeFlavor_Native (WriterFlavorPackage& flavorPackage, size_t from, size_t to);
#endif
        nonvirtual void ExternalizeFlavor_RTF (WriterFlavorPackage& flavorPackage, size_t from, size_t to);
        nonvirtual void ExternalizeFlavor_SingleSelectedEmbedding (WriterFlavorPackage& flavorPackage, SimpleEmbeddedObjectStyleMarker* embedding);

    protected:
        virtual StandardStyledTextIOSrcStream* mkStandardStyledTextIOSrcStream (size_t selectionStart, size_t selectionEnd);

    protected:
        shared_ptr<AbstractStyleDatabaseRep> fStyleDatabase;
    };

    /*
    @CLASS:         StandardStyledTextInteractor::EmptySelStyleTextRep
    @BASES:         @'InteractiveReplaceCommand::SavedTextRep'
    @DESCRIPTION:
    */
    class StandardStyledTextInteractor::EmptySelStyleTextRep : public InteractiveReplaceCommand::SavedTextRep {
    private:
        using inherited = InteractiveReplaceCommand::SavedTextRep;

    public:
        EmptySelStyleTextRep (StandardStyledTextInteractor* interactor, size_t selStart, size_t selEnd);

    public:
        virtual size_t GetLength () const override;
        virtual void   InsertSelf (TextInteractor* interactor, size_t at, size_t nBytesToOverwrite) override;

    private:
        FontSpecification fSavedStyle;
    };
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StandardStyledTextInteractor.inl"

#endif /*_Stroika_Frameworks_Led_StandardStyledTextInteractor_h_*/
