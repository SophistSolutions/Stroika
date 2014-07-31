This Folder contains the Foundation::Cryptography::Digest source code.

	Overview

	A Digest is an algorithm that takes a stream of bytes and computes a series of bits (can be thought of as a number, or string, or seqeunce
	of bits) which hopefully as nearly as possible (given the length of the digest) uniquely identifies the input.

	A digest is generally of fixed length - often 4, or 16, or 20 bytes long.


	We have Algorithms - that take a stream of bytes and convert it to a Digester<>::ReturnType. This is
	all template driven (at least the types and choice of algorithm) - so invoking a hash algorithm
	can be inlined.

	Then we have adapters - which can apply an arbitrary algorithm to an arbitrary type (though this
	template will only be specialized and supported for a few basic types - like String, int types, etc).

	Then - it should be easy to combine any choice of resulting hash size (e.g. 16 or 32 or 64 bit integer)
	with any hash algorithm and that with any basic type to be hashed over (or hashing over arbitrary blobs).



	Organization:

	Algorithms:
	Each file in the Algorithms folder repesents a different algorithm. Each follows a pattern layed out
	in the Digester<> template pattern.
	
	Conceptually its best for each to just operate on a stream, but for performance
	reasons, we must also allow Hash of individual ints etc.

See Also Cryptography/Hash.h