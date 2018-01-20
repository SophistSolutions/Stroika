/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef __LedLineItDocument_h__
#define __LedLineItDocument_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <afxole.h>
#include <afxwin.h>

#include "Stroika/Foundation/Characters/CodePage.h"

#include "Stroika/Frameworks/Led/ChunkedArrayTextStore.h"
#include "Stroika/Frameworks/Led/Command.h"

#include "LedLineItConfig.h"

using namespace Stroika::Foundation;

using namespace Stroika::Frameworks::Led;

using Stroika::Foundation::Characters::CodePage;
using Stroika::Foundation::Characters::CodePagesInstalled;
using Stroika::Foundation::Characters::GetDefaultSDKCodePage;
using Stroika::Foundation::Characters::kCodePage_UTF8;

const CodePage kDefaultNewDocCodePage = CodePagesInstalled ().IsCodePageAvailable (kCodePage_UTF8) ? kCodePage_UTF8 : GetDefaultSDKCodePage ();

const CodePage kAutomaticallyGuessCodePage = -2; // I HOPE this # doesn't conflict with any legit ones!!!
const CodePage kIGNORECodePage             = -3; // I HOPE this # doesn't conflict with any legit ones!!!

class LedLineItServerItem;

class LedLineItDocument : public COleServerDoc, public MarkerOwner {
protected: // create from serialization only
    LedLineItDocument ();
    DECLARE_DYNCREATE (LedLineItDocument)

public:
    virtual ~LedLineItDocument ();

public:
    virtual void       DidUpdateText (const UpdateInfo& /*updateInfo*/) noexcept override;
    virtual TextStore* PeekAtTextStore () const override;

public:
    nonvirtual LedLineItServerItem* GetEmbeddedItem ();

public:
    virtual BOOL OnNewDocument () override;
    virtual BOOL OnOpenDocument (LPCTSTR lpszPathName) override;
    virtual void Serialize (CArchive& ar) override;

protected:
    virtual COleServerItem* OnGetEmbeddedItem () override;

public:
    virtual BOOL DoSave (LPCTSTR lpszPathName, BOOL bReplace) override;

public:
    static CodePage LedLineItDocument::sHiddenDocOpenArg;

public:
    afx_msg void OnUpdateFileSave (CCmdUI* pCmdUI);
    afx_msg void OnFileSaveCopyAs ();
    virtual void DeleteContents () override;

public:
    nonvirtual TextStore& GetTextStore ();
    nonvirtual CommandHandler& GetCommandHandler ();

protected:
    ChunkedArrayTextStore        fTextStore;
    MultiLevelUndoCommandHandler fCommandHandler;

private:
    CodePage fCodePage;

    // utilities to pick save/open file names
public:
    static bool DoPromptSaveAsFileName (::CString* fileName, CodePage* codePage);
    static bool DoPromptSaveCopyAsFileName (::CString* fileName, CodePage* codePage);
    static bool DoPromptOpenFileName (::CString* fileName, CodePage* codePage);

private:
    static bool DoPromptFileName (::CString* fileName, UINT nIDSTitle, bool isOpenDialogCall, long fileDLogFlags, CodePage* codePage);

#if qDebug
public:
    virtual void AssertValid () const override;
#endif

    // Generated message map functions
protected:
    DECLARE_MESSAGE_MAP ()

    DECLARE_DISPATCH_MAP ()
    DECLARE_INTERFACE_MAP ()
};

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
inline LedLineItServerItem* LedLineItDocument::GetEmbeddedItem ()
{
    return (LedLineItServerItem*)COleServerDoc::GetEmbeddedItem ();
}
inline TextStore& LedLineItDocument::GetTextStore ()
{
    return fTextStore;
}
inline CommandHandler& LedLineItDocument::GetCommandHandler ()
{
    return fCommandHandler;
}

#endif /*__LedLineItDocument_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
