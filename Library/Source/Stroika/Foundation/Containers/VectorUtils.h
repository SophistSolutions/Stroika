/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__VectorSupport_h__
#define	__VectorSupport_h__	1

#include	"../StroikaPreComp.h"

#include	<vector>

#include	"../Support.h"



namespace	Stroika {	
	namespace	Foundation {

namespace	VectorUtils {
	template	<typename T>
		vector<typename T>	mkV ();
	template	<typename T>
		vector<typename T>	mkV (const T& t1);
	template	<typename T>
		vector<typename T>	mkV (const T& t1, const T& t2);
	template	<typename T>
		vector<typename T>	mkV (const T& t1, const T& t2, const T& t3);
	template	<typename T>
		vector<typename T>	mkV (const T& t1, const T& t2, const T& t3, const T& t4);
	template	<typename T>
		vector<typename T>	mkV (const T& t1, const T& t2, const T& t3, const T& t4, const T& t5);

	template	<typename T, typename ContainerOfT>
		vector<typename T>	mkVC (const ContainerOfT& ts);


	template	<typename T>
		void	Append2Vector (vector<typename T>* v, const vector<typename T>& v2);
	template	<typename T, typename ContainerOfT>
		void	Append2Vector (vector<typename T>* v, const typename ContainerOfT& v2);


	template	<typename T>
		vector<typename T>	operator+ (const vector<typename T>& l, const vector<typename T>& r);

	template	<typename T>
		vector<typename T>&	operator+= (vector<typename T>& l, const vector<typename T>& r);

	template	<typename T>
		vector<typename T>	Intersection (const vector<T>& s1, const vector<T>& s2);
}

	}
}
#endif	/*__VectorSupport_h__*/



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"VectorUtils.inl"

