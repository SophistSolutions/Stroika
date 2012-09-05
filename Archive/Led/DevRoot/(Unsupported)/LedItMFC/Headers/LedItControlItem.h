/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__LedItControlItem_h__
#define	__LedItControlItem_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Headers/LedItControlItem.h,v 2.19 2000/04/24 17:28:21 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedItControlItem.h,v $
 *	Revision 2.19  2000/04/24 17:28:21  lewis
 *	SPR#0743 use new Led_MFC_WordProcessor.h module. And lose override of InternalizeBestFlavor () - see SPR# 0744
 *	
 *	Revision 2.18  2000/04/14 22:40:52  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.17  2000/04/14 19:08:49  lewis
 *	SPR#0739- Added FlavorPackage support
 *	
 *	Revision 2.16  1997/12/24 04:46:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.15  1997/07/27  16:01:09  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.14  1997/07/14  14:49:00  lewis
 *	Renamed TextStore_ to TextStore.
 *
 *	Revision 2.13  1997/01/10  03:28:34  lewis
 *	*** empty log message ***
 *
 *	Revision 2.12  1996/12/13  18:02:41  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.11  1996/12/05  21:16:12  lewis
 *	*** empty log message ***
 *
 *	Revision 2.10  1996/09/01  15:43:38  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.9  1996/06/01  02:39:10  lewis
 *	subclass from Led_MFC_ControlItem and move its implemenation
 *	into Led_MFC.hh
 *
 *	Revision 2.8  1996/05/23  20:15:02  lewis
 *	changed args to DrawSegment
 *
 *	Revision 2.7  1996/05/14  20:41:27  lewis
 *	*** empty log message ***
 *
 *	Revision 2.6  1996/05/05  14:39:27  lewis
 *	No longer need MeasureSegmentBaseLine () override - default is right.
 *
 *	Revision 2.5  1996/04/18  15:46:12  lewis
 *	Lots of changes.
 *	new class DocContextDefiner() so we can create embeddings which know about
 *	their doc via internalize calls (paste etc).
 *	React to Led class lib embedding code changes to support mutliformat embeddings etc.
 *	cleanups
 *
 *	Revision 2.4  1996/03/16  19:07:05  lewis
 *	Cleanups - HandleOpen() - and override Serialize () to fix our
 *	size on readins.
 *
 *	Revision 2.3  1996/03/05  18:40:28  lewis
 *	Renamed Led_ClipboardObjectAcquire::ClipFormat --> Led_ClipFormat.
 *
 *	Revision 2.2  1996/03/04  07:57:36  lewis
 *	Must override new GetTag/Write/GetWriteToClipAsFormat () routines.
 *
 *	Revision 2.1  1996/01/11  08:29:00  lewis
 *	Cleanups and override DoVerb().
 *
 *	Revision 2.0  1996/01/04  00:55:44  lewis
 *	*** empty log message ***
 *
 *	Revision 1.3  1996/01/04  00:55:22  lewis
 *	*** empty log message ***
 *
 *	Revision 1.2  1995/12/08  07:46:23  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */
#include	"Led_MFC_WordProcessor.h"

#include	"LedItConfig.h"


class	LedItDocument;
class	LedItView;

class	LedItControlItem : public Led_MFC_ControlItem	{
	public:
		LedItControlItem (COleDocument* pContainer = NULL);

	public:
		// NB: See Led_MFC_ControlItem docs about DocContextDefiner
		static	SimpleEmbeddedObjectStyleMarker*	mkLedItControlItemStyleMarker (const char* embeddingTag, const void* data, size_t len);
		static	SimpleEmbeddedObjectStyleMarker*	mkLedItControlItemStyleMarker (ReaderFlavorPackage& flavorPackage);

	protected:
		typedef	Led_MFC_ControlItem	inherited;

	public:
		override	BOOL	CanActivate ();

	public:
		nonvirtual	LedItDocument&	GetDocument () const;

	private:
		DECLARE_SERIAL(LedItControlItem)
};



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
	#if		!qDebug
	inline	LedItDocument&	LedItControlItem::GetDocument () const
		{
			// See debug version for ensures...
			return *(LedItDocument*)COleClientItem::GetDocument();
		}
	#endif


#endif	/*__LedItControlItem_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

