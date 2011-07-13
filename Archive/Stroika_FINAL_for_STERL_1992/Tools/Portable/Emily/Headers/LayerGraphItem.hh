/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__LayerGraphItem__
#define	__LayerGraphItem__

/*
 * $Header: /fuji/lewis/RCS/LayerGraphItem.hh,v 1.3 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: LayerGraphItem.hh,v $
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 *
 */

#include	"ViewItem.hh"

class	LayerGraph;
class	LayerGraphItem : public ViewItem {
	public:
		LayerGraphItem (ItemType& type);

		override	Boolean	IsButton ();
		override	Boolean	IsSlider ();
		override	Boolean	IsText ();
		override	Boolean	IsFocusItem (CommandNumber gui);
		override	Boolean	ItemCanBeEnabled ();
		
		nonvirtual	LayerGraph&	GetLayerGraph () const;
	
		override	String	GetHeaderFileName ();

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;

		override	void	WriteBuilder (class ostream& to, int tabCount);
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);

	private:
		LayerGraph*	fLayerGraph;
};

class	LayerGraphItemType : public ItemType {
	public:
		LayerGraphItemType ();
		
		static	LayerGraphItemType&	Get ();
		
	private:		
		static	ViewItem*	LayerGraphItemBuilder ();
		static	LayerGraphItemType*	sThis;
};

#endif	/* __LayerGraphItem__ */
