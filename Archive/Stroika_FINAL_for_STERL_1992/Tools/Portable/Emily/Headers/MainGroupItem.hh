/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__MainGroupItem__
#define	__MainGroupItem__

/*
 * $Header: /fuji/lewis/RCS/MainGroupItem.hh,v 1.3 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: MainGroupItem.hh,v $
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 *
 */

#include	"GroupItem.hh"

class	EmilyWindow;
class	MainGroupItem : public GroupItem, public TextController {
	public:
		MainGroupItem (EmilyWindow& window);
		~MainGroupItem ();

		override	Boolean		DoCommand (const CommandSelection& selection);
		override	void		DoSetupMenus ();
		override	Boolean		TrackPress (const MousePressInfo& mouseInfo);
		override	void		Draw (const Region& update);
		override	void		Layout ();
		
		override	GroupItem*	GetMostSelectedGroup ();
		override	GroupItem*	GetGroupContaining (const Point& point);
		
		override	void	DirtyDocument ();

		override	void	SetPastePoint (const Point& point);
		override	Point	GetPastePoint () const;
		
		override	void	GUIChanged (CommandNumber oldGUI, CommandNumber newGUI);
		override	void	LanguageChanged (CommandNumber oldLanguage, CommandNumber newLanguage);

		override	void	WriteInitializer (class ostream& to, int tabCount, CommandNumber gui);

		nonvirtual	void	ResetCustomizeOnly ();
		
		nonvirtual	void	ResetFieldCounter ();

		override	String	GetHeaderFileName ();
		
		nonvirtual	Boolean	GetAutoSize () const;
		nonvirtual	void	SetAutoSize (Boolean autoSize);
				
		// given public scope
		nonvirtual	CommandNumber	GetBaseGUI () const;
		nonvirtual	CommandNumber	GetBaseLanguage () const;

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;

		override	void	SetSelected_ (Boolean selected, UpdateMode update);
		override	Boolean	CalcFramed () const;
		
		override	String	CalcFieldName ();
			
	private:
		EmilyWindow&	fWindow;
		Point			fPastePoint;
		int				fFieldCounter;
		class PixelMap*	fCachedPixelMap;
		Point			fOldGrid;
		Point			fOldSize;
		Boolean			fAutoSize;
};



#endif	/* __MainGroupItem__ */
