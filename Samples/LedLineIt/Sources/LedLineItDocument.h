/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef __LedLineItDocument_h__
#define __LedLineItDocument_h__ 1

#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <afxwin.h>
#include    <afxole.h>

#include    "Stroika/Frameworks/Led/CodePage.h"
#include    "Stroika/Frameworks/Led/Command.h"
#include    "Stroika/Frameworks/Led/ChunkedArrayTextStore.h"

#include    "LedLineItConfig.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Frameworks::Led;



const   CodePage    kDefaultNewDocCodePage  =   CodePagesInstalled::IsCodePageAvailable (kCodePage_UTF8) ? kCodePage_UTF8 : CodePagesInstalled::GetDefaultCodePage ();


const   CodePage    kAutomaticallyGuessCodePage =   -2; // I HOPE this # doesn't conflict with any legit ones!!!
const   CodePage    kIGNORECodePage             =   -3; // I HOPE this # doesn't conflict with any legit ones!!!


class   LedLineItServerItem;

class   LedLineItDocument : public COleServerDoc, public MarkerOwner {
protected: // create from serialization only
    LedLineItDocument ();
    DECLARE_DYNCREATE(LedLineItDocument)

public:
    virtual ~LedLineItDocument();

public:
    override    void        DidUpdateText (const UpdateInfo& /*updateInfo*/) throw ();
    override    TextStore*  PeekAtTextStore () const;

public:
    nonvirtual  LedLineItServerItem* GetEmbeddedItem ();

public:
    override    BOOL    OnNewDocument ();
    override    BOOL    OnOpenDocument (LPCTSTR lpszPathName);
    override    void    Serialize (CArchive& ar);

protected:
    override    COleServerItem* OnGetEmbeddedItem ();

public:
    override    BOOL DoSave (LPCTSTR lpszPathName, BOOL bReplace);


public:
    static  CodePage    LedLineItDocument::sHiddenDocOpenArg;

public:
    afx_msg     void    OnUpdateFileSave (CCmdUI* pCmdUI);
    afx_msg     void    OnFileSaveCopyAs ();
    override    void    DeleteContents ();

public:
    nonvirtual  TextStore&          GetTextStore ();
    nonvirtual  CommandHandler&     GetCommandHandler ();
protected:
    ChunkedArrayTextStore           fTextStore;
    MultiLevelUndoCommandHandler    fCommandHandler;

private:
    CodePage    fCodePage;


    // utilities to pick save/open file names
public:
    static  bool    DoPromptSaveAsFileName (CString* fileName, CodePage* codePage);
    static  bool    DoPromptSaveCopyAsFileName (CString* fileName, CodePage* codePage);
    static  bool    DoPromptOpenFileName (CString* fileName, CodePage* codePage);
private:
    static  bool    DoPromptFileName (CString* fileName, UINT nIDSTitle, bool isOpenDialogCall, long fileDLogFlags, CodePage* codePage);

#if     qDebug
public:
    override    void    AssertValid () const;
#endif

    // Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()

    DECLARE_DISPATCH_MAP()
    DECLARE_INTERFACE_MAP()
};





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
inline  LedLineItServerItem* LedLineItDocument::GetEmbeddedItem ()
{
    return (LedLineItServerItem*)COleServerDoc::GetEmbeddedItem();
}
inline  TextStore&          LedLineItDocument::GetTextStore ()
{
    return fTextStore;
}
inline  CommandHandler& LedLineItDocument::GetCommandHandler ()
{
    return fCommandHandler;
}

#endif  /*__LedLineItDocument_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

