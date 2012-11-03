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


	// Need CFX-style interceptors mechanism... maybe not for v1.0?
	class	SOAPStub {
		public:
			class MethodCallHandler {
			public:
			virtual Memory::Variant	Call (String methodName, Memory::Variant args) = 0;
			};
			typedef	shared_ptr<MethodCallHandler> MethodCallHandlerPtr;
			SOAPStub (WSDL, SocketStream, MethodCallHandlerPtr);
			SOAPProxy (SocketStream, MethodCallHandlerPtr);

			// blocks reading on input and invokes callback as needed
			void	ProcessNextCall();
	};