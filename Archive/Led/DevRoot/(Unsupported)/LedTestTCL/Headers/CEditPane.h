#ifndef	_H_CEditPane
#define _H_CEditPane

#include	"CEditText.h"


#ifndef	qUseBuiltinTE
	#define	qUseBuiltinTE	0
#endif
#ifndef	qStyledTextEdit
	#define	qStyledTextEdit	1
#endif
#ifndef	qUseLedUndo
	#define	qUseLedUndo	!qUseBuiltinTE
#endif


#if		qUseLedUndo
	#include	"Command.hh"
#endif

#if		!qUseBuiltinTE
	#include	"Led.hh"
#endif


#if		qUseBuiltinTE
	#define	qEditPaneBaseClass	CEditText
#else
	#define	qEditPaneBaseClass	StandardStyledWordWrappedLed_TCL
#endif


class	CEditPane : public qEditPaneBaseClass {
	public:
		CEditPane (CView *anEnclosure, CBureaucrat *aSupervisor);
		virtual ~CEditPane ();

	protected:
		typedef	qEditPaneBaseClass	inherited;

	public:
		void	UpdateMenus();
		void	DoCommand(long theCommand);

	public:
		void	OnFindEtcCommand (long theMenuCommand);
	private:
		char*	fLastSearchText;
		bool	fWrapSearch;
		bool	fWholeWordSearch;
		bool	fCaseSensativeSearch;

	public:
		void	DoKeyDown(char theChar, Byte keyCode, EventRecord *macEvent);
		void	DoAutoKey(char theChar, Byte keyCode, EventRecord *macEvent);

#if		qUseLedUndo
	private:
		MultiLevelUndoCommandHandler	fCommandHandler;
#endif
};


#endif	/*_H_CEditPane*/
