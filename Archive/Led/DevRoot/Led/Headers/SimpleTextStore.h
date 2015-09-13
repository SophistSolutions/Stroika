/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__SimpleTextStore_h__
#define	__SimpleTextStore_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/SimpleTextStore.h,v 2.30 2002/11/17 15:55:27 lewis Exp $
 */


/*
@MODULE:	SimpleTextStore
@DESCRIPTION:
		<p>SimpleTextStore is a module for the implementation of the class - @'SimpleTextStore' -
	a very simple implementation of the @'TextStore' class.</p>
 */


/*
 * Changes:
 *	$Log: SimpleTextStore.h,v $
 *	Revision 2.30  2002/11/17 15:55:27  lewis
 *	SPR#1173- Replace/ReplaceWithoutUpdate change
 *	
 *	Revision 2.29  2002/09/13 15:29:19  lewis
 *	Add TextStore::ConstructNewTextStore () support
 *	
 *	Revision 2.28  2002/05/06 21:33:33  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.27  2001/11/27 00:29:44  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.26  2001/10/17 21:46:41  lewis
 *	massive DocComment cleanups (mostly <p>/</p> fixups)
 *	
 *	Revision 2.25  2001/09/26 15:41:16  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.24  2001/08/28 18:43:29  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.23  2000/08/13 03:30:14  lewis
 *	SPR#0822- added TextStore::PreRemoveMarker
 *	
 *	Revision 2.22  2000/07/26 05:46:35  lewis
 *	SPR#0491/#0814. Lose CompareSameMarkerOwner () stuff, and add new MarkerOwner arg to
 *	CollectAllMarkersInRagne() APIs. And a few related changes. See the SPRs for a table of speedups.
 *	
 *	Revision 2.21  2000/04/15 14:32:35  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.20  2000/04/14 22:40:21  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.19  1999/11/13 16:32:19  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.18  1999/05/03 22:04:58  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.17  1998/10/30 14:14:13  lewis
 *	Use vector instead of Led_Array
 *	
 *	Revision 2.16  1998/03/04  20:17:22  lewis
 *	*** empty log message ***
 *
 *	Revision 2.15  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.14  1997/09/29  14:43:31  lewis
 *	Lose qSupportLed21CompatAPI support
 *
 *	Revision 2.13  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.12  1997/07/23  23:04:56  lewis
 *	doc changes
 *
 *	Revision 2.11  1997/07/12  20:03:13  lewis
 *	AutoDoc support.
 *
 *	Revision 2.10  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.9  1997/06/18  02:38:38  lewis
 *	CollectAllMarkersInRangeInto now takes MarkerSink& arg instead of array. And new _21Compat backward compat API.
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
 *	Revision 2.5  1996/08/05  05:59:55  lewis
 *	Change param names to fit new programmer docs.
 *
 *	Revision 2.4  1996/05/23  19:23:35  lewis
 *	Throw specifiers for a bync of things, and lose GetMarkerRange() - osbolete.
 *
 *	Revision 2.3  1996/03/16  18:39:05  lewis
 *	Invariant_ () now const
 *
 *	Revision 2.2  1996/02/26  18:39:32  lewis
 *	No longer any need to override RemoveMarker.
 *
 *	Revision 2.1  1995/09/06  20:52:38  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.10  1995/06/08  05:10:16  lewis
 *	Code cleanups
 *
 *	Revision 1.9  1995/05/29  23:45:07  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.8  1995/05/20  04:47:38  lewis
 *	Fix spelling of Invariant
 *
 *	Revision 1.7  1995/05/06  19:21:05  lewis
 *	Renamed GetByteLength->GetLength () - SPR #0232.
 *
 *	Revision 1.6  1995/05/05  19:43:24  lewis
 *	Use Led_tChar instead of char to help eventually support UNICODE.
 *
 *	Revision 1.5  1995/04/18  00:09:29  lewis
 *	Fix constness of arg to RemoveMarkers.
 *
 *	Revision 1.4  1995/04/16  19:24:25  lewis
 *	override RemoveMarkers() - see SPR 0207.
 *
 *	Revision 1.3  1995/03/13  03:17:29  lewis
 *	Renamed Array<T> Led_Array<T>
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

#include	"TextStore.h"






#if		qLedUsesNamespaces
namespace	Led {
#endif




/*
@CLASS:			SimpleTextStore
@BASES:			@'TextStore'
@DESCRIPTION:
		<p>SimpleTextStore is a very simple implementation of the TextStore API.
	It is useful as a starting point for any new TextStore implementations, because of
	its simplicity. And it is useful for understanding.</p>
*/
class	SimpleTextStore : public TextStore {
	public:
		SimpleTextStore ();
		virtual ~SimpleTextStore ();

	public:
		override	TextStore*	ConstructNewTextStore () const;

	public:
		override	size_t	GetLength () const throw ();
		override	void	CopyOut (size_t from, size_t count, Led_tChar* buffer) const throw ();
		override	void	ReplaceWithoutUpdate (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCount);

	protected:
		nonvirtual	void	InsertAfter_ (const Led_tChar* what, size_t howMany, size_t after);
		nonvirtual	void	DeleteAfter_ (size_t howMany, size_t after);

	public:
		override	void	AddMarker (Marker* marker, size_t lhs, size_t length, MarkerOwner* owner);
		override	void	RemoveMarkers (Marker*const markerArray[], size_t markerCount);
		override	void	PreRemoveMarker (Marker* marker);
		override	void	SetMarkerRange (Marker* m, size_t start, size_t end) throw ();
		override	void	CollectAllMarkersInRangeInto (size_t from, size_t to, const MarkerOwner* owner, MarkerSink& output) const;

	private:
		size_t			fLength;
		Led_tChar*		fBuffer;
		vector<Marker*>	fMarkers;

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
	inline	size_t	SimpleTextStore::GetLength () const throw ()
		{
			return (fLength);
		}


#if		qLedUsesNamespaces
}
#endif



#endif	/*__SimpleTextStore_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

