/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Containsersgbasics_inl
#define	_Containsersgbasics_inl	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace	Stroika {	
	namespace	Foundation {
		namespace	Containers {

			template	<typename CONTAINER>
				inline	typename	CONTAINER::value_type*	Start (CONTAINER& c)
					{
						size_t	cnt	=	c.size ();
						return cnt==0? NULL: &c[0];
					}
			template	<typename CONTAINER>
				inline	typename	typename const CONTAINER::value_type*	Start (const CONTAINER& c)
					{
						size_t	cnt	=	c.size ();
						return cnt==0? NULL: &c[0];
					}
			template	<typename CONTAINER>
				inline	typename	CONTAINER::value_type*	End (CONTAINER& c)
					{
						size_t	cnt	=	c.size ();
						return cnt == 0? NULL: &c[0] + cnt;
					}
			template	<typename CONTAINER>
				inline	typename	typename const CONTAINER::value_type*	End (const CONTAINER& c)
					{
						size_t	cnt	=	c.size ();
						return cnt == 0? NULL: &c[0] + cnt;
					}



			template	<typename	CONTAINER>
				inline	void	ReserveSpeedTweekAdd1 (typename CONTAINER& c, size_t kMinChunk)
					{
						size_t	size	=	c.size ();
						//Assert (size <= c.capacity ());	we don't want to include the assertion library in this .h file.... for now... --LGP 2007-03-08
						if (size == c.capacity ()) {
							size *= 2;
							size = min (size, kMinChunk);
							c.reserve (size);
						}
					}

			template	<typename	CONTAINER>
				inline	void	ReserveSpeedTweekAddN (typename CONTAINER& c, size_t n, size_t kMinChunk)
					{
						size_t	size	=	c.size () + n;
						if (size >= c.capacity ()) {
							size *= 2;
							size = min (size, kMinChunk);
							c.reserve (size);
						}
					}

		}
	}
}

#endif	/*_Containsersgbasics_inl*/



