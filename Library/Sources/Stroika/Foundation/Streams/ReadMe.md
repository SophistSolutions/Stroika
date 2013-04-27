This folder contains all the Stroika Library Foundation::Streams source code.

TODO:

	o	Must add BasicTextInputStream and BasicTextOutputStream (memory-based). MAYBE. Probably helpful, though could just
		use TextInputStreamBinaryAdapator and TextOutputStreamBinaryAdapter. MAYBE even make that the first
		draft implemeantion (using those two together)

	o	Add TextInputOutputStream, and TextTiedStreams (med/low priority - but  trival since they do nothing)

	o	Add BufferedBinaryInputOutputStream

	o	Add ChunkedArrayBinaryInputOutputStream using Sequence_ChunkedArray, or perhaps even change 
		BasicBinaryInputOutputStream to use Sequence_ChunkedArray

	o	New module Splitter
			class BinaryInputSplitter;
			class BinaryInputOutputSplitter;

			Both streams are input (or output) streams and take (input or output) streams, but also take an 
			extra output stream data is written to.

			This can be handy for logging. To log a stream, just wrap with a splitter, and add an extra
			filestream to log the data someplace

			Note that BinaryOutputStream COULD have been named 'tee' - as its extactly analogous
			to the tee program in UNIX.

			Note in docs, no obvious use for having a BinaryInputOutput variant of splitter.