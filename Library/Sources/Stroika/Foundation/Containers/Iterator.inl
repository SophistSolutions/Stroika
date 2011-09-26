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

					RangedForIterator (const Container& t) :
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


            // class Rep<T>
            template	<typename T> inline	Iterator<T>::Rep::Rep ()
            {
            }

            template	<typename T> inline	Iterator<T>::Rep::~Rep ()
            {
            }

            template	<typename T> inline	bool	Iterator<T>::Rep::Done () const
            {
                return not const_cast<Iterator<T>::Rep*> (this)->More (nullptr, false);
            }

            // class Iterator<T>
            template	<typename T> inline	Iterator<T>::Iterator (const Iterator<T>& from) :
                fIterator (from.fIterator, &Clone_),
                fCurrent (from.fCurrent)
            {
				RequireNotNull (from.fIterator);
            }

            template	<typename T> inline	Iterator<T>::Iterator (Rep* it)   :
                fIterator (it, &Clone_)
            {
				if (it == nullptr) {
					fIterator = GetSentinal ().fIterator;
				}

				EnsureNotNull (fIterator);
            }

            template	<typename T> inline	Iterator<T>::~Iterator ()
            {
            }

            template	<typename T> inline	Iterator<T>&	Iterator<T>::operator= (const Iterator<T>& rhs)
            {
            	fIterator = rhs.fIterator;
            	fCurrent = rhs.fCurrent;
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

            template	<typename T> inline bool   Iterator<T>::operator!= (Iterator rhs)  const
            {
            	return not operator== (rhs);
            }

            template	<typename T> inline bool   Iterator<T>::operator== (Iterator rhs)  const
            {
            	if (rhs.Done ()) {
					return Done ();
            	}
            	else if (not Done ()) {
					return false;
            	}

            	// assigning to local variables to ensure const version called
            	const	Iterator<T>::Rep* lhsRep = fIterator.GetPointer ();
            	const	Iterator<T>::Rep* rhsRep = fIterator.GetPointer ();
                return (lhsRep == rhsRep
						and fCurrent == rhs.fCurrent);
			}

			template	<typename T> inline  typename Iterator<T>::Rep*   Iterator<T>::Clone_ (const Iterator<T>::Rep& rep)
			{
				return rep.Clone ();
			}

			template	<typename T> 	Iterator<T>	Iterator<T>::GetSentinal ()
			{
				class RepSentinal : public Iterator<T>::Rep  {
					public:
						RepSentinal () {}
					public:
						virtual	bool	More (T* current, bool advance) override
						{
							return false;
						}
						virtual	Rep*	Clone () const override
						{
							RequireNotReached ();
							return nullptr;
						}

				};

				static	Iterator<T> kSentinal = Iterator<T> (new RepSentinal ());
				return Iterator<T> (kSentinal);
			}


		}
    }
}



#endif /* _Stroika_Foundation_Containers_Iterator_inl_ */

