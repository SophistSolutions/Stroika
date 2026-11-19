#
# LedLineIt, as its Makefile builds it - with Visual Studio and MFC (the configuration's ATLMFC): a Windows program, its
# resources compiled in. Not its Makefile's MIDL step: its output (LedLineIt.tlb, LedLineIt_h.h, LedLineIt_i.c) nothing uses.
#
add_executable (LedLineIt WIN32
    Sources/FontMenu.cpp Sources/LedLineItApplication.cpp Sources/LedLineItDocFrame.cpp Sources/LedLineItDocument.cpp
    Sources/LedLineItInPlaceFrame.cpp Sources/LedLineItMainFrame.cpp Sources/LedLineItServerItem.cpp Sources/LedLineItView.cpp
    Sources/Options.cpp
    Sources/LedLineIt.rc
)
target_compile_definitions (LedLineIt PRIVATE $<$<COMPILE_LANGUAGE:CXX>:_WIN32_WINNT=0x0A00> $<$<COMPILE_LANGUAGE:CXX>:STRICT>)
set_source_files_properties (Sources/LedLineIt.rc PROPERTIES COMPILE_OPTIONS /l0x0409) # one argument: LedIt's CMakeLists.txt says why
target_link_libraries (LedLineIt PRIVATE Stroika::Frameworks)
target_link_options (LedLineIt PRIVATE -ENTRY:wWinMainCRTStartup)
StroikaCopyFilesToExeDir (LedLineIt)
