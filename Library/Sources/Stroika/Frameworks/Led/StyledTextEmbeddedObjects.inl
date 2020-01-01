/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_StyledTextEmbeddedObjects_inl_
#define _Stroika_Framework_Led_StyledTextEmbeddedObjects_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     *********************** EmbeddedObjectCreatorRegistry::Assoc *******************
     ********************************************************************************
     */
    inline Led_ClipFormat EmbeddedObjectCreatorRegistry::Assoc::GetIthFormat (size_t i) const
    {
        Assert (fFormatTagCount >= 1);
        Require (i < fFormatTagCount);
        return (fFormatTagCount == 1) ? fFormatTag : fFormatTags[i];
    }

    // class EmbeddedObjectCreatorRegistry
    inline EmbeddedObjectCreatorRegistry::EmbeddedObjectCreatorRegistry ()
        : fAssocList ()
    {
    }
    inline EmbeddedObjectCreatorRegistry& EmbeddedObjectCreatorRegistry::Get ()
    {
        if (sThe == nullptr) {
            sThe = new EmbeddedObjectCreatorRegistry ();
        }
        return *sThe;
    }
    inline void EmbeddedObjectCreatorRegistry::AddAssoc (Assoc assoc)
    {
        fAssocList.push_back (assoc);
    }
    inline void EmbeddedObjectCreatorRegistry::AddAssoc (const char* embeddingTag,
                                                         SimpleEmbeddedObjectStyleMarker* (*memReader) (const char* embeddingTag, const void* data, size_t len))
    {
        Assoc assoc;
        assoc.fFormatTagCount = 0;
        memcpy (assoc.fEmbeddingTag, embeddingTag, sizeof (assoc.fEmbeddingTag));
        assoc.fReadFromMemory        = memReader;
        assoc.fReadFromFlavorPackage = nullptr;
        AddAssoc (assoc);
    }
    inline void EmbeddedObjectCreatorRegistry::AddAssoc (Led_ClipFormat clipFormat, const char* embeddingTag,
                                                         SimpleEmbeddedObjectStyleMarker* (*memReader) (const char* embeddingTag, const void* data, size_t len),
                                                         SimpleEmbeddedObjectStyleMarker* (*packageReader) (ReaderFlavorPackage& flavorPackage))
    {
        Assoc assoc;
        assoc.fFormatTag      = clipFormat;
        assoc.fFormatTagCount = 1;
        memcpy (assoc.fEmbeddingTag, embeddingTag, sizeof (assoc.fEmbeddingTag));
        assoc.fReadFromMemory        = memReader;
        assoc.fReadFromFlavorPackage = packageReader;
        AddAssoc (assoc);
    }
    inline void EmbeddedObjectCreatorRegistry::AddAssoc (const Led_ClipFormat* clipFormats, size_t clipFormatCount, const char* embeddingTag,
                                                         SimpleEmbeddedObjectStyleMarker* (*memReader) (const char* embeddingTag, const void* data, size_t len),
                                                         SimpleEmbeddedObjectStyleMarker* (*packageReader) (ReaderFlavorPackage& flavorPackage))
    {
        Assoc assoc;
        assoc.fFormatTags     = clipFormats;
        assoc.fFormatTagCount = clipFormatCount;
        memcpy (assoc.fEmbeddingTag, embeddingTag, sizeof (assoc.fEmbeddingTag));
        assoc.fReadFromMemory        = memReader;
        assoc.fReadFromFlavorPackage = packageReader;
        AddAssoc (assoc);
    }
    inline const vector<EmbeddedObjectCreatorRegistry::Assoc>& EmbeddedObjectCreatorRegistry::GetAssocList () const
    {
        return fAssocList;
    }
    inline void EmbeddedObjectCreatorRegistry::SetAssocList (const vector<Assoc>& assocList)
    {
        fAssocList = assocList;
    }

    //  class   SimpleEmbeddedObjectStyleMarker
    /*
    @METHOD:        SimpleEmbeddedObjectStyleMarker::GetCommandNames
    @DESCRIPTION:   <p>Returns command name for each of the user-visible commands produced by this module.</p>
            <p>See also @'TextInteractor::CommandNames'.</p>
    */
    inline const SimpleEmbeddedObjectStyleMarker::CommandNames& SimpleEmbeddedObjectStyleMarker::GetCommandNames ()
    {
        return sCommandNames;
    }
    /*
    @METHOD:        SimpleEmbeddedObjectStyleMarker::SetCommandNames
    @DESCRIPTION:   <p>See @'SimpleEmbeddedObjectStyleMarker::GetCommandNames'.</p>
    */
    inline void SimpleEmbeddedObjectStyleMarker::SetCommandNames (const SimpleEmbeddedObjectStyleMarker::CommandNames& cmdNames)
    {
        sCommandNames = cmdNames;
    }

#if qPlatform_MacOS || qPlatform_Windows
    // class StandardMacPictureStyleMarker
    inline StandardMacPictureStyleMarker::PictureHandle StandardMacPictureStyleMarker::GetPictureHandle () const
    {
        EnsureNotNull (fPictureHandle);
        return fPictureHandle;
    }
    inline size_t StandardMacPictureStyleMarker::GetPictureByteSize () const
    {
#if qPlatform_MacOS
        return ::GetHandleSize (Handle (fPictureHandle));
#elif qPlatform_Windows
        return fPictureSize; // cannot use ::GlobalSize () since that sometimes returns result larger than
                             // actual picture size (rounds up)
#endif
    }
#endif

    // class StandardDIBStyleMarker
    inline const Led_DIB* StandardDIBStyleMarker::GetDIBData () const
    {
        EnsureNotNull (fDIBData);
        return (fDIBData);
    }

#if qPlatform_MacOS || qPlatform_Windows
    // class StandardMacPictureWithURLStyleMarker
    inline StandardMacPictureStyleMarker::PictureHandle StandardMacPictureWithURLStyleMarker::GetPictureHandle () const
    {
        EnsureNotNull (fPictureHandle);
        return fPictureHandle;
    }
    inline size_t StandardMacPictureWithURLStyleMarker::GetPictureByteSize () const
    {
#if qPlatform_MacOS
        return ::GetHandleSize (Handle (fPictureHandle));
#elif qPlatform_Windows
        return fPictureSize; // cannot use ::GlobalSize () since that sometimes returns result larger than
                             // actual picture size (rounds up)
#endif
    }
#endif

    // class StandardDIBWithURLStyleMarker
    inline const Led_DIB* StandardDIBWithURLStyleMarker::GetDIBData () const
    {
        EnsureNotNull (fDIBData);
        return (fDIBData);
    }

    // class StandardUnknownTypeStyleMarker
    /*
    @METHOD:        StandardUnknownTypeStyleMarker::GetShownSize
    @DESCRIPTION:   <p>Return the size in TWIPS
                of this embeddings display. Defaults to some size appropriate for the picture drawn. But sometimes
                (like in reading RTF files which contain size annotations), we select an appropriate size.</p>
                    <p>See @'StandardUnknownTypeStyleMarker::SetShownSize'
    */
    inline Led_TWIPS_Point StandardUnknownTypeStyleMarker::GetShownSize () const
    {
        return fShownSize;
    }
    inline const void* StandardUnknownTypeStyleMarker::GetData () const
    {
        return fData;
    }
    inline size_t StandardUnknownTypeStyleMarker::GetDataLength () const
    {
        return fLength;
    }

}

#endif /*_Stroika_Framework_Led_StyledTextEmbeddedObjects_inl_*/
