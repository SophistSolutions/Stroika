/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_RefCntPtr_inl
#define	_RefCntPtr_inl	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"BlockAllocated.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {


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





//REDO AS TEMPLATE SPECIALIZATIONS
//	void	ThrowIfNull ()
template	<typename	T>
	inline	void	ThrowIfNull (const Memory::RefCntPtr<typename T>& p)
		{
			if (p.get () == NULL) {
				Execution::DoThrow (bad_alloc (), _T ("ThrowIfNull (RefCntPtr<typename T> ()) - throwing bad_alloc ()"));
			}
		}

		}





	}
}
#endif	/*_RefCntPtr_inl*/
