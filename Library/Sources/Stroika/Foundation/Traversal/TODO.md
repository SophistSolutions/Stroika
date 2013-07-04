TODO (Foundation::Traversal)

	o		Add RandomNumbers as DiscreteRange subclass, and take templated STL generator to make 
			Stroika iterator/genreator


Could also do for strings:
	(1..10).to_a»[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
	('bar'..'bat').to_a»["bar", "bas", "bat"]

	(PROBABY DONT DO FOR STRINGS)


DOCUMENT ABOUT CONNECTION TO RUBY, AND GENERATORS


FROM RUBY DOCS:
	So far we've shown ranges of numbers and strings. However, as you'd expect from an object-oriented language, 
	Ruby can create ranges based on objects that you define. The only constraints are that the 
	objects must respond to succ by returning the next object in sequence and the objects must be 
	comparable using <=>, the general comparison operator. 
	Sometimes called the spaceship operator, <=>compares two values, returning -1, 0, or +1 
	depending on whether the first is less than, equal to, or greater than the second.

	SOOOO - FOR STROIKA - ALSO must add IComparable support somehow - not thought through yet!

o	Look closely at Stroika DateRange code – for definitions of ends of range (when missing data)



