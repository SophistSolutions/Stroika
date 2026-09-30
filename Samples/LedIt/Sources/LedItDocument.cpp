/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include <cctype>

#if defined(WIN32)

#include <afxwin.h>
#endif

#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Frameworks/Led/SpellCheckEngine_Basic.h"
#include "Stroika/Frameworks/Led/StyledTextIO/StyledTextIO_LedNative.h"
#include "Stroika/Frameworks/Led/StyledTextIO/StyledTextIO_PlainText.h"

#if qStroika_Platform_Windows
#include "LedItControlItem.h"
#include "LedItServerItem.h"
#endif
#include "LedItView.h"
#include "Options.h"

#include "LedItApplication.h"

#include "LedItDocument.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::Platform;
using namespace Stroika::Frameworks::Led::StyledTextIO;

using Memory::MakeSharedPtr;
using Memory::StackBuffer;

#if qStroika_Platform_Windows
// special exception handling just for MFC library implementation
// copied here so I could clone MFC code as needed - not well understood - UGH!!! - LGP 951227
#ifndef _AFX_OLD_EXCEPTIONS
#define DELETE_EXCEPTION(e)                                                                                                                \
    do {                                                                                                                                   \
        e->Delete ();                                                                                                                      \
    } while (0)
#else //!_AFX_OLD_EXCEPTIONS
#define DELETE_EXCEPTION(e)
#endif //_AFX_OLD_EXCEPTIONS

static void AppendFilterSuffix (CString& filter, OPENFILENAME& ofn, CString strFilterExt, CString strFilterName);
static void AppendFilterSuffix (CString& filter, OPENFILENAME& ofn, CDocTemplate* pTemplate);

#endif

/*
 ********************************************************************************
 ******************************** LedItDocument *********************************
 ********************************************************************************
 */
#if qStroika_Platform_Windows
FileFormat LedItDocument::sHiddenDocOpenArg = eUnknownFormat; // See LedItDocument::OnOpenDocument ()

IMPLEMENT_DYNCREATE (LedItDocument, COleServerDoc)

BEGIN_MESSAGE_MAP (LedItDocument, COleServerDoc)
ON_UPDATE_COMMAND_UI (ID_EDIT_PASTE_LINK, OnUpdatePasteLinkMenu)
ON_UPDATE_COMMAND_UI (ID_OLE_EDIT_CONVERT, OnUpdateObjectVerbMenu)
ON_COMMAND (ID_OLE_EDIT_CONVERT, OnEditConvert)
ON_UPDATE_COMMAND_UI (ID_OLE_EDIT_LINKS, OnUpdateEditLinksMenu)
ON_COMMAND (ID_OLE_EDIT_LINKS, OnEditLinks)
ON_UPDATE_COMMAND_UI (ID_OLE_VERB_FIRST, OnUpdateObjectVerbMenu)
ON_UPDATE_COMMAND_UI (ID_FILE_SAVE, OnUpdateFileSave)
ON_COMMAND (ID_FILE_SAVE_COPY_AS, OnFileSaveCopyAs)
END_MESSAGE_MAP ()

BEGIN_DISPATCH_MAP (LedItDocument, COleServerDoc)
END_DISPATCH_MAP ()

// Note: we add support for IID_ILedIt to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the
//  dispinterface in the .ODL file.

// {0FC00622-28BD-11CF-899C-00AA00580324}
static const IID IID_ILedIt = {0xfc00622, 0x28bd, 0x11cf, {0x89, 0x9c, 0x0, 0xaa, 0x0, 0x58, 0x3, 0x24}};

BEGIN_INTERFACE_MAP (LedItDocument, COleServerDoc)
INTERFACE_PART (LedItDocument, IID_ILedIt, Dispatch)
END_INTERFACE_MAP ()

#endif

#if qStroika_Platform_Windows
LedItDocument::LedItDocument ()
    : COleServerDoc ()
    ,
#endif
    MarkerOwner ()
    , fTextStore ()
    , fRTFInfo ()
    , fStyleDatabase ()
    , fParagraphDatabase ()
    , fHidableTextDatabase ()
    , fCommandHandler (kMaxNumUndoLevels)
    ,
#if qStroika_Platform_Windows
    fFileFormat (eDefaultFormat)
    ,
#endif
    fHTMLInfo ()
{
#if qStroika_Platform_Windows
    EnableAutomation ();
    ::AfxOleLockApp ();
#endif
    fTextStore.AddMarkerOwner (this);
    fStyleDatabase       = MakeSharedPtr<StyleDatabaseRep> (fTextStore);
    fParagraphDatabase   = MakeSharedPtr<ParagraphDatabaseRep> (fTextStore);
    fHidableTextDatabase = MakeSharedPtr<UniformHidableTextMarkerOwner> (fTextStore);
}

LedItDocument::~LedItDocument ()
{
    fTextStore.RemoveMarkerOwner (this);
#if qStroika_Platform_Windows
    ::AfxOleUnlockApp ();
#endif
}

void LedItDocument::DidUpdateText (const UpdateInfo& updateInfo) noexcept
{
    if (updateInfo.fRealContentUpdate) {
#if qStroika_Platform_Windows
        SetModifiedFlag ();
#endif
    }
}

TextStore* LedItDocument::PeekAtTextStore () const
{
    return &const_cast<LedItDocument*> (this)->fTextStore;
}

#if qStroika_Platform_Windows
BOOL LedItDocument::OnNewDocument ()
{
    fCommandHandler.Commit ();
    if (!COleServerDoc::OnNewDocument ()) {
        return FALSE;
    }
    fFileFormat          = eDefaultFormat;
    fStyleDatabase       = MakeSharedPtr<StyleDatabaseRep> (fTextStore);
    fParagraphDatabase   = MakeSharedPtr<ParagraphDatabaseRep> (fTextStore);
    fHidableTextDatabase = MakeSharedPtr<UniformHidableTextMarkerOwner> (fTextStore);
    return TRUE;
}

COleServerItem* LedItDocument::OnGetEmbeddedItem ()
{
    // OnGetEmbeddedItem is called by the framework to get the COleServerItem
    //  that is associated with the document.  It is only called when necessary.
    LedItServerItem* pItem = new LedItServerItem (this);
    ASSERT_VALID (pItem);
    return pItem;
}

BOOL LedItDocument::DoSave (LPCTSTR lpszPathName, BOOL bReplace)
{
    FileFormat fileFormat = fFileFormat;

    CString newName = lpszPathName;
    if (newName.IsEmpty ()) {
        CDocTemplate* pTemplate = GetDocTemplate ();
        ASSERT (pTemplate != NULL);
        newName = m_strPathName;
        if (bReplace && newName.IsEmpty ()) {
            newName = m_strTitle;
            // check for dubious filename
            int iBad = newName.FindOneOf (_T(" #%;/\\"));
            if (iBad != -1) {
                newName.ReleaseBuffer (iBad);
            }
#if 0
            // append the default suffix if there is one
            CString strExt;
            if (pTemplate->GetDocString(strExt, CDocTemplate::filterExt) &&
                    !strExt.IsEmpty()) {
                ASSERT(strExt[0] == '.');
                newName += strExt;
            }
#endif
        }

        if (bReplace) {
            if (not DoPromptSaveAsFileName (newName, &fileFormat)) {
                return false;
            }
        }
        else {
            if (not DoPromptSaveCopyAsFileName (newName, &fileFormat)) {
                return false;
            }
        }
    }

    CWaitCursor wait;

    // During the actual save, we must temporarily reset the fFileFormat field so we
    // know what format to write. Don't make the change permanent til much later, when the
    // write has succeeded, and we know that this was a save, and not a save-a-copy call.
    FileFormat realSavedDocFormat = fFileFormat;
    fFileFormat                   = fileFormat;
    try {
        if (!OnSaveDocument (newName)) {
            if (lpszPathName == NULL) {
                // be sure to delete the file
                TRY
                {
                    CFile::Remove (newName);
                }
                CATCH_ALL (e)
                {
                    TRACE0 ("Warning: failed to delete file after failed SaveAs.\n");
                    DELETE_EXCEPTION (e);
                }
                END_CATCH_ALL
            }
            return FALSE;
        }
    }
    catch (...) {
        fFileFormat = realSavedDocFormat;
        throw;
    }
    fFileFormat = realSavedDocFormat;

    // reset the title and change the document name
    if (bReplace) {
        SetPathName (newName);
        fFileFormat = fileFormat;
    }

    return TRUE; // success
}

void LedItDocument::Serialize (CArchive& ar)
{
    if (ar.IsStoring ()) {
        Require (fFileFormat != eUnknownFormat); // We must have chosen a file format by now...

        WordProcessorTextIOSrcStream        source{&fTextStore, fStyleDatabase, fParagraphDatabase, fHidableTextDatabase};
        StyledTextIOWriterSinkStream_Memory sink;

        switch (fFileFormat) {
            case eTextFormat: {
                StyledTextIOWriter_PlainText textWriter{&source, &sink};
                textWriter.Write ();
            } break;

            case eRTFFormat: {
                StyledTextIOWriter_RTF textWriter{&source, &sink, &fRTFInfo};
                textWriter.Write ();
            } break;

            case eHTMLFormat: {
                StyledTextIOWriter_HTML textWriter{&source, &sink, &fHTMLInfo};
                textWriter.Write ();
            } break;

            case eLedPrivateFormat: {
                StyledTextIOWriter_LedNativeFileFormat textWriter{&source, &sink};
                textWriter.Write ();
            } break;

            default: {
                Assert (false); // don't support writing that format (yet?)!
            } break;
        }
        ar.Write ((char*)sink.PeekAtData (), static_cast<UINT> (sink.GetLength ()));
    }
    else {
        // NOTE - AS OF CHANGE FOR SPR#1552- this code for READING docs is probably not called
        // anymore. Should get eliminated/cleaned up in some future release...
        // -- LGP 2003-09-22
        {
            // Peculiar performance hack. Lots of parts of Led will do 'Refresh' to mark the
            // screen as needing redisplay. If done enuf times, these can be expensive. So Led is
            // 'clever' - and notices that if the entire screen is invalid, in need not do the elaborate
            // computations about what needs further invalidating. This initial call to Refresh () call
            // won't REALLY end up doing any extra redisplay, cuz we're reading in a new file, and would be
            // redisplaying the whole window anyhow. But it MIGHT save us a bit of time not calculating what
            // little bits of the screen to redisplay.
            // LGP 970619
            POSITION pos = GetFirstViewPosition ();
            for (CView* p = GetNextView (pos); p != NULL; p = GetNextView (pos)) {
                LedItView* v = dynamic_cast<LedItView*> (p);
                if (v != NULL) {
                    v->Refresh ();
                }
            }
        }
        CFile* file = ar.GetFile ();
        ASSERT_VALID (file);
        DWORD             nLen = static_cast<DWORD> (file->GetLength ()); // maybe should subtract current offset?
        StackBuffer<char> buf{Memory::eUninitialized, nLen};
        if (ar.Read (buf.data (), nLen) != nLen) {
            AfxThrowArchiveException (CArchiveException::endOfFile);
        }
        StyledTextIOSrcStream_Memory  source{buf.data (), nLen};
        WordProcessorTextIOSinkStream sink{&fTextStore, fStyleDatabase, fParagraphDatabase, fHidableTextDatabase};

    ReRead:
        switch (fFileFormat) {
            case eTextFormat: {
                StyledTextIOReader_PlainText textReader{&source, &sink};
                textReader.Read ();
            } break;

            case eLedPrivateFormat: {
                LedItControlItem::DocContextDefiner    tmp{this};
                StyledTextIOReader_LedNativeFileFormat textReader{&source, &sink};
                textReader.Read ();
            } break;

            case eRTFFormat: {
                LedItControlItem::DocContextDefiner tmp{this};
                StyledTextIOReader_RTF              textReader{&source, &sink, &fRTFInfo};
                textReader.Read ();
            } break;

            case eHTMLFormat: {
                StyledTextIOReader_HTML textReader{&source, &sink, &fHTMLInfo};
                textReader.Read ();
            } break;

            case eUnknownFormat: {
                /*
                 *  Should enhance this unknown/format reading code to take into account file suffix in our guess.
                 */

                // Try RTF
                try {
                    StyledTextIOReader_RTF reader{&source, &sink, &fRTFInfo};
                    if (reader.QuickLookAppearsToBeRightFormat ()) {
                        fFileFormat = eRTFFormat;
                        goto ReRead;
                    }
                }
                catch (...) {
                    // ignore any errors, and proceed to next file type
                }

                // Try LedNativeFileFormat
                try {
                    StyledTextIOReader_LedNativeFileFormat reader{&source, &sink};
                    if (reader.QuickLookAppearsToBeRightFormat ()) {
                        fFileFormat = eLedPrivateFormat;
                        goto ReRead;
                    }
                }
                catch (...) {
                    // ignore any errors, and proceed to next file type
                }

                // Try HTML
                try {
                    StyledTextIOReader_HTML reader{&source, &sink};
                    if (reader.QuickLookAppearsToBeRightFormat ()) {
                        fFileFormat = eHTMLFormat;
                        goto ReRead;
                    }
                }
                catch (...) {
                    // ignore any errors, and proceed to next file type
                }

                // Nothing left todo but to read the text file as plain text, as best we can...
                fFileFormat = eTextFormat;
                goto ReRead;
            } break;

            default: {
                Assert (false); // don't support reading that format (yet?)!
            } break;
        }
        sink.Flush (); // explicit Flush () call - DTOR would have done it - but there exceptions get silently eaten - this will at least show them...
    }
}

BOOL LedItDocument::OnOpenDocument (LPCTSTR lpszPathName)
{
    {
        // Peculiar performance hack. Lots of parts of Led will do 'Refresh' to mark the
        // screen as needing redisplay. If done enuf times, these can be expensive. So Led is
        // 'clever' - and notices that if the entire screen is invalid, in need not do the elaborate
        // computations about what needs further invalidating. This initial call to Refresh () call
        // won't REALLY end up doing any extra redisplay, cuz we're reading in a new file, and would be
        // redisplaying the whole window anyhow. But it MIGHT save us a bit of time not calculating what
        // little bits of the screen to redisplay.
        // LGP 970619
        POSITION pos = GetFirstViewPosition ();
        for (CView* p = GetNextView (pos); p != NULL; p = GetNextView (pos)) {
            LedItView* v = dynamic_cast<LedItView*> (p);
            if (v != NULL) {
                v->Refresh ();
            }
        }
    }

    fCommandHandler.Commit ();
    fStyleDatabase       = MakeSharedPtr<StyleDatabaseRep> (fTextStore);
    fParagraphDatabase   = MakeSharedPtr<ParagraphDatabaseRep> (fTextStore);
    fHidableTextDatabase = MakeSharedPtr<UniformHidableTextMarkerOwner> (fTextStore);

    // Slight performance hack - get rid of existing style/etc dbases for the current view
    {
        POSITION pos = GetFirstViewPosition ();
        while (pos != NULL) {
            CView* pView = GetNextView (pos);
            pView->OnInitialUpdate ();
        }
    }

    WordProcessor::WordProcessorFlavorPackageInternalizer internalizer (fTextStore, fStyleDatabase, fParagraphDatabase, fHidableTextDatabase);

    Led_ClipFormat readFileFormat = kBadClipFormat; // defaults to GUESSING file format on READ (or based on file name)

    /*
     *  Because we need to vector through a bunch of layers of MFC code which doesn't pass along this format
     *  inforamtion, we needed some hack to get it from where we knew the file type to here, where
     *  we open the file. The application/DocMgr code made it hard to get/propagate the file type info
     *  but we managed that. Were we dropped the ball was on the DocTemplate code. That looked too complex
     *  to clone/redo to pass this info along, so when we get to the point of saying template->OpenDocument()
     *  we first set this sHiddenDocOpenArg argument.
     */
    switch (sHiddenDocOpenArg) {
        case eTextFormat:
            readFileFormat = kTEXTClipFormat;
            break;
        case eLedPrivateFormat:
            readFileFormat = kLedPrivateClipFormat;
            break;
        case eRTFFormat:
            readFileFormat = kRTFClipFormat;
            break;
        case eHTMLFormat:
            readFileFormat = kHTMLClipFormat;
            break;
    }

    internalizer.InternalizeFlavor_FILEData (lpszPathName, &readFileFormat, NULL, 0, fTextStore.GetEnd ());

    // Set document file type based on the format READ from the disk...
    if (readFileFormat == kTEXTClipFormat) {
        fFileFormat = eTextFormat;
    }
    else if (readFileFormat == kLedPrivateClipFormat) {
        fFileFormat = eLedPrivateFormat;
    }
    else if (readFileFormat == kRTFClipFormat) {
        fFileFormat = eRTFFormat;
    }
    else if (readFileFormat == kHTMLClipFormat) {
        fFileFormat = eHTMLFormat;
    }
    else {
        // If we read some other format - then just set our file format to DEFAULT for the next write
        fFileFormat = eDefaultFormat;
    }

    SetModifiedFlag (FALSE); // start off with doc unmodified

    return true;
}

void LedItDocument::OnUpdateFileSave (CCmdUI* pCmdUI)
{
    ASSERT_VALID (this);
    RequireNotNull (pCmdUI);
    // only enable save command if dirty, or no file name associated with this document
    pCmdUI->Enable (IsModified () or GetPathName ().GetLength () == 0);
}

void LedItDocument::OnFileSaveCopyAs ()
{
    ASSERT_VALID (this);
    Assert (m_bRemember);

    LPSTORAGE savedStorage = m_lpRootStg;
    m_lpRootStg            = NULL;

    FileFormat savedFileFormat = fFileFormat;

    try {
        DoSave (NULL, false);
    }
    catch (...) {
        m_lpRootStg = savedStorage;
        m_bRemember = true;
        fFileFormat = savedFileFormat;
        throw;
    }

    m_lpRootStg = savedStorage;
    m_bRemember = true;
    fFileFormat = savedFileFormat;
}

void LedItDocument::DeleteContents ()
{
    fTextStore.Replace (fTextStore.GetStart (), fTextStore.GetEnd (), LED_TCHAR_OF (""), 0);
}

bool LedItDocument::DoPromptSaveAsFileName (CString& fileName, FileFormat* fileFormat)
{
    RequireNotNull (fileFormat);
    return DoPromptFileName (fileName, AFX_IDS_SAVEFILE, false, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, fileFormat);
}

bool LedItDocument::DoPromptSaveCopyAsFileName (CString& fileName, FileFormat* fileFormat)
{
    RequireNotNull (fileFormat);
    return DoPromptFileName (fileName, AFX_IDS_SAVEFILECOPY, false, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, fileFormat);
}

bool LedItDocument::DoPromptOpenFileName (CString& fileName, FileFormat* fileFormat)
{
    RequireNotNull (fileFormat);
    return DoPromptFileName (fileName, AFX_IDS_OPENFILE, true, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, fileFormat);
}

bool LedItDocument::DoPromptFileName (CString& fileName, UINT nIDSTitle, bool isOpenDialogCall, long fileDLogFlags, FileFormat* fileFormat)
{
    RequireNotNull (fileFormat);
    CFileDialog dlgFile{isOpenDialogCall};

    CString title;
    Verify (title.LoadString (nIDSTitle));

    dlgFile.m_ofn.Flags |= fileDLogFlags;

    CString strFilter;

    CDocTemplate* ledItPrivateDocFormatTemplate = NULL;
    {
        // do for all doc template
        POSITION pos                  = AfxGetApp ()->m_pDocManager->GetFirstDocTemplatePosition ();
        ledItPrivateDocFormatTemplate = AfxGetApp ()->m_pDocManager->GetNextDocTemplate (pos);
    }
    if (nIDSTitle == AFX_IDS_OPENFILE) {
        AppendFilterSuffix (strFilter, dlgFile.m_ofn, "*.htm;*.html;*.led;*.rtf;*.txt;", "All Recognized (*.htm;*.html;*.led;*.rtf;*.txt)");
        AppendFilterSuffix (strFilter, dlgFile.m_ofn, ".*", "All Files (*.*)");
    }
    AppendFilterSuffix (strFilter, dlgFile.m_ofn, "*.htm;*.html", "HTML file (*.htm;*.html)");
    AppendFilterSuffix (strFilter, dlgFile.m_ofn, ledItPrivateDocFormatTemplate);
    AppendFilterSuffix (strFilter, dlgFile.m_ofn, "*.rtf", "Microsoft Rich Text Format (*.rtf)");
    AppendFilterSuffix (strFilter, dlgFile.m_ofn, "*.txt", "Plain Text (*.txt)");

    strFilter += (TCHAR)'\0'; // last string

    dlgFile.m_ofn.lpstrFilter = strFilter;
    dlgFile.m_ofn.lpstrTitle  = title;

    // MAYBE SHOULD ELIMINATE TYPE-SUFFIX???
    dlgFile.m_ofn.lpstrFile = fileName.GetBuffer (_MAX_PATH);

    if (nIDSTitle == AFX_IDS_OPENFILE) {
        dlgFile.m_ofn.nFilterIndex = 1; // default to "All Recognized"
    }
    else {
        FileFormat initialFormat = *fileFormat;
        if (initialFormat == eUnknownFormat) {
            initialFormat = eDefaultFormat;
        }
        switch (initialFormat) {
            case eHTMLFormat:
                dlgFile.m_ofn.nFilterIndex = 1;
                break;
            case eLedPrivateFormat:
                dlgFile.m_ofn.nFilterIndex = 2;
                break;
            case eRTFFormat:
                dlgFile.m_ofn.nFilterIndex = 3;
                break;
            case eTextFormat:
                dlgFile.m_ofn.nFilterIndex = 4;
                break;
            default:
                Assert (false);
                break;
        }
    }
    bool bResult = (dlgFile.DoModal () == IDOK);
    fileName.ReleaseBuffer ();
    if (bResult) {
        if (nIDSTitle == AFX_IDS_OPENFILE) {
            switch (dlgFile.m_ofn.nFilterIndex) {
                case 1:
                    *fileFormat = eUnknownFormat;
                    break;
                case 2:
                    *fileFormat = eUnknownFormat;
                    break;
                case 3:
                    *fileFormat = eHTMLFormat;
                    break;
                case 4:
                    *fileFormat = eLedPrivateFormat;
                    break;
                case 5:
                    *fileFormat = eRTFFormat;
                    break;
                case 6:
                    *fileFormat = eTextFormat;
                    break;
                default:
                    *fileFormat = eUnknownFormat;
                    break;
            }
        }
        else {
            switch (dlgFile.m_ofn.nFilterIndex) {
                case 1:
                    *fileFormat = eHTMLFormat;
                    break;
                case 2:
                    *fileFormat = eLedPrivateFormat;
                    break;
                case 3:
                    *fileFormat = eRTFFormat;
                    break;
                case 4:
                    *fileFormat = eTextFormat;
                    break;
                default:
                    *fileFormat = eUnknownFormat;
                    break;
            }
        }

        // If saving a file, and user specified no extension, we add one. Not 100% sure this is the right way todo this.
        // But seems more often than not to be right... LGP 971019
        if (not(fileDLogFlags & OFN_FILEMUSTEXIST)) {
            if (dlgFile.GetFileExt () == "") {
                switch (*fileFormat) {
                    case eTextFormat:
                        fileName += ".txt";
                        break;
                    case eRTFFormat:
                        fileName += ".rtf";
                        break;
                    case eHTMLFormat:
                        fileName += ".html";
                        break;
                    case eLedPrivateFormat:
                        fileName += ".led";
                        break;
                }
            }
        }
    }
    return bResult;
}

#if qStroika_Foundation_Debug_AssertionsChecked
void LedItDocument::AssertValid () const
{
    COleServerDoc::AssertValid ();
    fTextStore.Invariant ();
    //fStyleDatabase.Invariant ();  Cannot do this cuz we're sometimes called at in-opportune times, while updating
    // the styles...called from MFC...
}
#endif

#endif

/*
 ********************************************************************************
 **************************** ExtractFileSuffix *********************************
 ********************************************************************************
 */
SDKString ExtractFileSuffix (const SDKString& from)
{
    size_t i = from.rfind ('.');
    if (i == SDKString::npos) {
        return SDKString{};
    }
    else {
        SDKString suffix = from.substr (i);
        for (size_t j = 0; j < suffix.length (); ++j) {
            if (isascii (suffix[j]) and isupper (suffix[j])) {
                suffix[j] = static_cast<Led_tChar> (tolower (suffix[j]));
            }
        }
        return suffix;
    }
}

#if qStroika_Platform_Windows
/*
 ********************************************************************************
 ******************************** AppendFilterSuffix ****************************
 ********************************************************************************
 */

static void AppendFilterSuffix (CString& filter, OPENFILENAME& ofn, CString strFilterExt, CString strFilterName)
{
    Require (not strFilterExt.IsEmpty ());
    Require (not strFilterName.IsEmpty ());

    // add to filter
    filter += strFilterName;
    ASSERT (!filter.IsEmpty ()); // must have a file type name
    filter += (TCHAR)'\0';       // next string please
    filter += (TCHAR)'*';
    filter += strFilterExt;
    filter += (TCHAR)'\0'; // next string please
    ++ofn.nMaxCustFilter;
}

static void AppendFilterSuffix (CString& filter, OPENFILENAME& ofn, CDocTemplate* pTemplate)
{
    ASSERT_VALID (pTemplate);
    ASSERT_KINDOF (CDocTemplate, pTemplate);
    CString strFilterExt;
    CString strFilterName;
    if (pTemplate->GetDocString (strFilterExt, CDocTemplate::filterExt) && !strFilterExt.IsEmpty () &&
        pTemplate->GetDocString (strFilterName, CDocTemplate::filterName) && !strFilterName.IsEmpty ()) {
        AppendFilterSuffix (filter, ofn, strFilterExt, strFilterName);
    }
}

#endif
