
template <typename KEY, typename VALUE>
KeyValue<KEY, VALUE>::KeyValue (const KeyType& k, const ValueType& v) :
	fKey (k),
	fValue (v)
{
}

template <typename KEY, typename VALUE>
const typename  KeyValue<KEY, VALUE>::KeyType&	KeyValue<KEY, VALUE>::GetKey ()  const
{
	return fKey;
}

template <typename KEY, typename VALUE>
const   typename    KeyValue<KEY, VALUE>::ValueType&	KeyValue<KEY, VALUE>::GetValue ()  const
{
	return fValue;
}

template <typename KEY, typename VALUE>
void	KeyValue<KEY, VALUE>::SetValue (const ValueType& v)
{
	fValue = v;
}

