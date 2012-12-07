This folder contains all the Stroika Library Foundation::Streams source code.

TODO:

	o	Add	BasicBinaryInputOutputStream, and BasicBinaryOutputStream
		(NOTE - if/when we implemented Sequence_ChunkedArray, that would be a GREAT implemenation backend for
		BasicBinaryOutputStream and BasicBinaryInputOutputStream)

	o	Add TextOutputStreamBinaryAdapter
	
	o	Must add BasicTextInputStream and BasicTextOutputStream (memory-based). MAYBE. Probably helpful, though could just
		use TextInputStreamBinaryAdapator and TextOutputStreamBinaryAdapter. MAYBE even make that the first
		draft implemeantion (using those two together)

	o	Add TextInputOutputStream, and TextTiedStreams (med/low priority - but  trival since they do nothing)
