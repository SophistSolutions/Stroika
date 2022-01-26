/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__BlockAllocated__
#define	__BlockAllocated__

/*
 * $Header: /fuji/lewis/RCS/BlockAllocated.hh,v 1.22 1992/11/20 19:16:21 lewis Exp $
 *
 * Description:
 * 		Block allocation utility class template.
 *
 *
 * TODO:
 *
 *		When we get more practice with templates, try making BlockAllocated
 *	template take #of chunks as optionaly parameter.
 *
 *		Do guard support for BlockAlloced, just as we do for regular memory
 *	allocations in Memory.cc.
 *
 * Changes:
 *	$Log: BlockAllocated.hh,v $
 *		Revision 1.22  1992/11/20  19:16:21  lewis
 *		Added Block_Alloced_sizeof_40_sNextLink, Block_Alloced_sizeof_44_sNextLink.
 *
 *		Revision 1.21  1992/11/17  05:28:42  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.20  1992/11/16  22:12:09  lewis
 *		Get rid of workaround for qBCC_InlinesBreakTemplates - now
 *		handled differently.
 *
 *		Revision 1.19  1992/11/16  21:57:13  lewis
 *		Add Block_Alloced_sizeof_36_sNextLink.
 *
 *		Revision 1.18  1992/11/16  04:42:07  lewis
 *		Workaround qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.16  1992/11/07  16:29:01  lewis
 *		Moved Block_Alloced_GetMem_Util to header cuz of GenClass quirk.
 *
 *		Revision 1.15  1992/11/06  02:45:08  lewis
 *		Did some todos.
 *
 *		Revision 1.14  1992/11/05  22:40:50  lewis
 *		Use sizeof (T) <= N rather than == N in accessing shared
 *		freepools.
 *		Note that this breaks allocator::GetMem () for those new cases-
 *		fix that soon!!!
 *
 *		Revision 1.12  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.11  1992/10/29  15:52:24  lewis
 *		Added #if !qCFront_InlinesNotFullyImplemented to wrap op new since apples
 *		cfront barfs.
 *
 *		Revision 1.10  1992/10/22  18:12:57  lewis
 *		add to todo.
 *
 *		Revision 1.9  1992/10/22  04:03:05  lewis
 *		Add BlockAllocated free list head for sizes 20, 24, 28, and 32.
 *		Instead of having GetNextLink return reference, have return value, and
 *		add new SetNextLink method instead.
 *
 *		Revision 1.8  1992/10/20  21:19:41  lewis
 *		Use inline GetNextLink () method regardless of whether or not static
 *		members are supported in templates. And, introduced new freepool heads:
 *		Block_Alloced_sizeof_4_sNextLink, Block_Alloced_sizeof_8_sNextLink, and
 *		Block_Alloced_sizeof_12_sNextLink for freepools of those respective sizes.
 *		GetNextLink now inline compares sizeof(T) with those sizes to pick the right
 *		size freepool to use, or if not one of the predefined ones, then use
 *		old style separate mechanism. This should dramatically increase memory
 *		utilization, and decrease fragmentation. Also, it will increase runtime size
 *		we get fewer calls to malloc. ALSO, it should have NO runtime cost if
 *		the compiler is smart enuf to recognize that we are inlinging a global
 *		variable reference and which one of the if tests can be told at compile time.
 *
 *		Revision 1.7  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.6  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.5  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.4  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.2  1992/07/17  19:16:25  lewis
 *		Worked around qStaticDataMembersInTemplatesSupported bug with gcc. Now
 *		this template at least compiles here.
 *		Minor cleanups, including putting inlines at the bottom of the file
 *		in the inlines section, etc...
 *		Tried to use default arg size for blockallocated template but gcc
 *		considered it a syntax error. Not sure I got it wrong, or gcc 2.2.1.
 *		Anyhow we can wait a bit before trying this.
 *
 *		<copied from Memory.hh>
 *
 */

#include	"Debug.hh"
#include	"Math.hh"



#if		qRealTemplatesAvailable


template	<class	T>	class	BlockAllocated {
	public:
		static	void*	operator new (size_t n);
		static	void	operator delete (void* p);

	private:
		static	void	GetMem ();	// Break out into separate function so we can
									// make op new inline for MOST important case
									// were alloc is cheap linked list operation...

#if		qStaticDataMembersInTemplatesSupported || !qRealTemplatesAvailable
		static	void*	sNextLink;
#else
		static	void*&	NLHack ();
#endif
		static	void*	GetNextLink ();
		static	void	SetNextLink (void* nextLink);
};


#endif	/*qRealTemplatesAvailable*/








/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

	// This must be included here to keep genclass happy, since the .cc file will not be included
	// in the genclassed .cc file....
	inline	void**	Block_Alloced_GetMem_Util (const size_t kSize)
	{
		Assert (kSize >= sizeof (void*));	//	cuz we overwrite first sizeof(void*) for link

		/*
		 * Picked 1000 bytes since with malloc overhead likely to turn out to be
		 * 1K chunk which memory allocator can do a good job on.
		 */
		const	size_t	kChunks = Max (1000 / kSize, (size_t)10);

		void**	newLinks	=	(void**)::new char [kChunks*kSize];
		void**	curLink		=	newLinks;
		for (register size_t i = 1; i < kChunks; i++) {
			*curLink = &(((char*)newLinks)[i*kSize]);
			curLink = (void**)*curLink;
		}
		*curLink = Nil;		// Nil-terminate the link list
		return (newLinks);
	}



#if		qRealTemplatesAvailable

/*
 *		In order to reduce fragmentation, we allocate all chunks of common sizes out of
 *	the same pool. Assuming the compiler is able to inline the references to these
 *	below, there is really no cost in adding more. I picked the ones I thought most
 *	likely to come up, but adding more should be just fine - strictly a win.
 *
 *		Don't bother implementing Block_Alloced_sizeof_N_GetMems() cuz flunging will
 *	genericly give us the same code-sharing effect.
 */
extern	void*	Block_Alloced_sizeof_4_sNextLink;
extern	void*	Block_Alloced_sizeof_8_sNextLink;
extern	void*	Block_Alloced_sizeof_12_sNextLink;
extern	void*	Block_Alloced_sizeof_16_sNextLink;
extern	void*	Block_Alloced_sizeof_20_sNextLink;
extern	void*	Block_Alloced_sizeof_24_sNextLink;
extern	void*	Block_Alloced_sizeof_28_sNextLink;
extern	void*	Block_Alloced_sizeof_32_sNextLink;
extern	void*	Block_Alloced_sizeof_36_sNextLink;
extern	void*	Block_Alloced_sizeof_40_sNextLink;
extern	void*	Block_Alloced_sizeof_44_sNextLink;


	#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
		#ifdef	inline
			#error "Ooops - I guess my workaround wont work!!!"
		#endif
		#define	inline
	#endif

#if		qStaticDataMembersInTemplatesSupported || !qRealTemplatesAvailable
	template	<class	T>	inline	void*	BlockAllocated<T>::GetNextLink ()
	{
		/*
		 * It is hoped that since all these comparisons can be evaluated at compile
		 * time, they will be. Then this code reduces to just a return of a single
		 * global variable. That should inline nicely.
		 *
		 *	NB: For Apples compiler (bar far our worst at optimizing), this does successfully
		 *		get translated into just one variable reference (C 3.2.3, CFront 3.2, ETO #8, Thursday, November 5, 1992 1:51:42 PM)
		 */
		if (sizeof (T) <= 4) {
			return (Block_Alloced_sizeof_4_sNextLink);
		}
		else if (sizeof (T) <= 8) {
			return (Block_Alloced_sizeof_8_sNextLink);
		}
		else if (sizeof (T) <= 12) {
			return (Block_Alloced_sizeof_12_sNextLink);
		}
		else if (sizeof (T) <= 16) {
			return (Block_Alloced_sizeof_16_sNextLink);
		}
		else if (sizeof (T) <= 20) {
			return (Block_Alloced_sizeof_20_sNextLink);
		}
		else if (sizeof (T) <= 24) {
			return (Block_Alloced_sizeof_24_sNextLink);
		}
		else if (sizeof (T) <= 28) {
			return (Block_Alloced_sizeof_28_sNextLink);
		}
		else if (sizeof (T) <= 32) {
			return (Block_Alloced_sizeof_32_sNextLink);
		}
		else if (sizeof (T) <= 36) {
			return (Block_Alloced_sizeof_36_sNextLink);
		}
		else if (sizeof (T) <= 40) {
			return (Block_Alloced_sizeof_40_sNextLink);
		}
		else if (sizeof (T) <= 44) {
			return (Block_Alloced_sizeof_44_sNextLink);
		}
		else {
			return (sNextLink);
		}
	}
	template	<class	T>	inline	void	BlockAllocated<T>::SetNextLink (void* nextLink)
	{
		/*
		 * It is hoped that since all these comparisons can be evaluated at compile
		 * time, they will be. Then this code reduces to just an assignement to a single
		 * global variable. That should inline nicely.
		 */
		if (sizeof (T) <= 4) {
			Block_Alloced_sizeof_4_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 8) {
			Block_Alloced_sizeof_8_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 12) {
			Block_Alloced_sizeof_12_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 16) {
			Block_Alloced_sizeof_16_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 20) {
			Block_Alloced_sizeof_20_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 24) {
			Block_Alloced_sizeof_24_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 28) {
			Block_Alloced_sizeof_28_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 32) {
			Block_Alloced_sizeof_32_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 36) {
			Block_Alloced_sizeof_36_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 40) {
			Block_Alloced_sizeof_40_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 44) {
			Block_Alloced_sizeof_44_sNextLink = nextLink;
		}
		else {
			sNextLink = nextLink;
		}
	}
#endif


#if		!qCFront_InlinesNotFullyImplemented
	template	<class	T>	inline	void*	BlockAllocated<T>::operator new (size_t n)
	{
		Assert (sizeof (T) >= sizeof (void*));	//	cuz we overwrite first sizeof(void*) for link
		Assert (n == sizeof (T));
	
		/*
		 * To implement linked list of BlockAllocated(T)'s before they are
		 * actually alloced, re-use the begining of this as a link pointer.
		 */
		if (GetNextLink () == Nil) {
			GetMem ();
		}
		void*	result = GetNextLink ();
		SetNextLink (*(void**)GetNextLink ());

		return (result);
	}
	template	<class	T>	inline	void	BlockAllocated<T>::operator delete (void* p)
	{
		if (p != Nil) {
			(*(void**)p) = GetNextLink ();
			SetNextLink (p);
		}
	}
#endif	/*!qCFront_InlinesNotFullyImplemented*/


	#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
		#ifndef	inline
			#error	"How did it get undefined?"
		#endif
		#undef	inline
	#endif

#endif	/*qRealTemplatesAvailable*/



#if		qRealTemplatesAvailable && !qTemplatesHasRepository
	/*
	 *		We must include our .cc file here because of this limitation.
	 *	But, under some systems (notably UNIX) we cannot compile some parts
	 *	of our .cc file from the .hh.
	 *
	 *		The other problem is that there is sometimes some untemplated code
	 *	in the .cc file, and to detect this fact, we define another magic
	 *	flag which is looked for in those files.
	 */
	#define qIncluding_BlockAllocatedCC	1
		#include	"../Sources/BlockAllocated.cc"
	#undef  qIncluding_BlockAllocatedCC
#endif




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__BlockAllocated__*/

