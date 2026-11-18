#
# LedIt, as its Makefile builds it - with Visual Studio and MFC (the configuration's ATLMFC): a Windows program, its
# resources compiled in. Not its Makefile's MIDL step: its output (LedIt.tlb, LedIt_h.h, LedIt_i.c) nothing uses.
#
add_executable (LedIt WIN32
    Sources/ColorMenu.cpp Sources/FormatToolbar.cpp Sources/LedItApplication.cpp Sources/LedItControlItem.cpp
    Sources/LedItDocument.cpp Sources/LedItInPlaceFrame.cpp Sources/LedItMainFrame.cpp Sources/LedItServerItem.cpp
    Sources/LedItView.cpp Sources/Main.cpp Sources/Options.cpp Sources/RulerToolbar.cpp Sources/Units.cpp
    Sources/LedIt.rc
)
target_compile_definitions (LedIt PRIVATE _WIN32_WINNT=0x0A00 STRICT WIN32)
# /l0x0409, one argument: with Ninja, cl is given rc's options too (finding the headers rc reads), and takes a lone 0x0409
# for a file
set_source_files_properties (Sources/LedIt.rc PROPERTIES COMPILE_OPTIONS /l0x0409)
target_link_libraries (LedIt PRIVATE stroika-frameworks)
target_link_options (LedIt PRIVATE -ENTRY:wWinMainCRTStartup)
StroikaCopyFilesToExeDir (LedIt)
