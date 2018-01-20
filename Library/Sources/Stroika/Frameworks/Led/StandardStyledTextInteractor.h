/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_StandardStyledTextInteractor_h_
#define _Stroika_Frameworks_Led_StandardStyledTextInteractor_h_ 1

#include "../../Foundation/StroikaPreComp.h"

/*
@MODULE:    StandardStyledTextInteractor
@DESCRIPTION:
        <p>Combine the functionality of @'TextInteractor', and @'StandardStyledTextImager', and add a bunch of
    related functionality (which only makes sense when you have both of these features).</p>

 */

#include "StandardStyledTextImager.h"
#include "TextInteractor.h"

#include "StyledTextIO/StyledTextIO.h"

namespace Stroika {
    namespace Frameworks {
        namespace Led {

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

#if qXWindows
            extern Led_ClipFormat kLedPrivateClipFormat;
            extern Led_ClipFormat kRTFClipFormat;
            extern Led_ClipFormat kHTMLClipFormat;
#else
            extern const Led_ClipFormat kLedPrivateClipFormat;
            extern const Led_ClipFormat kRTFClipFormat;
            extern const Led_ClipFormat kHTMLClipFormat;
#endif

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

            class SimpleEmbeddedObjectStyleMarker;
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
                virtual ~StandardStyledTextInteractor ();

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
#if qRuntimeCrashMaybeCodeGenBugWithAppStartupBug
                static CommandNames& sCommandNames ();
#else
                static CommandNames sCommandNames;
#endif

            protected:
                virtual void    HookLosingTextStore () override;
                nonvirtual void HookLosingTextStore_ ();
                virtual void    HookGainedNewTextStore () override;
                nonvirtual void HookGainedNewTextStore_ ();

            public:
                virtual void SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont) override;

            public:
                virtual void InteractiveSetFont (const Led_IncrementalFontSpecification& defaultFont);

            public:
                virtual Led_IncrementalFontSpecification GetContinuousStyleInfo (size_t from, size_t nTChars) const override;

            public:
                virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

            public:
                virtual bool ShouldEnablePasteCommand () const override;
                virtual bool CanAcceptFlavor (Led_ClipFormat clipFormat) const override;

            public:
                class StandardStyledTextIOSinkStream;
                class StandardStyledTextIOSrcStream;

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
                bool                  fEmptySelectionStyleSuppressMode;
                Led_FontSpecification fEmptySelectionStyle;

            public:
                nonvirtual Led_FontSpecification GetEmptySelectionStyle () const;
                nonvirtual void                  SetEmptySelectionStyle ();
                nonvirtual void                  SetEmptySelectionStyle (Led_FontSpecification newEmptyFontSpec);

            public:
                nonvirtual vector<SimpleEmbeddedObjectStyleMarker*> CollectAllEmbeddingMarkersInRange (size_t from, size_t to) const;

            protected:
                virtual InteractiveReplaceCommand::SavedTextRep* InteractiveUndoHelperMakeTextRep (size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd) override;

            public:
                class EmptySelStyleTextRep;
                friend class EmptySelStyleTextRep;
            };

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
                Led_SDK_String fFontChangeCommandName;
            };

            /*
            @CLASS:         StandardStyledTextInteractor::StandardStyledTextIOSinkStream
            @BASES:         @'StyledTextIOReader::SinkStream'
            @DESCRIPTION:   <p>This is a writer sink stream which talks to a StandardStyledTextImager and/or WordProcessor
                class. It knows about StyleDatabases, and ParagraphDatabases, and writes content to them from the
                input reader class.</p>
            */
            class StandardStyledTextInteractor::StandardStyledTextIOSinkStream : public virtual StyledTextIO::StyledTextIOReader::SinkStream {
            private:
                using inherited = StyledTextIO::StyledTextIOReader::SinkStream;

            public:
                StandardStyledTextIOSinkStream (TextStore*                                        textStore,
                                                const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
                                                size_t                                            insertionStart = 0);
                ~StandardStyledTextIOSinkStream ();

            public:
                // if fontSpec is nullptr, use default. Probably later we will return and update the fontspec with
                // ApplyStyle
                virtual size_t                current_offset () const override;
                virtual void                  AppendText (const Led_tChar* text, size_t nTChars, const Led_FontSpecification* fontSpec) override;
                virtual void                  ApplyStyle (size_t from, size_t to, const vector<StandardStyledTextImager::InfoSummaryRecord>& styleRuns) override;
                virtual Led_FontSpecification GetDefaultFontSpec () const override;
                virtual void                  InsertEmbeddingForExistingSentinal (SimpleEmbeddedObjectStyleMarker* embedding, size_t at) override;
                virtual void                  AppendEmbedding (SimpleEmbeddedObjectStyleMarker* embedding) override;
                virtual void                  AppendSoftLineBreak () override;
                virtual void                  InsertMarker (Marker* m, size_t at, size_t length, MarkerOwner* markerOwner) override;
                virtual void                  Flush () override;

            public:
                nonvirtual size_t GetInsertionStart () const;
                nonvirtual void   SetInsertionStart (size_t insertionStart);
                nonvirtual size_t GetOriginalStart () const;
                nonvirtual size_t GetCachedTextSize () const;

            protected:
                nonvirtual const vector<Led_tChar>& GetCachedText () const;

            protected:
                nonvirtual TextStore& GetTextStore () const;
                nonvirtual StandardStyledTextImager::StyleDatabasePtr GetStyleDatabase () const;

            protected:
                nonvirtual void PushContext (TextStore*                                        ts,
                                             const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
                                             size_t                                            insertionStart);
                nonvirtual void PopContext ();

            private:
                struct Context {
                    TextStore*                                 fTextStore;
                    StandardStyledTextImager::StyleDatabasePtr fStyleRunDatabase;
                    size_t                                     fOriginalStart;
                    size_t                                     fInsertionStart;
                };
                vector<Context> fSavedContexts;

            private:
                TextStore*                                          fTextStore;
                StandardStyledTextImager::StyleDatabasePtr          fStyleRunDatabase;
                size_t                                              fOriginalStart;
                size_t                                              fInsertionStart;
                vector<StandardStyledTextImager::InfoSummaryRecord> fSavedStyleInfo;
                vector<Led_tChar>                                   fCachedText;
            };

            /*
            @CLASS:         StandardStyledTextInteractor::StandardStyledTextIOSrcStream
            @BASES:         @'StyledTextIOWriter::SrcStream'
            @DESCRIPTION:   <p>This is a writer source stream which talks to a StandardStyledTextImager and/or WordProcessor
                class. It knows about StyleDatabases, and ParagraphDatabases, and gets content from them for the
                output writer class.</p>
            */
            class StandardStyledTextInteractor::StandardStyledTextIOSrcStream : public virtual StyledTextIO::StyledTextIOWriter::SrcStream {
            private:
                using inherited = StyledTextIO::StyledTextIOWriter::SrcStream;

            public:
                StandardStyledTextIOSrcStream (TextStore*                                        textStore,
                                               const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
                                               size_t selectionStart = 0, size_t selectionEnd = kBadIndex);
                StandardStyledTextIOSrcStream (StandardStyledTextImager* textImager, size_t selectionStart = 0, size_t selectionEnd = kBadIndex);

            public:
                virtual size_t                                   readNTChars (Led_tChar* intoBuf, size_t maxTChars) override;
                virtual size_t                                   current_offset () const override;
                virtual void                                     seek_to (size_t to) override;
                virtual size_t                                   GetTotalTextLength () const override;
                virtual vector<InfoSummaryRecord>                GetStyleInfo (size_t from, size_t len) const override;
                virtual vector<SimpleEmbeddedObjectStyleMarker*> CollectAllEmbeddingMarkersInRange (size_t from, size_t to) const override;
                virtual Table*                                   GetTableAt (size_t at) const override;
                virtual void                                     SummarizeFontAndColorTable (set<Led_SDK_String>* fontNames, set<Led_Color>* colorsUsed) const override;
                virtual size_t                                   GetEmbeddingMarkerPosOffset () const override;

            public:
                nonvirtual size_t GetCurOffset () const;
                nonvirtual size_t GetSelStart () const;
                nonvirtual size_t GetSelEnd () const;

            private:
                TextStore*                                 fTextStore;
                StandardStyledTextImager::StyleDatabasePtr fStyleRunDatabase;
                size_t                                     fCurOffset;
                size_t                                     fSelStart;
                size_t                                     fSelEnd;
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
                StyledTextFlavorPackageInternalizer (TextStore& ts, const StandardStyledTextImager::StyleDatabasePtr& styleDatabase);

            public:
                virtual bool InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage,
                                                    size_t from, size_t to) override;

            public:
                virtual void InternalizeFlavor_FILEGuessFormatsFromName (
#if qPlatform_MacOS
                    const FSSpec* fileName,
#elif qPlatform_Windows || qXWindows
                    const Led_SDK_Char* fileName,
#endif
                    Led_ClipFormat* suggestedClipFormat,
                    CodePage*       suggestedCodePage) override;
                virtual void InternalizeFlavor_FILEGuessFormatsFromStartOfData (
                    Led_ClipFormat* suggestedClipFormat,
                    CodePage*       suggestedCodePage,
                    const Byte* fileStart, const Byte* fileEnd) override;

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
                StandardStyledTextImager::StyleDatabasePtr fStyleDatabase;
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
                StyledTextFlavorPackageExternalizer (TextStore& ts, const StandardStyledTextImager::StyleDatabasePtr& styleDatabase);

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
                StandardStyledTextImager::StyleDatabasePtr fStyleDatabase;
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
                Led_FontSpecification fSavedStyle;
            };

            /*
             ********************************************************************************
             ***************************** Implementation Details ***************************
             ********************************************************************************
             */
            //  class   StandardStyledTextInteractor
            /*
            @METHOD:        StandardStyledTextInteractor::GetCommandNames
            @DESCRIPTION:   <p>Returns command name for each of the user-visible commands produced by this module.
                This name is used used in the constructed Undo command name, as
                in, "Undo Change Font". You can replace this name with whatever you like.You change this value with
                WordProcessor::SetCommandNames.</p>
                    <p> The point of this is to allow for different UI-language localizations,
                        without having to change Led itself.</p>
                    <p>See also @'StandardStyledTextInteractor::CommandNames'.</p>
            */
            inline const StandardStyledTextInteractor::CommandNames& StandardStyledTextInteractor::GetCommandNames ()
            {
#if qRuntimeCrashMaybeCodeGenBugWithAppStartupBug
                return sCommandNames ();
#else
                return sCommandNames;
#endif
            }
            /*
            @METHOD:        StandardStyledTextInteractor::SetCommandNames
            @DESCRIPTION:   <p>See @'StandardStyledTextInteractor::GetCommandNames'.</p>
            */
            inline void StandardStyledTextInteractor::SetCommandNames (const StandardStyledTextInteractor::CommandNames& cmdNames)
            {
#if qRuntimeCrashMaybeCodeGenBugWithAppStartupBug
                sCommandNames () = cmdNames;
#else
                sCommandNames = cmdNames;
#endif
            }

            //  class   StandardStyledTextInteractor::StandardStyledTextIOSinkStream
            inline TextStore& StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetTextStore () const
            {
                EnsureNotNull (fTextStore);
                return *fTextStore;
            }
            inline StandardStyledTextImager::StyleDatabasePtr StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetStyleDatabase () const
            {
                return fStyleRunDatabase;
            }
            /*
            @METHOD:        StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetInsertionStart
            @DESCRIPTION:   <p>Returns where (in TextStore marker coordinates - not relative to the sinkstream) where the next character
                        will be inserted. See also @'StandardStyledTextInteractor::StandardStyledTextIOSinkStream::SetInsertionStart'</p>
            */
            inline size_t StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetInsertionStart () const
            {
                return fInsertionStart;
            }
            /*
            @METHOD:        StandardStyledTextInteractor::StandardStyledTextIOSinkStream::SetInsertionStart
            @DESCRIPTION:   <p>See also @'StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetInsertionStart'</p>
            */
            inline void StandardStyledTextInteractor::StandardStyledTextIOSinkStream::SetInsertionStart (size_t insertionStart)
            {
                fInsertionStart = insertionStart;
            }
            inline size_t StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetOriginalStart () const
            {
                return fOriginalStart;
            }
            inline size_t StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetCachedTextSize () const
            {
                return fCachedText.size ();
            }
            inline const vector<Led_tChar>& StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetCachedText () const
            {
                return fCachedText;
            }

            //  class   StandardStyledTextInteractor::StandardStyledTextIOSrcStream
            inline size_t StandardStyledTextInteractor::StandardStyledTextIOSrcStream::GetCurOffset () const
            {
                return fCurOffset;
            }
            inline size_t StandardStyledTextInteractor::StandardStyledTextIOSrcStream::GetSelStart () const
            {
                return fSelStart;
            }
            inline size_t StandardStyledTextInteractor::StandardStyledTextIOSrcStream::GetSelEnd () const
            {
                return fSelEnd;
            }
        }
    }
}

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif

#endif /*_Stroika_Frameworks_Led_StandardStyledTextInteractor_h_*/
