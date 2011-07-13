/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Shared__
#define	__Shared__

/*
 * $Header: /fuji/lewis/RCS/Shared.hh,v 1.25 1992/11/17 05:29:58 lewis Exp $
 *
 * Description:
 *
 *		This class is for keeping track of a data structure with reference counts,
 *	and disposing of that structure when the reference count drops to zero.
 *	Copying one of these Shared<T> just increments the referce count,
 *	and destroying/overwriting one decrements it.
 *
 *		You can have a ptr having a Nil value, and it can be copied.
 *	(Implementation detail - the reference count itself is NEVER nil except upon
 *	failure of alloction of memory in ctor and then only valid op on class is
 *	destruction). You can access the value with GetPointer () but this is not
 *	advised - only if it may be legitimately Nil do you want to do this.
 *	Generaly just use ptr-> to access the data, and this will do the
 *	RequireNotNil (POINTER) for you.
 *
 *		This class can be enourmously useful in implementing letter/envelope -
 *	type data structures - see String, or Shapes, for examples.
 *
 *
 * TODO:
 *		+	Use BlockAllocated instead of our manual implementation
 *
 *
 * Changes:
 *	$Log: Shared.hh,v $
 *		Revision 1.25  1992/11/17  05:29:58  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.24  1992/11/15  16:04:11  lewis
 *		Added missing inline declarations to implemtations of GetPointer
 *		and Assure1Reference() in implemenation section (forgotten).
 *
 *		Revision 1.23  1992/11/12  08:10:32  lewis
 *		Moved inlines (most) to the implemenation section instead of being
 *		in the class declaration.
 *
 *		Revision 1.22  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.20  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.19  1992/10/15  12:43:23  lewis
 *		Make no arg CTOR private, and comment on other CTORs.
 *
 *		Revision 1.18  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.17  1992/10/07  23:00:11  sterling
 *		Move some of the inlines in class declartion to InLine section, and
 *		properly ifdef for MPW compiler bugs (in macro version used toust
 *		comment out asserts so they would compile).
 *
 *		Revision 1.16  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.15  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.13  1992/09/05  13:56:42  lewis
 *		Fixed funny bug with macro version of inline Shared::CTOR(REFCNT):
 *		Inside class declaration left in Shared:: before CTOR name,
 *		and CFront was happy, gcc didn't complain, but genated link warnings in
 *		the end.???
 *
 *		Revision 1.12  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.11  1992/09/04  20:32:27  sterling
 *		Make X(&X) CTOR inline again, since made 2x factor performance hit
 *		under MPW tString benchmarks. Also, lots of name shortenings.
 *
 *		Revision 1.9  1992/07/21  06:09:10  lewis
 *		Forgot inline directive on Shared<T>::Shared (T* ptr)
 *		and Shared(T)::Shared(T) (const Shared(T)& src).
 *
 *		Revision 1.8  1992/07/17  19:50:31  lewis
 *		Comment cleanups (80 columns). Change Shared<T> to Shared in
 *		a few places cuz that appeased gcc 2.2.2 somewhat - this is a known bug and
 *		supposedly will be fixed in 2.3.
 *		Make Shared(T)::(const Shared(T)&) and operator* inline
 *		in macro version.
 *		Moved non-inline templates into .cc file and added the usual
 *		"qRealTemplatesAvailable && !qTemplatesHasRepository" hack to pull them in.
 *
 *		Revision 1.7  1992/07/16  06:54:14  lewis
 *		In macro version, if qBorlandCPlus use shorted name cuz borland has 32
 *		character limit (even 3.1!).
 *		Also, use UInt32 in Shared<T>::CountReferences () definition -
 *		bcc barfs on much of anything else.
 *
 *		Revision 1.6  1992/07/08  05:51:25  lewis
 *		Get rid of TODO, and add Description.
 *
 *		Revision 1.5  1992/07/08  00:21:33  lewis
 *		General cleanups, and commenting, and adding assertions.
 *		Added operator * ().
 *		Added nested typedef Counter. Also, added pseudo scoped functions and
 *		globals for Shared: NewCounter/DeleteCounter/sCounters..
 *		Did this so we get block allocated counter variables - Assert
 *		Shared_Counter type same as ReferneceCounted<T>::Counter
 *		(really we could only assert about sizes). Added efficiency hack to
 *		Shared<T>::operator= (T* ptr) that serl had in macro version.
 *
 *		Revision 1.4  1992/07/07  07:00:02  lewis
 *		Fix return type of Shared<T>::GetPointer ().
 *
 *		Revision 1.3  1992/07/04  02:23:35  lewis
 *		Added a few items to the TODO section.
 *
 *		Revision 1.2  1992/06/25  02:56:24  lewis
 *		Fix template syntax errors.
 *
 *		Revision 1.3  92/06/09  12:36:19  12:36:19  lewis (Lewis Pringle)
 *		Renamed SmartPointer back to Shared. And, deleted old refernece
 *		counted implementation.
 *		
 *		Revision 1.2  92/06/09  01:08:33  01:08:33  lewis (Lewis Pringle)
 *		Sterl added new class SmartPointer, with templated version too. This
 *		completely obsoletes the Shared class. Much better. Old stuff
 *		around temporarily for backward compatability. Idea for change originally
 *		suggested/prototyped by kdj.
 *
 *		<copied from Memory.hh>
 *
 */

#include	"Debug.hh"



#if		qRealTemplatesAvailable


template	<class T>	class	Shared {
	private:
		Shared ();	// illegal - dont call this...

	public:
		Shared (T* (*cloneFunction) (const T&), T* ptr);	// OK to pass Nil for ptr but assign before use
		Shared (const Shared<T>& src);

		~Shared ();											// nonvirtual DTOR cuz we always keep these by value

		nonvirtual	Shared<T>& operator= (const Shared<T>& src);
		nonvirtual	Shared<T>& operator= (T* ptr);


		/*
		 * These operators require that the underlying ptr is non-nil.
		 */
		nonvirtual	const T*	operator-> () const;
		nonvirtual	T*			operator-> ();
		nonvirtual	const T&	operator* () const;
		nonvirtual	T&			operator* ();


		/*
		 * Counter is the type used for keeping track of how many references there are to a single
		 * object.
		 *
		 * CountReferences () returns how many references there are to the given object - typically
		 * we only care about the values 0, 1, and > 1.
		 *
		 * We define this method (CountReferences ()) here to avoid confusing over how to write
		 * the return type with broken, stupid compilers.
		 */
		typedef	UInt32	Counter;
		nonvirtual	Counter	CountReferences () const	{ AssertNotNil (fCount); return (*fCount); }


		/*
		 * GetPointer () returns the real underlying ptr we store. It can be Nil. This should
		 * rarely be used - use operator-> in preference. This is only for dealing with cases where
		 * the ptr could legitimately be nil.
		 */
		nonvirtual	const T*	GetPointer () const;
		nonvirtual	T*			GetPointer ();


		/*
		 * This routine is automatically called whenever a non-const method is called, to ensure
		 * that only the current copy, and no shared references, are affected by a modification.
		 * Users should never need to call this method directly, although class implementers may
		 * occasionally call this method directly. One plausible example of when this is needed
		 * is when the type T being wrapped does not make correct use of const.
		 */
		nonvirtual	void	Assure1Reference ();

	private:
		T*			fPtr;					// Pointer to the  thing being reference counted
		Counter*	fCount;					// Shared count - never nil - even for nil ptr
		T*			(*fCloner) (const T&);	// Called by BreakReferences to create new T

		nonvirtual	void	BreakReferences ();
};

template	<class	T>	Boolean	operator== (const Shared<T>& lhs, const Shared<T>& rhs);
template	<class	T>	Boolean	operator!= (const Shared<T>& lhs, const Shared<T>& rhs);


#endif	/*qRealTemplatesAvailable*/





/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
	typedef	UInt32		Counter_Shared;			// MUST BE SAME TYPE AS Shared<T>::Counter
	extern	Counter_Shared*		sCounterList_Shared;
	extern	void				GetMem_Shared ();
	
	inline	Counter_Shared*	NewCounter_Shared ()
	{
		/*
		 * We assume this in our free/list scheme.
		 * Otherwise alloc union of two types (ptr and Counter_Shared)
		 */
		Assert (sizeof (Counter_Shared) >= sizeof (Counter_Shared*));
	
		if (sCounterList_Shared == Nil) {
			GetMem_Shared ();
		}
		AssertNotNil (sCounterList_Shared);
		Counter_Shared*	result = sCounterList_Shared;
		sCounterList_Shared = *(Counter_Shared**)sCounterList_Shared;
		return (result);
	}
	
	inline	void	DeleteCounter_Shared (Counter_Shared* counter)
	{
		/*
		 * We assume this in our free/list scheme.
		 * Otherwise alloc union of two types (ptr and Counter_Shared)
		 */
		Assert (sizeof (Counter_Shared) >= sizeof (Counter_Shared*));
	
		RequireNotNil (counter);
		(*(Counter_Shared**)counter) = sCounterList_Shared;
		sCounterList_Shared = counter;
	}



#if		qRealTemplatesAvailable

	#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
		#ifdef	inline
			#error "Ooops - I guess my workaround wont work!!!"
		#endif
		#define	inline
	#endif

	template <class T> inline	Shared<T>::Shared (T* (*cloneFunction) (const T&), T* ptr) : 
		fPtr (ptr),
		fCount (Nil),
		fCloner (cloneFunction)
	{
		Assert (sizeof (Counter_Shared) == sizeof (Counter));
		fCount = NewCounter_Shared ();
		AssertNotNil (fCount);
		(*fCount) = 1;
	}
	template <class T> inline	Shared<T>::Shared (const Shared<T>& src) :
		fPtr (src.fPtr),
		fCount (src.fCount),
		fCloner (src.fCloner)
	{
		Assert (sizeof (Counter_Shared) == sizeof (Counter));
		RequireNotNil (fCount);
		(*fCount)++;
	}
	#if		!qMPW_C_COMPILER_REGISTER_OVERFLOW_BUG || !qDebug
		template <class T> inline	const T*	Shared<T>::operator-> () const
		{
			EnsureNotNil (fPtr);
			return (fPtr);
		}
		template <class T> inline	const T&	Shared<T>::operator* () const
		{
			EnsureNotNil (fPtr);
			return (*fPtr);
		}
	#endif
	template	<class T>	inline	const T*	Shared<T>::GetPointer () const
	{
		return (fPtr);
	}
	template	<class T>	inline	void	Shared<T>::Assure1Reference ()
	{
		if (CountReferences () != 1) {
			BreakReferences ();
		}
	}
	template	<class	T>	inline	Boolean	operator== (const Shared<T>& lhs, const Shared<T>& rhs)
	{
		return (Boolean (lhs.GetPointer () == rhs.GetPointer ()));
	}
	
	template	<class	T>	inline	Boolean	operator!= (const Shared<T>& lhs, const Shared<T>& rhs)
	{
		return (Boolean (lhs.GetPointer () != rhs.GetPointer ()));
	}

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
	#define qIncluding_SharedCC	1
		#include	"../Sources/Shared.cc"
	#undef	qIncluding_SharedCC
#endif



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Shared__*/

