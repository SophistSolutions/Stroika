target_sources (stroika-frameworks PRIVATE
    AllInstruments.cpp
    Capturer.cpp
    CaptureSet.cpp
    Instrument.cpp
    Measurement.cpp
)

add_subdirectory (Instruments)
add_subdirectory (Support)
