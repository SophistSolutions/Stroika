/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Containers_Basics_inl_
#define	_Stroika_Foundation_Containers_Basics_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	<cstddef>

#include	<algorithm>


namespace	Stroika {	
	namespace	Foundation {
		namespace	Containers {

			template	<typename CONTAINER>
				inline	typename CONTAINER::value_type*	Start (CONTAINER& c)
					{
						size_t	cnt	=	c.size ();
						return cnt==0? NULL: &c[0];
					}
			template	<typename CONTAINER>
				inline	 const typename CONTAINER::value_type*	Start (const CONTAINER& c)
					{
						size_t	cnt	=	c.size ();
						return cnt==0? NULL: &c[0];
					}
			template	<typename CONTAINER>
				inline	typename CONTAINER::value_type*	End (CONTAINER& c)
					{
						size_t	cnt	=	c.size ();
						return cnt == 0? NULL: &c[0] + cnt;
					}
			template	<typename CONTAINER>
				inline	const typename CONTAINER::value_type*	End (const CONTAINER& c)
					{
						size_t	cnt	=	c.size ();
						return cnt == 0? NULL: &c[0] + cnt;
					}



			template	<typename	CONTAINER>
				inline	void	ReserveSpeedTweekAdd1 (CONTAINER& c, size_t kMinChunk)
					{
						size_t	size	=	c.size ();
						//Assert (size <= c.capacity ());	we don't want to include the assertion library in this .h file.... for now... --LGP 2007-03-08
						if (size == c.capacity ()) {
							size *= 2;
							size = std::min (size, kMinChunk);
							c.reserve (size);
						}
					}

			template	<typename	CONTAINER>
				inline	void	ReserveSpeedTweekAddN (CONTAINER& c, size_t n, size_t kMinChunk)
					{
						size_t	size	=	c.size () + n;
						if (size >= c.capacity ()) {
							size *= 2;
							size = std::min (size, kMinChunk);
							c.reserve (size);
						}
					}

		}
	}
}
#endif	/*_Stroika_Foundation_Containers_Basics_inl_*/



