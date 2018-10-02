TODO (Foundation::Traversal)

	o		Add new FuzzyRange, and DiscreteFuzzyRange classes, just like Range<> and DiscreteRange<>
			but replace methods like Contains with MayContain, and DefinitelyContains.
			
			THIS can be used to replace the HealthFrameWorks DateRange code - which currently has
			endpoints UNKNOWN versus ENDOFTIME.

	o		Add RandomNumbers as DiscreteRange subclass, and take templated STL generator to make 
			Stroika iterator/genreator

	o		ALSO must add IComparable support somehow - not thought through yet!


Could also do for strings:
	(1..10).to_a»[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
	('bar'..'bat').to_a»["bar", "bas", "bat"]

	(PROBABY DON't DO FOR STRINGS)


FROM RUBY DOCS:
	So far we've shown ranges of numbers and strings. However, as you'd expect from an object-oriented language, 
	Ruby can create ranges based on objects that you define. The only constraints are that the 
	objects must respond to succ by returning the next object in sequence and the objects must be 
	comparable using <=>, the general comparison operator. 
	Sometimes called the spaceship operator, <=>compares two values, returning -1, 0, or +1 
	depending on whether the first is less than, equal to, or greater than the second.
