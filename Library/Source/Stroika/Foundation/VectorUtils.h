/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__VectorSupport_h__
#define	__VectorSupport_h__	1

#include	"StroikaPreComp.h"

#include	<vector>

#include	"Support.h"



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

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
	#include	"Assertions.h"

namespace	Stroika {	
	namespace	Foundation {
	template	<typename T>
		inline	vector<typename T>	VectorUtils::mkV ()
			{
				vector<T>	v;
				return v;
			}
	template	<typename T>
		inline	vector<typename T>	VectorUtils::mkV (const T& t1)
			{
				vector<T>	v;
				v.push_back (t1);
				return v;
			}
	template	<typename T>
		inline	vector<typename T>	VectorUtils::mkV (const T& t1, const T& t2)
			{
				vector<T>	v;
				v.reserve (2);
				v.push_back (t1);
				v.push_back (t2);
				return v;
			}
	template	<typename T>
		inline	vector<typename T>	VectorUtils::mkV (const T& t1, const T& t2, const T& t3)
			{
				vector<T>	v;
				v.reserve (3);
				v.push_back (t1);
				v.push_back (t2);
				v.push_back (t3);
				return v;
			}
	template	<typename T>
		inline	vector<typename T>	VectorUtils::mkV (const T& t1, const T& t2, const T& t3, const T& t4)
			{
				vector<T>	v;
				v.reserve (4);
				v.push_back (t1);
				v.push_back (t2);
				v.push_back (t3);
				v.push_back (t4);
				return v;
			}
	template	<typename T>
		inline	vector<typename T>	VectorUtils::mkV (const T& t1, const T& t2, const T& t3, const T& t4, const T& t5)
			{
				vector<T>	v;
				v.reserve (5);
				v.push_back (t1);
				v.push_back (t2);
				v.push_back (t3);
				v.push_back (t4);
				v.push_back (t5);
				return v;
			}

	template	<typename T, typename ContainerOfT>
		vector<typename T>	VectorUtils::mkVC (const ContainerOfT& ts)
			{
				vector<T>	result;
				result.reserve (ts.size ());
				for (ContainerOfT::const_iterator i = ts.begin (); i != ts.end (); ++i) {
					result.push_back (*i);
				}
				return result;
			}


	template	<typename T>
		void	VectorUtils::Append2Vector (vector<typename T>* v, const vector<typename T>& v2)
			{
				RequireNotNil (v);
				size_t	c	=	max (v->capacity (), v->size () + v2.size ());
				v->reserve (c);
				for (vector<typename T>::const_iterator i = v2.begin (); i != v2.end (); ++i) {
					v->push_back (*i);
				}
			}
	template	<typename T, typename ContainerOfT>
		void	VectorUtils::Append2Vector (vector<typename T>* v, const typename ContainerOfT& v2)
			{
				RequireNotNil (v);
				size_t	c	=	max (v->capacity (), v->size () + v2.size ());
				v->reserve (c);
				for (ContainerOfT::const_iterator i = v2.begin (); i != v2.end (); ++i) {
					v->push_back (*i);
				}
			}

	template	<typename T>
		inline	vector<typename T>	VectorUtils::operator+ (const vector<typename T>& l, const vector<typename T>& r)
			{
				vector<typename T>	result	=	l;
				Append2Vector (&result, r);
				return result;
			}

	template	<typename T>
		inline	vector<typename T>&	VectorUtils::operator+= (vector<typename T>& l, const vector<typename T>& r)
			{
				Append2Vector (&l, r);
				return l;
			}

	template	<typename T>
		vector<typename T>	VectorUtils::Intersection (const vector<T>& s1, const vector<T>& s2)
			{
				vector<T>	result;
				result.reserve (min (s1.size (), s2.size ()));
				for (vector<T>::const_iterator i = s1.begin (); i != s1.end (); ++i) {
					if (std::find (s2.begin (), s2.end (), *i) != s2.end ()) {
						result.push_back (*i);
					}
				}
				return result;
			}

	}
}
#endif	/*__VectorSupport_h__*/
