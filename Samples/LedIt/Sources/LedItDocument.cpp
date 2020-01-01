/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include <cctype>

#if qPlatform_MacOS
#include <Finder.h>

#include <LFile.h>
#include <LMenu.h>
#include <LMenuBar.h>
#include <LPlaceHolder.h>
#include <LPrintout.h>
#include <LString.h>
#include <LWindow.h>
#include <UMemoryMgr.h>
#include <UWindows.h>
#endif
#if defined(WIN32)

#include <afxwin.h>
#elif qStroika_FeatureSupported_XWindows
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "Stroika/Frameworks/Led/SpellCheckEngine_Basic.h"
#include "Stroika/Frameworks/Led/StyledTextIO/StyledTextIO_LedNative.h"
#include "Stroika/Frameworks/Led/StyledTextIO/StyledTextIO_PlainText.h"
#if qPlatform_MacOS
#include "Stroika/Frameworks/Led/StyledTextIO/StyledTextIO_STYLText.h"
#endif

#if qPlatform_MacOS
#include "Stroika/Frameworks/Led/FilteredFilePicker.h"
#elif qPlatform_Windows
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

using Memory::SmallStackBuffer;

#if qPlatform_MacOS
class LedItDocumentWindow : public LWindow {
public:
    LedItDocumentWindow (ResIDT inWINDid, UInt32 inAttributes, LCommander* inSuper)
        : LWindow (inWINDid, inAttributes, inSuper)
    {
        sWindowList.push_back (this);
        ::AppendMenu (::GetMenuHandle (kWindowsMenuID), "\pREPLACEME");
        LMenu* windowMenu = LMenuBar::GetCurrentMenuBar ()->FetchMenu (kWindowsMenuID);
        AssertNotNil (windowMenu);
        size_t nMenuItems = ::CountMenuItems (windowMenu->GetMacMenuH ());
        for (size_t i = 1; i <= nMenuItems; i++) {
            windowMenu->SetCommand (i, i - 1 + kBaseWindowCmd); // make first cmd = kBaseWindowCmd
        }
    }

    ~LedItDocumentWindow ()
    {
        LWindow* w = this;
        sWindowList.erase (std::find (sWindowList.begin (), sWindowList.end (), w));
        LMenu* windowMenu = LMenuBar::GetCurrentMenuBar ()->FetchMenu (kWindowsMenuID);
        AssertNotNil (windowMenu);
        windowMenu->RemoveItem (1);
        size_t nMenuItems = ::CountMenuItems (windowMenu->GetMacMenuH ());
        for (size_t i = 1; i <= nMenuItems; i++) {
            windowMenu->SetCommand (i, i - 1 + kBaseWindowCmd); // make first cmd = kBaseWindowCmd
        }
    }

    // make zoom produce a roughly page-size window
    virtual void CalcStandardBoundsForScreen (const Rect& inScreenBounds, Rect& outStdBounds) const override
    {
        LWindow::CalcStandardBoundsForScreen (inScreenBounds, outStdBounds);
        short winWidth     = ::GetRectWidth (outStdBounds);
        short desiredWidth = 8.5 * 72; // 8.5 inches by 72dpi
        short newWidth     = Led_Min (winWidth, desiredWidth);
        outStdBounds.right = outStdBounds.left + newWidth;
    }

public:
    static vector<LWindow*> sWindowList;
};
vector<LWindow*> LedItDocumentWindow::sWindowList;

static FilteredSFPutDLog::TypeSpec sPutFileTypeList[] = {
    {"HTML file", kTEXTFileType},
    {"Led Rich Text Format", kLedPrivateDocumentFileType},
    {"Microsoft Rich Text Format (RTF)", kTEXTFileType},
    {"Text file", kTEXTFileType},
};

inline FileFormat MapPutFileTypeListIdxToFormat (size_t typeIndex)
{
    switch (typeIndex) {
        case 0:
            return (eHTMLFormat);
        case 1:
            return (eLedPrivateFormat);
        case 2:
            return (eRTFFormat);
        case 3:
            return (eTextFormat);
        default:
            Assert (false);
            return (eTextFormat);
    }
}

inline size_t MapPutFileFormatToTypeListIdx (FileFormat format)
{
    switch (format) {
        case eHTMLFormat:
            return (0);
        case eLedPrivateFormat:
            return (1);
        case eRTFFormat:
            return (2);
        case eTextFormat:
            return (3);
        default:
            Assert (false);
            return (0);
    }
}

inline OSType MapFormatToOSType (FileFormat fileFormat)
{
    switch (fileFormat) {
        case eTextFormat:
            return (kTEXTFileType);
        case eLedPrivateFormat:
            return (kLedPrivateDocumentFileType);
        case eRTFFormat:
            return (kTEXTFileType);
        case eHTMLFormat:
            return (kTEXTFileType);
        case eUnknownFormat:
            return (kLedPrivateDocumentFileType);
        default:
            return (kLedPrivateDocumentFileType);
    }
}

/*
 *  I tried using the PowerPlant StCursor class, but it worked LOUSY. It kept getting
 *  its internal cache of what cursor was shown stuck, and stoped going busy!
 *
 *  This is extremely primitive, and lacks many features I'd like (like not showing up
 *  unless needed - like if enuf time has past and looks like its gonna take longer).
 *
 *  But it will do for now.... LGP 960702
 *
 *  Maybe consider lifting the Stroika cursor support sometime in the future? LGP 960702
 */
class Led_BusyCursor {
public:
    Led_BusyCursor ()
    {
        ::SetCursor (*::GetCursor (watchCursor));
    }
    ~Led_BusyCursor ()
    {
        ::InitCursor ();
    }
};
#endif

#if qPlatform_Windows
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

#endif

/*
 ********************************************************************************
 ******************************** LedItDocument *********************************
 ********************************************************************************
 */
#if qPlatform_Windows
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

#if qPlatform_MacOS
LedItDocument::LedItDocument (LCommander* inSuper, FileFormat format)
    : LSingleDoc (inSuper)
    ,
#elif qPlatform_Windows
LedItDocument::LedItDocument ()
    : COleServerDoc ()
    ,
#elif qStroika_FeatureSupported_XWindows
LedItDocument::LedItDocument ()
    :
#endif
    MarkerOwner ()
    , fTextStore ()
    , fRTFInfo ()
    , fStyleDatabase ()
    , fParagraphDatabase ()
    , fHidableTextDatabase ()
    , fCommandHandler (kMaxNumUndoLevels)
    ,
#if qPlatform_MacOS
    fFileFormat (format)
    ,
#elif qPlatform_Windows || qStroika_FeatureSupported_XWindows
    fFileFormat (eDefaultFormat)
    ,
#endif
    fHTMLInfo ()
#if qStroika_FeatureSupported_XWindows
    , fPathName ()
#endif
#if qPlatform_MacOS
    , fTextView (NULL)
#endif
{
#if qPlatform_Windows
    EnableAutomation ();
    ::AfxOleLockApp ();
#endif
    fTextStore.AddMarkerOwner (this);
    fStyleDatabase       = StandardStyledTextImager::StyleDatabasePtr (new StandardStyledTextImager::StyleDatabaseRep (fTextStore));
    fParagraphDatabase   = WordProcessor::ParagraphDatabasePtr (new WordProcessor::ParagraphDatabaseRep (fTextStore));
    fHidableTextDatabase = WordProcessor::HidableTextDatabasePtr (new UniformHidableTextMarkerOwner (fTextStore));
}

LedItDocument::~LedItDocument ()
{
#if qPlatform_MacOS
    if (mWindow != NULL) {
        mWindow->PostAction (NULL); //  Flush undo buffer
    }

    delete mWindow;
    mWindow = NULL; // delete now (rather than letting base class) so
// all links to our textstore/style dbase etc go away before our
// DTOR is done...
#endif
    fTextStore.RemoveMarkerOwner (this);
#if qPlatform_Windows
    ::AfxOleUnlockApp ();
#endif
}

void LedItDocument::DidUpdateText (const UpdateInfo& updateInfo) noexcept
{
    if (updateInfo.fRealContentUpdate) {
#if qPlatform_MacOS
        mIsModified = true;
        SetUpdateCommandStatus (true);
#elif qPlatform_Windows
        SetModifiedFlag ();
#endif
    }
}

TextStore* LedItDocument::PeekAtTextStore () const
{
    return &const_cast<LedItDocument*> (this)->fTextStore;
}

#if qStroika_FeatureSupported_XWindows
void LedItDocument::LoadFromFile (const string& fileName, FileFormat fileFormat)
{
    Require (not fileName.empty ());
    fPathName   = fileName;
    fFileFormat = fileFormat;

// Now do actual reading stuff..
#if qPrintGLIBTraceMessages
    g_message ("DOING LedItDocument::LoadFromFile (path= '%s', format=%d)\n", fPathName.c_str (), fileFormat);
#endif
    size_t                 fileLen = 0;
    SmallStackBuffer<char> fileData (fileLen);
    int                    fd = ::open (fPathName.c_str (), O_RDONLY);
    if (fd == -1) {
        Led_ThrowOutOfMemoryException (); // WRONG EXCEPTION
    }
    fileLen = ::lseek (fd, 0, SEEK_END);
    fileData.GrowToSize (fileLen);
    try {
        ::lseek (fd, 0, SEEK_SET);
        if (::read (fd, fileData, fileLen) != int (fileLen)) {
            Led_ThrowOutOfMemoryException (); // WRONG EXCEPTION
        }
    }
    catch (...) {
        ::close (fd);
        throw;
    }
    ::close (fd);

    StyledTextIOSrcStream_Memory                 source (fileData, fileLen);
    WordProcessor::WordProcessorTextIOSinkStream sink (&fTextStore, fStyleDatabase, fParagraphDatabase, fHidableTextDatabase);

ReRead:
    switch (fFileFormat) {
        case eTextFormat: {
            StyledTextIOReader_PlainText textReader (&source, &sink);
            textReader.Read ();
        } break;

        case eLedPrivateFormat: {
            StyledTextIOReader_LedNativeFileFormat textReader (&source, &sink);
            textReader.Read ();
        } break;

        case eRTFFormat: {
            StyledTextIOReader_RTF textReader (&source, &sink, &fRTFInfo);
            textReader.Read ();
        } break;

        case eHTMLFormat: {
            StyledTextIOReader_HTML textReader (&source, &sink, &fHTMLInfo);
            textReader.Read ();
        } break;

        case eUnknownFormat: {
            /*
                 *  Should enhance this unknown/format reading code to take into account file suffix in our guess.
                 *  First look at file suffix. THAT takes precedence over guessing file format based on
                 *  contents.
                 */
            Led_SDK_String suffix = ExtractFileSuffix (fPathName);
            if (suffix == ".rtf") {
                fFileFormat = eRTFFormat;
                goto ReRead;
            }
            if (suffix == ".htm" or suffix == ".html") {
                fFileFormat = eHTMLFormat;
                goto ReRead;
            }
            if (suffix == ".led") {
                fFileFormat = eLedPrivateFormat;
                goto ReRead;
            }
            if (suffix == ".txt") {
                fFileFormat = eTextFormat;
                goto ReRead;
            }
            // Try RTF
            try {
                StyledTextIOReader_RTF reader (&source, &sink, &fRTFInfo);
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
                StyledTextIOReader_LedNativeFileFormat reader (&source, &sink);
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
                StyledTextIOReader_HTML reader (&source, &sink);
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

#if 0
    // Should do something like this for XWin too - but right now - no backpointer from Doc to View kept... LGP 2002-11-20
    fTextView->SetEmptySelectionStyle ();
#endif
}

void LedItDocument::Save ()
{
    // Now do actual reading stuff..
    g_message ("DOING Save- '%s'\n", fPathName.c_str ());
    Require (fFileFormat != eUnknownFormat); // We must have chosen a file format by now...

    WordProcessor::WordProcessorTextIOSrcStream source (&fTextStore, fStyleDatabase, fParagraphDatabase, fHidableTextDatabase);
    StyledTextIOWriterSinkStream_Memory         sink;

    switch (fFileFormat) {
        case eTextFormat: {
            StyledTextIOWriter_PlainText textWriter (&source, &sink);
            textWriter.Write ();
        } break;

        case eRTFFormat: {
            StyledTextIOWriter_RTF textWriter (&source, &sink, &fRTFInfo);
            textWriter.Write ();
        } break;

        case eHTMLFormat: {
            StyledTextIOWriter_HTML textWriter (&source, &sink, &fHTMLInfo);
            textWriter.Write ();
        } break;

        case eLedPrivateFormat: {
            StyledTextIOWriter_LedNativeFileFormat textWriter (&source, &sink);
            textWriter.Write ();
        } break;

        default: {
            Assert (false); // don't support writing that format (yet?)!
        } break;
    }
    int fd = ::open (fPathName.c_str (), O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        Led_ThrowOutOfMemoryException (); // WRONG EXCEPTION
    }
    try {
        ::lseek (fd, 0, SEEK_SET);
        if (::write (fd, sink.PeekAtData (), sink.GetLength ()) != int (sink.GetLength ())) {
            Led_ThrowOutOfMemoryException (); // WRONG EXCEPTION
        }
    }
    catch (...) {
        ::close (fd);
        throw;
    }
    ::close (fd);
}
#endif

#if qPlatform_MacOS
const vector<LWindow*>& LedItDocument::GetDocumentWindows ()
{
    return LedItDocumentWindow::sWindowList;
}

void LedItDocument::DoSaveHelper ()
{
    Led_BusyCursor busyCursor;

    //  write text
    ThrowIfNULL_ (mFile);
    ThrowIfNULL_ (mWindow);

    mWindow->PostAction (NULL);

    WordProcessor::WordProcessorTextIOSrcStream source (&fTextStore, fStyleDatabase, fParagraphDatabase, fHidableTextDatabase);

    ThrowIfOSErr_ (::SetFPos (mFile->GetDataForkRefNum (), fsFromStart, 0));
    StyledTextIOWriterSinkStream_FileDescriptor sink (mFile->GetDataForkRefNum ());

    // Setup output buffer, but be sure there is plenty of RAM before doing so
    {
        char* cheeseBuf = NULL;
        try {
            cheeseBuf = new char[64 * 1024];
            sink.SetBufferSize (64 * 1024);
            delete[] cheeseBuf;
        }
        catch (...) {
            delete[] cheeseBuf;
        }
    }

    // Now actually write the file
    switch (fFileFormat) {
        case eTextFormat: {
            StyledTextIOWriter_PlainText textWriter (&source, &sink);
            textWriter.Write ();

            {
                short curResFile = ::CurResFile ();
                try {
                    mFile->CreateNewFile (kApplicationSignature, kTEXTFileType, 0);
                }
                catch (...) {
                }
                try {
                    mFile->OpenResourceFork (fsRdWrPerm);
                }
                catch (...) {
                }

                if (mFile->GetResourceForkRefNum () != -1) {
                    ::UseResFile (mFile->GetResourceForkRefNum ());

                    try {
                        StScrpHandle macStyleHandle = (StScrpHandle)::Get1Resource ('styl', 128);
                        if (macStyleHandle != NULL) {
                            ::RemoveResource (Handle (macStyleHandle)); // remove old styl resource
                        }

                        vector<StandardStyledTextImager::InfoSummaryRecord>
                            ledStyleRuns  = fStyleDatabase->GetStyleInfo (0, fTextStore.GetLength ());
                        size_t nStyleRuns = ledStyleRuns.size ();
                        Assert (offsetof (StScrpRec, scrpStyleTab) == sizeof (short)); // thats why we add sizeof (short)
                        macStyleHandle = (StScrpHandle)::Led_DoNewHandle (sizeof (short) + nStyleRuns * sizeof (ScrpSTElement));
                        HLock (Handle (macStyleHandle));
                        (*macStyleHandle)->scrpNStyles = nStyleRuns;
                        StandardStyledTextImager::Convert (ledStyleRuns, (*macStyleHandle)->scrpStyleTab);
                        HUnlock (Handle (macStyleHandle));

                        ::AddResource (Handle (macStyleHandle), 'styl', 128, "\p");
                        ThrowIfResError_ ();
                    }
                    catch (...) {
                        ::UseResFile (curResFile);
                        mFile->CloseResourceFork ();
                        throw;
                    }

                    ::UseResFile (curResFile);
                    mFile->CloseResourceFork ();
                }
            }
        } break;

        case eRTFFormat: {
            StyledTextIOWriter_RTF textWriter (&source, &sink);
            textWriter.Write ();
        } break;

        case eHTMLFormat: {
            StyledTextIOWriter_HTML textWriter (&source, &sink, &fHTMLInfo);
            textWriter.Write ();
        } break;

        case eLedPrivateFormat: {
            StyledTextIOWriter_LedNativeFileFormat textWriter (&source, &sink);
            textWriter.Write ();
        } break;

        default: {
            Assert (false);
        } break;
    }

    sink.UpdateEOF ();
}

//  Name a new, untitled document window
//
//  Untitled windows start with "untitled", then "untitled 1",
//  "untitled 2", etc. Old numbers are reused, so there won't be
//  gaps in the numbering.
//
//  This routine uses a STR# resource to store the "untitled" string,
//  which can be localized to different languages. The first string
//  is "untitled" and the second is "untitled " (trailing space),
//  which is used when appending a number to the name.
void LedItDocument::NameNewDoc ()
{
    // Start with the default name ("untitled")
    Str255 name;
    ::GetIndString (name, STRx_Untitled, 1);

    long num = 0;
    while (UWindows::FindNamedWindow (name) != nil) {
        // An existing window has the current name
        // Increment counter and try again
        ::GetIndString (name, STRx_Untitled, 2);
        num++;
        Str15 numStr;
        ::NumToString (num, numStr);
        LString::AppendPStr (name, numStr);
    }
    AssertNotNull (mWindow);
    mWindow->SetDescriptor (name); // Finally, set window title
}

void LedItDocument::OpenFile (const FSSpec& inFileSpec)
{
    // Create a new File object, read the entire File contents,
    // put the contents into the text view, and set the Window
    // title to the name of the File.
    try {
        Require (mFile == NULL);
        mFile = new LFile (inFileSpec);

        FInfo docFinderInfo;
        ThrowIfError_ (::FSpGetFInfo (&inFileSpec, &docFinderInfo));
        bool isStationary   = docFinderInfo.fdFlags & kIsStationery;
        bool openedReadOnly = false;
        try {
            mFile->OpenDataFork (fsRdWrPerm);
        }
        catch (...) {
            openedReadOnly = true;
            mFile->OpenDataFork (fsRdPerm);
        }

        DoReadCode ();

        fTextView->SetEmptySelectionStyle ();

        if (isStationary) {
            NameNewDoc ();
        }
        else {
            mWindow->SetDescriptor (inFileSpec.name);
        }

        if (openedReadOnly or isStationary) {
            delete mFile;
            mFile = NULL;
        }
        mIsSpecified = not(openedReadOnly or isStationary); // if we opened RO or stationary, then hitting save button should bring up save-dialog
        mIsModified  = isStationary;                        // if we stationary, then closing should bring up save-dialog
    }
    catch (...) {
        // if an error opening the file, then delete the file object - in case its left open...
        delete mFile;
        mFile = NULL;
        throw;
    }
}

//  Return whether the Document is has changed since the last save
Boolean LedItDocument::IsModified ()
{
    return mIsModified;
}

//  Save Document in the specified file with the specified file type
//
//  If file type is fileType_Default, use the normal file type for
//  this document
void LedItDocument::DoAESave (FSSpec& inFileSpec, OSType inFileType)
{
    delete mFile;
    mFile = NULL;

    mFile = new LFile (inFileSpec); // Make new file object

    OSType fileType = MapFormatToOSType (fFileFormat); // Find proper file type
    if (inFileType != fileType_Default) {
        fileType = inFileType;
    }

    // Make new file on disk
    mFile->CreateNewDataFile (kApplicationSignature, fileType, 0);
    mFile->OpenDataFork (fsRdWrPerm);
    DoSave (); // Write out data
    // Change window name
    mWindow->SetDescriptor (inFileSpec.name);
    mIsSpecified = true; // Document now has a specified file
}

//  Ask the user to save a Document and give it a name
//
//  Returns false if the user cancels the operation
Boolean LedItDocument::AskSaveAs (FSSpec& outFSSpec, Boolean inRecordIt)
{
    FilteredSFPutDLog filteredPicker (sPutFileTypeList, (sizeof sPutFileTypeList) / (sizeof sPutFileTypeList[0]));

    Str255 defaultName;
    GetDescriptor (defaultName);
    size_t typeIndex = MapPutFileFormatToTypeListIdx (fFileFormat);
    FSSpec fileResult;
    bool   replacingFile = false;
    if (filteredPicker.PickFile (defaultName, &fileResult, &replacingFile, &typeIndex)) {
        fFileFormat = MapPutFileTypeListIdxToFormat (typeIndex);

        /*
         *  Close the file (in case were overwriting our original, so the delete below doesn't fail.
         *  Any error writing NEW file out, mark us as UNTITLED, and NOT associated with any file...
         */
        delete mFile;
        mFile        = NULL;
        mIsSpecified = false;
        try {
            if (inRecordIt) {
                SendAESaveAs (fileResult, fileType_Default, false);
            }
            if (replacingFile) { // Delete existing file
                ThrowIfOSErr_ (::FSpDelete (&fileResult));
            }
            DoAESave (fileResult, fileType_Default);
            outFSSpec = fileResult;
        }
        catch (...) {
            delete mFile;
            mFile        = NULL;
            mIsSpecified = false;
            NameNewDoc ();
            throw;
        }
        return true;
    }
    return false;
}

//  Save the entire Document to its associated File (which must already exist)
void LedItDocument::DoSave ()
{
    DoSaveHelper ();

    FSSpec fileSpec;
    mFile->GetSpecifier (fileSpec);
    mWindow->SetDescriptor (fileSpec.name);

    mIsModified = false;
}

//  Revert the Document to the last saved version on disk
void LedItDocument::DoRevert ()
{
    fTextStore.Replace (fTextStore.GetStart (), fTextStore.GetEnd (), LED_TCHAR_OF (""), 0);
    fCommandHandler.Commit ();
    DoReadCode ();
    fCommandHandler.Commit ();
    mIsModified = false;
}

//  Print the contents of the Document
void LedItDocument::DoPrint ()
{
    LPrintout* thePrintout = LPrintout::CreatePrintout (prto_TextDoc);
    thePrintout->SetPrintSpec (mPrintSpec);
    LPlaceHolder* textPlace = (LPlaceHolder*)thePrintout->FindPaneByID ('TBox');

    LedItView editorView;
    editorView.AddAttributes (textAttr_MultiStyle | textAttr_Editable | textAttr_Selectable | textAttr_WordWrap);
    editorView.SpecifyTextStore (&fTextStore);
    editorView.SetStyleDatabase (fStyleDatabase);
    editorView.SetParagraphDatabase (fParagraphDatabase);
    editorView.SetHidableTextDatabase (fHidableTextDatabase);
    editorView.SetScrollBarType (Led_PPView::v, Led_PPView::eScrollBarNever);
    editorView.SetScrollBarType (Led_PPView::h, Led_PPView::eScrollBarNever);
    textPlace->InstallOccupant (&editorView, atNone);
    editorView.FinishCreate ();
    editorView.SetForceAllRowsShowing (false);

    thePrintout->DoPrintJob ();
    delete thePrintout;
}

void LedItDocument::PurgeUnneededMemory ()
{
    // If we are running out of memory, best to commit all our existing commands, to free some up.
    fCommandHandler.Commit ();

    // Also, we can punt cached linebreak etc information
    AssertNotNull (fTextView);
    fTextView->PurgeUnneededMemory ();
}

void LedItDocument::BuildDocWindow (const FSSpec* inFileSpec)
{
    Assert (mWindow == NULL);

    // GetNewCWindow() seems to crash when we are nearly out of RAM in our local heap (rather
    // than just returning NULL. So try to avoid that situation.
    Led_CheckSomeLocalHeapRAMAvailable ();

    mWindow = new LedItDocumentWindow (WIND_TextDoc, windAttr_Regular | windAttr_CloseBox | windAttr_TitleBar | windAttr_Resizable | windAttr_SizeBox | windAttr_Zoomable | windAttr_Enabled | windAttr_Targetable, this);
    mWindow->FinishCreate ();

    // By default window created with a silly size. Don't know how todo this well within
    // PowerPlant. Seems you have to specify size in WIND resource. But thats not what I want!
    // Well, we'll just take matters into our own hands then...
    {
// Find GDevice containing largest portion of Window
#if TARGET_CARBON
        WindowPtr theWindPtr = ::GetWindowFromPort (mWindow->GetMacPort ());
#else
        WindowPtr theWindPtr = mWindow->GetMacPort ();
#endif
        Rect      windowStructureRect                    = UWindows::GetWindowStructureRect (theWindPtr);
        GDHandle  dominantDevice                         = UWindows::FindDominantDevice (windowStructureRect);
        Rect      screenRect                             = (**dominantDevice).gdRect;
        const int kSluffBetweenWindBottomAndScreenBottom = 10;
        if (windowStructureRect.bottom + kSluffBetweenWindBottomAndScreenBottom < screenRect.bottom) {
            Rect newWindBounds = UWindows::GetWindowContentRect (theWindPtr);
            // being careful about size of title bar and rest of structure region, we want to incremnt content region size by
            // how much we want to grow structure region.
            newWindBounds.bottom += (screenRect.bottom - windowStructureRect.bottom - kSluffBetweenWindBottomAndScreenBottom);
            mWindow->DoSetBounds (newWindBounds);
        }
    }

    SDimension16 winSize;
    mWindow->GetFrameSize (winSize);

    {
        Rect minMax;
        minMax.top    = 72;        // minHeight
        minMax.left   = 72;        // minWidth
        minMax.bottom = 10 * 1024; // maxHeight
        minMax.right  = 10 * 1024; // maxWidth;
        mWindow->SetMinMaxSize (minMax);
    }

    LedItView* editorView = new LedItView ();

    editorView->AddAttributes (textAttr_MultiStyle | textAttr_Editable | textAttr_Selectable | textAttr_WordWrap);
    editorView->SpecifyTextStore (&fTextStore);
    editorView->SetStyleDatabase (fStyleDatabase);
    editorView->SetParagraphDatabase (fParagraphDatabase);
    editorView->SetHidableTextDatabase (fHidableTextDatabase);
    editorView->SetCommandHandler (&fCommandHandler);
    editorView->SetSpellCheckEngine (&LedItApplication::Get ().fSpellCheckEngine);
    editorView->PutInside (mWindow);
    editorView->PlaceInSuperFrameAt (0, 0, false);
    editorView->ResizeFrameTo (winSize.width, winSize.height, false);
    SBooleanRect editViewBindings;
    editViewBindings.top    = true;
    editViewBindings.left   = true;
    editViewBindings.bottom = true;
    editViewBindings.right  = true;
    editorView->SetFrameBinding (editViewBindings);
    editorView->FinishCreate ();

    fTextView = editorView;
    mWindow->SetLatentSub (editorView); // make editorView get the focus when window shown...

    if (inFileSpec == NULL) {
        NameNewDoc (); // Set name of untitled window
    }
    else {
        OpenFile (*inFileSpec); // Display contents of file in window
    }

    mWindow->Show ();
}

void LedItDocument::DoReadCode ()
{
    Led_BusyCursor busyCursor;

    StScrpHandle styleInfo = NULL;
    try {
        short curResFile = ::CurResFile ();
        mFile->OpenResourceFork (fsRdPerm);
        ::UseResFile (mFile->GetResourceForkRefNum ());
        styleInfo = (StScrpHandle)::Get1Resource ('styl', 128);
        if (styleInfo != NULL) {
            ::DetachResource ((Handle)styleInfo);
        }
        ::UseResFile (curResFile);
        mFile->CloseResourceFork ();
    }
    catch (...) {
    }
    try {
        ThrowIfOSErr_ (::SetFPos (mFile->GetDataForkRefNum (), fsFromStart, 0));
        StyledTextIOSrcStream_FileDescriptor         source (mFile->GetDataForkRefNum (), Handle (styleInfo));
        WordProcessor::WordProcessorTextIOSinkStream sink (&fTextStore, fStyleDatabase, fParagraphDatabase, fHidableTextDatabase);

    ReRead:
        switch (fFileFormat) {
            case eTextFormat: {
                source.SetBufferSize (0); // no need for buffering with these readers
                if (styleInfo == NULL) {
                    StyledTextIOReader_PlainText textReader (&source, &sink);
                    textReader.Read ();
                }
                else {
                    StyledTextIOReader_STYLText textReader (&source, &sink);
                    textReader.Read ();
                }
            } break;

            case eRTFFormat: {
                source.SetBufferSize (16 * 1024); // reads a byte at a time - buffering helps ALOT
                StyledTextIOReader_RTF textReader (&source, &sink);
                textReader.Read ();
            } break;

            case eHTMLFormat: {
                source.SetBufferSize (16 * 1024); // reads a byte at a time - buffering helps ALOT
                StyledTextIOReader_HTML textReader (&source, &sink, &fHTMLInfo);
                textReader.Read ();
            } break;

            case eLedPrivateFormat: {
                // unsure if buffering would help here or not - try both ways - LGP 960902
                StyledTextIOReader_LedNativeFileFormat textReader (&source, &sink);
                textReader.Read ();
            } break;

            default: {
                /*
                     *  Should enhance this unknown/format reading code to take into
                     *  account file suffix in our guess. Also should take into account
                     *  FINDER File-Type info.
                     */

                // for unknown file format,
                //      first try RTF
                //      then Led private format
                //      first try HTML
                //      then try to read as plain text
                source.SetBufferSize (1 * 1024); // most/all of these quicklook routines only examine the first 1K or so

                // Try RTF
                try {
                    StyledTextIOReader_RTF reader (&source, &sink);
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
                    StyledTextIOReader_LedNativeFileFormat reader (&source, &sink);
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
                    StyledTextIOReader_HTML reader (&source, &sink);
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
        }
    }
    catch (...) {
        if (styleInfo != NULL) {
            ::DisposeHandle ((Handle)styleInfo);
        }
        throw;
    }
    if (styleInfo != NULL) {
        ::DisposeHandle ((Handle)styleInfo);
    }
}

Boolean LedItDocument::ObeyCommand (CommandT inCommand, void* ioParam)
{
    Boolean cmdHandled = true;

    switch (inCommand) {
        case kCmdSaveACopyAs: {
            OnSaveACopyAsCommand ();
        } break;

        default:
            cmdHandled = LSingleDoc::ObeyCommand (inCommand, ioParam);
            break;
    }

    return cmdHandled;
}

//  Pass back status of a (menu) command
void LedItDocument::FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark,
                                       UInt16& outMark, Str255 outName)
{
    outUsesMark = false;
    switch (inCommand) {
        case kCmdSaveACopyAs: {
            outEnabled = true;
        } break;

        default:
            LSingleDoc::FindCommandStatus (inCommand, outEnabled, outUsesMark, outMark, outName);
            break;
    }
}

void LedItDocument::OnSaveACopyAsCommand ()
{
    FilteredSFPutDLog filteredPicker (sPutFileTypeList, (sizeof sPutFileTypeList) / (sizeof sPutFileTypeList[0]));

    Str255 defaultName;
    GetDescriptor (defaultName);
    size_t typeIndex = MapPutFileFormatToTypeListIdx (fFileFormat);
    FSSpec fileResult;
    bool   replacingFile = false;
    if (filteredPicker.PickFile (defaultName, &fileResult, &replacingFile, &typeIndex)) {
        if (replacingFile) { // Delete existing file
            ThrowIfOSErr_ (::FSpDelete (&fileResult));
        }

        FileFormat savedFileFormat = fFileFormat;
        LFile*     savedFile       = mFile;

        fFileFormat = MapPutFileTypeListIdxToFormat (typeIndex);
        try {
            mFile = new LFile (fileResult); // Make a temporary file object

            OSType fileType = MapFormatToOSType (fFileFormat); // Find proper file type

            // Make new file on disk
            mFile->CreateNewDataFile (kApplicationSignature, fileType, 0);
            mFile->OpenDataFork (fsRdWrPerm);
            DoSaveHelper ();
        }
        catch (...) {
            delete mFile;
            mFile       = savedFile;
            fFileFormat = savedFileFormat;
            throw;
        }
        fFileFormat = savedFileFormat;
        delete mFile;
        mFile = savedFile;
    }
}
#endif

#if qPlatform_Windows
BOOL LedItDocument::OnNewDocument ()
{
    fCommandHandler.Commit ();
    if (!COleServerDoc::OnNewDocument ()) {
        return FALSE;
    }
    fFileFormat          = eDefaultFormat;
    fStyleDatabase       = StandardStyledTextImager::StyleDatabasePtr (new StandardStyledTextImager::StyleDatabaseRep (fTextStore));
    fParagraphDatabase   = WordProcessor::ParagraphDatabasePtr (new WordProcessor::ParagraphDatabaseRep (fTextStore));
    fHidableTextDatabase = WordProcessor::HidableTextDatabasePtr (new UniformHidableTextMarkerOwner (fTextStore));
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

        WordProcessor::WordProcessorTextIOSrcStream source (&fTextStore, fStyleDatabase, fParagraphDatabase, fHidableTextDatabase);
        StyledTextIOWriterSinkStream_Memory         sink;

        switch (fFileFormat) {
            case eTextFormat: {
                StyledTextIOWriter_PlainText textWriter (&source, &sink);
                textWriter.Write ();
            } break;

            case eRTFFormat: {
                StyledTextIOWriter_RTF textWriter (&source, &sink, &fRTFInfo);
                textWriter.Write ();
            } break;

            case eHTMLFormat: {
                StyledTextIOWriter_HTML textWriter (&source, &sink, &fHTMLInfo);
                textWriter.Write ();
            } break;

            case eLedPrivateFormat: {
                StyledTextIOWriter_LedNativeFileFormat textWriter (&source, &sink);
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
        DWORD                  nLen = static_cast<DWORD> (file->GetLength ()); // maybe should subtract current offset?
        SmallStackBuffer<char> buf (nLen);
        if (ar.Read (buf, nLen) != nLen) {
            AfxThrowArchiveException (CArchiveException::endOfFile);
        }
        StyledTextIOSrcStream_Memory                 source (buf, nLen);
        WordProcessor::WordProcessorTextIOSinkStream sink (&fTextStore, fStyleDatabase, fParagraphDatabase, fHidableTextDatabase);

    ReRead:
        switch (fFileFormat) {
            case eTextFormat: {
                StyledTextIOReader_PlainText textReader (&source, &sink);
                textReader.Read ();
            } break;

            case eLedPrivateFormat: {
                LedItControlItem::DocContextDefiner    tmp (this);
                StyledTextIOReader_LedNativeFileFormat textReader (&source, &sink);
                textReader.Read ();
            } break;

            case eRTFFormat: {
                LedItControlItem::DocContextDefiner tmp (this);
                StyledTextIOReader_RTF              textReader (&source, &sink, &fRTFInfo);
                textReader.Read ();
            } break;

            case eHTMLFormat: {
                StyledTextIOReader_HTML textReader (&source, &sink, &fHTMLInfo);
                textReader.Read ();
            } break;

            case eUnknownFormat: {
                /*
                     *  Should enhance this unknown/format reading code to take into account file suffix in our guess.
                     */

                // Try RTF
                try {
                    StyledTextIOReader_RTF reader (&source, &sink, &fRTFInfo);
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
                    StyledTextIOReader_LedNativeFileFormat reader (&source, &sink);
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
                    StyledTextIOReader_HTML reader (&source, &sink);
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
    fStyleDatabase       = StandardStyledTextImager::StyleDatabasePtr (new StandardStyledTextImager::StyleDatabaseRep (fTextStore));
    fParagraphDatabase   = WordProcessor::ParagraphDatabasePtr (new WordProcessor::ParagraphDatabaseRep (fTextStore));
    fHidableTextDatabase = WordProcessor::HidableTextDatabasePtr (new UniformHidableTextMarkerOwner (fTextStore));

    // Slight performance hack - get rid of existing style/etc dbases for the current view
    {
        POSITION pos = GetFirstViewPosition ();
        while (pos != NULL) {
            CView* pView = GetNextView (pos);
            pView->OnInitialUpdate ();
        }
    }

    WordProcessor::WordProcessorFlavorPackageInternalizer internalizer (fTextStore, fStyleDatabase,
                                                                        fParagraphDatabase,
                                                                        fHidableTextDatabase);

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
    CFileDialog dlgFile (isOpenDialogCall);

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

#if qDebug
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
Led_SDK_String ExtractFileSuffix (const Led_SDK_String& from)
{
    size_t i = from.rfind ('.');
    if (i == Led_SDK_String::npos) {
        return Led_SDK_String ();
    }
    else {
        Led_SDK_String suffix = from.substr (i);
        for (size_t j = 0; j < suffix.length (); ++j) {
            if (isascii (suffix[j]) and isupper (suffix[j])) {
                suffix[j] = static_cast<Led_tChar> (tolower (suffix[j]));
            }
        }
        return suffix;
    }
}

#if qPlatform_Windows
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

#endif
