/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__Toolbar_h__
#define	__Toolbar_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveLedIt/Headers/Toolbar.h,v 2.11 2004/02/10 03:20:02 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Toolbar.h,v $
 *	Revision 2.11  2004/02/10 03:20:02  lewis
 *	SPR#1634: add a few NonIdleContext calls around OLE AUT calls in ActiveLedIt (since they have similar effects to commands). SPR#1631: Used new OnEnterIdle (insead of SpendIdleTime()) to handle updating toolbars
 *	
 *	Revision 2.10  2004/02/08 14:09:10  lewis
 *	SPR#1640: fix display of multiple toolbars by creating a window (with toolbar edges) assocaited with the toolbar list, and get rid of edges with toolbar itself. Had to change some COM apis and way AL/Toolbarlist interaction/creation worked (make it more like toolbarlist/toolbar interaction). Seems to work pretty well now
 *	
 *	Revision 2.9  2004/02/07 21:20:13  lewis
 *	SPR#1627: add IALToolbar::MergeAdd () method - and use it internally, and in a bunch of samples in javascript. Also -
 *	fixed bug with popup menu handling enabling code (hierarchical menus) and tested that in javascript testing sample
 *	
 *	Revision 2.8  2004/02/07 01:32:35  lewis
 *	SPR#1629: update: Added font-color icon, and associated the icon with the color popup menu. Added support to
 *	ActiveLedIt_IconButtonToolbarElement to make that work. Including - changed ToggleButton property to a
 *	'ButtonStyle' property - where one style was 'ICON/POPUP MENU'.
 *	
 *	Revision 2.7  2004/02/06 16:12:16  lewis
 *	SPR#1629: Update: Lose private 'width' property for combobox class, and instead - autocompute the
 *	prefered width based on the attached menu. Improved the automatic LAYOUT code so that changes to the
 *	menu should resize the combobox.
 *	
 *	Revision 2.6  2004/02/06 01:36:00  lewis
 *	SPR#1629: added fairly functional (not perfect) ActiveLedIt_ComboBoxToolbarElement - and used that to
 *	implement first cut at fontname and fontsize combo boxes.
 *	
 *	Revision 2.5  2004/02/03 22:29:29  lewis
 *	SPR#1618: add ToggleButton property to IconButtonToolbarItem, and CommandChecked () method to ActiveLedIt -
 *	to support checked buttons - like BOLD, ITALIC, and UNDERLINE
 *	
 *	Revision 2.4  2004/02/03 14:51:29  lewis
 *	SPR#1618: added bold, italics, underline (fixed paste icon) to toolbar. Fixed SETFOCUS issue clicking
 *	on toolbar buttons
 *	
 *	Revision 2.3  2004/02/03 02:23:05  lewis
 *	SPR#1618: Added undo/redo buttons, and separator support
 *	
 *	Revision 2.2  2004/02/03 00:50:12  lewis
 *	SPR#1618: update enable state of buttons according to COMMAND-CHAIN OnUpdateCommand. May possibly need
 *	future OPTIMIZATION (to not call too often) - but empirically - seems OK for now
 *	
 *	Revision 2.1  2004/02/02 21:15:10  lewis
 *	SPR#1618: Added preliminary toolbar support. A small toolbar with a few icons appears - but no DO
 *	command or UPDATE_ENABLE_DISABLE CMD support yet (and just plain icon cmd items)
 *	
 *
 *
 *
 *
 */
#include	<afxctl.h>
#include	<atlwin.h>

#include	"IdleManager.h"
#include	"Led_ATL.h"
#include	"TextInteractor.h"

#include	"ActiveLedItConfig.h"
#include	"Resource.h"
#include	"UserConfigCommands.h"

#include	"ActiveLedIt_h.h"







class	ATL_NO_VTABLE	ActiveLedIt_IconButtonToolbarElement : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<ActiveLedIt_IconButtonToolbarElement>,
	public IDispatchImpl<IALToolbarElement, &IID_IALToolbarElement, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion>,
	public IDispatchImpl<IALIconButtonToolbarElement, &IID_IALIconButtonToolbarElement, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion>,
	public CWindowImpl<ActiveLedIt_IconButtonToolbarElement>
{
	public:
		ActiveLedIt_IconButtonToolbarElement ();
		virtual ~ActiveLedIt_IconButtonToolbarElement ();

	public:
		DECLARE_NO_REGISTRY ()

	public:
		DECLARE_WND_SUPERCLASS	(NULL, _T("Button"))
		BEGIN_MSG_MAP(ActiveLedIt_IconButtonToolbarElement)
			MESSAGE_HANDLER		(WM_LBUTTONDOWN,	OnLButtonDown)
			MESSAGE_HANDLER		(WM_LBUTTONUP,		OnLButtonUp)
			MESSAGE_HANDLER		(WM_MOUSEMOVE,		OnMouseMove)
		END_MSG_MAP()
		nonvirtual	LRESULT		OnCreate (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		nonvirtual	LRESULT		OnSetFocus (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		nonvirtual	LRESULT		OnLButtonDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		nonvirtual	LRESULT		OnLButtonUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		nonvirtual	LRESULT		OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	public:
		BEGIN_COM_MAP(ActiveLedIt_IconButtonToolbarElement)
			COM_INTERFACE_ENTRY(IALIconButtonToolbarElement)
			COM_INTERFACE_ENTRY2(IDispatch, IALIconButtonToolbarElement)
			COM_INTERFACE_ENTRY2(IALToolbarElement, IALIconButtonToolbarElement)
		END_COM_MAP()

	public:
		DECLARE_PROTECT_FINAL_CONSTRUCT()

	//	IALToolbarElement
	public:
		STDMETHOD(get_PreferredHeight)(UINT* pVal);
		STDMETHOD(get_PreferredWidth)(UINT* pVal);
		STDMETHOD(get_X)(INT* pVal);
		STDMETHOD(get_Y)(INT* pVal);
		STDMETHOD(get_Width)(UINT* pVal);
		STDMETHOD(get_Height)(UINT* pVal);
		STDMETHOD(SetRectangle)(int X, int Y, UINT width, UINT height);
		STDMETHOD(NotifyOfOwningToolbar)(IDispatch* owningToolbar, IDispatch* owningActiveLedIt);
		STDMETHOD(UpdateEnableState)();


	//	IALIconButtonToolbarElement
	public:
		STDMETHOD(get_ButtonImage)(IDispatch** pVal);
		STDMETHOD(put_ButtonImage)(IDispatch* val);
		STDMETHOD(get_Command)(VARIANT* pVal);
		STDMETHOD(put_Command)(VARIANT val);
		STDMETHOD(get_ButtonStyle)(IconButtonStyle* pVal);
		STDMETHOD(put_ButtonStyle)(IconButtonStyle val);

	public:
		HRESULT	FinalConstruct ();
		void	FinalRelease ();

	private:
		nonvirtual	void	UpdateButtonObj ();
		nonvirtual	void	CheckFixButtonStyle ();

	private:
		HWND					fHWnd;
		bool					fPressedOnClick;
		CComQIPtr<IPicture>		fButtonImage;
		CComVariant				fCommand;
		IconButtonStyle			fIconButtonStyle;
		IDispatch*				fOwningToolbar;
		IDispatch*				fOwningActiveLedIt;
		Led_Rect				fBounds;
};





class	ATL_NO_VTABLE	ActiveLedIt_ComboBoxToolbarElement : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<ActiveLedIt_ComboBoxToolbarElement>,
	public IDispatchImpl<IALToolbarElement, &IID_IALToolbarElement, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion>,
	public IDispatchImpl<IALComboBoxToolbarElement, &IID_IALComboBoxToolbarElement, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion>,
	public CWindowImpl<ActiveLedIt_ComboBoxToolbarElement>
{
	public:
		ActiveLedIt_ComboBoxToolbarElement ();
		virtual ~ActiveLedIt_ComboBoxToolbarElement ();

	public:
		DECLARE_NO_REGISTRY ()

	public:
		DECLARE_WND_CLASS (NULL)
		BEGIN_MSG_MAP(ActiveLedIt_ComboBoxToolbarElement)
			COMMAND_HANDLER		(0,				CBN_DROPDOWN,		OnCBDropDown)
			COMMAND_HANDLER		(0,				CBN_CLOSEUP,		OnCBCloseUp)
			COMMAND_HANDLER		(0,				CBN_SELCHANGE,		OnCBSelChange)
		END_MSG_MAP()
		nonvirtual	LRESULT		OnCBSelChange (USHORT uMsg, USHORT wParam, HWND ctlHandle, BOOL& bHandled);
		nonvirtual	LRESULT		OnCBDropDown (USHORT uMsg, USHORT wParam, HWND ctlHandle, BOOL& bHandled);
		nonvirtual	LRESULT		OnCBCloseUp (USHORT uMsg, USHORT wParam, HWND ctlHandle, BOOL& bHandled);

	public:
		BEGIN_COM_MAP(ActiveLedIt_ComboBoxToolbarElement)
			COM_INTERFACE_ENTRY2(IDispatch, IALComboBoxToolbarElement)
			COM_INTERFACE_ENTRY2(IALToolbarElement, IALComboBoxToolbarElement)
			COM_INTERFACE_ENTRY(IALComboBoxToolbarElement)
		END_COM_MAP()

	public:
		DECLARE_PROTECT_FINAL_CONSTRUCT()

	//	IALToolbarElement
	public:
		STDMETHOD(get_PreferredHeight)(UINT* pVal);
		STDMETHOD(get_PreferredWidth)(UINT* pVal);
		STDMETHOD(get_X)(INT* pVal);
		STDMETHOD(get_Y)(INT* pVal);
		STDMETHOD(get_Width)(UINT* pVal);
		STDMETHOD(get_Height)(UINT* pVal);
		STDMETHOD(SetRectangle)(int X, int Y, UINT width, UINT height);
		STDMETHOD(NotifyOfOwningToolbar)(IDispatch* owningToolbar, IDispatch* owningActiveLedIt);
		STDMETHOD(UpdateEnableState)();

	//	IALComboBoxToolbarElement
	public:
		STDMETHOD(get_CommandList)(IDispatch** pVal);
		STDMETHOD(put_CommandList)(IDispatch* val);

	// Specify elements
	private:
		Led_Distance	fPreferredWidth;

	public:
		HRESULT	FinalConstruct ();
		void	FinalRelease ();

	private:
		nonvirtual	void	UpdatePopupObj ();
		nonvirtual	void	CallInvalidateLayout ();

	private:
		bool							fDropDownActive;
		CComPtr<IDispatch>				fCommandList;
		vector<CComPtr<IALCommand> >	fCommandListCache;
		HWND							fHWnd;
		CWindow							fComboBox;		// in sub-window because of how combobox sends notifications (to parent - not self)
		bool							fPressedOnClick;
		IDispatch*						fOwningToolbar;
		IDispatch*						fOwningActiveLedIt;
		Led_Rect						fBounds;
};




class	ATL_NO_VTABLE	ActiveLedIt_SeparatorToolbarElement : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<ActiveLedIt_SeparatorToolbarElement>,
	public IDispatchImpl<IALToolbarElement, &IID_IALToolbarElement, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion>
{
	public:
		ActiveLedIt_SeparatorToolbarElement ();
		virtual ~ActiveLedIt_SeparatorToolbarElement ();

	public:
		DECLARE_NO_REGISTRY ()

	public:
		BEGIN_COM_MAP(ActiveLedIt_SeparatorToolbarElement)
			COM_INTERFACE_ENTRY(IALToolbarElement)
			COM_INTERFACE_ENTRY(IDispatch)
		END_COM_MAP()

	public:
		DECLARE_PROTECT_FINAL_CONSTRUCT()

	//	IALToolbarElement
	public:
		STDMETHOD(get_PreferredHeight)(UINT* pVal);
		STDMETHOD(get_PreferredWidth)(UINT* pVal);
		STDMETHOD(get_X)(INT* pVal);
		STDMETHOD(get_Y)(INT* pVal);
		STDMETHOD(get_Width)(UINT* pVal);
		STDMETHOD(get_Height)(UINT* pVal);
		STDMETHOD(SetRectangle)(int X, int Y, UINT width, UINT height);
		STDMETHOD(NotifyOfOwningToolbar)(IDispatch* owningToolbar, IDispatch* owningActiveLedIt);
		STDMETHOD(UpdateEnableState)();

	public:
		HRESULT	FinalConstruct ();
		void	FinalRelease ();

	private:
		Led_Rect	fBounds;
};






class	ATL_NO_VTABLE	ActiveLedIt_Toolbar : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<ActiveLedIt_Toolbar>,
	public IDispatchImpl<IALToolbar, &IID_IALToolbar, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion>,
	public CWindowImpl<ActiveLedIt_Toolbar>,
	public EnterIdler
{
	public:
		ActiveLedIt_Toolbar ();
		virtual ~ActiveLedIt_Toolbar ();

	public:
		DECLARE_NO_REGISTRY ()

	public:
		DECLARE_WND_CLASS_EX (NULL, CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, COLOR_BTNFACE);
		BEGIN_MSG_MAP(ActiveLedIt_Toolbar)
			MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		END_MSG_MAP()
	protected:
		nonvirtual	LRESULT	OnCommand (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	public:
		enum	{ kHSluff			=	1	};
		enum	{ kVInset			=	3	};
		enum	{ kHTBEdge			=	3	};

	public:
		BEGIN_COM_MAP(ActiveLedIt_Toolbar)
			COM_INTERFACE_ENTRY(IALToolbar)
			COM_INTERFACE_ENTRY(IDispatch)
		END_COM_MAP()

	public:
		DECLARE_PROTECT_FINAL_CONSTRUCT()

	protected:
		override	void	OnEnterIdle ();

	public:
		STDMETHOD(get__NewEnum)(IUnknown** ppUnk);
		STDMETHOD(get_Item)(long Index, IDispatch** pVal);
		STDMETHOD(get_Count)(long* pVal);
		STDMETHOD(Add) (IDispatch* newElt, UINT atIndex = 0xffffffff);
		STDMETHOD(MergeAdd)(IDispatch* newElts, UINT afterElt = static_cast<UINT> (-1));
		STDMETHOD(Remove)(VARIANT eltIntNameOrIndex);
		STDMETHOD(Clear)();
		STDMETHOD(get_hWnd)(HWND* pVal);
		STDMETHOD(get_PreferredHeight)(UINT* pVal);
		STDMETHOD(get_PreferredWidth)(UINT* pVal);
		STDMETHOD(NotifyOfOwningActiveLedIt)(IDispatch* owningActiveLedIt, IDispatch* owningALToolbar);
		STDMETHOD(SetRectangle)(int X, int Y, UINT width, UINT height);

	private:
		nonvirtual	void	CallInvalidateLayout ();
		nonvirtual	void	DoLayout ();

	public:
		HRESULT	FinalConstruct ();
		void	FinalRelease ();

	private:
		vector<CComPtr<IDispatch> >	fToolbarItems;
		IDispatch*					fOwningActiveLedIt;		// don't use CComPtr<> cuz don't want to addref and create circular reference
		IDispatch*					fOwningALToolbar;		//	''
		Led_Rect					fBounds;
};









class	ATL_NO_VTABLE	ActiveLedIt_ToolbarList : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<ActiveLedIt_ToolbarList>,
	public IDispatchImpl<IALToolbarList, &IID_IALToolbarList, &LIBID_ACTIVELEDITLib, qActiveLedIt_MajorVersion, qActiveLedIt_MinorVersion>,
	public CWindowImpl<ActiveLedIt_ToolbarList>
{
	public:
		ActiveLedIt_ToolbarList ();
		virtual ~ActiveLedIt_ToolbarList ();

	public:
		DECLARE_NO_REGISTRY ()

	public:
		enum	{ kRoomForBotLine	=	1	};

	public:
		DECLARE_WND_CLASS_EX (NULL, CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, COLOR_BTNFACE);
		BEGIN_MSG_MAP(ActiveLedIt_ToolbarList)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
		END_MSG_MAP()
	protected:
		nonvirtual	LRESULT	OnPaint (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	public:
		BEGIN_COM_MAP(ActiveLedIt_ToolbarList)
			COM_INTERFACE_ENTRY(IALToolbarList)
			COM_INTERFACE_ENTRY(IDispatch)
		END_COM_MAP()

	public:
		DECLARE_PROTECT_FINAL_CONSTRUCT()

	public:
		STDMETHOD(get__NewEnum)(IUnknown** ppUnk);
		STDMETHOD(get_Item)(long Index, IDispatch** pVal);
		STDMETHOD(get_Count)(UINT* pVal);
		STDMETHOD(Add) (IDispatch* newElt, UINT atIndex = 0xffffffff);
		STDMETHOD(Remove)(VARIANT eltIntNameOrIndex);
		STDMETHOD(Clear)();
		STDMETHOD(get_hWnd)(HWND* pVal);
		STDMETHOD(get_PreferredHeight)(UINT* pVal);
		STDMETHOD(get_PreferredWidth)(UINT* pVal);
		STDMETHOD(NotifyOfOwningActiveLedIt)(IDispatch* owningActiveLedIt, HWND owningHWND);
		STDMETHOD(SetRectangle)(int X, int Y, UINT width, UINT height);

	private:
		nonvirtual	void	CallInvalidateLayout ();
		nonvirtual	void	DoLayout ();

	public:
		HRESULT	FinalConstruct ();
		void	FinalRelease ();

	private:
		vector<CComPtr<IDispatch> >	fToolbars;
		IDispatch*					fOwningActiveLedIt;		// don't use CComPtr<> cuz don't want to addref and create circular reference
		Led_Rect					fBounds;
};




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif	/*__Toolbar_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

