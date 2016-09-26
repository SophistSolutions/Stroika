
template <typename KEY, typename VALUE>
KeyValue<KEY, VALUE>::KeyValue (const KEY& k, const VALUE& v) :
    fKey (k),
    fValue (v)
{
}

template <typename KEY, typename VALUE>
const KEY&  KeyValue<KEY, VALUE>::GetKey ()  const
{
    return fKey;
}

template <typename KEY, typename VALUE>
const VALUE&    KeyValue<KEY, VALUE>::GetValue ()  const
{
    return fValue;
}
