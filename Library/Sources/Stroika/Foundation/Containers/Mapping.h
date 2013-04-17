/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Mapping_h_
#define _Stroika_Foundation_Containers_Mapping_h_  1

/*
 *
 *
 *  STATUS:
 *      CODE NO WHERE NEAR COMPILING - just rough draft of API based on 1992 Stroika
 *
 *
 *
 *  TODO:
 *
 *      (o)         Implement first draft of code based on
 *                  http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/Mapping.hh
 *
 *      (o)         Should inherit from Iterable<T>
 *
 *
 */


#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             *      Mapping which allows for the association of two elements, and key and
             *  a value. The key UNIQUELY specifies its associated value.
             *
             *  @see    Dictionary<Key,T>
             */
            template    <class Key, class T>
            class   Mapping : public Iterable<pair<Key, T>> {
            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                /**
                 */
                Mapping ();
                Mapping (const Mapping<TKey, T>& s);
                explicit Mapping (const pair<Key, T>* start, const pair<Key, T>* end);

            protected:
                explicit Mapping (const _SharedPtrIRep& rep);

            public:
                nonvirtual  Mapping<Key, T>& operator= (const Mapping<Key, T>& src);

            public:
                /**
                 */
                nonvirtual  void    RemoveAll ();

            public:
                /**
                 * Note this method may not return a collection which is sorted. Note also, the
                 *returned value is a copy of the keys (by value) - at least logically (implementations
                 * maybe smart enough to use lazy copying)
                 */
                nonvirtual  Iterable<Key>   Keys () const;

            public:
                /**
                 */
                nonvirtual  bool Lookup (Key key, T* item) const;

            public:
                /**
                 */
                nonvirtual  void    Enter (Key key, T newElt);
                nonvirtual  void    Enter (const Mapping<Key, T>& items);

            public:
                /**
                 */
                nonvirtual  void    Remove (const Mapping<Key, T>& items);
                nonvirtual  void    Remove (Key key);
                nonvirtual  void    Remove (Iterator<T> i);

            public:
                nonvirtual  Mapping<Key, T>& operator+= (const Mapping<Key, T>& items);
                nonvirtual  Mapping<Key, T>& operator-= (const Mapping<Key, T>& items);

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            /**
             *  \brief  Implementation detail for Mapping<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Mapping<T> container API.
             */
            template    <typename T>
            class   Mapping<T>::_IRep : public Iterable<pair<Key, T>>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual void            RemoveAll ()                        =   0;
                virtual  Iterable<Key>  Keys () const                       =   0;
                virtual  bool           Lookup (Key key, T* item) const     =   0;
                virtual  void           Enter (Key key, T newElt)           =   0;
                virtual  void           Remove (Key key)                    =   0;
                virtual  void           Remove (Iterator<T> i)              =   0;
            };


        }
    }
}

#endif  /*_Stroika_Foundation_Containers_Mapping_h_ */



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Mapping.inl"
