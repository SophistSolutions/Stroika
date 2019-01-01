/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */

#ifndef __LedItDocument_h__
#define __LedItDocument_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#if qPlatform_MacOS
#include <LSingleDoc.h>
#elif defined(WIN32)
#include <afxole.h>
#include <afxwin.h>
#endif

#include "Stroika/Frameworks/Led/ChunkedArrayTextStore.h"
#include "Stroika/Frameworks/Led/Command.h"
#include "Stroika/Frameworks/Led/StyledTextIO/StyledTextIO.h"
#include "Stroika/Frameworks/Led/StyledTextIO/StyledTextIO_HTML.h"
#include "Stroika/Frameworks/Led/StyledTextIO/StyledTextIO_RTF.h"
#include "Stroika/Frameworks/Led/StyledTextImager.h"
#include "Stroika/Frameworks/Led/WordProcessor.h"

#include "LedItConfig.h"

class LedItView;

#if qPlatform_Windows
class LedItServerItem;
#endif

class LedItDocument :
#if qPlatform_MacOS
    public LSingleDoc,
#elif qPlatform_Windows
    public COleServerDoc,
#endif
    public MarkerOwner {
#if qPlatform_MacOS
public:
    LedItDocument (LCommander* inSuper, FileFormat format);
#elif qPlatform_Windows
protected: // create from serialization only
    LedItDocument ();
    DECLARE_DYNCREATE (LedItDocument)
#elif qXWindows
public:
    LedItDocument ();
#endif

public:
    virtual ~LedItDocument ();

public:
    virtual void       DidUpdateText (const UpdateInfo& /*updateInfo*/) noexcept override;
    virtual TextStore* PeekAtTextStore () const override;

#if qPlatform_MacOS
    // Call exactly once (inFileSpec==NULL for new window)
public:
    nonvirtual void BuildDocWindow (const FSSpec* inFileSpec);

public:
    static const vector<LWindow*>& GetDocumentWindows ();

public:
    virtual Boolean ObeyCommand (CommandT inCommand, void* ioParam) override;
    virtual void    FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark,
                                       UInt16& outMark, Str255 outName) override;

    nonvirtual void OnSaveACopyAsCommand ();

public:
    virtual Boolean IsModified () override;

    virtual Boolean AskSaveAs (FSSpec& outFSSpec, Boolean inRecordIt) override;

    virtual void DoAESave (FSSpec& inFileSpec, OSType inFileType) override;
    virtual void DoSave () override;
    virtual void DoRevert () override;
    virtual void DoPrint () override;

public:
    nonvirtual void PurgeUnneededMemory ();

private:
    nonvirtual void DoReadCode ();
#elif qPlatform_Windows
public:
    nonvirtual LedItServerItem* GetEmbeddedItem ();

public:
    virtual BOOL OnNewDocument () override;
    virtual void Serialize (CArchive& ar) override;

public:
    static FileFormat sHiddenDocOpenArg;
    virtual BOOL      OnOpenDocument (LPCTSTR lpszPathName) override;

protected:
    virtual COleServerItem* OnGetEmbeddedItem () override;

public:
    virtual BOOL DoSave (LPCTSTR lpszPathName, BOOL bReplace) override;

public:
    afx_msg void OnUpdateFileSave (CCmdUI* pCmdUI);
    afx_msg void OnFileSaveCopyAs ();
    virtual void DeleteContents () override;
#endif

public:
    nonvirtual TextStore& GetTextStore ();
    nonvirtual StandardStyledTextImager::StyleDatabasePtr& GetStyleDatabase ();
    nonvirtual WordProcessor::ParagraphDatabasePtr& GetParagraphDatabase ();
    nonvirtual WordProcessor::HidableTextDatabasePtr& GetHidableTextDatabase ();
    nonvirtual CommandHandler& GetCommandHandler ();

protected:
public:
    ChunkedArrayTextStore                      fTextStore;
    StyledTextIO::RTFInfo                      fRTFInfo;
    StandardStyledTextImager::StyleDatabasePtr fStyleDatabase;
    WordProcessor::ParagraphDatabasePtr        fParagraphDatabase;
    WordProcessor::HidableTextDatabasePtr      fHidableTextDatabase;
    MultiLevelUndoCommandHandler               fCommandHandler;

#if qXWindows
public:
    nonvirtual void LoadFromFile (const string& fileName, FileFormat fileFormat);
    nonvirtual void Save ();
#endif

#if qPlatform_Windows
    // utilities to pick save/open file names
public:
    static bool DoPromptSaveAsFileName (CString& fileName, FileFormat* fileFormat);
    static bool DoPromptSaveCopyAsFileName (CString& fileName, FileFormat* fileFormat);
    static bool DoPromptOpenFileName (CString& fileName, FileFormat* fileFormat);

private:
    static bool DoPromptFileName (CString& fileName, UINT nIDSTitle, bool isOpenDialogCall, long fileDLogFlags, FileFormat* fileFormat);
#endif

#if qXWindows
private:
public: //tmphack to allow easier change of old code...
    string fPathName;
#endif
private:
public:
    FileFormat             fFileFormat;
    StyledTextIO::HTMLInfo fHTMLInfo;

#if qPlatform_MacOS
protected:
    nonvirtual void DoSaveHelper ();

public:
    nonvirtual LedItView* GetTextView () const { return fTextView; }

private:
    LedItView* fTextView;

    nonvirtual void NameNewDoc ();
    nonvirtual void OpenFile (const FSSpec& inFileSpec);
#endif
#if qPlatform_Windows
#if qDebug
public:
    virtual void AssertValid () const override;
#endif

    // Generated message map functions
protected:
    DECLARE_MESSAGE_MAP ()

    DECLARE_DISPATCH_MAP ()
    DECLARE_INTERFACE_MAP ()
#endif
};

Led_SDK_String ExtractFileSuffix (const Led_SDK_String& from);

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#if qPlatform_Windows
inline LedItServerItem* LedItDocument::GetEmbeddedItem ()
{
    return (LedItServerItem*)COleServerDoc::GetEmbeddedItem ();
}
#endif
inline TextStore& LedItDocument::GetTextStore ()
{
    return fTextStore;
}
inline StandardStyledTextImager::StyleDatabasePtr& LedItDocument::GetStyleDatabase ()
{
    return fStyleDatabase;
}
inline WordProcessor::ParagraphDatabasePtr& LedItDocument::GetParagraphDatabase ()
{
    return fParagraphDatabase;
}
inline WordProcessor::HidableTextDatabasePtr& LedItDocument::GetHidableTextDatabase ()
{
    return fHidableTextDatabase;
}
inline CommandHandler& LedItDocument::GetCommandHandler ()
{
    return fCommandHandler;
}

#endif /*__LedItDocument_h__*/
