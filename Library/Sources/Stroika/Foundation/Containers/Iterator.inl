/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Iterator_inl_
#define _Stroika_Foundation_Containers_Iterator_inl_

#include	"../Debug/Assertions.h"


namespace	Stroika {
	namespace	Foundation {
		namespace	Containers {
            // class IteratorRep<T>
            template	<typename T> inline	IteratorRep<T>::IteratorRep ()
            {
            }

            template	<typename T> inline	IteratorRep<T>::~IteratorRep ()
            {
            }

            // class Iterator<T>
            template	<typename T> inline	Iterator<T>::Iterator (const Iterator<T>& from) :
                fIterator (0),
                fCurrent (from.fCurrent)
            {
                RequireNotNull (from.fIterator);
                fIterator = from.fIterator->Clone ();
                EnsureNotNull (fIterator);
            }

            template	<typename T> inline	Iterator<T>::Iterator (IteratorRep<T>* it) :
                fIterator (it)
            {
                RequireNotNull (it);
                EnsureNotNull (fIterator);
            }

            template	<typename T> inline	Iterator<T>::~Iterator ()
            {
                delete fIterator;
            }

            template	<typename T> inline	Iterator<T>&	Iterator<T>::operator= (const Iterator<T>& rhs)
            {
                RequireNotNull (fIterator);
                RequireNotNull (rhs.fIterator);
                if (fIterator != rhs.fIterator) {
                    delete fIterator;
                    fIterator = rhs.fIterator->Clone ();
                    EnsureNotNull (fIterator);
                }
                return (*this);
            }

            template	<typename T> inline	bool	Iterator<T>::More ()
            {
                RequireNotNull (fIterator);
                return (fIterator->More (&fCurrent));
            }

            template	<typename T> inline	T	Iterator<T>::Current () const
            {
                RequireNotNull (fIterator);
                return (fCurrent);
            }

             template	<typename T> inline      const   T   operator* () const
            {
                return Current ();
            }

             template	<typename T> inline       const void  operator++ ()
            {
                More ();
            }

             template	<typename T> inline       bool    operator!= (IterationState rhs)
            {
                return (not fIterator->Done ());
            }
		}
    }
}



#endif /* _Stroika_Foundation_Containers_Iterator_inl_ */

