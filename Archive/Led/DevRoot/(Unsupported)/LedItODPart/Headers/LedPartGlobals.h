#ifndef _LEDPARTGLOBALS_
#define _LEDPARTGLOBALS_

#include <MenuBar.xh>
#include <ODTypesB.xh>

#include <IText.h>




// -- Forward Class declarations --
class ODShape;
class ODFocusSet;


struct	LedPartGlobals;

extern	ODUShort		gGlobalsUsageCount;
extern	LedPartGlobals*	gGlobals;
	
struct LedPartGlobals {
	public:
	LedPartGlobals();
	~LedPartGlobals() {}

	ODMenuBar*		fMenuBar;
	ODPlatformMenu	fFontNameMenu;
	ODPlatformMenu	fFontSizeMenu;
	ODPlatformMenu	fFontStyleMenu;

	ODFocusSet*		fUIFocusSet;	
				
	ODTypeToken		fSelectionFocus;
	ODTypeToken		fMenuFocus;
	ODTypeToken		fModalFocus;
	ODTypeToken		fClipboardFocus;
	ODTypeToken		fKeyFocus;
	
	ODTypeToken		fFrameView;
	ODTypeToken		fLargeIconView;
	ODTypeToken		fSmallIconView;
	ODTypeToken		fThumbnailView;
	
	ODTypeToken		fMainPresentation;
	ODTypeToken		fUndefinedPresentation;
	
	ODType			fCurrentTextPartKind;
			
	ODScriptCode	fEditorsScript;
	ODLangCode		fEditorsLanguage;

// from textentions editor - not sure we need to keep all this crap...
	ODValueType		fAppleHFSFlavor;
	ODValueType		fScrapTextValue;
	#if		qSTTXTSupported
		ODValueType		fStyledTextKind;
	#endif
	ODValueType		fSTYLKind;
	ODValueType		fTextFileValue;
};







/// IMP DETAILS

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include	"LedPartDef.h"


inline LedPartGlobals::LedPartGlobals()
{
	fMenuBar				= kODNULL;
	fFontNameMenu				= kODNULL;
	fFontSizeMenu				= kODNULL;
	fFontStyleMenu				= kODNULL;
	fUIFocusSet				= kODNULL;	
				
	fSelectionFocus			= kODNullTypeToken;
	fMenuFocus				= kODNullTypeToken;
	fModalFocus				= kODNullTypeToken;
	fKeyFocus				= kODNullTypeToken;
	fClipboardFocus			= kODNullTypeToken;
	
	fFrameView				= kODNullTypeToken;
	fLargeIconView			= kODNullTypeToken;
	fSmallIconView			= kODNullTypeToken;
	fThumbnailView			= kODNullTypeToken;
	
	fMainPresentation		= kODNullTypeToken;
	fUndefinedPresentation	= kODNullTypeToken;
			
	fCurrentTextPartKind	= kODNullTypeToken;
	
	fEditorsScript			= 0;
	fEditorsLanguage		= 0;
}
			

#endif