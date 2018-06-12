/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include <afxwin.h>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/LineEndings.h"
#include "Stroika/Frameworks/Led/FlavorPackage.h"

#include "LedLineItServerItem.h"
#include "Resource.h"

#include "LedLineItDocument.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;

using Stroika::Foundation::Characters::CodePageConverter;
using Stroika::Foundation::Characters::CodePagePrettyNameMapper;
using Stroika::Foundation::Characters::CodePagesGuesser;
using Stroika::Foundation::Characters::kCodePage_INVALID;
using Stroika::Foundation::Characters::kCodePage_UNICODE_WIDE;
using Stroika::Foundation::Characters::kCodePage_UNICODE_WIDE_BIGENDIAN;
using Stroika::Foundation::Characters::kCodePage_UTF7;
using Stroika::Foundation::Memory::SmallStackBuffer;

// special exception handling just for MFC library implementation
// copied here so I could clone MFC code as needed - not well understood - UGH!!! - LGP 951227
#ifndef _AFX_OLD_EXCEPTIONS
#define DELETE_EXCEPTION(e) \
    do {                    \
        e->Delete ();       \
    } while (0)
#else //!_AFX_OLD_EXCEPTIONS
#define DELETE_EXCEPTION(e)
#endif //_AFX_OLD_EXCEPTIONS

static void AppendFilterSuffix (CString& filter, OPENFILENAME& ofn, CString strFilterExt, CString strFilterName);
static void AppendFilterSuffix (CString& filter, OPENFILENAME& ofn, CDocTemplate* pTemplate);

static Led_SDK_String MapCodePageToPrettyName (CodePage cp)
{
    switch (cp) {
        case kAutomaticallyGuessCodePage:
            return Led_SDK_TCHAROF ("Automaticly Detect");
        default:
            return CodePagePrettyNameMapper::GetName (cp);
    }
}

static bool ShuffleToFront (vector<CodePage>* codePages, CodePage cp)
{
    vector<CodePage>::iterator i = std::find (codePages->begin (), codePages->end (), cp);
    if (i != codePages->end ()) {
        codePages->erase (i);
        codePages->insert (codePages->begin (), cp);
        return true;
    }
    return false;
}

class FileDialogWithCodePage : public CFileDialog {
private:
    using inherited = CFileDialog;

public:
    FileDialogWithCodePage (bool asOpenDialog, const vector<CodePage>& codePages, CodePage initialCodePage)
        : CFileDialog (asOpenDialog)
        , fCodePages (codePages)
        , fCodePage (initialCodePage)
    {
        m_ofn.Flags |= OFN_ENABLETEMPLATE | OFN_EXPLORER;
        m_ofn.lpTemplateName = m_lpszTemplateName = MAKEINTRESOURCE (kFileDialogAddOnID);
    }

    virtual BOOL OnInitDialog () override
    {
        inherited::OnInitDialog ();
        fCodePageComboBox.SubclassWindow (::GetDlgItem (GetSafeHwnd (), kFileDialog_EncodingComboBox));
        for (vector<CodePage>::const_iterator i = fCodePages.begin (); i != fCodePages.end (); ++i) {
            fCodePageComboBox.AddString (MapCodePageToPrettyName (*i).c_str ());
            if (*i == fCodePage) {
                fCodePageComboBox.SetCurSel (i - fCodePages.begin ());
            }
        }
        return (true);
    }

    CodePage fCodePage;

protected:
    afx_msg void OnCodePageSelChange ()
    {
        int curSel = fCodePageComboBox.GetCurSel ();
        if (curSel != CB_ERR) {
            Assert (curSel < static_cast<int> (fCodePages.size ()));
            fCodePage = fCodePages[curSel];
        }
    }

private:
    DECLARE_MESSAGE_MAP ()

private:
    vector<CodePage> fCodePages;
    CComboBox        fCodePageComboBox;
};

BEGIN_MESSAGE_MAP (FileDialogWithCodePage, CFileDialog)
ON_CBN_SELCHANGE (kFileDialog_EncodingComboBox, OnCodePageSelChange)
END_MESSAGE_MAP ()

namespace {
    class LineTooLongOnReadDialog : public CDialog {
    public:
        LineTooLongOnReadDialog (const Led_SDK_String& message, size_t breakCount)
            : CDialog (kLineTooLongOnRead_DialogID)
            , fMessage (message)
            , fBreakCount (breakCount)
        {
        }
        virtual BOOL OnInitDialog () override
        {
            BOOL result = CDialog::OnInitDialog ();
            Led_CenterWindowInParent (m_hWnd);
            SetDlgItemText (kLineTooLongOnRead_Dialog_MessageFieldID, fMessage.c_str ());
            SetDlgItemInt (kLineTooLongOnRead_Dialog_BreakNumFieldID, fBreakCount);
            return (result);
        }
        virtual void OnOK () override
        {
            size_t origBreakCount = fBreakCount;
            BOOL   trans          = false;
            fBreakCount           = GetDlgItemInt (kLineTooLongOnRead_Dialog_BreakNumFieldID, &trans);
            if (not trans) {
                fBreakCount = origBreakCount;
            }
            CDialog::OnOK ();
        }

    private:
        Led_SDK_String fMessage;

    public:
        size_t fBreakCount;

    protected:
        DECLARE_MESSAGE_MAP ()
    };
    BEGIN_MESSAGE_MAP (LineTooLongOnReadDialog, CDialog)
    END_MESSAGE_MAP ()
}

/*
 ********************************************************************************
 **************************** LedLineItDocument *********************************
 ********************************************************************************
 */
CodePage LedLineItDocument::sHiddenDocOpenArg = kIGNORECodePage;

IMPLEMENT_DYNCREATE (LedLineItDocument, COleServerDoc)

BEGIN_MESSAGE_MAP (LedLineItDocument, COleServerDoc)
ON_UPDATE_COMMAND_UI (ID_EDIT_PASTE_LINK, OnUpdatePasteLinkMenu)
ON_UPDATE_COMMAND_UI (ID_OLE_EDIT_CONVERT, OnUpdateObjectVerbMenu)
ON_COMMAND (ID_OLE_EDIT_CONVERT, OnEditConvert)
ON_UPDATE_COMMAND_UI (ID_OLE_EDIT_LINKS, OnUpdateEditLinksMenu)
ON_COMMAND (ID_OLE_EDIT_LINKS, OnEditLinks)
ON_UPDATE_COMMAND_UI (ID_OLE_VERB_FIRST, OnUpdateObjectVerbMenu)
ON_UPDATE_COMMAND_UI (ID_FILE_SAVE, OnUpdateFileSave)
ON_COMMAND (ID_FILE_SAVE_COPY_AS, OnFileSaveCopyAs)
END_MESSAGE_MAP ()

BEGIN_DISPATCH_MAP (LedLineItDocument, COleServerDoc)
END_DISPATCH_MAP ()

// Note: we add support for IID_ILedLineIt to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the
//  dispinterface in the .ODL file.

// {0FC00622-28BD-11CF-899C-00AA00580324}
static const IID IID_ILedLineIt = {0xfc00622, 0x28bd, 0x11cf, {0x89, 0x9c, 0x0, 0xaa, 0x0, 0x58, 0x3, 0x24}};

BEGIN_INTERFACE_MAP (LedLineItDocument, COleServerDoc)
INTERFACE_PART (LedLineItDocument, IID_ILedLineIt, Dispatch)
END_INTERFACE_MAP ()

LedLineItDocument::LedLineItDocument ()
    : COleServerDoc ()
    , MarkerOwner ()
    , fTextStore ()
    , fCommandHandler (kMaxNumUndoLevels)
    , fCodePage (kDefaultNewDocCodePage)
{
    EnableAutomation ();
    AfxOleLockApp ();

    fTextStore.SetTextBreaker (shared_ptr<TextBreaks> (new TextBreaks_Basic_TextEditor ()));
    fTextStore.AddMarkerOwner (this);
}

LedLineItDocument::~LedLineItDocument ()
{
    fTextStore.RemoveMarkerOwner (this);
    AfxOleUnlockApp ();
}

void LedLineItDocument::DidUpdateText (const UpdateInfo& updateInfo) noexcept
{
    if (updateInfo.fRealContentUpdate) {
        SetModifiedFlag ();
    }
}

TextStore* LedLineItDocument::PeekAtTextStore () const
{
    return &const_cast<LedLineItDocument*> (this)->fTextStore;
}

BOOL LedLineItDocument::OnNewDocument ()
{
    fCommandHandler.Commit ();
    if (!COleServerDoc::OnNewDocument ()) {
        return FALSE;
    }
    return TRUE;
}

COleServerItem* LedLineItDocument::OnGetEmbeddedItem ()
{
    // OnGetEmbeddedItem is called by the framework to get the COleServerItem
    //  that is associated with the document.  It is only called when necessary.
    LedLineItServerItem* pItem = new LedLineItServerItem (this);
    ASSERT_VALID (pItem);
    return pItem;
}

BOOL LedLineItDocument::DoSave (LPCTSTR lpszPathName, BOOL bReplace)
{
    CodePage useCodePage = fCodePage;
    CString  newName     = lpszPathName;
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

            // append .txt by default (SPR#1433) instead of grabbing from DocTemplate
            if (not newName.IsEmpty ()) {
                newName += _T (".txt");
            }
        }

        if (bReplace) {
            if (not DoPromptSaveAsFileName (&newName, &useCodePage)) {
                return false;
            }
        }
        else {
            if (not DoPromptSaveCopyAsFileName (&newName, &useCodePage)) {
                return false;
            }
        }
    }

    CWaitCursor wait;

    CodePage savedCodePage = fCodePage;
    fCodePage              = useCodePage;
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
            if (not bReplace) {
                fCodePage = savedCodePage;
            }
            return FALSE;
        }
        if (not bReplace) {
            fCodePage = savedCodePage;
        }
    }
    catch (...) {
        if (not bReplace) {
            fCodePage = savedCodePage;
        }
        throw;
    }

    // reset the title and change the document name
    if (bReplace) {
        SetPathName (newName);
    }

    return TRUE; // success
}

BOOL LedLineItDocument::OnOpenDocument (LPCTSTR lpszPathName)
{
    /*
     *  Override this - instead of counting on default implementation - because the MFC version with
     *  Visual Studio.Net 2003 is buggy when opening read-only files (cuz they are already opened
     *  someplace else, or because they are on a CD). Do this simple override to
     *  get good/better reading files functionality.
     *
     *  Note that this code being here probably means the READING side of the Serialize code is never
     *  executed (but leave it in there in case someone clones the code to use in another app, and there
     *  they use the serialize code).
     *
     *          -- LGP 2003-09-22 - for SPR#1552.
     *
     *  Also - later added further changes - such as using MyFlavorPackageInternalizer to breakup
     *  long lines, etc...
     *          -- LGP 2004-01-27
     */
    fCommandHandler.Commit ();

    class MyFlavorPackageInternalizer : public FlavorPackageInternalizer {
    private:
        using inherited = FlavorPackageInternalizer;

    public:
        enum { kMaxLineSize = 1024 };

    public:
        MyFlavorPackageInternalizer (TextStore& ts)
            : inherited (ts)
            , fBreakLongLines (false)
            , fBreakWidths (kMaxLineSize)
        {
        }

    public:
        virtual void InternalizeFlavor_FILEGuessFormatsFromStartOfData (
            [[maybe_unused]] Led_ClipFormat* suggestedClipFormat,
            CodePage*                        suggestedCodePage,
            const Byte* fileStart, const Byte* fileEnd) override
        {
            size_t curLineSize = 0;
            size_t maxLineSize = 0;
            for (const Byte* p = fileStart; p != fileEnd; ++p) {
                if (*p == '\n' or *p == '\r') {
                    curLineSize = 0;
                }
                else {
                    curLineSize++;
                }
                maxLineSize = max (maxLineSize, curLineSize);
            }
            if (suggestedCodePage != NULL and (*suggestedCodePage == kCodePage_UNICODE_WIDE or *suggestedCodePage == kCodePage_UNICODE_WIDE_BIGENDIAN)) {
                maxLineSize /= 2; // because we'd be counting null-bytes between chars.
                // Note this whole computation is VERY approximate - because its counting raw bytes of what could be
                // encoded text. For example - if the text was SJIS or UTF-7 encoding of far-east text - this would
                // greatly exagerate the estimated line size...
            }

            if (maxLineSize > kMaxLineSize) {
                LineTooLongOnReadDialog dlg (Characters::CString::Format (Led_SDK_TCHAROF ("This file contains at least one very long line (approximately %d characters). This may reduce editor performance, and make viewing the file awkward. Long lines can optionally be automatically broken up if they exceed the 'Break at characer count' value below."), maxLineSize / 100 * 100), kMaxLineSize);
                fBreakLongLines = (dlg.DoModal () == IDOK);
                fBreakWidths    = dlg.fBreakCount;
            }
        }

        virtual bool InternalizeFlavor_FILEDataRawBytes (
            Led_ClipFormat* suggestedClipFormat,
            CodePage*       suggestedCodePage,
            size_t from, size_t to,
            const void* rawBytes, size_t nRawBytes) override
        {
            Led_ClipFormat cf = (suggestedClipFormat == NULL or *suggestedClipFormat == kBadClipFormat) ? kTEXTClipFormat : *suggestedClipFormat;
            Require (cf == kTEXTClipFormat);

            fBreakWidths = max (fBreakWidths, 1U); // assure a decent value given...

            if (fBreakLongLines) {
#if qWideCharacters
                CodePage useCodePage = (suggestedCodePage == NULL or *suggestedCodePage == kCodePage_INVALID) ? CodePagesGuesser ().Guess (rawBytes, nRawBytes) : *suggestedCodePage;
                if (suggestedCodePage != NULL) {
                    *suggestedCodePage = useCodePage;
                }
                CodePageConverter           cpc        = CodePageConverter (useCodePage, CodePageConverter::eHandleBOM);
                size_t                      outCharCnt = cpc.MapToUNICODE_QuickComputeOutBufSize (reinterpret_cast<const char*> (rawBytes), nRawBytes + 1);
                SmallStackBuffer<Led_tChar> fileData2 (outCharCnt);
                cpc.MapToUNICODE (reinterpret_cast<const char*> (rawBytes), nRawBytes, static_cast<wchar_t*> (fileData2), &outCharCnt);
                size_t charsRead = outCharCnt;
                Assert (charsRead <= nRawBytes);
                charsRead = Characters::NormalizeTextToNL<Led_tChar> (fileData2, charsRead, fileData2, charsRead);

                {
                    SmallStackBuffer<Led_tChar> patchedData (charsRead + charsRead / fBreakWidths);
                    size_t                      curLineSize = 0;
                    size_t                      ourIdx      = 0;
                    for (const Led_tChar* p = fileData2; p != fileData2 + charsRead; ++p) {
                        if (*p == '\n' or *p == '\r') {
                            curLineSize = 0;
                        }
                        else {
                            curLineSize++;
                        }
                        patchedData[ourIdx++] = *p;
                        if (curLineSize >= fBreakWidths) {
                            curLineSize           = 0;
                            patchedData[ourIdx++] = '\n';
                        }
                    }
                    charsRead = ourIdx;
                    GetTextStore ().Replace (from, to, patchedData, charsRead);
                }
                return true;
#else
                // Copy byte by byte to a new buffer, and break lines that are too long - as I go....
                SmallStackBuffer<Byte> patchedBytes (nRawBytes + nRawBytes / fBreakWidths);
                size_t                 curLineSize = 0;
                size_t                 ourIdx      = 0;
                for (const Byte* p = reinterpret_cast<const Byte*> (rawBytes); p != reinterpret_cast<const Byte*> (rawBytes) + nRawBytes; ++p) {
                    if (*p == '\n' or *p == '\r') {
                        curLineSize = 0;
                    }
                    else {
                        curLineSize++;
                    }
                    patchedBytes[ourIdx++] = *p;
                    if (curLineSize >= fBreakWidths) {
                        curLineSize            = 0;
                        patchedBytes[ourIdx++] = '\n';
                    }
                }
                return inherited::InternalizeFlavor_FILEDataRawBytes (suggestedClipFormat, suggestedCodePage, from, to, patchedBytes, ourIdx);
#endif
            }
            else {
                return inherited::InternalizeFlavor_FILEDataRawBytes (suggestedClipFormat, suggestedCodePage, from, to, rawBytes, nRawBytes);
            }
        }

    public:
        bool   fBreakLongLines;
        size_t fBreakWidths;
    };

    MyFlavorPackageInternalizer internalizer (fTextStore);

    CodePage useCodePage = fCodePage;
    if (LedLineItDocument::sHiddenDocOpenArg != kIGNORECodePage) {
        useCodePage = sHiddenDocOpenArg;
        if (useCodePage == kAutomaticallyGuessCodePage) {
            useCodePage = kCodePage_INVALID; // implies automatically guess
        }
    }
    Led_ClipFormat cf = kTEXTClipFormat;
    internalizer.InternalizeFlavor_FILEData (lpszPathName, &cf, &useCodePage, 0, fTextStore.GetEnd ());
    fCodePage = useCodePage; // use whatever codePage file was opened with... by default... for future saves

    if (not internalizer.fBreakLongLines) {
        SetModifiedFlag (FALSE); // start off with doc unmodified
    }
    return true;
}

void LedLineItDocument::Serialize (CArchive& ar)
{
    if (ar.IsStoring ()) {
        const size_t kBufSize = 8 * 1024;
        Led_tChar    buf[kBufSize];
        size_t       offset    = 0;
        size_t       eob       = fTextStore.GetLength ();
        bool         firstTime = true;
        while (offset < eob) {
            size_t charsToWrite = min (kBufSize, eob - offset);
            fTextStore.CopyOut (offset, charsToWrite, buf);
            offset += charsToWrite;
#if qPlatform_MacOS
            Led_tChar buf2[sizeof (buf)];
#elif qPlatform_Windows
            Led_tChar buf2[2 * sizeof (buf)];
#endif
            charsToWrite = Characters::NLToNative<Led_tChar> (buf, charsToWrite, buf2, sizeof (buf2));
#if qWideCharacters
            CodePageConverter cpc = CodePageConverter (fCodePage);
            cpc.SetHandleBOM (firstTime); // only for the first block of text do we write a byte-order mark
            firstTime                         = false;
            size_t                 outCharCnt = cpc.MapFromUNICODE_QuickComputeOutBufSize (static_cast<Led_tChar*> (buf2), charsToWrite + 1);
            SmallStackBuffer<char> buf3_ (outCharCnt);
            size_t                 nBytesToWrite = 0;
            cpc.MapFromUNICODE (static_cast<Led_tChar*> (buf2), charsToWrite, static_cast<char*> (buf3_), &outCharCnt);
            nBytesToWrite = outCharCnt;
            char* buffp   = static_cast<char*> (buf3_);
#else
            char* buffp = static_cast<char*> (buf2);
            size_t nBytesToWrite = charsToWrite;
#endif
            ar.Write (buffp, nBytesToWrite);
        }
    }
    else {
        CFile* file = ar.GetFile ();
        ASSERT_VALID (file);
        DWORD                  nLen = static_cast<DWORD> (file->GetLength ()); // maybe should subtract current offset?
        SmallStackBuffer<char> buf (nLen);
        if (ar.Read (buf, nLen) != nLen) {
            AfxThrowArchiveException (CArchiveException::endOfFile);
        }

        CodePage useCodePage = fCodePage;
        if (LedLineItDocument::sHiddenDocOpenArg != kIGNORECodePage) {
            useCodePage = sHiddenDocOpenArg;
            if (useCodePage == kAutomaticallyGuessCodePage) {
                CodePagesGuesser::Confidence conf         = CodePagesGuesser::Confidence::eLow;
                size_t                       bytesToStrip = 0;
                useCodePage                               = CodePagesGuesser ().Guess (buf, nLen, &conf, &bytesToStrip);
            }
        }

#if qWideCharacters
        CodePageConverter           cpc        = CodePageConverter (useCodePage, CodePageConverter::eHandleBOM);
        size_t                      outCharCnt = cpc.MapToUNICODE_QuickComputeOutBufSize (static_cast<char*> (buf), nLen + 1);
        SmallStackBuffer<Led_tChar> result (outCharCnt);
        cpc.MapToUNICODE (static_cast<char*> (buf), nLen, static_cast<wchar_t*> (result), &outCharCnt);
        nLen             = outCharCnt;
        result[nLen]     = '\0'; // assure NUL-Term
        Led_tChar* buffp = static_cast<Led_tChar*> (result);
#else
        Led_tChar* buffp = static_cast<char*> (buf);
#endif

        nLen = Characters::NormalizeTextToNL<Led_tChar> (buffp, nLen, buffp, nLen);
        fTextStore.Replace (0, 0, buffp, nLen);

        fCodePage = useCodePage; // whatever codepage I just used to read the doc should be the new codepage...
    }
}

void LedLineItDocument::OnUpdateFileSave (CCmdUI* pCmdUI)
{
    ASSERT_VALID (this);
    RequireNotNull (pCmdUI);
    // only enable save command if dirty, or no file name associated with this document
    pCmdUI->Enable (IsModified () or GetPathName ().GetLength () == 0);
}

void LedLineItDocument::OnFileSaveCopyAs ()
{
    ASSERT_VALID (this);
    Assert (m_bRemember);

    LPSTORAGE savedStorage = m_lpRootStg;
    m_lpRootStg            = NULL;

    try {
        DoSave (NULL, false);
    }
    catch (...) {
        m_lpRootStg = savedStorage;
        m_bRemember = true;
        throw;
    }

    m_lpRootStg = savedStorage;
    m_bRemember = true;
}

void LedLineItDocument::DeleteContents ()
{
    fTextStore.Replace (fTextStore.GetStart (), fTextStore.GetEnd (), LED_TCHAR_OF (""), 0);
}

bool LedLineItDocument::DoPromptSaveAsFileName (CString* fileName, CodePage* codePage)
{
    return DoPromptFileName (fileName, AFX_IDS_SAVEFILE, false, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, codePage);
}

bool LedLineItDocument::DoPromptSaveCopyAsFileName (CString* fileName, CodePage* codePage)
{
    return DoPromptFileName (fileName, AFX_IDS_SAVEFILECOPY, false, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, codePage);
}

bool LedLineItDocument::DoPromptOpenFileName (CString* fileName, CodePage* codePage)
{
    return DoPromptFileName (fileName, AFX_IDS_OPENFILE, true, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, codePage);
}

bool LedLineItDocument::DoPromptFileName (CString* fileName, UINT nIDSTitle, bool isOpenDialogCall, long fileDLogFlags, CodePage* codePage)
{
    vector<CodePage> codePages = CodePagesInstalled ().GetAll ();
#if qDebug
    {
        // We use these magic numbers internally here - just assure they don't conflict...
        Assert (std::find (codePages.begin (), codePages.end (), kAutomaticallyGuessCodePage) == codePages.end ());
        Assert (std::find (codePages.begin (), codePages.end (), kIGNORECodePage) == codePages.end ());
    }
#endif

#if 0
    if (not isOpenDialogCall) {
        /*
         *  Assure the given (argument) code page is in the list.
         */
        vector<CodePage>::iterator  i   =   std::find (codePages.begin (), codePages.end (), *codePage);
        if (i == codePages.end ()) {
            codePages.push_back (*codePage);
        }
    }
#endif
    sort (codePages.begin (), codePages.end ());

    /*
     *  Bring certain special code pages to the head of the list.
     */
    (void)ShuffleToFront (&codePages, kCodePage_UNICODE_WIDE_BIGENDIAN);
    (void)ShuffleToFront (&codePages, kCodePage_UNICODE_WIDE);
    (void)ShuffleToFront (&codePages, kCodePage_UTF8);
    (void)ShuffleToFront (&codePages, kCodePage_UTF7);
    if (isOpenDialogCall) {
        codePages.insert (codePages.begin (), kAutomaticallyGuessCodePage);
    }

    FileDialogWithCodePage dlgFile (isOpenDialogCall, codePages, isOpenDialogCall ? kAutomaticallyGuessCodePage : *codePage);

    CString title;
    Verify (title.LoadString (nIDSTitle));

    dlgFile.m_ofn.Flags |= fileDLogFlags;

    CString strFilter;
    AppendFilterSuffix (strFilter, dlgFile.m_ofn, ".txt", "Text Files (*.txt)");
    AppendFilterSuffix (strFilter, dlgFile.m_ofn, ".*", "All Files (*.*)");

    strFilter += (TCHAR)'\0'; // last string

    dlgFile.m_ofn.lpstrFilter = strFilter;
    dlgFile.m_ofn.lpstrTitle  = title;

    dlgFile.m_ofn.lpstrFile = fileName->GetBuffer (_MAX_PATH);

    dlgFile.m_ofn.nFilterIndex = 2; // default to "All"
    bool bResult               = (dlgFile.DoModal () == IDOK);
    fileName->ReleaseBuffer ();
    *codePage = dlgFile.fCodePage;
    return bResult;
}

#if qDebug
void LedLineItDocument::AssertValid () const
{
    COleServerDoc::AssertValid ();
    fTextStore.Invariant ();
    //fStyleDatabase.Invariant ();  Cannot do this cuz we're sometimes called at in-opportune times, while updating
    // the styles...called from MFC...
}
#endif

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
    ofn.nMaxCustFilter++;
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
