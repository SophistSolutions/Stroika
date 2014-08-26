>	Redo most of the code using AsSDKString() to use wide-char verisons of APIs. Will be much more performnant
	and otherwise probably equivalent!

>	More cleanups of FileSystem code? Move stuff from FileUtils if truly useful? 
	Like MemoryMappedFile SB redone using STREAMS – as MemoryMappedFileReaderSTREAM?
		o	MAYBE also a memory-mapped writer, but that’s tricker. Maybe just document todo 
			for that one. Mgit work though (cuz we know enough to grow size). Just not as useful. 
			Actaully – the memory-mapped reader stuff may not be that useful for streams? 
			But could be beeficial demand paging in file? Maybe not?

