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

			/*
				Subclassed by front-end container writers.
				Most of the work is done in More, which does a lot of work because it is the
				only virtual function called during iteration, and will need to lock its
				container when doing "safe" iteration. More does the following:
					iterate to the next container value if advance is true
					(then) copy the current value into current, if current is not null
					return true if iteration can continue (not done iterating)

					typical uses:
						it++ -> More (null, true)
						*it -> More (&v, false); return v;
						Done -> More (null, false)

						(note that for performance and safety reasons the iterator envelope actually
						passes fCurrent into More when implenenting ++it
			*/
            template	<typename T> class	IteratorRep {
                protected:
                    IteratorRep ();

                public:
                    virtual	~IteratorRep ();

                public:
                    virtual	bool			More (T* current, bool advance)   = 0;
                    virtual	IteratorRep<T>*	Clone () const		= 0;
                    nonvirtual bool         Done () const;
            };

			/*
				Support for ranged for syntax: for (it : v) { it.Current (); }
				This typedef lets you easily construct iterators other than the basic
				iterator for the container.
				Sample usage:
				typedef	RangedForIterator<Tally<T>, TallyMutator<T> >		Mutator;
			*/
			template	<typename Container, typename IteratorClass>	class	RangedForIterator {
				public:
					RangedForIterator (Container& t) :
						fIt (t)
					{
					}

					nonvirtual  IteratorClass    begin () const
					{
						return fIt;
					}

					IteratorClass end () const
					{
						return (IteratorClass (nullptr));
					}

				private:
					IteratorClass	fIt;
			};


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

            template	<typename T> inline	Iterator<T>::Iterator (IteratorRep<T>* it)   :
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

            template	<typename T> inline	T	Iterator<T>::Current () const
            {
                return (operator* ());
            }

            template	<typename T> inline	bool	Iterator<T>::Done () const
            {
            	return fIterator->Done ();
            }

            template	<typename T>  inline    T   Iterator<T>::operator* () const
            {
                RequireNotNull (fIterator);
                return (fCurrent);
            }

            template	<typename T>   inline   void  Iterator<T>::operator++ ()
            {
                RequireNotNull (fIterator);
                fIterator->More (&fCurrent, true);
			}

            template	<typename T>   inline   void  Iterator<T>::operator++ (int)
            {
                RequireNotNull (fIterator);
                fIterator->More (&fCurrent, true);
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

