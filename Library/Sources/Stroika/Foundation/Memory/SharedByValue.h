/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_SharedByValue_h_
#define	_Stroika_Foundation_Memory_SharedByValue_h_	1

#include	"../StroikaPreComp.h"

#include	"../Memory/SharedPtr.h"


/*
 * TODO:
 *		o	Redo SharedByValue<> so its possible to have a COPIER as a static member(so no size wasted) OR 
 *			copier as data member.
 */

namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {


			/*
			@CLASS:			SharedByValue<T>
			@DESCRIPTION:	<p>This utility class should not be used lightly. Its somewhat tricky to use properly. Its meant
				to facilitiate implementing the copy-on-write semantics which are often handy in providing high-performance
				data structures.</p>
					<p>This class should allow SHARED_IMLP to be either Memory::SharedPtr<> or std::shared_ptr</p>
					<p>This class template was originally called CopyOnWrite<></p>
			*/
			template    <typename	T, typename SHARED_IMLP = SharedPtr<T>>
				class	SharedByValue : public SHARED_IMLP {
					public:
						SharedByValue ();
						SharedByValue (const SharedByValue<T,SHARED_IMLP>& from);

					#if		!qCompilerAndStdLib_Supports_lambda_default_argument
					public:
						inline	T*	DefaultElementCopier_ (const T& t)
							{
								return new T (t);
							}
					#endif

					public:
						#if		qCompilerAndStdLib_Supports_lambda_default_argument
						SharedByValue (const SHARED_IMLP& from, T* (*copier) (const T&) = [](const T& t) { return new T (t); });
						SharedByValue (T* from, T* (*copier) (const T&) = [](const T& t) { return new T (t); });
						#else
						SharedByValue (const SHARED_IMLP& from, T* (*copier) (const T&) = DefaultElementCopier_);
						SharedByValue (T* from, T* (*copier) (const T&) = DefaultElementCopier_);
						#endif

					public:
						nonvirtual	SharedByValue<T,SHARED_IMLP>& operator= (const SharedByValue<T,SHARED_IMLP>& src);

					public:
						/*
							* GetPointer () returns the real underlying ptr we store. It can be nullptr. This should
							* rarely be used - use operator-> in preference. This is only for dealing with cases where
							* the ptr could legitimately be nil.
							*/
						nonvirtual	const T*	GetPointer () const;
						nonvirtual	T*			GetPointer ();

					public:
						/*
						 * These operators require that the underlying ptr is non-nil.
						 */
						nonvirtual	const T*	operator-> () const;
						nonvirtual	T*			operator-> ();
						nonvirtual	const T&	operator* () const;
						nonvirtual	T&			operator* ();

					private:
						T*		(*fCopier) (const T&);

					public:
						nonvirtual	void	Assure1Reference ();

					private:
						nonvirtual	void	BreakReferences_ ();
				};
	
		}
	}
}
#endif	/*_Stroika_Foundation_Memory_SharedByValue_h_*/







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"SharedByValue.inl"
