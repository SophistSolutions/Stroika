/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_RefCntPtr_h_
#define	_Stroika_Foundation_Memory_RefCntPtr_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"
#include	"../Execution/Exceptions.h"



/*
 * Description:
 *
 *	...
 *
 * TODO:
 *		+	SERIOUSLY CONSIDER relationship between this class and Shared<>
 *
 *
 */


namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {


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
									Execution::AtomicIncrement (&fCountHolder->fCount_DONT_ACCESS);
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



// REDO THIS - SB PaRTIAL TEMPLATE SPECIALIZATION
template	<typename	T>
	void	ThrowIfNull (const Memory::RefCntPtr<typename T>& p);
	
		}




	}
}
#endif	/*_Stroika_Foundation_Memory_RefCntPtr_h_*/







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"RefCntPtr.inl"
