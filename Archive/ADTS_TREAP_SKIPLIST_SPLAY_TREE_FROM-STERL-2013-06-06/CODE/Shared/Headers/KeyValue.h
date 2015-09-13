#pragma once

#include "Config.h"


/*
	By default you store the key in a separate field. In cases where you can extract the key from the value you should
	use template specialization to make more space efficient. For example, a skiplist of strings, where the key and value are the
	same, can be supported as follows:

	template <>
	struct	KeyValue<string, string> {
		KeyValue (string k,string v) :
			fValue (v)
			{
			}

			string	GetKey () const
			{
				return fValue;
			}

		string	fValue;
	};
*/
template <typename KEY, typename VALUE>
class	KeyValueInterface {
	public:
        typedef KEY     KeyType;
        typedef VALUE   ValueType;

	public:
		const KEY&		GetKey ()  const;
		const VALUE&	GetValue ()  const;
};


template <typename KEY, typename VALUE>
class	KeyValue : public KeyValueInterface<KEY, VALUE> {
	public:
        typedef KEY     KeyType;
        typedef VALUE   ValueType;

 	public:
		KeyValue (const KeyType& k, const ValueType& v);

		const KeyType&		GetKey ()  const;
		const ValueType&	GetValue ()  const;

		void    SetValue (const ValueType& t);

	private:
		KeyType		fKey;
		ValueType	fValue;
};


#include "KeyValue.inl"

