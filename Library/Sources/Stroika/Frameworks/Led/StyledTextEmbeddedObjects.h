/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_StyledTextEmbeddedObjects_h_
#define _Stroika_Frameworks_Led_StyledTextEmbeddedObjects_h_ 1

#include "../StroikaPreComp.h"

/*
@MODULE:    StyledTextEmbeddedObjects
@DESCRIPTION:
        <p>This module introduces a whole bunch of classes which leverage the Led @'Marker', and particularly the
    Led 'StyledTextImager::StyleMarker' mechanism - to implement visual object embedding within the text.</p>
        <p>The objects we embed are of type @'SimpleEmbeddedObjectStyleMarker'. But this module also contains other classes
    to support the creation of these embedding markers, to get them properly inserted into the text. And this module
    contains many @'SimpleEmbeddedObjectStyleMarker' subclasses, such as @'StandardDIBStyleMarker', which utilize this
    mechanism to display a particular data type as an emebdding.</p>
 */

#include <memory>

#if qPlatform_MacOS
#include <Processes.h> // for URL support
#endif

#include "StyledTextImager.h"
#include "TextInteractor.h"

namespace Stroika::Frameworks::Led {

    using Led_PrivateEmbeddingTag = char[10];

/*
        *  qOverrideDefaultSentinalValue allows you to use a different value for the sentinal than
        *  the default of zero. NB, that zero is a good default choice because it isn't a valid
        *  character code, and nearly all other (if not all other) choices are valid characters
        *  which you might be confusing with a sentinal.
        */
#if qOverrideDefaultSentinalValue
    const Led_tChar kEmbeddingSentinalChar = qOverrideDefaultSentinalValue;
#else
    const Led_tChar kEmbeddingSentinalChar = '\0';
#endif

    // Its up to you in your application for each one of these you want to support to add a line such
    // as:
    //
    //  EmbeddedObjectCreatorRegistry::Get ().AddAssoc (CL::PICT, StandardMacPictureStyleMarker::Creator);
    //
    //      For each of the marker types you want automatically taken care of. All the default ones,
    //  are added automatlicly by default. If you don't like these, you can use SetAssocList to replace
    //  them.
    //
    //
    class SimpleEmbeddedObjectStyleMarker;

    /*
    @CLASS:         EmbeddedObjectCreatorRegistry
    @DESCRIPTION:
            <p>Class which manages the registry of mappings from clipboard and Led private file format magic tag strings,
        to function pointers capable of reading these objects from RAM or a flavor package.</p>
    */
    class EmbeddedObjectCreatorRegistry {
    public:
        EmbeddedObjectCreatorRegistry ();

    public:
        static EmbeddedObjectCreatorRegistry& Get (); // build one if doesn't exist yet, and return ref to it
    private:
    public: //tmphack - til I can fix the code that references this... -- LGP 2012-09-11
        static EmbeddedObjectCreatorRegistry* sThe;

    public:
        struct Assoc {
        private:
            union {
                Led_ClipFormat        fFormatTag;  // use if fFormatTagCount==1
                const Led_ClipFormat* fFormatTags; // use if fFormatTagCount>1
            };

        public:
            size_t                  fFormatTagCount;
            Led_PrivateEmbeddingTag fEmbeddingTag;
            SimpleEmbeddedObjectStyleMarker* (*fReadFromMemory) (const char* embeddingTag, const void* data, size_t len);
            SimpleEmbeddedObjectStyleMarker* (*fReadFromFlavorPackage) (ReaderFlavorPackage& flavorPackage);

        public:
            nonvirtual Led_ClipFormat GetIthFormat (size_t i) const;

        private:
            friend class EmbeddedObjectCreatorRegistry;
        };

    public:
        nonvirtual void  AddAssoc (Assoc assoc);
        nonvirtual void  AddAssoc (const char* embeddingTag,
                                   SimpleEmbeddedObjectStyleMarker* (*memReader) (const char* embeddingTag, const void* data, size_t len));
        nonvirtual void  AddAssoc (Led_ClipFormat clipFormat, const char* embeddingTag,
                                   SimpleEmbeddedObjectStyleMarker* (*memReader) (const char* embeddingTag, const void* data, size_t len),
                                   SimpleEmbeddedObjectStyleMarker* (*packageReader) (ReaderFlavorPackage& flavorPackage));
        nonvirtual void  AddAssoc (const Led_ClipFormat* clipFormats, size_t clipFormatCount, const char* embeddingTag,
                                   SimpleEmbeddedObjectStyleMarker* (*memReader) (const char* embeddingTag, const void* data, size_t len),
                                   SimpleEmbeddedObjectStyleMarker* (*packageReader) (ReaderFlavorPackage& flavorPackage));
        nonvirtual const vector<Assoc>& GetAssocList () const;
        nonvirtual void                 SetAssocList (const vector<Assoc>& assocList);
        nonvirtual void                 AddStandardTypes ();
        nonvirtual bool                 Lookup (const char* embeddingTag, Assoc* result) const;

    private:
        vector<Assoc> fAssocList;

        friend struct FooBarBlatzRegistryCleanupHack;
    };

    const Led_Size kDefaultEmbeddingMargin = Led_Size (4, 4); // space left around embedding so you can see selected.
    // Not needed for all embeddings, but often helpful
    /*
    @CLASS:         SimpleEmbeddedObjectStyleMarker
    @BASES:         @'StyledTextImager::StyleMarker'
    @DESCRIPTION:   <p>An abstract class which contains most of the functionality for supporting Led embeddings in a Styled Text
        document.</p>
            <p>All the various kinds of embeddings Led supports are subclasses of this class.</p>
            <p>See @'InsertEmbeddingForExistingSentinal' and @'AddEmbedding' for more details on how to add these markers to the
        text. These utilities are not <em>needed</em> - but can simplify the process of adding embeddings.</p>
            <p>Also, to understand more about how these work, see @'StyledTextImager::StyleMarker' for more details.</p>
            <p>(NEED LOTS MORE DETAILS - THIS IS IMPORTANT)</p>
    */
    class SimpleEmbeddedObjectStyleMarker : public StyledTextImager::StyleMarker {
    private:
        using inherited = StyledTextImager::StyleMarker;

    protected:
        SimpleEmbeddedObjectStyleMarker ();

    public:
        enum { eEmbeddedObjectPriority = eBaselinePriority + 100 };
        virtual int GetPriority () const override;

        // Subclasses simply must override DrawSegment and MeasureSegmentWidth/MeasureSegmentHeight
        // the rest we can default properly here
    public:
        virtual Led_Distance MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const override;

    public:
        virtual void DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo) noexcept override;

        // Native clip format writing support
    public:
        class SinkStream {
        public:
            virtual void write (const void* buffer, size_t bytes) = 0;
        };
        virtual const char* GetTag () const                                         = 0;
        virtual void        Write (SinkStream& sink)                                = 0;
        virtual void        ExternalizeFlavors (WriterFlavorPackage& flavorPackage) = 0;

        // Click in embedding processing
        // HandleClick () by default, does nothing on single click, and on double click calls Open method - which in turn by
        // default does nothing. HandleClick returns false if it EATS the click. If it returns true if the click
        // should be handled as it normally would
    public:
        virtual bool HandleClick (Led_Point clickedAt, unsigned clickCount);
        virtual bool HandleOpen ();

    public:
        enum PrivateCmdNumber { eMinPrivateCmdNum = 1,
                                eOpenCmdNum       = eMinPrivateCmdNum,
                                eMaxPrivateCmdNum = 100 };
        virtual vector<PrivateCmdNumber> GetCmdNumbers () const;
        virtual bool                     IsCmdEnabled (PrivateCmdNumber cmd) const;
        virtual Led_SDK_String           GetCmdText (PrivateCmdNumber cmd);
        virtual void                     DoCommand (PrivateCmdNumber cmd);

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
    };

    /*
    @CLASS:         SimpleEmbeddedObjectStyleMarker::CommandNames
    @DESCRIPTION:   <p>Command names for each of the user-visible commands produced by the
                @'SimpleEmbeddedObjectStyleMarker' module.</p>
    */
    struct SimpleEmbeddedObjectStyleMarker::CommandNames {
        Led_SDK_String fOpenCommandName;
    };

#if qPlatform_MacOS || qPlatform_Windows
    /*
    @CLASS:         StandardMacPictureStyleMarker
    @BASES:         @'SimpleEmbeddedObjectStyleMarker'
    @DESCRIPTION:   <p>A Macintosh format picture embedding.</p>
    */
    class StandardMacPictureStyleMarker : public SimpleEmbeddedObjectStyleMarker {
    private:
        using inherited = SimpleEmbeddedObjectStyleMarker;

    public:
#if qPlatform_MacOS
        using PictureHandle = Led_Picture**;
#elif qPlatform_Windows
        using PictureHandle = HANDLE;
#endif

    public:
        static const Led_ClipFormat          kClipFormat;
        static const Led_PrivateEmbeddingTag kEmbeddingTag;

    public:
        StandardMacPictureStyleMarker (const Led_Picture* pictData, size_t picSize);
        ~StandardMacPictureStyleMarker ();

    public:
        static SimpleEmbeddedObjectStyleMarker* mk (const char* embeddingTag, const void* data, size_t len);
        static SimpleEmbeddedObjectStyleMarker* mk (ReaderFlavorPackage& flavorPackage);

#if qPlatform_Windows
        static const Led_DIB* sUnsupportedFormatPict; // Must be set externally by user of this class before we ever build one of these
                                                      // objects, or an assert error.
                                                      // Reason for this design is we need access to some pict resource, but we don't want
                                                      // Led to depend on any such things (would make build/distr/name conflicts etc
                                                      // more complex). So in main, if you ever plan to use these, then load resource and assign
                                                      // to this member.
#endif
    public:
        virtual void         DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
                                          size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
                                          Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn) override;
        virtual void         MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
                                                  const Led_tChar* text,
                                                  Led_Distance*    distanceResults) const override;
        virtual Led_Distance MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const override;

    public:
        virtual const char* GetTag () const override;
        virtual void        Write (SinkStream& sink) override;
        virtual void        ExternalizeFlavors (WriterFlavorPackage& flavorPackage) override;

    public:
        nonvirtual PictureHandle GetPictureHandle () const;
        nonvirtual size_t GetPictureByteSize () const;

    private:
        PictureHandle fPictureHandle;
#if qPlatform_Windows
        size_t fPictureSize;
#endif
    };
#endif

    /*
    @CLASS:         StandardDIBStyleMarker
    @BASES:         @'SimpleEmbeddedObjectStyleMarker'
    @DESCRIPTION:   <p>A Windows DIB (Device Independent Bitmap) picture embedding. Note that this can still be
                portably displayed on both Mac and Windows.</p>
    */
    class StandardDIBStyleMarker : public SimpleEmbeddedObjectStyleMarker {
    private:
        using inherited = SimpleEmbeddedObjectStyleMarker;

    public:
        static const Led_ClipFormat          kClipFormat;
        static const Led_PrivateEmbeddingTag kEmbeddingTag;

    public:
        StandardDIBStyleMarker (const Led_DIB* dibData);
        ~StandardDIBStyleMarker ();

    public:
        static SimpleEmbeddedObjectStyleMarker* mk (const char* embeddingTag, const void* data, size_t len);
        static SimpleEmbeddedObjectStyleMarker* mk (ReaderFlavorPackage& flavorPackage);

    public:
#if qPlatform_MacOS
        static Led_Picture** sUnsupportedFormatPict; // Must be set externally by user of this class before we ever build one of these
                                                     // objects, or an assert error.
                                                     // Reason for this design is we need access to some pict resource, but we don't want
                                                     // Led to depend on any such things (would make build/distr/name conflicts etc
                                                     // more complex). So in main, if you ever plan to use these, then load resource and assign
                                                     // to this member.
#endif

    public:
        virtual void         DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
                                          size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
                                          Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn) override;
        virtual void         MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
                                                  const Led_tChar* text,
                                                  Led_Distance*    distanceResults) const override;
        virtual Led_Distance MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const override;

    public:
        virtual const char* GetTag () const override;
        virtual void        Write (SinkStream& sink) override;
        virtual void        ExternalizeFlavors (WriterFlavorPackage& flavorPackage) override;

    public:
        nonvirtual const Led_DIB* GetDIBData () const;

    private:
        Led_DIB* fDIBData;
    };

/*
    @CONFIGVAR:     qURLStyleMarkerNewDisplayMode
    @DESCRIPTION:
            <p>The old display mode was somewhat idiosyncratic, and the new one is more like how other browsers
        display URLs. But keep the old one available in case some prefer it (at least for 3.0).</p>
            <p>Turn ON by default.</p>
        */
#ifndef qURLStyleMarkerNewDisplayMode
#define qURLStyleMarkerNewDisplayMode 1
#endif

    /*
    @CLASS:         StandardURLStyleMarker
    @BASES:         @'SimpleEmbeddedObjectStyleMarker'
    @DESCRIPTION:   <p>A URL object. Double click on this to open the URL.</p>
    */
    class StandardURLStyleMarker : public SimpleEmbeddedObjectStyleMarker {
    private:
        using inherited = SimpleEmbeddedObjectStyleMarker;

    public:
        static const Led_ClipFormat kURLDClipFormat;
#if qPlatform_Windows
        static const Led_ClipFormat kWin32URLClipFormat;
#endif
        static const Led_PrivateEmbeddingTag kEmbeddingTag;

        StandardURLStyleMarker (const Led_URLD& urlData);
        ~StandardURLStyleMarker ();

    public:
        static SimpleEmbeddedObjectStyleMarker* mk (const char* embeddingTag, const void* data, size_t len);
        static SimpleEmbeddedObjectStyleMarker* mk (ReaderFlavorPackage& flavorPackage);

#if !qURLStyleMarkerNewDisplayMode
    public:
#if qPlatform_MacOS
        static Led_Picture** sURLPict; // Must be set externally by user of this class before we ever build one of these
                                       // objects, or an assert error.
                                       // Reason for this design is we need access to some pict resource, but we don't want
                                       // Led to depend on any such things (would make build/distr/name conflicts etc
                                       // more complex). So in main, if you ever plan to use these, then load resource and assign
                                       // to this member.
#elif qPlatform_Windows
        static const Led_DIB* sURLPict;
#endif
#endif

    public:
        virtual void         DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
                                          size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
                                          Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn) override;
        virtual void         MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
                                                  const Led_tChar* text,
                                                  Led_Distance*    distanceResults) const override;
        virtual Led_Distance MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const override;

    public:
        virtual const char* GetTag () const override;
        virtual void        Write (SinkStream& sink) override;
        virtual void        ExternalizeFlavors (WriterFlavorPackage& flavorPackage) override;

    public:
        virtual bool HandleOpen () override;

    public:
        virtual vector<PrivateCmdNumber> GetCmdNumbers () const override;
        virtual bool                     IsCmdEnabled (PrivateCmdNumber cmd) const override;

    public:
        nonvirtual const Led_URLD& GetURLData () const;
        nonvirtual void            SetURLData (const Led_URLD& urlData);

    private:
        Led_URLD fURLData;

    protected:
        nonvirtual Led_tString GetDisplayString () const;
        nonvirtual Led_FontSpecification GetDisplayFont (const RunElement& runElement) const;
    };

#if qPlatform_MacOS || qPlatform_Windows
    class StandardMacPictureWithURLStyleMarker : public SimpleEmbeddedObjectStyleMarker {
    private:
        using inherited = SimpleEmbeddedObjectStyleMarker;

    public:
        static const Led_ClipFormat          kClipFormats[];
        static const size_t                  kClipFormatCount;
        static const Led_PrivateEmbeddingTag kEmbeddingTag;
        static const Led_PrivateEmbeddingTag kOld1EmbeddingTag;

    public:
        StandardMacPictureWithURLStyleMarker (const Led_Picture* pictData, size_t picSize, const Led_URLD& urlData);
        ~StandardMacPictureWithURLStyleMarker ();

    public:
        static SimpleEmbeddedObjectStyleMarker* mk (const char* embeddingTag, const void* data, size_t len);
        static SimpleEmbeddedObjectStyleMarker* mk (ReaderFlavorPackage& flavorPackage);

    public:
        virtual void         DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
                                          size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
                                          Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn) override;
        virtual void         MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
                                                  const Led_tChar* text,
                                                  Led_Distance*    distanceResults) const override;
        virtual Led_Distance MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const override;

    public:
        virtual const char* GetTag () const override;
        virtual void        Write (SinkStream& sink) override;
        virtual void        ExternalizeFlavors (WriterFlavorPackage& flavorPackage) override;

    public:
        virtual bool HandleOpen () override;

    public:
        virtual vector<PrivateCmdNumber> GetCmdNumbers () const override;
        virtual bool                     IsCmdEnabled (PrivateCmdNumber cmd) const override;

    private:
        Led_URLD fURLData;

    public:
        nonvirtual StandardMacPictureStyleMarker::PictureHandle GetPictureHandle () const;
        nonvirtual size_t GetPictureByteSize () const;
        nonvirtual const Led_URLD& GetURLData () const;
        nonvirtual void            SetURLData (const Led_URLD& urlData);

    private:
        StandardMacPictureStyleMarker::PictureHandle fPictureHandle;
#if qPlatform_Windows
        size_t fPictureSize;
#endif
    };
#endif

    class StandardDIBWithURLStyleMarker : public SimpleEmbeddedObjectStyleMarker {
    private:
        using inherited = SimpleEmbeddedObjectStyleMarker;

    public:
        static const Led_ClipFormat          kClipFormats[];
        static const size_t                  kClipFormatCount;
        static const Led_PrivateEmbeddingTag kEmbeddingTag;

    public:
        StandardDIBWithURLStyleMarker (const Led_DIB* dibData, const Led_URLD& urlData);
        ~StandardDIBWithURLStyleMarker ();

    public:
        static SimpleEmbeddedObjectStyleMarker* mk (const char* embeddingTag, const void* data, size_t len);
        static SimpleEmbeddedObjectStyleMarker* mk (ReaderFlavorPackage& flavorPackage);

    public:
        virtual void         DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
                                          size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
                                          Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn) override;
        virtual void         MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
                                                  const Led_tChar* text,
                                                  Led_Distance*    distanceResults) const override;
        virtual Led_Distance MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const override;

    public:
        virtual const char* GetTag () const override;
        virtual void        Write (SinkStream& sink) override;
        virtual void        ExternalizeFlavors (WriterFlavorPackage& flavorPackage) override;

    public:
        virtual bool HandleOpen () override;

    public:
        virtual vector<PrivateCmdNumber> GetCmdNumbers () const override;
        virtual bool                     IsCmdEnabled (PrivateCmdNumber cmd) const override;

    public:
        nonvirtual const Led_DIB* GetDIBData () const;

    private:
        Led_DIB* fDIBData;

    public:
        nonvirtual const Led_URLD& GetURLData () const;
        nonvirtual void            SetURLData (const Led_URLD& urlData);

    private:
        Led_URLD fURLData;
    };

    /*
    @CLASS:         StandardUnknownTypeStyleMarker
    @BASES:         @'SimpleEmbeddedObjectStyleMarker'
    @DESCRIPTION:   <p>Use to represent visually embeddings which we've read from a file, and want to keep track of, but
                don't know how to display.</p>
    */
    class StandardUnknownTypeStyleMarker : public SimpleEmbeddedObjectStyleMarker {
    private:
        using inherited = SimpleEmbeddedObjectStyleMarker;

    public:
        static const Led_PrivateEmbeddingTag kDefaultEmbeddingTag;

    public:
        StandardUnknownTypeStyleMarker (Led_ClipFormat format, const char* embeddingTag, const void* unknownTypeData, size_t nBytes, const Led_DIB* dib = nullptr);
        ~StandardUnknownTypeStyleMarker ();

    public:
#if qPlatform_MacOS
        static Led_Picture** sUnknownPict; // Must be set externally by user of this class before we ever build one of these
                                           // objects, or an assert error.
                                           // Reason for this design is we need access to some pict resource, but we don't want
                                           // Led to depend on any such things (would make build/distr/name conflicts etc
                                           // more complex). So in main, if you ever plan to use these, then load resource and assign
                                           // to this member.
#elif qPlatform_Windows
        static const Led_DIB* sUnknownPict;
#endif

    public:
        virtual void         DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
                                          size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
                                          Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn) override;
        virtual void         MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
                                                  const Led_tChar* text,
                                                  Led_Distance*    distanceResults) const override;
        virtual Led_Distance MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const override;

    public:
        virtual const char* GetTag () const override;
        virtual void        Write (SinkStream& sink) override;
        virtual void        ExternalizeFlavors (WriterFlavorPackage& flavorPackage) override;

    public:
        nonvirtual Led_TWIPS_Point GetShownSize () const;
        nonvirtual void            SetShownSize (Led_TWIPS_Point size);
        nonvirtual Led_TWIPS_Point CalcDefaultShownSize ();
        static Led_TWIPS_Point     CalcStaticDefaultShownSize ();

    private:
        Led_TWIPS_Point fShownSize;

    public:
        nonvirtual const void* GetData () const;
        nonvirtual size_t GetDataLength () const;

    private:
        void*                   fData;
        size_t                  fLength;
        Led_ClipFormat          fFormat;
        Led_PrivateEmbeddingTag fEmbeddingTag;
        unique_ptr<Led_DIB>     fDisplayDIB;
    };

    /*
    @METHOD:        InsertEmbeddingForExistingSentinal
    @DESCRIPTION:   <p>Utility method to insert the given embedding (@'SimpleEmbeddedObjectStyleMarker') into the given
                @'TextStore', at a given position in the text. To use this routine, the sentinal character must already
                be present. Use @'AddEmbedding' if the sentinal has not yet been added.</p>
    */
    void InsertEmbeddingForExistingSentinal (SimpleEmbeddedObjectStyleMarker* embedding, TextStore& textStore, size_t insertAt, MarkerOwner* ownerForEmbedding);

    /*
    @METHOD:        AddEmbedding
    @DESCRIPTION:   <p>Utility method to insert the given embedding (@'SimpleEmbeddedObjectStyleMarker') into the given
                @'TextStore', at a given position in the text. This routine, adds a sentinal character at that position. If
                the sentinal character is already there, use @'InsertEmbeddingForExistingSentinal'.</p>
                    <p>Actually, this function isn't strictly needed. But there is alot of mumbo-jumbo that needs to be
                done when adding embeddings (e.g. DoAboutToUpdateCalls etc), and this packages up all those things and
                avoids you from having todo them in a bunch of places.</p>
    */
    void AddEmbedding (SimpleEmbeddedObjectStyleMarker* embedding, TextStore& textStore, size_t insertAt, MarkerOwner* ownerForEmbedding);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StyledTextEmbeddedObjects.inl"

#endif /*_Stroika_Frameworks_Led_StyledTextEmbeddedObjects_h_*/
