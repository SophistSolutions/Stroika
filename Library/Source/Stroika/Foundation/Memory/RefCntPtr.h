/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__RefCntPtr_h__
#define	__RefCntPtr_h__	1

#include	"../StroikaPreComp.h"

#include	"../Exceptions.h"
#include	"../Support.h"


#ifndef	qUseInterlockedIncrement
#define	qUseInterlockedIncrement	1
#endif



namespace	Stroika {	
	namespace	Foundation {


// An OPTIONAL class you can mix into 'T', and use with RefCntPtr<>. If the 'T' used in RefCntPtr<T> inherits
// from this, then you can re-constitute a RefCntPtr<T> from it's T* (since the count is pulled along-side).
// This is sometimes handy if you wish to take a RefCntPtr<> object, and pass the underlying pointer through
// a layer of code, and then re-constitute the RefCntPtr<> part later.
struct	RefCntPtrBase {
	// we really want to treat this fCount_ as PRIVATE!!! DONT ACCESS IN SUBCLASSES - but it needs
	// access from the RefCntPtr<> template
	private:
	public:
		size_t	fCount_DONT_ACCESS;

	public:
		RefCntPtrBase ();
		virtual ~RefCntPtrBase ();

	public:
		// called to delete the 'RefCntPtrBase'. But - if this gets mixed into another object, just override
		// to ignore (cuz the actual object will get deleted too)
		virtual	void	DO_DELETE_REF_CNT () = 0;
};





/*
@CLASS:			RefCntPtr<T>
@DESCRIPTION:	<p>A very simple reference counted pointer implementation. Alas - there doesn't seem to
	be anything in STL which provides this functionality! std::auto_ptr is closest, but no cigar.
	For one thing, std::auto_ptr doesn't work with std::vector.</p>
		<p>This implementation would be slightly more efficient, and slightly less flexible, if we assumed
	T had a base-class which contained the reference count. Right now, places where I'm using this don't warrant
	the worry about efficiency.</p>
		<p>NB: - we do allow for a refCntPtr to be NULL. But any call to GetRep() assert its non-null. Check with call
	to IsNull() first if you aren't sure.</p>
		<P>Since this class is strictly more powerful than the STL template auto_ptr<>, but many people may already be
	using auto_ptr<>, I've provided some mimicing routines to make the transition easier if you want to make some of your
	auto_ptr's be RefCntPtr<T>.</p>
		<ul>
			<li>@'RefCntPtr<T>::get'</li>
			<li>@'RefCntPtr<T>::release'</li>
			<li>@'RefCntPtr<T>::reset'</li>
		</ul>
*/
template	<typename	T>	class	RefCntPtr {
	public:
		RefCntPtr ();
		explicit RefCntPtr (T* from);
		enum UsesRefCntPtrBase { eUsesRefCntPtrBase };
		explicit RefCntPtr (UsesRefCntPtrBase, T* from);
		explicit RefCntPtr (T* from, RefCntPtrBase* useCounter);
		RefCntPtr (const RefCntPtr<T>& from);

		template <typename T2>
		/*
		@METHOD:		RefCntPtr::RefCntPtr
		@DESCRIPTION:	<p>This CTOR is meant to allow for the semantics of assigning a sub-type pointer to a pointer
					to the base class. There isn't any way to express this in template requirements, but this template
					will fail to compile (error assigning to its fPtr member in the CTOR) if its ever used to
					assign inappropriate pointer combinations.</p>
		*/
			RefCntPtr (const RefCntPtr<T2>& from):
				fPtr (from.get ()),
				fCountHolder (from._PEEK_CNT_PTR_ ())
				{
					if (fPtr != NULL) {
						RequireNotNil (fCountHolder);
						#if		qUseInterlockedIncrement
							::InterlockedIncrement (reinterpret_cast<LONG*> (&fCountHolder->fCount_DONT_ACCESS));
						#else
							fCountHolder->fCount_DONT_ACCESS++;
						#endif
					}
				}

	public:
		nonvirtual		RefCntPtr<T>& operator= (const RefCntPtr<T>& rhs);
	public:
		~RefCntPtr ();

	public:
		nonvirtual	bool		IsNull () const;
		nonvirtual	T&			GetRep () const;

	public:
		nonvirtual	T* operator-> () const;
		nonvirtual	T& operator* () const;
		nonvirtual	operator T* () const;

	public:
		nonvirtual	T*		get () const;
		nonvirtual	void	release ();
		nonvirtual	void	clear ();
		nonvirtual	void	reset (T* p = NULL);

	public:
		template <typename T2>
		/*
		@METHOD:		RefCntPtr::Dynamic_Cast
		@DESCRIPTION:	<p>Similar to RefCntPtr<T2> () CTOR - which does base type. NB couldn't call this dynamic_cast - thats a reserved word.</p>
		*/
			RefCntPtr<T2> Dynamic_Cast ()
				{
					return RefCntPtr<T2> (dynamic_cast<T2*> (get ()), _PEEK_CNT_PTR_ ());
				}


	protected:
		T*				fPtr;
		RefCntPtrBase*	fCountHolder;

	public:
		nonvirtual	size_t	CurrentRefCount () const;

	public:
		nonvirtual	bool	operator< (const RefCntPtr<T>& rhs) const;
		nonvirtual	bool	operator<= (const RefCntPtr<T>& rhs) const;
		nonvirtual	bool	operator> (const RefCntPtr<T>& rhs) const;
		nonvirtual	bool	operator>= (const RefCntPtr<T>& rhs) const;
		nonvirtual	bool	operator== (const RefCntPtr<T>& rhs) const;
		nonvirtual	bool	operator!= (const RefCntPtr<T>& rhs) const;

	public:
		nonvirtual	RefCntPtrBase*		_PEEK_CNT_PTR_ () const;
};


template	<typename	T>
	void	ThrowIfNull (const RefCntPtr<typename T>& p);
	

	}
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"BlockAllocated.h"

namespace	Stroika {	
	namespace	Foundation {

	namespace	RefCntPtrNS {
		namespace	Private {
			struct	SimpleRefCntPtrBase : RefCntPtrBase {
				public:
					virtual	void	DO_DELETE_REF_CNT ();

				public:
					DECLARE_USE_BLOCK_ALLOCATION(SimpleRefCntPtrBase);
			};
		}
	}



//	class	RefCntPtrBase
	inline	RefCntPtrBase::RefCntPtrBase ():
		fCount_DONT_ACCESS (0)
		{
		}
	inline	RefCntPtrBase::~RefCntPtrBase ()
		{
		}



//	class	RefCntPtr<T>
	template	<typename T>
		inline	RefCntPtr<T>::RefCntPtr ():
			fPtr (NULL),
			fCountHolder (NULL)
			{
			}
	template	<typename T>
		inline	RefCntPtr<T>::RefCntPtr (T* from):
			fPtr (from),
			fCountHolder (NULL)
			{
				if (from != NULL) {
					fCountHolder = DEBUG_NEW RefCntPtrNS::Private::SimpleRefCntPtrBase ();
					Assert (fCountHolder->fCount_DONT_ACCESS == 0);
					#if		qUseInterlockedIncrement
						::InterlockedIncrement (reinterpret_cast<LONG*> (&fCountHolder->fCount_DONT_ACCESS));
					#else
						fCountHolder->fCount_DONT_ACCESS++;
					#endif
				}
			}
	template	<typename T>
		inline	RefCntPtr<T>::RefCntPtr (UsesRefCntPtrBase, T* from):
			fPtr (from),
			fCountHolder (from)
			{
				if (fCountHolder != NULL) {
					#if		qUseInterlockedIncrement
						::InterlockedIncrement (reinterpret_cast<LONG*> (&fCountHolder->fCount_DONT_ACCESS));
					#else
						fCountHolder->fCount_DONT_ACCESS++;
					#endif
				}
			}
	template	<typename T>
		inline	RefCntPtr<T>::RefCntPtr (T* from, RefCntPtrBase* useCounter):
			fPtr (from),
			fCountHolder (from == NULL? NULL: useCounter)
			{
				if (fCountHolder != NULL) {
					#if		qUseInterlockedIncrement
						::InterlockedIncrement (reinterpret_cast<LONG*> (&fCountHolder->fCount_DONT_ACCESS));
					#else
						fCountHolder->fCount_DONT_ACCESS++;
					#endif
				}
			}
	template	<typename T>
		inline	RefCntPtr<T>::RefCntPtr (const RefCntPtr<T>& from):
			fPtr (from.fPtr),
			fCountHolder (from.fCountHolder)
			{
				if (fPtr != NULL) {
					RequireNotNil (fCountHolder);
					#if		qUseInterlockedIncrement
						::InterlockedIncrement (reinterpret_cast<LONG*> (&fCountHolder->fCount_DONT_ACCESS));
					#else
						fCountHolder->fCount_DONT_ACCESS++;
					#endif
				}
			}
	template	<typename T>
		inline	RefCntPtr<T>& RefCntPtr<T>::operator= (const RefCntPtr<T>& rhs)
			{
				if (rhs.fPtr != fPtr) {
					if (fPtr != NULL) {
						AssertNotNil (fCountHolder);
						Assert (fCountHolder->fCount_DONT_ACCESS > 0);
						if (
							#if		qUseInterlockedIncrement
								::InterlockedDecrement (reinterpret_cast<LONG*> (&fCountHolder->fCount_DONT_ACCESS)) == 0
							#else
								--fCountHolder->fCount_DONT_ACCESS == 0
							#endif
							) {
							fCountHolder->DO_DELETE_REF_CNT ();
							delete fPtr;
							fCountHolder = NULL;
							fPtr = NULL;
						}
					}
					fPtr = rhs.fPtr;
					fCountHolder = rhs.fCountHolder;
					if (fPtr != NULL) {
						AssertNotNil (fCountHolder);
						Assert (fCountHolder->fCount_DONT_ACCESS > 0);
						#if		qUseInterlockedIncrement
							::InterlockedIncrement (reinterpret_cast<LONG*> (&fCountHolder->fCount_DONT_ACCESS));
						#else
							fCountHolder->fCount_DONT_ACCESS++;
						#endif
					}
				}
				return *this;
			}
	template	<typename T>
		inline	RefCntPtr<T>::~RefCntPtr ()
			{
				if (fPtr != NULL) {
					AssertNotNil (fCountHolder);
					Assert (fCountHolder->fCount_DONT_ACCESS > 0);
					if (
						#if		qUseInterlockedIncrement
							::InterlockedDecrement (reinterpret_cast<LONG*> (&fCountHolder->fCount_DONT_ACCESS)) == 0
						#else
							--fCountHolder->fCount_DONT_ACCESS == 0
						#endif
						) {
						fCountHolder->DO_DELETE_REF_CNT ();
						delete fPtr;
					}
				}
			}
	template	<typename T>
		inline	bool	RefCntPtr<T>::IsNull () const
			{
				return fPtr == NULL;
			}
	template	<typename T>
		/*
		@METHOD:		RefCntPtr<T>::GetRep
		@DESCRIPTION:	<p>Asserts that the pointer is non-NULL.</p>
		*/
		inline	T&	RefCntPtr<T>::GetRep () const
			{
				AssertNotNil (fPtr);
				AssertNotNil (fCountHolder);
				Assert (fCountHolder->fCount_DONT_ACCESS >= 1);
				return *fPtr;
			}
	template	<typename T>
		/*
		@METHOD:		RefCntPtr<T>::operator->
		@DESCRIPTION:	<p>Note - this CAN NOT return NULL (because -> semantics are typically invalid for a logically null pointer)</p>
		*/
		inline	T* RefCntPtr<T>::operator-> () const
			{
				return &GetRep ();
			}
	template	<typename T>
		/*
		@METHOD:		RefCntPtr<T>::operator*
		@DESCRIPTION:	<p></p>
		*/
		inline	T& RefCntPtr<T>::operator* () const
			{
				return GetRep ();
			}
	template	<typename T>
		/*
		@METHOD:		RefCntPtr<T>::operator->
		@DESCRIPTION:	<p>Note - this CAN return NULL</p>
		*/
		inline	RefCntPtr<T>::operator T* () const
			{
				return fPtr;
			}
	template	<typename T>
		/*
		@METHOD:		RefCntPtr<T>::get
		@DESCRIPTION:	<p>Mimic the 'get' API of the std::auto_ptr&lt;T&gt; class. Just return the pointed to object, with no
					asserts about it being non-null.</p>
		*/
		inline	T*	RefCntPtr<T>::get () const
			{
				return (fPtr);
			}
	template	<typename T>
		/*
		@METHOD:		RefCntPtr<T>::release
		@DESCRIPTION:	<p>Mimic the 'get' API of the std::auto_ptr&lt;T&gt; class. Make this pointer NULL, but first return the
					pre-existing pointer value. Note - if there were more than one references to the underlying object, its not destroyed.
					<br>
					NO - Changed API to NOT return old pointer, since COULD have been destroyed, and leads to buggy coding.
					If you want the pointer before release, explicitly call get () first!!!
					</p>
		*/
		inline	void	RefCntPtr<T>::release ()
			{
				*this = RefCntPtr<T> (NULL);
			}
	template	<typename T>
		/*
		@METHOD:		RefCntPtr<T>::clear
		@DESCRIPTION:	<p>Synonymn for RefCntPtr<T>::release ()
					</p>
		*/
		inline	void	RefCntPtr<T>::clear ()
			{
				release ();
			}
	template	<typename T>
		/*
		@METHOD:		RefCntPtr<T>::reset
		@DESCRIPTION:	<p>Mimic the 'get' API of the std::auto_ptr&lt;T&gt; class. Make this pointer 'p', but first return the
					pre-existing pointer value. Unreference any previous value. Note - if there were more than one references to the underlying object, its not destroyed.</p>
		*/
		inline	void	RefCntPtr<T>::reset (T* p)
			{
				if (fPtr != p) {
					*this = RefCntPtr<T> (p);
				}
			}
	template	<typename T>
		/*
		@METHOD:		RefCntPtr<T>::CurrentRefCount
		@DESCRIPTION:	<p>I used to keep this available only for debugging, but I've found a few cases where its handy outside the debugging context
		so not its awlays avaialble (it has no cost to keep available).</p>
		*/
		inline	size_t	RefCntPtr<T>::CurrentRefCount () const
			{
				return fCountHolder==NULL? 0: fCountHolder->fCount_DONT_ACCESS;
			}
	template	<typename T>
		bool	RefCntPtr<T>::operator< (const RefCntPtr<T>& rhs) const
			{
				// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
				// this does the same thing...
				//		-- LGP 2009-01-11
				return fPtr < rhs.fPtr;
			}
	template	<typename T>
		bool	RefCntPtr<T>::operator<= (const RefCntPtr<T>& rhs) const
			{
				// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
				// this does the same thing...
				//		-- LGP 2009-01-11
				return fPtr <= rhs.fPtr;
			}
	template	<typename T>
		bool	RefCntPtr<T>::operator> (const RefCntPtr<T>& rhs) const
			{
				// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
				// this does the same thing...
				//		-- LGP 2009-01-11
				return fPtr > rhs.fPtr;
			}
	template	<typename T>
		bool	RefCntPtr<T>::operator>= (const RefCntPtr<T>& rhs) const
			{
				// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
				// this does the same thing...
				//		-- LGP 2009-01-11
				return fPtr >= rhs.fPtr;
			}
	template	<typename T>
		bool	RefCntPtr<T>::operator== (const RefCntPtr<T>& rhs) const
			{
				return fPtr == rhs.fPtr;
			}
	template	<typename T>
		bool	RefCntPtr<T>::operator!= (const RefCntPtr<T>& rhs) const
			{
				return fPtr != rhs.fPtr;
			}
	template	<typename T>
		RefCntPtrBase*		RefCntPtr<T>::_PEEK_CNT_PTR_ () const
			{
				return fCountHolder;
			}






//	void	ThrowIfNull ()
	template	<typename	T>
		inline	void	ThrowIfNull (const RefCntPtr<typename T>& p)
			{
				if (p.get () == NULL) {
					Exceptions::DoThrow (bad_alloc (), _T ("ThrowIfNull (RefCntPtr<typename T> ()) - throwing bad_alloc ()"));
				}
			}
}
}

#endif	/*__RefCntPtr_h__*/
