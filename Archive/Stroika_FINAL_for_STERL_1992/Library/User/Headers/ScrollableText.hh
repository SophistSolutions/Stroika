/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__TextScroller__
#define	__TextScroller__	1

/*
 * $Header: /fuji/lewis/RCS/ScrollableText.hh,v 1.3 1992/09/01 15:54:46 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ScrollableText.hh,v $
 *		Revision 1.3  1992/09/01  15:54:46  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.10  1992/05/19  09:27:34  sterling
 *		overrode layout as first step towards removing AutoSize from TextEdit
 *
 *		Revision 1.8  1992/03/05  21:46:06  sterling
 *		total rewrite
 *
 *		Revision 1.7  1992/02/15  06:21:17  sterling
 *		used EffectiveLiveChanged
 *
 */

#include	"FocusItem.hh"
#include	"TextEdit.hh"
#include	"Scroller.hh"

class	TextScroller : public Scroller, public TextController {
	public:
		TextScroller (AbstractScrollBar* verticalSlider = Scroller::kBuildDefaultSlider, AbstractScrollBar* horizontalSlider = Scroller::kBuildDefaultSlider);

		override	void	AdjustStepAndPageSizes ();
		override	void	TextChanged (TextEditBase* text);
		override	void	TextTracking (const Point& mouseAt);
		
		nonvirtual	void				SetTextEdit (AbstractTextEdit* textEdit);
		nonvirtual	AbstractTextEdit*	GetTextEdit () const;
	
		nonvirtual	void	ScrollTo (CollectionSize at, UpdateMode updateMode = eDelayedUpdate);

	protected:
		override	void	Layout ();
		virtual		void	SetTextEdit_ (AbstractTextEdit* textEdit);
	
	private:
		AbstractTextEdit*	fTextEdit;
};



// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/*__TextScroller__*/
