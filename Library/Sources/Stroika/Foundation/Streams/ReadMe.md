This folder contains all the Stroika Library Foundation::Streams source code.

TODO:

	o	Must add BasicTextInputStream and BasicTextOutputStream (memory-based). MAYBE. Probably helpful, though could just
		use TextInputStreamBinaryAdapator and TextOutputStreamBinaryAdapter. MAYBE even make that the first
		draft implemeantion (using those two together)

	o	Add TextInputOutputStream, and TextTiedStreams (med/low priority - but  trival since they do nothing)

	o	Add BufferedBinaryInputOutputStream

	o	Add ChunkedArrayBinaryInputOutputStream using Sequence_ChunkedArray, or perhaps even change 
		BasicBinaryInputOutputStream to use Sequence_ChunkedArray
