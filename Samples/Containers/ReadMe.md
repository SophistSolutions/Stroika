 * Containers

   Containers are classes that manage a homogenous collection of objects, in some well defined organization.
   
   Stroika's container classes are well documented, and examples are provided in the headers/class declarations.

   The 'containers' sample application walks you through a few helpful examples of how to use the Container
   classes. It illustrates:

   *  **Collection&lt;T&gt;** - the most basic 'modifyable Iterable' container
   *  **Mapping&lt;KEY,VALUE&gt;** - a key-value-pair association, where each item is associated with 0 or 1 associated (or mapped) element
   *  **Sequence&lt;T&gt;** - which is best thought of as a vector (though it can be implemented with a linked list, or doubly linkled list or other structure quite reasonably). It is a mapping from the natural numbers to elements of type T. Those natural numbers are called the indexes of the sequence.
   *  **Set&lt;T&gt;** - like a collection, but each element can be stored at most one time (attempts to store more not an error - just silently ignored). This closely matches the mathematical definition of a set.
