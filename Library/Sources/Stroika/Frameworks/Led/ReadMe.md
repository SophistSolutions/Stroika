# Stroika::[Frameworks](../ReadMe.md)::Led

This Folder contains the [Frameworks](../ReadMe.md)::Led Framework source code.

The Led framework is a collection of libraries which support text editing. This includes
code to serialize and deserialize rich text formats (such as RTF and HTML), and code to
render rich text (including RTL text, tables, etc), support for interactors (full text
editing/word processing functionality.

## See the Sample apps

### [LedIt](../../../../../Samples/LedIt/ReadMe.md)

Word Processor built using the Led framework

### [LedLineIt](../../../../../Samples/LedLineIt/ReadMe.md)

Simple Text Editor built using the Led framework

## Modules

- [BiDiLayoutEngine.h](BiDiLayoutEngine.h)
- [ChunkedArrayTextStore.h](ChunkedArrayTextStore.h)
- [Command.h](Command.h)
- [Config.h](Config.h)
- [FlavorPackage.h](FlavorPackage.h)
- [GDI.h](GDI.h)
- [HandySimple.h](HandySimple.h)
- [HiddenText.h](HiddenText.h)
- [IdleManager.h](IdleManager.h)
- [LineBasedPartition.h](LineBasedPartition.h)
- [Marker.h](Marker.h) - central concept in Led - used to track positions in the text buffer
- [MarkerCover.h](MarkerCover.h) - set of markers that 'covers' (without overlap) - aka PARTITION of the text positions
- [Platform/](Platform/ReadMe.md)
- [SimpleLed.h](SimpleLed.h)
- [SimpleTextImager.h](SimpleTextImager.h)
- [SimpleTextInteractor.h](SimpleTextInteractor.h)
- [SimpleTextStore.h](SimpleTextStore.h)
- [SpellCheckEngine.h](SpellCheckEngine.h)
- [SpellCheckEngine_Basic.h](SpellCheckEngine_Basic.h)
- [StandardStyledTextImager.h](StandardStyledTextImager.h)
- [StandardStyledTextInteractor.h](StandardStyledTextInteractor.h)
- [StdDialogs.h](StdDialogs.h)
- [StyledTextEmbeddedObjects.h](StyledTextEmbeddedObjects.h)
- [StyledTextImager.h](StyledTextImager.h)
- [StyledTextIO/](StyledTextIO/ReadMe.md)
- [Support.h](Support.h)
- [SyntaxColoring.h](SyntaxColoring.h)
- [TextBreaks.h](TextBreaks.h)
- [TextImager.h](TextImager.h) - a core building block abstract class which renders text to a GDI
- [TextInteractor.h](TextInteractor.h) - a core building block abstract class inherits from TextImager and adds GUI interaction
- [TextInteractorMixins.h](TextInteractorMixins.h)
- [TextStore.h](TextStore.h)
- [WordProcessor.h](WordProcessor.h)
- [WordWrappedTextImager.h](WordWrappedTextImager.h)
- [WordWrappedTextImager.h](WordWrappedTextImager.h)

## History/Notes

THIS snapshot is from HealthFrame (as of 2011-09-21).

I should review the other snapshots I have on

```
\Sandbox\Led (Dev and ARCHIVE_BACKUP_APX_2005_10_15)
```
