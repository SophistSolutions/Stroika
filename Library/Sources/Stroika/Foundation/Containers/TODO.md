TODO (Foundation::Containers)


	(o)		Fix (already documented in Iterator.h todo) T fCurrent stuff - so no default CTOR required,
			and then fix the crazy extra operator++ funky stuff needed in the MakeIterator calls.

	(o)		Sent email to Sterl/Kdj to review - but I think I can completely lose patching code
			and stuff to maintain list of current iterators!
			--LGP 2013-01-19
			Review comments from Sterl/KDJ - but gist is we really DO need the patching code. Just
			document the details of hte conversaiton and WHY/WHERE we need patching,a dn that it
			only appliest o letter, and not envelope!

	(o)		Probably ready todo Sequence<> next.

	(o)		See comment "Consider adding class TWithCompareEquals<T> to add Iterable<T> like functions - where we can count on "T".
            Perhaps implement with a Require (TWithCompareEquals<T>) in CTORs for class?".

			Do this. See design spec for CONCEPTS (rejected for C++11 but probably in C++next). Maybe just do as DOCS. OR maybe real
			classes.

	(o)		When this code matures (when we have most containers implemented to first draft level),
			plan is to OBSOLETE/DELETE the Collection code. Make sure the appropraite APIs from
			that code are migrated into the appropriate containers, and see if any additional
			abstractions (like Iterable<T>) are appropriate to extract from Colleciton<T> to
			make shared/common bases.



