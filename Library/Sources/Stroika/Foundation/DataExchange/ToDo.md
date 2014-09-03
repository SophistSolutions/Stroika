>	Consider if JSON Reader/XML Reader and JSON Writer XML/Writer should have factory?
	So far appears unneeded.

>	Consider Losing SAXObjectReader code - or think out how to combine it with 
	ObjectVariantMapper code - already documented todo item


>	DataExchange reader/writer classes (INI/JSON/XML) need a 'Context' feature. This is MAINLY to capture
	comments, but could also capture other optional features, like external code page, or flags about pretty printing.

	Then the reader would optionally read in this 'context' object (in addition to th real data) and you could make small
	changes, and then re-use the context to rewrite the file.

	Comments MIGHT be marked as Section/Item/Offset-From-Item (or maybe using something like xpath or something
	like Stroika markers?). Not sure. But come up with something so you can preserve comemnts when reading/writing these
	files!
