/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__WordWrappedTextInteractor_h__
#define	__WordWrappedTextInteractor_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/WordWrappedTextInteractor.h,v 2.18 2002/05/06 21:33:41 lewis Exp $
 */

/*
@MODULE:	WordWrappedTextInteractor
@DESCRIPTION:
		<p>This module provides the class @'WordWrappedTextInteractor' - a simple helper class to deal
	with mixing together @'WordWrappedTextImager' and @'TextInteractor'.</p>
 */

/*
 * Changes:
 *	$Log: WordWrappedTextInteractor.h,v $
 *	Revision 2.18  2002/05/06 21:33:41  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.17  2001/11/27 00:29:48  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.16  2001/10/17 20:42:56  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.15  2001/08/29 23:01:47  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.14  2001/08/28 18:43:33  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.13  2000/04/15 14:32:37  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.12  2000/04/14 22:40:24  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.11  1999/12/19 16:37:13  lewis
 *	For SPR#0667- override WordWrappedTextInteractor::OnTypedNormalCharacter ()
 *	to handle SHIFT-RETURN
 *	
 *	Revision 2.10  1999/11/29 18:56:17  lewis
 *	add default arg to SetTopRowInWindowByMarkerPosition() so can be called with one
 *	arg when i rearange inheritance hierarchy (so this class calls the method)
 *	
 *	Revision 2.9  1999/11/13 16:32:20  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.8  1999/05/03 22:05:06  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.7  1998/10/30 14:16:13  lewis
 *	changed MSVC60 compiler warnign support
 *	
 *	Revision 2.6  1998/04/25  01:28:36  lewis
 *	Docs.
 *
 *	Revision 2.5  1998/04/09  01:29:48  lewis
 *	*** empty log message ***
 *
 *	Revision 2.4  1998/03/04  20:15:43  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1997/09/29  14:58:02  lewis
 *	New TemporarilySetUpdateMode-based UpdateMode handling for SetTopRowInWIndow,
 *	instead of old multi-override
 *	kind of approach.
 *
 *	Revision 2.1  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.0  1997/07/23  23:01:42  lewis
 *	revised docs
 *
 *	Revision 1.4  1997/07/12  20:06:35  lewis
 *	AutoDoc.
 *
 *	Revision 1.3  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 1.2  1997/06/18  02:28:31  lewis
 *	*** empty log message ***
 *
 * Revision 1.1  1997/06/18  02:28:10  lewis
 * Initial revision
 *
 *
 *
 *
 *	<========== CODE MOVED HERE FROM TextInteractorMixins.h ==========>
 *
 */

#include	"TextInteractorMixins.h"
#include	"WordWrappedTextImager.h"




#if		qLedUsesNamespaces
namespace	Led {
#endif



#if		qSilenceAnnoyingCompilerWarnings && __MWERKS__
	// Shut-off SetDefaultFont() warning instantiating InteractorImagerMixinHelper<>
	#pragma	warn_hidevirtual	off
#endif
#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (push)
	#pragma	warning (disable : 4250)		//qQuiteAnnoyingDominanceWarnings
#endif



/*
@CLASS:			WordWrappedTextInteractor
@BASES:			@'InteractorImagerMixinHelper<IMAGER>', <IMAGER=@'WordWrappedTextImager'>
@DESCRIPTION:
			<p>Simple mixin of @'WordWrappedTextImager' and @'TextInteractor'
		(using the utility class @'InteractorImagerMixinHelper<IMAGER>').</p>
*/
class	WordWrappedTextInteractor : public InteractorImagerMixinHelper<WordWrappedTextImager> {
	private:
		typedef	InteractorImagerMixinHelper<WordWrappedTextImager>	inherited;
	protected:
		WordWrappedTextInteractor ();

	public:
		override		void	OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed);

	/*
	 *	Must combine behaviors of different mixins.
	 */
	public:
		override	void	SetTopRowInWindow (size_t newTopRow);
		nonvirtual	void	SetTopRowInWindow (size_t newTopRow, UpdateMode updateMode);
		override	void	SetTopRowInWindow (RowReference row);
		nonvirtual	void	SetTopRowInWindow (RowReference row, UpdateMode updateMode);

	// Speed tweek - use rowreferences...
	public:
		override	void	SetTopRowInWindowByMarkerPosition (size_t markerPos, UpdateMode updateMode = eDefaultUpdate);
};









/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//	class	WordWrappedTextInteractor
	inline	void	WordWrappedTextInteractor::SetTopRowInWindow (size_t newTopRow, UpdateMode updateMode)
		{
			TemporarilySetUpdateMode	updateModeSetter (*this, updateMode);
			SetTopRowInWindow (newTopRow);
		}
	inline	void	WordWrappedTextInteractor::SetTopRowInWindow (RowReference row, UpdateMode updateMode)
		{
			TemporarilySetUpdateMode	updateModeSetter (*this, updateMode);
			SetTopRowInWindow (row);
		}


#if		qLedUsesNamespaces
}
#endif

#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (pop)
#endif
#if		qSilenceAnnoyingCompilerWarnings && __MWERKS__
	#pragma	warn_hidevirtual	reset
#endif


#endif	/*__WordWrappedTextInteractor_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
