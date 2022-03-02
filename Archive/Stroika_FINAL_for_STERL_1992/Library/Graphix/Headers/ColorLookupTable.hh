/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__ColorLookupTable__
#define	__ColorLookupTable__

/*
 * $Header: /fuji/lewis/RCS/ColorLookupTable.hh,v 1.5 1992/11/25 22:39:12 lewis Exp $
 *
 * Description:
 *
 *		ColorLookupTable is a class designed to map color indexs (or ids) to colors.
 *		It is expected to be used with PixelMap to help give meaning to pixel values.
 *		It is an associative array - you can lookup colors by index, or index by color.
 *
 *		Since the array is organized by index (ie an array indexed by integers), the
 *		indexing by color is difficult. Also, there is no obvious metric for comparing
 *		the distance betwen colors. We use a default algorithm for this, which can
 *		be overriden by specifying a proc to do-so.
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: ColorLookupTable.hh,v $
 *		Revision 1.5  1992/11/25  22:39:12  lewis
 *		Start conversion to new Genclass based template support.
 *
 *		Revision 1.3  1992/09/05  16:14:25  lewis
 *		Renamed NULL->Nil.
 *
 *		Revision 1.2  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.19  1992/06/09  12:37:36  lewis
 *		Renamed SmartPointer back to ReferenceCounted.
 *
 *		Revision 1.18  92/06/09  02:02:50  02:02:50  lewis (Lewis Pringle)
 *		Merged in sterls new smartpointer/referenceCounting support.
 *		
 *		Revision 1.17  1992/05/23  00:09:08  lewis
 *		Included ReferenceCounted instead of Memory.hh
 *
 *		Revision 1.16  92/05/13  00:12:53  00:12:53  lewis (Lewis Pringle)
 *		Comment on CLutSharedPart::DTOR.
 *		
 *		Revision 1.15  92/04/30  03:25:57  03:25:57  lewis (Lewis Pringle)
 *		Only use fWeOwnClut for qMacGDI.
 *		Cleanup docs.
 *		Make ColorLookupTable::operator[] (ColorIndex i) const inline for XGDI.
 *		
 *		Revision 1.14  92/04/29  12:16:12  12:16:12  lewis (Lewis Pringle)
 *		Added InverseTable (const InverseTable& from); and const InverseTable& operator= (const InverseTable& rhs) methods.
 *		
 *		Revision 1.13  92/04/17  17:09:55  17:09:55  lewis (Lewis Pringle)
 *		Added support for Invserse Tables.
 *		
 *		Revision 1.12  92/04/17  12:36:37  12:36:37  sterling (Sterling Wight)
 *		Make operator== a freind so he can peek at shared parts for quick equality test.
 *		
 *		Revision 1.11  92/04/07  01:01:32  01:01:32  lewis (Lewis Pringle)
 *		Make ColorIndex a short, since otherwise we cannot tell the difference between 0 colors and 256.
 *		Also, color compare function for lookup now returns UInt32.
 *		
 *		Revision 1.9  1992/02/21  18:01:52  lewis
 *		Got rid of qGPlus_ClassOpNewDelBroke workaround.
 *
 *
 *
 *
 */


#include	"Debug.hh"
#include	"Shared.hh"

#include	"Color.hh"




/*
 * aka pixel (when in indirect mode only?)
 *
 * This could be larger than a UInt8, but I've never heard of this being done. Actually it must
 * be slightly larger, so it can tell the difference between 256 colors, and zero!!!
 */
typedef	UInt16	ColorIndex;

#if		qMacGDI
struct osColorTable;
#endif	/*qMacGDI*/


class	CLUTSharedPart;

#if		!qRealTemplatesAvailable
	#include "TFileMap.hh"
	#include SharedOfCLUTSharedPart_hh
#endif

class	ColorLookupTable {
	public:
		/*
		 * like pixmap stuff, if object handed an ospointer, it will not be freed.  Otherwise, CLUTS
		 * are copied by value.  If none given, we create our own by default (no entries?).
		 */
		ColorLookupTable ();

#if		qMacGDI
		/*
		 * Keep the given pointer. Callers responsability to dispose of it, after this object
		 * is destroyed.
		 */
		ColorLookupTable (osColorTable** anOSColorLookupTable);
#endif	/*qMacGDI*/

		ColorLookupTable (const Color colorList[], ColorIndex nColorListEntries);
		ColorLookupTable (const ColorLookupTable& colorLookupTable);

		~ColorLookupTable ();	// NB: nonvirtual dtor - THEREFORE WE CANNOT SAFELY SUBCLASS

		const ColorLookupTable&	operator= (const ColorLookupTable& colorLookupTable);

		nonvirtual	ColorIndex	GetEntryCount () const;
		nonvirtual	void		SetEntryCount (ColorIndex entryCount, const Color& fillWith = kBlackColor);

		nonvirtual	Color		operator[] (ColorIndex i) const;
		/*
		 * approximates - if necessary using a default algorithm. Override this algorithm yourself with
		 * your own metric procedure.
		 */
		nonvirtual	ColorIndex	operator[] (const Color& c) const;
		nonvirtual	ColorIndex	Lookup (const Color& c) const;
		nonvirtual	ColorIndex	Lookup (const Color& c, UInt32 (*distance) (const Color& lhs, const Color& rhs)) const;

		nonvirtual	void		SetEntry (ColorIndex at, const Color& c);

		/*
		 * on mac, returns CTabHandle
		 */
#if		qMacGDI
		nonvirtual	osColorTable**			GetOSRepresention ();
		nonvirtual	const	osColorTable**	PeekOSRepresention () const;
#endif	/*qMacGDI*/

	private:
#if		qRealTemplatesAvailable
		Shared<CLUTSharedPart>	fSharedPart;
#else
		Shared(CLUTSharedPart)	fSharedPart;
#endif

		nonvirtual	void	Assure1Reference ();
		nonvirtual	void	BreakReferences ();

	friend	Boolean	operator== (const ColorLookupTable& lhs, const ColorLookupTable& rhs);
};


extern	const	ColorLookupTable	kBlackAndWhiteCLUT;




/*
 * Data structure to be able to rapidly (though approximately) find the closested color
 * to the given color.
 */

class	InverseTable {
	public:
  		InverseTable (const InverseTable& from);
		InverseTable (const ColorLookupTable& clut, UInt8 resulution = 0);
		~InverseTable ();
		const InverseTable& operator= (const InverseTable& rhs);

		nonvirtual	ColorIndex	Lookup (const Color& c) const;

	private:
		UInt8		fResolution;
		ColorIndex	*fColorIndexesTable;
		nonvirtual	size_t	TableEntryCount () const;
		nonvirtual	void	FillInTable (const ColorLookupTable& clut);
};







/*
 * Stream inserters and extractors.
 */
extern	class ostream&	operator<< (class ostream& out, const ColorLookupTable& clut);
extern	class istream&	operator>> (class istream& in, ColorLookupTable& clut);


extern	Boolean	operator== (const ColorLookupTable& lhs, const ColorLookupTable& rhs);
extern	Boolean	operator!= (const ColorLookupTable& lhs, const ColorLookupTable& rhs);





// Should be scoped, or private in .cc file but for compiler bugs - would like it to be
// scoped class so we can refer to it below in inlines...

		class	CLUTSharedPart {	
			public:
				static	void*	operator new (size_t n);
				static	void	operator delete (void* p);
				
#if		qMacGDI
				nonvirtual	CLUTSharedPart (osColorTable** anOSColorLookupTable, Boolean weOwnCLUT);
#elif	qXGDI
				nonvirtual	CLUTSharedPart (ColorIndex count, Color* colors);
#endif	/*qMacGDI*/

				// Note that we are not using virtual DTORs here, and so its important that we be careful not to
				// keep pointers to this guys base class so DTOR called properly... We really SB private base class
				// but then we'd need to forward declare half the methods from ReferenceCounted - do eventually...
				nonvirtual	~CLUTSharedPart ();
				
#if		qMacGDI
				Boolean			fWeOwnCLUT;
				osColorTable**	fCTabHandle;
#elif	qXGDI
				ColorIndex		fColorCount;
				Color*			fColors;
#endif	/*qMacGDI*/
			private:
				CLUTSharedPart (const CLUTSharedPart&);		// Not defined
				operator= (const CLUTSharedPart&);			// Not defined
		};


/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	ColorIndex	ColorLookupTable::operator[] (const Color& c) const		{ return (Lookup (c)); }
#if		qXGDI
inline	Color	ColorLookupTable::operator[] (ColorIndex i) const
	{
		Require (i >= 1);
		Require (i <= GetEntryCount ());
		AssertNotNil (fSharedPart->fColors);
		return (fSharedPart->fColors [i-1]);
	}
#endif	/*qXGDI*/




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__ColorLookupTable__*/

