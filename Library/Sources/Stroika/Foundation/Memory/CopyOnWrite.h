/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_CopyOnWrite_h_
#define	_Stroika_Foundation_Memory_CopyOnWrite_h_	1

#include	"../StroikaPreComp.h"

#include	"../Memory/RefCntPtr.h"



namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {


			/*
			@CLASS:			CopyOnWrite<T>
			@DESCRIPTION:	<p>This utility class should not be used lightly. Its somewhat tricky to use properly. Its meant
				to facilitiate implementing the copy-on-write semantics which are often handy in providing high-performance
				data structures.</p>
					<p>This class SHOULD be completely compatible with either Memory::RefCntPtr<> or std::shared_ptr</p>
			*/
			template    <typename	T, typename SHARED_IMLP = RefCntPtr<T>>
				class	CopyOnWrite : public SHARED_IMLP {
					public:
						CopyOnWrite ();
						CopyOnWrite (const CopyOnWrite<T,SHARED_IMLP>& from);

					#if		!qCompilerAndStdLib_Supports_lambda_default_argument
					public:
						inline	T*	DefaultElementCopier_ (const T& t)
							{
								return new T (t);
							}
					#endif

					public:
						#if		qCompilerAndStdLib_Supports_lambda_default_argument
						CopyOnWrite (const SHARED_IMLP& from, T* (*copier) (const T&) = [](const T& t) { return new T (t); });
						CopyOnWrite (T* from, T* (*copier) (const T&) = [](const T& t) { return new T (t); });
						#else
						CopyOnWrite (const SHARED_IMLP& from, T* (*copier) (const T&) = DefaultElementCopier_);
						CopyOnWrite (T* from, T* (*copier) (const T&) = DefaultElementCopier_);
						#endif

					public:
						nonvirtual	CopyOnWrite<T,SHARED_IMLP>& operator= (const CopyOnWrite<T,SHARED_IMLP>& src);

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
#endif	/*_Stroika_Foundation_Memory_CopyOnWrite_h_*/







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"CopyOnWrite.inl"
