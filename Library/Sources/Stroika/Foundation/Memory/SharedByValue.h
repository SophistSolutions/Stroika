/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_SharedByValue_h_
#define	_Stroika_Foundation_Memory_SharedByValue_h_	1

#include	"../StroikaPreComp.h"

#include	"../Memory/SharedPtr.h"


/*
 * TODO:
 *		o	SharedByValue - even when given an effectively ZERO-SIZED arg for COPIER - since it saves it as a data member and has min-size 1 byte it wastes
 *			space in SharedByValue<T>. See if this is optimized away in release builds and if not, see if we can find some clever way around it.
 *			(template specializaiton?). Note - this is exactly what the msvc vector<T> template does so presumably it IS optimized.
 *
 */

namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {




			/*
			 */
			template	<typename	T>
				struct	SharedByValue_CopyByFunction {
						#if		qCompilerAndStdLib_Supports_lambda_default_argument
						SharedByValue_CopyByFunction (T* (*copier) (const T&) = [](const T& t) { return new T (t); });
						#else
						static	T*	DefaultElementCopier_ (const T& t);
						SharedByValue_CopyByFunction (T* (*copier) (const T&) = DefaultElementCopier_);
						#endif
					nonvirtual	T*	Copy (const T& t) const;
					T*		(*fCopier_) (const T&);
				};




			/*
			@CLASS:			SharedByValue<T>
			@DESCRIPTION:	<p>This utility class should not be used lightly. Its somewhat tricky to use properly. Its meant
				to facilitiate implementing the copy-on-write semantics which are often handy in providing high-performance
				data structures.</p>
					<p>This class should allow SHARED_IMLP to be either Memory::SharedPtr<> or std::shared_ptr</p>
					<p>This class template was originally called CopyOnWrite<></p>
			*/
			template    <typename	T, typename COPIER = SharedByValue_CopyByFunction<T>, typename SHARED_IMLP = SharedPtr<T>>
				class	SharedByValue : public SHARED_IMLP {
					public:
						SharedByValue ();
						SharedByValue (const SharedByValue<T,COPIER,SHARED_IMLP>& from);

					public:
						SharedByValue (const SHARED_IMLP& from, const COPIER& copier = COPIER ());
						SharedByValue (T* from, const COPIER& copier = COPIER ());

					public:
						nonvirtual	SharedByValue<T,COPIER,SHARED_IMLP>& operator= (const SharedByValue<T,COPIER,SHARED_IMLP>& src);

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
						COPIER	fCopier_;

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
