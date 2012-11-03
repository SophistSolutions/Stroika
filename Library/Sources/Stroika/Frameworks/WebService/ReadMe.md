The Stroika WebService Framework allows you to expose programatic APIs through web services (SOAP).

Status:
	[[[This is not even really started, just at the early desgin phase]]]



TODO/DESIGN NOTES:

	o	Stroika soap method take and reurning variant objects. At least params. Avoids issues with defining structure and handling optionals
	o	Layered – eventually maybe support typesafe stubs.
	o	But first part of Framework is Memory::Variant() – based, with optional function to validate according to WSDL (on in and out)

	class	SOAPProxy {
		public:
			SOAPProxy (WSDL, OPTIONAL URL);
			SOAPProxy (URL);

			// Sync call - with soap exceptions etc. Does validation if WSDL provided.
			Memory::Variant	Call (String methodName, Memory::Variant args);
	};

	class	SOAPStub {
		public:
			SOAPProxy (WSDL, OPTIONAL URL);
			SOAPProxy (URL);

			// Sync call - with soap exceptions etc. Does validation if WSDL provided.
			Memory::Variant	Call (String methodName, Memory::Variant args);
	};