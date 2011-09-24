/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Iterator_inl_
#define _Stroika_Foundation_Containers_Iterator_inl_

#include	"../Debug/Assertions.h"

// we allow fIterator to equal nullptr, as a sentinal value during iteration, signaling that iteration is Done
#define	qIteratorUsingNullRepAsSentinalValue	1


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

            template	<typename T> inline	bool	IteratorRep<T>::Done () const
            {
                return not const_cast<IteratorRep<T>*> (this)->More (nullptr, false);
            }

            // class Iterator<T>
            template	<typename T> inline	Iterator<T>::Iterator (const Iterator<T>& from) :
                fIterator (0),
                fCurrent (from.fCurrent)
            {
#if qIteratorUsingNullRepAsSentinalValue
				if (from.fIterator != nullptr) {
#endif
					RequireNotNull (from.fIterator);
					fIterator = from.fIterator->Clone ();
					EnsureNotNull (fIterator);
#if qIteratorUsingNullRepAsSentinalValue
				}
#endif
            }

            template	<typename T> inline	Iterator<T>::Iterator (IteratorRep<T>* it) :
                fIterator (it)
            {
#if !qIteratorUsingNullRepAsSentinalValue
				RequireNotNull (it);
				EnsureNotNull (fIterator);
#endif
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
                return (fIterator->More (&fCurrent, true));
            }

            template	<typename T> inline	T	Iterator<T>::Current () const
            {
                RequireNotNull (fIterator);
                return (fCurrent);
            }

            template	<typename T>  inline    T   Iterator<T>::operator* () const
            {
                return Current ();
            }

            template	<typename T>   inline   void  Iterator<T>::operator++ ()
            {
                More ();
            }

            template	<typename T> inline bool   Iterator<T>::operator!= (Iterator rhs)
            {
            	if (rhs.fIterator == nullptr) {
					return (not fIterator->Done ());
            	}
                return (fIterator != rhs.fIterator);
            }
		}
    }
}



#endif /* _Stroika_Foundation_Containers_Iterator_inl_ */

