
template <typename KEY>
inline  size_t Hash (const KEY& k)
{
    return (HashMem ((const char*)&k, sizeof (k)));
}


template <>
inline  size_t /*Hashing::*/Hash<std::string> (const std::string& k)
{
    return (HashMem ((const char*)k.c_str (), k.size ()));
}


template <typename KEY>
HashKey<KEY>::HashKey (const KEY& k) :
    fKey (/*Hashing::*/Hash (k))
{
}

template <typename KEY>
size_t  HashKey<KEY>::GetKey () const
{
    return fKey;
}
