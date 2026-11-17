target_sources (stroika-frameworks PRIVATE
    DataTypes.cpp
    Device.cpp
    DeviceDescription.cpp
    DeviceDescriptionSchema.cpp
    ServiceDescription.cpp
    ServiceDescriptionSchema.cpp
)
# the vendored schemas the *DescriptionSchema.cpp files include
StroikaEmbedFiles (stroika-frameworks Schemas/device-1-0.xsd Schemas/service-1-0.xsd)

add_subdirectory (GENA)
add_subdirectory (SOAP)
add_subdirectory (SSDP)
