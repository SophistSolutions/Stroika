/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__SimpleTextInteractor_h__
#define	__SimpleTextInteractor_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/SimpleTextInteractor.h,v 2.20 2003/01/30 14:58:18 lewis Exp $
 */


/*
@MODULE:	SimpleTextInteractor
@DESCRIPTION:
		<p>SimpleTextInteractor.</p>
 */


/*
 * Changes:
 *	$Log: SimpleTextInteractor.h,v $
 *	Revision 2.20  2003/01/30 14:58:18  lewis
 *	add 'inherited' typedef
 *	
 *	Revision 2.19  2003/01/29 17:59:48  lewis
 *	SPR#1264- Get rid of most of the SetDefaultFont overrides and crap - and made InteractiveSetFont REALLY do the interactive command setfont - and leave SetDefaultFont to just setting the 'default font'
 *	
 *	Revision 2.18  2002/05/06 21:33:33  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.17  2001/11/27 00:29:44  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.16  2001/10/17 21:46:41  lewis
 *	massive DocComment cleanups (mostly <p>/</p> fixups)
 *	
 *	Revision 2.15  2001/08/28 18:43:29  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.14  2000/04/15 14:32:35  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.13  2000/04/14 22:40:21  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.12  1999/11/29 18:56:38  lewis
 *	add default arg to SetTopRowInWindowByMarkerPosition() so can be called with one arg when i rearange inheritance hierarchy (so this class calls the method)
 *	
 *	Revision 2.11  1999/11/13 16:32:19  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.10  1999/05/03 22:04:58  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.9  1998/07/24 01:02:05  lewis
 *	override new CHangedInterlineSpace()
 *	
 *	Revision 2.8  1998/03/04  20:17:28  lewis
 *	*** empty log message ***
 *
 *	Revision 2.7  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.6  1997/09/29  14:43:13  lewis
 *	Revised for chagnes in SimpleTextImager
 *
 *	Revision 2.5  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.4  1997/07/23  23:04:49  lewis
 *	Doc changes
 *
 *	Revision 2.3  1997/07/15  05:25:15  lewis
 *	AutoDoc content.
 *
 *	Revision 2.2  1997/07/12  20:03:05  lewis
 *	AutoDoc support
 *
 *	Revision 2.1  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1997/06/18  02:37:47  lewis
 *	l
 *
 *
 *
 *
 *	<========== CODE MOVED HERE FROM TextInteractorMixins.h ==========>
 *
 *
 */

#include	"SimpleTextImager.h"
#include	"TextInteractorMixins.h"



#if		qLedUsesNamespaces
namespace	Led {
#endif


#if		__MWERKS__
	// Shut-off SetDefaultFont() warning instantiating InteractorImagerMixinHelper<>
	#pragma	warn_hidevirtual	off
#endif
#if		qQuiteAnnoyingDominanceWarnings
	#pragma warning (disable : 4250)
#endif
/*
@CLASS:			SimpleTextInteractor
@BASES:			InteractorImagerMixinHelper<SimpleTextImager>
@DESCRIPTION:
		<p>Simple mixin of @'SimpleTextImager' and @'TextInteractor' (using the utility class @'InteractorImagerMixinHelper<IMAGER>').
	You might use this class as an argument to the mixin template for your class library wrapper class, as in
	Led_MFC_X&ltSimpleTextInteractor&gt, for MFC.</p>
*/
class	SimpleTextInteractor : public InteractorImagerMixinHelper<SimpleTextImager> {
	private:
		typedef	InteractorImagerMixinHelper<SimpleTextImager>	inherited;

	protected:
		SimpleTextInteractor ();

	protected:
		override	void	TabletChangedMetrics ();
		override	void	ChangedInterLineSpace (PartitionMarker* pm);

	/*
	 *	Must combine behaviors of different mixins.
	 */
	public:
		override	void	SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont);
		nonvirtual	void	SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont, UpdateMode updateMode);
		override	void	SetTopRowInWindow (size_t newTopRow);
		nonvirtual	void	SetTopRowInWindow (size_t newTopRow, UpdateMode updateMode);
		override	void	SetTopRowInWindow (RowReference row);
		nonvirtual	void	SetTopRowInWindow (RowReference row, UpdateMode updateMode);

	// Speed tweek - use rowreferences...
	public:
		override	void	SetTopRowInWindowByMarkerPosition (size_t markerPos, UpdateMode updateMode = eDefaultUpdate);
};
#if		qQuiteAnnoyingDominanceWarnings
	#pragma warning (default : 4250)
#endif
#if		__MWERKS__
	#pragma	warn_hidevirtual	reset
#endif









/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//	class	SimpleTextInteractor
	inline	void	SimpleTextInteractor::SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont, TextInteractor::UpdateMode updateMode)
		{
			TextInteractor::SetDefaultFont (defaultFont, updateMode);
		}
	inline	void	SimpleTextInteractor::SetTopRowInWindow (size_t newTopRow, UpdateMode updateMode)
		{
			TemporarilySetUpdateMode	updateModeSetter (*this, updateMode);
			SetTopRowInWindow (newTopRow);
		}
	inline	void	SimpleTextInteractor::SetTopRowInWindow (RowReference row, UpdateMode updateMode)
		{
			TemporarilySetUpdateMode	updateModeSetter (*this, updateMode);
			SetTopRowInWindow (row);
		}


#if		qLedUsesNamespaces
}
#endif

#endif	/*__SimpleTextInteractor_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
