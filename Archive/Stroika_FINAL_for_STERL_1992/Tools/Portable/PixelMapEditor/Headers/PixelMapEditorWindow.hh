/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PixelMapEditorWindow__
#define	__PixelMapEditorWindow__

/*
 * $Header: /fuji/lewis/RCS/PixelMapEditorWindow.hh,v 1.2 1992/06/25 09:55:14 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: PixelMapEditorWindow.hh,v $
 *		Revision 1.2  1992/06/25  09:55:14  sterling
 *		Add window dtor.
 *
 *		Revision 1.1  1992/06/20  18:21:59  lewis
 *		Initial revision
 *
 *		Revision 1.1  92/04/20  11:05:26  11:05:26  lewis (Lewis Pringle)
 *		Initial revision
 *
 *
 *
 */

#include	"Window.hh"


class PixelMapEditorDocument;
class	PixelMapEditorWindow : public Window {
	public:
		PixelMapEditorWindow (PixelMapEditorDocument& myDocument);
		~PixelMapEditorWindow ();

		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

		nonvirtual	PixelMap	GetPixelMap () const;
		nonvirtual	void		SetPixelMap (const PixelMap& pm);

	private:
		PixelMapEditorDocument&		fDocument;
		class PixelMapEditorView*	fView;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__PixelMapEditorWindow__*/

