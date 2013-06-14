#pragma once

#include "Config.h"
#include "Comparer.h"

/*
	Wrapper to allow simple usage of hashed keys in container classes.
	Usage: replace KEY template argument with HashKey<KEY>
	For example, replace SplayTree<string, int> with SplayTree<HashKey<string>, int>
	For most non-integer key it is a good idea to wrap in HashKey, as the savings in storage space and find speed usually
	exceeds the cost of the hash call. The container interface assumes that type KEY is cheap to copy, which is true for HashKey
	but not necessarily true for the original key. For example
	Treap::Find (KEY k) will create a copy of KEY on the stack, which is trivial for the 4 byte HashKey but potentially expensive is an arbitrarily long string.

	You can make things slightly nicer for the user by declaring an appropriate KeyValue overide. For example:
		struct	HashedStringKeyValue : public KeyValueInterface<HashKey<string>, string> {
			HashedStringKeyValue (HashKey<string> k,const string& v) :
				fKey (k),
				fValue (v)
			{
			}
			HashedStringKeyValue (string v) :
				fKey (HashKey<string> (v)),
				fValue (v)
			{
			}

			HashedStringKeyValue (const char* v) :
				fKey (HashKey<string> (string (v))),
				fValue (string (v))
			{
			}

			HashKey<string>	GetKey ()  const	{ return fKey; }
			string			GetValue () const	{ return fValue; }

			HashKey<string>	fKey;
			string			fValue;
		};
	Now the user can call container.Add ("fred") rather than the more laborious container.Add (HashKey<string> ("fred"), "fred");

	Note that we automatically create a Comparer<HashKey> declaration which is needed in all container classes that require a key.
*/

// sadly, if I namespace Hashing, then I can't template Hash (string) because different namespaces
//namespace Hashing {
	extern	unsigned int HashMem (const char * data, int len);

	template <typename KEY>
    size_t Hash (const KEY& k);
//}

template <typename KEY>
struct	HashKey {
	HashKey (const KEY& k);
	~HashKey () {}

	size_t	GetKey ()  const;

	size_t	fKey;
};


template <typename T>
struct Comparer<HashKey<T> > {
	static	int	Compare (HashKey<T> v1, HashKey<T> v2)
	{
		return Comparer<size_t>::Compare (v1.GetKey (), v2.GetKey ());
	}
};


#include "HashKey.inl"

