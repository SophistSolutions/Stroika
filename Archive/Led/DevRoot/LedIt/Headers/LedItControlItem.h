/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedItControlItem_h__
#define	__LedItControlItem_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedIt/Headers/LedItControlItem.h,v 1.4 2002/05/06 21:30:57 lewis Exp $
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
 *	Revision 1.4  2002/05/06 21:30:57  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 1.3  2001/11/27 00:28:08  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.2  2001/08/29 22:59:16  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.1  2001/05/14 20:54:43  lewis
 *	New LedIt! CrossPlatform app - based on merging LedItPP and LedItMFC and parts of LedTextXWindows
 *	
 *
 *
 *
 *	<<<***		Based on LedItMFC project from Led 3.0b6		***>>>
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

