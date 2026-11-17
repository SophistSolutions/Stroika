target_sources (stroika-frameworks PRIVATE
    JSONRPC.cpp
    SOAP.cpp
    SOAP_EnvelopeSchema.cpp
)
# the vendored schema SOAP_EnvelopeSchema.cpp includes
StroikaEmbedFiles (stroika-frameworks Schemas/soap-envelope-1.1.xsd)

add_subdirectory (OpenAPI)
add_subdirectory (Server)
