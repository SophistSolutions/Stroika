/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Lockable_inl_
#define	_Stroika_Foundation_Execution_Lockable_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Exceptions.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

		//	class	Lockable
			template	<typename BASE, typename LOCKTYPE>
				inline	Lockable<BASE,LOCKTYPE>::Lockable ()
					: BASE ()
					, LOCKTYPE ()
					{
					}
			template	<typename BASE, typename LOCKTYPE>
				inline	Lockable<BASE,LOCKTYPE>::Lockable (const BASE& from)
					: BASE (from)
					, LOCKTYPE ()
					{
					}
			template	<typename BASE, typename LOCKTYPE>
				inline	const Lockable<BASE,LOCKTYPE>& Lockable<BASE,LOCKTYPE>::operator= (const BASE& rhs)
					{
						BASE&	THIS_BASE	=	*this;
						THIS_BASE = rhs;
						return *this;
					}


			namespace	Private {
				template	<typename BASE, typename LOCKTYPE = CriticalSection>
					class	Lockable_POD : public LOCKTYPE {
						public:
							Lockable_POD ()
								: fBase (BASE ())
								{
								}
							Lockable_POD (const BASE& from)
								: fBase (from)
								{
								}
							const Lockable_POD& operator= (const BASE& rhs)
								{
									fBase = rhs;
									return *this;
								}
							operator BASE () const
								{
									return fBase;
								}
						BASE	fBase;
					};
			}
			//	Specailizations for POD types
			template	<>
				class	Lockable<int,CriticalSection> : public Private::Lockable_POD<int,CriticalSection>  {
					public:
						typedef	int	T;
						Lockable<T,CriticalSection> ()															{}
						Lockable<T,CriticalSection> (T from):			Lockable_POD<T,CriticalSection> (from)	{}
						const Lockable_POD& operator= (const T& rhs)	{ return Lockable_POD<T,CriticalSection>::operator= (rhs); }
				};
			template	<>
				class	Lockable<unsigned int,CriticalSection> : public Private::Lockable_POD<unsigned int,CriticalSection>  {
					public:
						typedef	unsigned int	T;
						Lockable<T,CriticalSection> ()															{}
						Lockable<T,CriticalSection> (T from):			Lockable_POD<T,CriticalSection> (from)	{}
						const Lockable_POD& operator= (const T& rhs)	{ return Lockable_POD<T,CriticalSection>::operator= (rhs); }
				};
			template	<>
				class	Lockable<double,CriticalSection> : public Private::Lockable_POD<double,CriticalSection>  {
					public:
						typedef	double	T;
						Lockable<T,CriticalSection> ()															{}
						Lockable<T,CriticalSection> (T from):			Lockable_POD<T,CriticalSection> (from)	{}
						const Lockable_POD& operator= (const T& rhs)	{ return Lockable_POD<T,CriticalSection>::operator= (rhs); }
				};



		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Lockable_inl_*/
