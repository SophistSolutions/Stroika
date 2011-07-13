/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Containers_SetUtils_h_
#define	_Stroika_Foundation_Containers_SetUtils_h_	1

#include	"../StroikaPreComp.h"

#include	<set>

#include	"../Configuration/Common.h"



/// THIS MODULE SB OBSOLETE ONCE WE GET STROIKA CONTAINERS WORKING


namespace	Stroika {	
	namespace	Foundation {
		namespace	Containers {


			template	<class T>
				set<typename T>	operator- (const set<typename T>& lhs, const set<typename T>& rhs);
			template	<class T>
				set<typename T>	operator+ (const set<typename T>& lhs, const set<typename T>& rhs);
			template	<typename T, typename FROMCONTAINER>
				set<typename T>&	operator+= (set<typename T>& lhs, const FROMCONTAINER& rhs);
			template	<typename T, typename FROMCONTAINER>
				set<typename T>&	operator-= (set<typename T>& lhs, const FROMCONTAINER& rhs);

			template	<typename T>
				set<typename T>	mkS ();
			template	<typename T>
				set<typename T>	mkS (const T& t1);
			template	<typename T>
				set<typename T>	mkS (const T& t1, const T& t2);
			template	<typename T>
				set<typename T>	mkS (const T& t1, const T& t2, const T& t3);
			template	<typename T>
				set<typename T>	mkS (const T& t1, const T& t2, const T& t3, const T& t4);
			template	<typename T, typename FROMCONTAINER>
				set<typename T>	mkSfromC (const FROMCONTAINER& rhs);

			template	<typename T>
				void	Intersect (set<T>* s1, const set<T>& s2);

			template	<typename T>
				set<typename T>	Intersection (const set<T>& s1, const set<T>& s2);
			template	<typename T>
				bool	Intersect (const set<T>& s1, const set<T>& s2);

			template	<typename T>
				void	Union (set<T>* s1, const set<T>& s2);
			template	<typename T>
				set<typename T>	Union (const set<T>& s1, const set<T>& s2);

		}
	}
}
#endif	/*_Stroika_Foundation_Containers_SetUtils_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"SetUtils.inl"
