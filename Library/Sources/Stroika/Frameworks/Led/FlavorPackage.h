/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_FlavorPackage_h_
#define _Stroika_Frameworks_Led_FlavorPackage_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

/*
@MODULE:    FlavorPackage
@DESCRIPTION:
        <p>This module defines the packaged up contents of serialized data. This data is usually used to exchange
    in drag and drop, and clipboard operations. But its sometimes used/saved for other purposes, like UNDO and
    hidable text.
 */

#include <map>
#include <vector>

#include "Stroika/Foundation/Characters/CodePage.h"

#include "Command.h"
#include "TextImager.h"

namespace Stroika::Frameworks::Led {

    using Foundation::Characters::CodePage;
    using Foundation::Characters::SDKChar;

    class ReaderFlavorPackage;
    class WriterFlavorPackage;

    /**
     *      <p>Helper class for implementing externalizing. Can be subclassed to add new formats.
     *  Call it with a @'WriterFlavorPackage', and a range to copy from, and the externalizing will be done.</p>
     *      <p>See also @'FlavorPackageInternalizer'.</p>
     */
    class FlavorPackageExternalizer : public virtual MarkerOwner {
    private:
        using inherited = MarkerOwner;

    public:
        FlavorPackageExternalizer (TextStore& ts);

    public:
        virtual TextStore* PeekAtTextStore () const override;

    public:
        virtual void ExternalizeFlavors (WriterFlavorPackage& flavorPackage, size_t from, size_t to);
        virtual void ExternalizeBestFlavor (WriterFlavorPackage& flavorPackage, size_t from, size_t to);
        virtual void ExternalizeFlavor_TEXT (WriterFlavorPackage& flavorPackage, size_t from, size_t to);

    private:
        TextStore& fTextStore;
    };

    /**
     *      <p>Helper class for implementing internalizing. Can be subclassed to add new formats.
     *  Call it with a @'ReaderFlavorPackage', and a range to insert it into, and the internalizing will be done.</p>
     *      <p>See also @'FlavorPackageExternalizer'.</p>
     */
    class FlavorPackageInternalizer : public virtual MarkerOwner {
    private:
        using inherited = MarkerOwner;

    public:
        FlavorPackageInternalizer (TextStore& ts);

    public:
        virtual TextStore* PeekAtTextStore () const override;

    public:
        virtual bool    InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage, size_t from, size_t to);
        nonvirtual bool InternalizeFlavor_TEXT (ReaderFlavorPackage& flavorPackage, size_t from, size_t to);
        virtual bool    InternalizeFlavor_FILE (ReaderFlavorPackage& flavorPackage, size_t from, size_t to);
        virtual bool    InternalizeFlavor_FILEData (filesystem::path fileName, Led_ClipFormat* suggestedClipFormat,
                                                    optional<CodePage> suggestedCodePage, size_t from, size_t to);
        virtual void    InternalizeFlavor_FILEGuessFormatsFromName (filesystem::path fileName, Led_ClipFormat* suggestedClipFormat,
                                                                    optional<CodePage> suggestedCodePage);
        virtual void InternalizeFlavor_FILEGuessFormatsFromStartOfData (Led_ClipFormat* suggestedClipFormat, optional<CodePage> suggestedCodePage,
                                                                        const byte* fileStart, const byte* fileEnd);
        virtual bool InternalizeFlavor_FILEDataRawBytes (Led_ClipFormat* suggestedClipFormat, optional<CodePage> suggestedCodePage,
                                                         size_t from, size_t to, const void* rawBytes, size_t nRawBytes);

    private:
        TextStore& fTextStore;
    };

    /**
     *  Abstraction wrapping both Drag&Drop packages, and clipboard access. Used by @'FlavorPackageInternalizer'.</p>
     */
    class ReaderFlavorPackage {
    public:
        nonvirtual bool GetFlavorAvailable_TEXT () const;
        virtual bool    GetFlavorAvailable (Led_ClipFormat clipFormat) const = 0;

    public:
        /** 
         * Return an upper bound on the size of the given flavor element. Try to get the right size, but
         * the OS frequently makes this impossible. Just garuantee that when you do a ReaderFlavorPackage::ReadFlavorData
         * you get the right size, and that is smaller or equal to what this returns.</p>
         */
        virtual size_t GetFlavorSize (Led_ClipFormat clipFormat) const = 0;

    public:
        /*
        @METHOD:        ReaderFlavorPackage::ReadFlavorData
        @DESCRIPTION:   <p>Return the data of a given clip format, copied into the passed in buffer. The caller must allocate/free
            the buffer. An upper bound on the size needed for the buffer can be retrieved with @'ReaderFlavorPackage::GetFlavorSize'.</p>
        */
        virtual size_t ReadFlavorData (Led_ClipFormat clipFormat, size_t bufSize, void* buf) const = 0;
    };

#if qStroika_Frameworks_Led_SupportClipboard
    /**
     *     <p>NB: On windows - it is REQUIRED the ClipboardFlavorPackage objects only be
     *  created in the context of Open/Close clipboard operations (for example done in
     *  OnPasteCommand_Before/OnPasteCommand_After - so typically no problem).</p>
     *      <p>NB: For X-Windows, the clip data is just stored in the global variable ReaderClipboardFlavorPackage::sPrivateClipData.</p>
     */
    class ReaderClipboardFlavorPackage : public ReaderFlavorPackage {
    public:
        virtual bool   GetFlavorAvailable (Led_ClipFormat clipFormat) const override;
        virtual size_t GetFlavorSize (Led_ClipFormat clipFormat) const override;
        virtual size_t ReadFlavorData (Led_ClipFormat clipFormat, size_t bufSize, void* buf) const override;
#if qStroika_FeatureSupported_XWindows
    public:
        static map<Led_ClipFormat, vector<char>> sPrivateClipData;
#endif
    };
#endif

    /**
     *    <p>Abstraction wrapping both Drag&Drop packages, and clipboard access. Used by @'FlavorPackageExternalizer'.</p>
     */
    class WriterFlavorPackage {
    public:
        virtual void AddFlavorData (Led_ClipFormat clipFormat, size_t bufSize, const void* buf) = 0;
    };

#if qStroika_Frameworks_Led_SupportClipboard
    /**
     *      <p>NB: On windows - it is REQUIRED the ClipboardFlavorPackage objects only be
     *  created in the context of Open/Close clipboard operations (for example done in
     *  OnCopyCommand_Before/OnCopyCommand_After - so typically no problem).</p>
     *      <p>See also @'ReaderClipboardFlavorPackage'.</p>
     */
    class WriterClipboardFlavorPackage : public WriterFlavorPackage {
    public:
        virtual void AddFlavorData (Led_ClipFormat clipFormat, size_t bufSize, const void* buf) override;
    };
#endif

    /**
     *   <p>Dual purpose, store-and-reuse package. Useful for undo.</p>
     */
    class ReadWriteMemBufferPackage : public ReaderFlavorPackage, public WriterFlavorPackage {
    public:
        ReadWriteMemBufferPackage ();
        ~ReadWriteMemBufferPackage ();

        //  ReaderFlavorPackage
    public:
        virtual bool   GetFlavorAvailable (Led_ClipFormat clipFormat) const override;
        virtual size_t GetFlavorSize (Led_ClipFormat clipFormat) const override;
        virtual size_t ReadFlavorData (Led_ClipFormat clipFormat, size_t bufSize, void* buf) const override;

        //  WriterFlavorPackage
    public:
        virtual void AddFlavorData (Led_ClipFormat clipFormat, size_t bufSize, const void* buf) override;

    private:
        struct PackageRecord {
            Led_ClipFormat fFormat;
            vector<char>   fData;
        };
        vector<PackageRecord> fPackages;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FlavorPackage.inl"

#endif /*_Stroika_Frameworks_Led_FlavorPackage_h_*/
