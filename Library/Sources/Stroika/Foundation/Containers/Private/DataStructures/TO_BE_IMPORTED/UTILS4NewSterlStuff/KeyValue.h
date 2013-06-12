#pragma once

#include "Config.h"


/*
    By default you store the key in a separate field. In cases where you can extract the key from the value you should
    use template specialization to make more space efficient. For example, a skiplist of strings, where the key and value are the
    same, can be supported as follows:

    template <>
    struct  KeyValue<string, string> {
        KeyValue (string k,string v) :
            fValue (v)
            {
            }

            string  GetKey () const
            {
                return fValue;
            }

        string  fValue;
    };
*/
template <typename KEY, typename VALUE>
class   KeyValueInterface {
public:
    const KEY&      GetKey ()  const;
    const VALUE&    GetValue ()  const;
};


template <typename KEY, typename VALUE>
class   KeyValue : public KeyValueInterface<KEY, VALUE> {
public:
    KeyValue (const KEY& k, const VALUE& v);

    const KEY&      GetKey ()  const;
    const VALUE&    GetValue ()  const;

private:
    KEY     fKey;
    VALUE   fValue;
};


#include "KeyValue.inl"

