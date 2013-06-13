This folder contains all the Stroika Library Foundation::Streams source code.

TODO:


	o	Consider Getting rid of 'Seekable' class. Instead - ALWAYS have 'GetOffset()' method available. And have 
		GetFeatures() method on BinaryStream () - and TextStrema () - and have IsSeekable() there. Innoncuous,
		just so you dont have to call Seek() in order to see if it raises exception.

		This will be helpful for 'BinarySubStream'

	o	BinarySubStream
		This takes an offset to start, and max offset end in CTOR (and a wrapped stream). 
		Then - it queries the offset of the original stream, and uses this to pull just the right subset
		out of the underlying Stream (NOT THOUGHT THROUGH - MAYBE USING SEEK?). Then calls to
		read on THIS substream make it look like it goes from 0..max-offset MINUS original-offset.

		THIS can be used for taking a BLOB of data - say from a webservice call - and then
		the argument to a PROXY/STUB de-serializer would be a regular stream and c++ structures
		would come back.

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