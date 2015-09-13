/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LineBasedPartition_h__
#define	__LineBasedPartition_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/LineBasedPartition.h,v 2.30 2002/11/18 21:10:41 lewis Exp $
 */


/*
@MODULE:	LineBasedPartition
@DESCRIPTION:
		<p>A LineBasedPartition is a concrete @'Partition' subclass in which the character '\n' is used to break
	up the text into peices (partition elements). This is the most basic, default, standard kind of Partition.</p>
 */


/*
 * Changes:
 *	$Log: LineBasedPartition.h,v $
 *	Revision 2.30  2002/11/18 21:10:41  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.29  2002/10/08 16:37:50  lewis
 *	SPR#1119- Added CheckForSplits/NeedToCoalesce to LineBasedPartition. Add WordProcessor::WPPartition.
 *	Add / use MakeDefaultPartition () so we create the new partition for WPs.
 *	
 *	Revision 2.28  2002/05/06 21:33:31  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.27  2001/11/27 00:29:43  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.26  2001/10/20 13:38:55  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.25  2001/10/17 20:42:51  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.24  2001/08/28 18:43:27  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.23  2000/04/15 14:32:34  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.22  2000/04/14 22:40:21  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.21  1999/11/13 16:32:18  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.20  1999/05/03 22:04:55  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.19  1998/04/09 01:26:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.18  1998/04/08  01:11:20  lewis
 *	Renamed from LineBasedTextImager - and implemented new design where Partition is object
 *	separate and apart from imager, and simply owned by imager.
 *
 *
 *
 *	<<<Renamed LineBasedPartition.h from LineBasedTextImager.h>>>
 *
 *
 *	Revision 2.17  1998/03/04  20:18:12  lewis
 *	*** empty log message ***
 *
 *	Revision 2.16  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.15  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.14  1997/07/23  23:03:33  lewis
 *	doc changes
 *
 *	Revision 2.13  1997/07/15  05:25:15  lewis
 *	AutoDoc content.
 *
 *	Revision 2.12  1997/07/12  20:00:55  lewis
 *	AutoDoc.
 *
 *	Revision 2.11  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.10  1997/03/04  20:04:16  lewis
 *	Moved most of the content to the new PartitioningTextImager file.
 *	-------------------------------------------------------
 *	-----------------------------------------------------
 *
 *	Revision 2.9  1997/01/10  02:49:55  lewis
 *	Added throw specifiers, and now AboutToUpdateText () returns void.
 *
 *	Revision 2.8  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.7  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.6  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.5  1996/03/16  18:38:37  lewis
 *	Invariant's now const.
 *
 *	Revision 2.4  1996/02/05  04:10:22  lewis
 *	Get rif of TextSTore_ arg to CTOR, and override of SpecifyTextStore - instead
 *	we have overrides of HookLosting/Gaining TextStore.
 *
 *	Revision 2.3  1995/11/02  21:50:01  lewis
 *	Accumulate/FinalizeMarkers etc now non-virtual.
 *
 *	Revision 2.2  1995/10/09  22:17:44  lewis
 *	Instead of PrePostReplace stuff, use new AboutTo/DidChange callbacks
 *	for images from TextStore.
 *
 *	Revision 2.1  1995/09/06  20:52:38  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.9  1995/05/29  23:45:07  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.8  1995/05/21  17:04:15  lewis
 *	Make PartitionMarker stuff public.
 *
 *	Revision 1.7  1995/05/20  04:43:50  lewis
 *	Fixed spelling of name Invariant.
 *	Added GetPartitionMark
 *	erContainingPosition (size_t charPosition) const;
 *	(and last accessed cache varaible)
 *
 *	Revision 1.6  1995/05/09  23:21:41  lewis
 *	Lose unused withWhat parameter to Pre/PostReplace.
 *	Led SPR#0240.
 *
 *	Revision 1.5  1995/05/06  19:23:52  lewis
 *	Use Led_tChar instead of char for UNICODE support (SPR 0232).
 *	Lose workaround for obsolete bug qCallingBaseClassMemberFunctionWithNestedClassesSymCPlusPlusBug.
 *
 *	Revision 1.4  1995/04/18  00:07:36  lewis
 *	AccumulateMarkerForDeletion/FinalizeMarkerDeletions.
 *	Override LineBasedTextImager::PreReplace to assert (fMarkersToBeDeleted.GetLength () == 0 (ie finalized already).
 *
 *	Revision 1.3  1995/04/16  19:22:16  lewis
 *	Added override of PostReplace to call FinalizeMarkersForDeletion
 *	and added AccumulateMarkerforDleteion and FinalizeMarkersForDeltion
 *	instead of removing and deleting markers directly in coalesce. Instead
 *	can use RemoveMarkers (plural) which is MUCH faster for large deletions.
 *	See SPR#0207.
 *
 *	Revision 1.2  1995/03/02  05:45:35  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1995/03/02  05:30:22  lewis
 *	Initial revision
 *
 *
 *
 */

#include	"PartitioningTextImager.h"


#if		qLedUsesNamespaces
namespace	Led {
#endif

/*
@CLASS:			LineBasedPartition
@BASES:			virtual @'Partition'
@DESCRIPTION:	<p>Partition which implements the partitioning based on NL characters.</p>
				<p>As of Led 3.0, it is the only concrete partition supported, and its the overwhelmingly
			most likely one you would want to use. But - in some applications (notably LogoVista Translator Assistant) -
			it can be quite handy being able to override how this partitioning takes place.</p>
*/
class	LineBasedPartition : public Partition {
	private:
		typedef	Partition	inherited;
	public:
		LineBasedPartition (TextStore& textStore);
	protected:
		enum SpecialHackToDisableInit { eSpecialHackToDisableInit };
		LineBasedPartition (TextStore& textStore, SpecialHackToDisableInit hack);
		override	void	FinalConstruct ();

	public:
		override	void	UpdatePartitions (PartitionMarker* pm, const UpdateInfo& updateInfo) throw ();

	protected:
		virtual	void	CheckForSplits (PartitionMarker* pm, const UpdateInfo& updateInfo, size_t i) throw ();
		virtual	bool	NeedToCoalesce (PartitionMarker* pm) throw ();

#if		qDebug
	protected:
		override	void	Invariant_ () const;
#endif
};





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */



#if		qLedUsesNamespaces
}
#endif


#endif	/*__LineBasedPartition_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
